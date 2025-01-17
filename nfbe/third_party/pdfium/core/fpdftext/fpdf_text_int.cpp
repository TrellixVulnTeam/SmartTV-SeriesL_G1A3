// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdftext/fpdf_text_int.h"

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_formobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_pageobject.h"
#include "core/fpdfapi/fpdf_page/include/cpdf_textobject.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/fpdftext/include/ipdf_linkextract.h"
#include "core/fpdftext/include/ipdf_textpage.h"
#include "core/fpdftext/include/ipdf_textpagefind.h"
#include "core/fpdftext/unicodenormalization.h"
#include "core/fxcrt/fx_bidi.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/fxcrt/include/fx_ucd.h"
#include "third_party/base/stl_util.h"

#define FPDFTEXT_RLTB 1
#define FPDFTEXT_LEFT -1
#define FPDFTEXT_RIGHT 1

#define FPDFTEXT_MATCHCASE 0x00000001
#define FPDFTEXT_MATCHWHOLEWORD 0x00000002
#define FPDFTEXT_CONSECUTIVE 0x00000004

namespace {

FX_BOOL _IsIgnoreSpaceCharacter(FX_WCHAR curChar) {
  if (curChar < 255) {
    return FALSE;
  }
  if ((curChar >= 0x0600 && curChar <= 0x06FF) ||
      (curChar >= 0xFE70 && curChar <= 0xFEFF) ||
      (curChar >= 0xFB50 && curChar <= 0xFDFF) ||
      (curChar >= 0x0400 && curChar <= 0x04FF) ||
      (curChar >= 0x0500 && curChar <= 0x052F) ||
      (curChar >= 0xA640 && curChar <= 0xA69F) ||
      (curChar >= 0x2DE0 && curChar <= 0x2DFF) || curChar == 8467 ||
      (curChar >= 0x2000 && curChar <= 0x206F)) {
    return FALSE;
  }
  return TRUE;
}

FX_FLOAT _NormalizeThreshold(FX_FLOAT threshold) {
  if (threshold < 300) {
    return threshold / 2.0f;
  }
  if (threshold < 500) {
    return threshold / 4.0f;
  }
  if (threshold < 700) {
    return threshold / 5.0f;
  }
  return threshold / 6.0f;
}

FX_FLOAT _CalculateBaseSpace(const CPDF_TextObject* pTextObj,
                             const CFX_Matrix& matrix) {
  FX_FLOAT baseSpace = 0.0;
  const int nItems = pTextObj->CountItems();
  if (pTextObj->m_TextState.GetObject()->m_CharSpace && nItems >= 3) {
    FX_BOOL bAllChar = TRUE;
    FX_FLOAT spacing = matrix.TransformDistance(
        pTextObj->m_TextState.GetObject()->m_CharSpace);
    baseSpace = spacing;
    for (int i = 0; i < nItems; i++) {
      CPDF_TextObjectItem item;
      pTextObj->GetItemInfo(i, &item);
      if (item.m_CharCode == (uint32_t)-1) {
        FX_FLOAT fontsize_h = pTextObj->m_TextState.GetFontSizeH();
        FX_FLOAT kerning = -fontsize_h * item.m_OriginX / 1000;
        baseSpace = std::min(baseSpace, kerning + spacing);
        bAllChar = FALSE;
      }
    }
    if (baseSpace < 0.0 || (nItems == 3 && !bAllChar)) {
      baseSpace = 0.0;
    }
  }
  return baseSpace;
}

const FX_FLOAT kDefaultFontSize = 1.0f;

}  // namespace

IPDF_TextPage* IPDF_TextPage::CreateTextPage(const CPDF_Page* pPage,
                                             int flags) {
  return new CPDF_TextPage(pPage, flags);
}

IPDF_TextPageFind* IPDF_TextPageFind::CreatePageFind(
    const IPDF_TextPage* pTextPage) {
  return pTextPage ? new CPDF_TextPageFind(pTextPage) : nullptr;
}

IPDF_LinkExtract* IPDF_LinkExtract::CreateLinkExtract() {
  return new CPDF_LinkExtract();
}

#define TEXT_BLANK_CHAR L' '
#define TEXT_LINEFEED_CHAR L'\n'
#define TEXT_RETURN_CHAR L'\r'
#define TEXT_EMPTY L""
#define TEXT_BLANK L" "
#define TEXT_RETURN_LINEFEED L"\r\n"
#define TEXT_LINEFEED L"\n"
#define TEXT_CHARRATIO_GAPDELTA 0.070

CPDF_TextPage::CPDF_TextPage(const CPDF_Page* pPage, int flags)
    : m_pPage(pPage),
      m_parserflag(flags),
      m_pPreTextObj(nullptr),
      m_bIsParsed(false),
      m_TextlineDir(-1),
      m_CurlineRect(0, 0, 0, 0) {
  m_TextBuf.EstimateSize(0, 10240);
  pPage->GetDisplayMatrix(m_DisplayMatrix, 0, 0, (int)pPage->GetPageWidth(),
                          (int)pPage->GetPageHeight(), 0);
}

bool CPDF_TextPage::IsControlChar(const PAGECHAR_INFO& charInfo) {
  switch (charInfo.m_Unicode) {
    case 0x2:
    case 0x3:
    case 0x93:
    case 0x94:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0xfffe:
      return charInfo.m_Flag != FPDFTEXT_CHAR_HYPHEN;
    default:
      return false;
  }
}

void CPDF_TextPage::ParseTextPage() {
  m_bIsParsed = false;
  m_TextBuf.Clear();
  m_CharList.clear();
  m_pPreTextObj = NULL;
  ProcessObject();

  m_bIsParsed = true;
  m_CharIndex.clear();
  int nCount = pdfium::CollectionSize<int>(m_CharList);
  if (nCount) {
    m_CharIndex.push_back(0);
  }
  for (int i = 0; i < nCount; i++) {
    int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
    FX_BOOL bNormal = FALSE;
    const PAGECHAR_INFO& charinfo = m_CharList[i];
    if (charinfo.m_Flag == FPDFTEXT_CHAR_GENERATED) {
      bNormal = TRUE;
    } else if (charinfo.m_Unicode == 0 || IsControlChar(charinfo)) {
      bNormal = FALSE;
    } else {
      bNormal = TRUE;
    }
    if (bNormal) {
      if (indexSize % 2) {
        m_CharIndex.push_back(1);
      } else {
        if (indexSize <= 0) {
          continue;
        }
        m_CharIndex[indexSize - 1] += 1;
      }
    } else {
      if (indexSize % 2) {
        if (indexSize <= 0) {
          continue;
        }
        m_CharIndex[indexSize - 1] = i + 1;
      } else {
        m_CharIndex.push_back(i + 1);
      }
    }
  }
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  if (indexSize % 2) {
    m_CharIndex.erase(m_CharIndex.begin() + indexSize - 1);
  }
}

int CPDF_TextPage::CountChars() const {
  return pdfium::CollectionSize<int>(m_CharList);
}

int CPDF_TextPage::CharIndexFromTextIndex(int TextIndex) const {
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  int count = 0;
  for (int i = 0; i < indexSize; i += 2) {
    count += m_CharIndex[i + 1];
    if (count > TextIndex)
      return TextIndex - count + m_CharIndex[i + 1] + m_CharIndex[i];
  }
  return -1;
}

int CPDF_TextPage::TextIndexFromCharIndex(int CharIndex) const {
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  int count = 0;
  for (int i = 0; i < indexSize; i += 2) {
    count += m_CharIndex[i + 1];
    if (m_CharIndex[i + 1] + m_CharIndex[i] > CharIndex) {
      if (CharIndex - m_CharIndex[i] < 0)
        return -1;

      return CharIndex - m_CharIndex[i] + count - m_CharIndex[i + 1];
    }
  }
  return -1;
}

void CPDF_TextPage::GetRectArray(int start,
                                 int nCount,
                                 CFX_RectArray& rectArray) const {
  if (start < 0 || nCount == 0) {
    return;
  }
  if (!m_bIsParsed) {
    return;
  }
  CPDF_TextObject* pCurObj = NULL;
  CFX_FloatRect rect;
  int curPos = start;
  FX_BOOL flagNewRect = TRUE;
  if (nCount + start > pdfium::CollectionSize<int>(m_CharList) ||
      nCount == -1) {
    nCount = pdfium::CollectionSize<int>(m_CharList) - start;
  }
  while (nCount--) {
    PAGECHAR_INFO info_curchar = m_CharList[curPos++];
    if (info_curchar.m_Flag == FPDFTEXT_CHAR_GENERATED) {
      continue;
    }
    if (info_curchar.m_CharBox.Width() < 0.01 ||
        info_curchar.m_CharBox.Height() < 0.01) {
      continue;
    }
    if (!pCurObj) {
      pCurObj = info_curchar.m_pTextObj;
    }
    if (pCurObj != info_curchar.m_pTextObj) {
      rectArray.Add(rect);
      pCurObj = info_curchar.m_pTextObj;
      flagNewRect = TRUE;
    }
    if (flagNewRect) {
      FX_FLOAT orgX = info_curchar.m_OriginX, orgY = info_curchar.m_OriginY;
      CFX_Matrix matrix, matrix_reverse;
      info_curchar.m_pTextObj->GetTextMatrix(&matrix);
      matrix.Concat(info_curchar.m_Matrix);
      matrix_reverse.SetReverse(matrix);
      matrix_reverse.Transform(orgX, orgY);
      rect.left = info_curchar.m_CharBox.left;
      rect.right = info_curchar.m_CharBox.right;
      if (pCurObj->GetFont()->GetTypeDescent()) {
        rect.bottom = orgY +
                      pCurObj->GetFont()->GetTypeDescent() *
                          pCurObj->GetFontSize() / 1000;
        FX_FLOAT xPosTemp = orgX;
        matrix.Transform(xPosTemp, rect.bottom);
      } else {
        rect.bottom = info_curchar.m_CharBox.bottom;
      }
      if (pCurObj->GetFont()->GetTypeAscent()) {
        rect.top =
            orgY +
            pCurObj->GetFont()->GetTypeAscent() * pCurObj->GetFontSize() / 1000;
        FX_FLOAT xPosTemp =
            orgX +
            GetCharWidth(info_curchar.m_CharCode, pCurObj->GetFont()) *
                pCurObj->GetFontSize() / 1000;
        matrix.Transform(xPosTemp, rect.top);
      } else {
        rect.top = info_curchar.m_CharBox.top;
      }
      flagNewRect = FALSE;
      rect = info_curchar.m_CharBox;
      rect.Normalize();
    } else {
      info_curchar.m_CharBox.Normalize();
      if (rect.left > info_curchar.m_CharBox.left) {
        rect.left = info_curchar.m_CharBox.left;
      }
      if (rect.right < info_curchar.m_CharBox.right) {
        rect.right = info_curchar.m_CharBox.right;
      }
      if (rect.top < info_curchar.m_CharBox.top) {
        rect.top = info_curchar.m_CharBox.top;
      }
      if (rect.bottom > info_curchar.m_CharBox.bottom) {
        rect.bottom = info_curchar.m_CharBox.bottom;
      }
    }
  }
  rectArray.Add(rect);
}

int CPDF_TextPage::GetIndexAtPos(CFX_FloatPoint point,
                                 FX_FLOAT xTolerance,
                                 FX_FLOAT yTolerance) const {
  if (!m_bIsParsed)
    return -3;

  int pos = 0;
  int NearPos = -1;
  double xdif = 5000;
  double ydif = 5000;
  while (pos < pdfium::CollectionSize<int>(m_CharList)) {
    PAGECHAR_INFO charinfo = m_CharList[pos];
    CFX_FloatRect charrect = charinfo.m_CharBox;
    if (charrect.Contains(point.x, point.y)) {
      break;
    }
    if (xTolerance > 0 || yTolerance > 0) {
      CFX_FloatRect charRectExt;
      charrect.Normalize();
      charRectExt.left = charrect.left - xTolerance / 2;
      charRectExt.right = charrect.right + xTolerance / 2;
      charRectExt.top = charrect.top + yTolerance / 2;
      charRectExt.bottom = charrect.bottom - yTolerance / 2;
      if (charRectExt.Contains(point.x, point.y)) {
        double curXdif, curYdif;
        curXdif = FXSYS_fabs(point.x - charrect.left) <
                          FXSYS_fabs(point.x - charrect.right)
                      ? FXSYS_fabs(point.x - charrect.left)
                      : FXSYS_fabs(point.x - charrect.right);
        curYdif = FXSYS_fabs(point.y - charrect.bottom) <
                          FXSYS_fabs(point.y - charrect.top)
                      ? FXSYS_fabs(point.y - charrect.bottom)
                      : FXSYS_fabs(point.y - charrect.top);
        if (curYdif + curXdif < xdif + ydif) {
          ydif = curYdif;
          xdif = curXdif;
          NearPos = pos;
        }
      }
    }
    ++pos;
  }
  return pos < pdfium::CollectionSize<int>(m_CharList) ? pos : NearPos;
}

CFX_WideString CPDF_TextPage::GetTextByRect(const CFX_FloatRect& rect) const {
  if (!m_bIsParsed)
    return CFX_WideString();

  FX_FLOAT posy = 0;
  bool IsContainPreChar = false;
  bool IsAddLineFeed = false;
  CFX_WideString strText;
  for (const auto& charinfo : m_CharList) {
    if (IsRectIntersect(rect, charinfo.m_CharBox)) {
      if (FXSYS_fabs(posy - charinfo.m_OriginY) > 0 && !IsContainPreChar &&
          IsAddLineFeed) {
        posy = charinfo.m_OriginY;
        if (strText.GetLength() > 0) {
          strText += L"\r\n";
        }
      }
      IsContainPreChar = true;
      IsAddLineFeed = false;
      if (charinfo.m_Unicode) {
        strText += charinfo.m_Unicode;
      }
    } else if (charinfo.m_Unicode == 32) {
      if (IsContainPreChar && charinfo.m_Unicode) {
        strText += charinfo.m_Unicode;
        IsContainPreChar = false;
        IsAddLineFeed = false;
      }
    } else {
      IsContainPreChar = false;
      IsAddLineFeed = true;
    }
  }
  return strText;
}

void CPDF_TextPage::GetRectsArrayByRect(const CFX_FloatRect& rect,
                                        CFX_RectArray& resRectArray) const {
  if (!m_bIsParsed)
    return;

  CFX_FloatRect curRect;
  bool flagNewRect = true;
  CPDF_TextObject* pCurObj = nullptr;
  for (auto info_curchar : m_CharList) {
    if (info_curchar.m_Flag == FPDFTEXT_CHAR_GENERATED) {
      continue;
    }
    if (!IsRectIntersect(rect, info_curchar.m_CharBox)) {
      continue;
    }
    if (!pCurObj) {
      pCurObj = info_curchar.m_pTextObj;
    }
    if (pCurObj != info_curchar.m_pTextObj) {
      resRectArray.Add(curRect);
      pCurObj = info_curchar.m_pTextObj;
      flagNewRect = true;
    }
    if (flagNewRect) {
      curRect = info_curchar.m_CharBox;
      curRect.Normalize();
      flagNewRect = false;
    } else {
      info_curchar.m_CharBox.Normalize();
      curRect.left = std::min(curRect.left, info_curchar.m_CharBox.left);
      curRect.bottom = std::min(curRect.bottom, info_curchar.m_CharBox.bottom);
      curRect.right = std::max(curRect.right, info_curchar.m_CharBox.right);
      curRect.top = std::max(curRect.top, info_curchar.m_CharBox.top);
    }
  }
  resRectArray.Add(curRect);
}

int CPDF_TextPage::GetIndexAtPos(FX_FLOAT x,
                                 FX_FLOAT y,
                                 FX_FLOAT xTolerance,
                                 FX_FLOAT yTolerance) const {
  CFX_FloatPoint point(x, y);
  return GetIndexAtPos(point, xTolerance, yTolerance);
}

void CPDF_TextPage::GetCharInfo(int index, FPDF_CHAR_INFO* info) const {
  if (!m_bIsParsed)
    return;

  if (index < 0 || index >= pdfium::CollectionSize<int>(m_CharList))
    return;

  const PAGECHAR_INFO& charinfo = m_CharList[index];
  info->m_Charcode = charinfo.m_CharCode;
  info->m_OriginX = charinfo.m_OriginX;
  info->m_OriginY = charinfo.m_OriginY;
  info->m_Unicode = charinfo.m_Unicode;
  info->m_Flag = charinfo.m_Flag;
  info->m_CharBox = charinfo.m_CharBox;
  info->m_pTextObj = charinfo.m_pTextObj;
  if (charinfo.m_pTextObj && charinfo.m_pTextObj->GetFont()) {
    info->m_FontSize = charinfo.m_pTextObj->GetFontSize();
  } else {
    info->m_FontSize = kDefaultFontSize;
  }
  info->m_Matrix.Copy(charinfo.m_Matrix);
}

void CPDF_TextPage::CheckMarkedContentObject(int32_t& start,
                                             int32_t& nCount) const {
  PAGECHAR_INFO charinfo = m_CharList[start];
  PAGECHAR_INFO charinfo2 = m_CharList[start + nCount - 1];
  if (FPDFTEXT_CHAR_PIECE != charinfo.m_Flag &&
      FPDFTEXT_CHAR_PIECE != charinfo2.m_Flag) {
    return;
  }
  if (FPDFTEXT_CHAR_PIECE == charinfo.m_Flag) {
    PAGECHAR_INFO charinfo1 = charinfo;
    int startIndex = start;
    while (FPDFTEXT_CHAR_PIECE == charinfo1.m_Flag &&
           charinfo1.m_Index == charinfo.m_Index) {
      startIndex--;
      if (startIndex < 0) {
        break;
      }
      charinfo1 = m_CharList[startIndex];
    }
    startIndex++;
    start = startIndex;
  }
  if (FPDFTEXT_CHAR_PIECE == charinfo2.m_Flag) {
    PAGECHAR_INFO charinfo3 = charinfo2;
    int endIndex = start + nCount - 1;
    while (FPDFTEXT_CHAR_PIECE == charinfo3.m_Flag &&
           charinfo3.m_Index == charinfo2.m_Index) {
      endIndex++;
      if (endIndex >= pdfium::CollectionSize<int>(m_CharList)) {
        break;
      }
      charinfo3 = m_CharList[endIndex];
    }
    endIndex--;
    nCount = endIndex - start + 1;
  }
}

CFX_WideString CPDF_TextPage::GetPageText(int start, int nCount) const {
  if (!m_bIsParsed || nCount == 0)
    return L"";

  if (start < 0)
    start = 0;

  if (nCount == -1) {
    nCount = pdfium::CollectionSize<int>(m_CharList) - start;
    return m_TextBuf.GetWideString().Mid(start,
                                         m_TextBuf.GetWideString().GetLength());
  }
  if (nCount <= 0 || m_CharList.empty()) {
    return L"";
  }
  if (nCount + start > pdfium::CollectionSize<int>(m_CharList) - 1) {
    nCount = pdfium::CollectionSize<int>(m_CharList) - start;
  }
  if (nCount <= 0) {
    return L"";
  }
  CheckMarkedContentObject(start, nCount);
  int startindex = 0;
  PAGECHAR_INFO charinfo = m_CharList[start];
  int startOffset = 0;
  while (charinfo.m_Index == -1) {
    startOffset++;
    if (startOffset > nCount ||
        start + startOffset >= pdfium::CollectionSize<int>(m_CharList)) {
      return L"";
    }
    charinfo = m_CharList[start + startOffset];
  }
  startindex = charinfo.m_Index;
  charinfo = m_CharList[start + nCount - 1];
  int nCountOffset = 0;
  while (charinfo.m_Index == -1) {
    nCountOffset++;
    if (nCountOffset >= nCount) {
      return L"";
    }
    charinfo = m_CharList[start + nCount - nCountOffset - 1];
  }
  nCount = start + nCount - nCountOffset - startindex;
  if (nCount <= 0) {
    return L"";
  }
  return m_TextBuf.GetWideString().Mid(startindex, nCount);
}

int CPDF_TextPage::CountRects(int start, int nCount) {
  if (!m_bIsParsed || start < 0)
    return -1;

  if (nCount == -1 ||
      nCount + start > pdfium::CollectionSize<int>(m_CharList)) {
    nCount = pdfium::CollectionSize<int>(m_CharList) - start;
  }
  m_SelRects.RemoveAll();
  GetRectArray(start, nCount, m_SelRects);
  return m_SelRects.GetSize();
}

void CPDF_TextPage::GetRect(int rectIndex,
                            FX_FLOAT& left,
                            FX_FLOAT& top,
                            FX_FLOAT& right,
                            FX_FLOAT& bottom) const {
  if (!m_bIsParsed)
    return;

  if (rectIndex < 0 || rectIndex >= m_SelRects.GetSize())
    return;

  left = m_SelRects.GetAt(rectIndex).left;
  top = m_SelRects.GetAt(rectIndex).top;
  right = m_SelRects.GetAt(rectIndex).right;
  bottom = m_SelRects.GetAt(rectIndex).bottom;
}

FX_BOOL CPDF_TextPage::GetBaselineRotate(int start, int end, int& Rotate) {
  if (end == start) {
    return FALSE;
  }
  FPDF_CHAR_INFO info_start;
  FPDF_CHAR_INFO info_end;
  GetCharInfo(start, &info_start);
  GetCharInfo(end, &info_end);
  while (info_end.m_CharBox.Width() == 0 || info_end.m_CharBox.Height() == 0) {
    if (--end <= start)
      return FALSE;

    GetCharInfo(end, &info_end);
  }
  FX_FLOAT dx = (info_end.m_OriginX - info_start.m_OriginX);
  FX_FLOAT dy = (info_end.m_OriginY - info_start.m_OriginY);
  if (dx == 0) {
    if (dy > 0) {
      Rotate = 90;
    } else if (dy < 0) {
      Rotate = 270;
    } else {
      Rotate = 0;
    }
  } else {
    float a = FXSYS_atan2(dy, dx);
    Rotate = (int)(a * 180 / FX_PI + 0.5);
  }
  if (Rotate < 0) {
    Rotate = -Rotate;
  } else if (Rotate > 0) {
    Rotate = 360 - Rotate;
  }
  return TRUE;
}

FX_BOOL CPDF_TextPage::GetBaselineRotate(const CFX_FloatRect& rect,
                                         int& Rotate) {
  int start, end, count,
      n = CountBoundedSegments(rect.left, rect.top, rect.right, rect.bottom,
                               TRUE);
  if (n < 1) {
    return FALSE;
  }
  if (n > 1) {
    GetBoundedSegment(n - 1, start, count);
    end = start + count - 1;
    GetBoundedSegment(0, start, count);
  } else {
    GetBoundedSegment(0, start, count);
    end = start + count - 1;
  }
  return GetBaselineRotate(start, end, Rotate);
}
FX_BOOL CPDF_TextPage::GetBaselineRotate(int rectIndex, int& Rotate) {
  if (!m_bIsParsed)
    return FALSE;

  if (rectIndex < 0 || rectIndex >= m_SelRects.GetSize())
    return FALSE;

  CFX_FloatRect rect = m_SelRects.GetAt(rectIndex);
  return GetBaselineRotate(rect, Rotate);
}

int CPDF_TextPage::CountBoundedSegments(FX_FLOAT left,
                                        FX_FLOAT top,
                                        FX_FLOAT right,
                                        FX_FLOAT bottom,
                                        FX_BOOL bContains) {
  m_Segments.RemoveAll();
  if (!m_bIsParsed)
    return -1;

  CFX_FloatRect rect(left, bottom, right, top);
  rect.Normalize();

  FPDF_SEGMENT segment;
  segment.m_Start = 0;
  segment.m_nCount = 0;

  int pos = 0;
  int segmentStatus = 0;
  FX_BOOL IsContainPreChar = FALSE;
  for (const auto& charinfo : m_CharList) {
    if (bContains && rect.Contains(charinfo.m_CharBox)) {
      if (segmentStatus == 0 || segmentStatus == 2) {
        segment.m_Start = pos;
        segment.m_nCount = 1;
        segmentStatus = 1;
      } else if (segmentStatus == 1) {
        segment.m_nCount++;
      }
      IsContainPreChar = TRUE;
    } else if (!bContains &&
               (IsRectIntersect(rect, charinfo.m_CharBox) ||
                rect.Contains(charinfo.m_OriginX, charinfo.m_OriginY))) {
      if (segmentStatus == 0 || segmentStatus == 2) {
        segment.m_Start = pos;
        segment.m_nCount = 1;
        segmentStatus = 1;
      } else if (segmentStatus == 1) {
        segment.m_nCount++;
      }
      IsContainPreChar = TRUE;
    } else if (charinfo.m_Unicode == 32) {
      if (IsContainPreChar == TRUE) {
        if (segmentStatus == 0 || segmentStatus == 2) {
          segment.m_Start = pos;
          segment.m_nCount = 1;
          segmentStatus = 1;
        } else if (segmentStatus == 1) {
          segment.m_nCount++;
        }
        IsContainPreChar = FALSE;
      } else {
        if (segmentStatus == 1) {
          segmentStatus = 2;
          m_Segments.Add(segment);
          segment.m_Start = 0;
          segment.m_nCount = 0;
        }
      }
    } else {
      if (segmentStatus == 1) {
        segmentStatus = 2;
        m_Segments.Add(segment);
        segment.m_Start = 0;
        segment.m_nCount = 0;
      }
      IsContainPreChar = FALSE;
    }
    pos++;
  }
  if (segmentStatus == 1) {
    segmentStatus = 2;
    m_Segments.Add(segment);
    segment.m_Start = 0;
    segment.m_nCount = 0;
  }
  return m_Segments.GetSize();
}

void CPDF_TextPage::GetBoundedSegment(int index, int& start, int& count) const {
  if (index < 0 || index >= m_Segments.GetSize()) {
    return;
  }
  start = m_Segments.GetAt(index).m_Start;
  count = m_Segments.GetAt(index).m_nCount;
}

int CPDF_TextPage::GetWordBreak(int index, int direction) const {
  if (!m_bIsParsed)
    return -1;

  if (direction != FPDFTEXT_LEFT && direction != FPDFTEXT_RIGHT)
    return -1;

  if (index < 0 || index >= pdfium::CollectionSize<int>(m_CharList))
    return -1;

  const PAGECHAR_INFO& charinfo = m_CharList[index];
  if (charinfo.m_Index == -1 || charinfo.m_Flag == FPDFTEXT_CHAR_GENERATED) {
    return index;
  }
  if (!IsLetter(charinfo.m_Unicode)) {
    return index;
  }
  int breakPos = index;
  if (direction == FPDFTEXT_LEFT) {
    while (--breakPos > 0) {
      if (!IsLetter(m_CharList[breakPos].m_Unicode))
        break;
    }
  } else if (direction == FPDFTEXT_RIGHT) {
    while (++breakPos < pdfium::CollectionSize<int>(m_CharList)) {
      if (!IsLetter(m_CharList[breakPos].m_Unicode))
        break;
    }
  }
  return breakPos;
}

int32_t CPDF_TextPage::FindTextlineFlowDirection() {
  const int32_t nPageWidth = static_cast<int32_t>(m_pPage->GetPageWidth());
  const int32_t nPageHeight = static_cast<int32_t>(m_pPage->GetPageHeight());
  std::vector<uint8_t> nHorizontalMask(nPageWidth);
  std::vector<uint8_t> nVerticalMask(nPageHeight);
  uint8_t* pDataH = nHorizontalMask.data();
  uint8_t* pDataV = nVerticalMask.data();
  int32_t index = 0;
  FX_FLOAT fLineHeight = 0.0f;
  if (m_pPage->GetPageObjectList()->empty())
    return -1;

  for (auto& pPageObj : *m_pPage->GetPageObjectList()) {
    if (!pPageObj || !pPageObj->IsText())
      continue;

    int32_t minH =
        (int32_t)pPageObj->m_Left < 0 ? 0 : (int32_t)pPageObj->m_Left;
    int32_t maxH = (int32_t)pPageObj->m_Right > nPageWidth
                       ? nPageWidth
                       : (int32_t)pPageObj->m_Right;
    int32_t minV =
        (int32_t)pPageObj->m_Bottom < 0 ? 0 : (int32_t)pPageObj->m_Bottom;
    int32_t maxV = (int32_t)pPageObj->m_Top > nPageHeight
                       ? nPageHeight
                       : (int32_t)pPageObj->m_Top;
    if (minH >= maxH || minV >= maxV)
      continue;

    FXSYS_memset(pDataH + minH, 1, maxH - minH);
    FXSYS_memset(pDataV + minV, 1, maxV - minV);
    if (fLineHeight <= 0.0f)
      fLineHeight = pPageObj->m_Top - pPageObj->m_Bottom;
  }
  int32_t nStartH = 0;
  int32_t nEndH = 0;
  FX_FLOAT nSumH = 0.0f;
  for (index = 0; index < nPageWidth; index++) {
    if (1 == nHorizontalMask[index])
      break;
  }
  nStartH = index;
  for (index = nPageWidth; index > 0; index--) {
    if (1 == nHorizontalMask[index - 1])
      break;
  }
  nEndH = index;
  for (index = nStartH; index < nEndH; index++) {
    nSumH += nHorizontalMask[index];
  }
  nSumH /= nEndH - nStartH;
  int32_t nStartV = 0;
  int32_t nEndV = 0;
  FX_FLOAT nSumV = 0.0f;
  for (index = 0; index < nPageHeight; index++) {
    if (1 == nVerticalMask[index])
      break;
  }
  nStartV = index;
  for (index = nPageHeight; index > 0; index--) {
    if (1 == nVerticalMask[index - 1])
      break;
  }
  nEndV = index;
  for (index = nStartV; index < nEndV; index++) {
    nSumV += nVerticalMask[index];
  }
  nSumV /= nEndV - nStartV;
  if ((nEndV - nStartV) < (int32_t)(2 * fLineHeight)) {
    return 0;
  }
  if ((nEndH - nStartH) < (int32_t)(2 * fLineHeight)) {
    return 1;
  }
  if (nSumH > 0.8f) {
    return 0;
  }
  if (nSumH - nSumV > 0.0f) {
    return 0;
  }
  if (nSumV - nSumH > 0.0f) {
    return 1;
  }
  return -1;
}

void CPDF_TextPage::ProcessObject() {
  if (m_pPage->GetPageObjectList()->empty())
    return;

  m_TextlineDir = FindTextlineFlowDirection();
  const CPDF_PageObjectList* pObjList = m_pPage->GetPageObjectList();
  for (auto it = pObjList->begin(); it != pObjList->end(); ++it) {
    if (CPDF_PageObject* pObj = it->get()) {
      if (pObj->IsText()) {
        CFX_Matrix matrix;
        ProcessTextObject(pObj->AsText(), matrix, pObjList, it);
      } else if (pObj->IsForm()) {
        CFX_Matrix formMatrix(1, 0, 0, 1, 0, 0);
        ProcessFormObject(pObj->AsForm(), formMatrix);
      }
    }
  }
  for (int i = 0; i < m_LineObj.GetSize(); i++)
    ProcessTextObject(m_LineObj.GetAt(i));

  m_LineObj.RemoveAll();
  CloseTempLine();
}

void CPDF_TextPage::ProcessFormObject(CPDF_FormObject* pFormObj,
                                      const CFX_Matrix& formMatrix) {
  CPDF_PageObjectList* pObjectList = pFormObj->m_pForm->GetPageObjectList();
  if (pObjectList->empty())
    return;

  CFX_Matrix curFormMatrix;
  curFormMatrix.Copy(pFormObj->m_FormMatrix);
  curFormMatrix.Concat(formMatrix);

  for (auto it = pObjectList->begin(); it != pObjectList->end(); ++it) {
    if (CPDF_PageObject* pPageObj = it->get()) {
      if (pPageObj->IsText()) {
        ProcessTextObject(pPageObj->AsText(), curFormMatrix, pObjectList, it);
      } else if (pPageObj->IsForm()) {
        ProcessFormObject(pPageObj->AsForm(), curFormMatrix);
      }
    }
  }
}

int CPDF_TextPage::GetCharWidth(uint32_t charCode, CPDF_Font* pFont) const {
  if (charCode == CPDF_Font::kInvalidCharCode)
    return 0;

  if (int w = pFont->GetCharWidthF(charCode))
    return w;

  CFX_ByteString str;
  pFont->AppendChar(str, charCode);
  if (int w = pFont->GetStringWidth(str, 1))
    return w;

  return pFont->GetCharBBox(charCode).Width();
}

void CPDF_TextPage::OnPiece(CFX_BidiChar* pBidi, CFX_WideString& str) {
  CFX_BidiChar::Segment seg = pBidi->GetSegmentInfo();
  if (seg.direction == CFX_BidiChar::RIGHT) {
    for (int i = seg.start + seg.count; i > seg.start; i--) {
      m_TextBuf.AppendChar(str.GetAt(i - i));
      m_CharList.push_back(m_TempCharList[i - 1]);
    }
  } else {
    for (int i = seg.start; i < seg.start + seg.count; i++) {
      m_TextBuf.AppendChar(str.GetAt(i));
      m_CharList.push_back(m_TempCharList[i]);
    }
  }
}

void CPDF_TextPage::AddCharInfoByLRDirection(FX_WCHAR wChar,
                                             PAGECHAR_INFO info) {
  if (!IsControlChar(info)) {
    info.m_Index = m_TextBuf.GetLength();
    if (wChar >= 0xFB00 && wChar <= 0xFB06) {
      FX_WCHAR* pDst = NULL;
      FX_STRSIZE nCount = FX_Unicode_GetNormalization(wChar, pDst);
      if (nCount >= 1) {
        pDst = FX_Alloc(FX_WCHAR, nCount);
        FX_Unicode_GetNormalization(wChar, pDst);
        for (int nIndex = 0; nIndex < nCount; nIndex++) {
          PAGECHAR_INFO info2 = info;
          info2.m_Unicode = pDst[nIndex];
          info2.m_Flag = FPDFTEXT_CHAR_PIECE;
          m_TextBuf.AppendChar(info2.m_Unicode);
          m_CharList.push_back(info2);
        }
        FX_Free(pDst);
        return;
      }
    }
    m_TextBuf.AppendChar(wChar);
  } else {
    info.m_Index = -1;
  }
  m_CharList.push_back(info);
}

void CPDF_TextPage::AddCharInfoByRLDirection(FX_WCHAR wChar,
                                             PAGECHAR_INFO info) {
  if (!IsControlChar(info)) {
    info.m_Index = m_TextBuf.GetLength();
    wChar = FX_GetMirrorChar(wChar, TRUE, FALSE);
    FX_WCHAR* pDst = NULL;
    FX_STRSIZE nCount = FX_Unicode_GetNormalization(wChar, pDst);
    if (nCount >= 1) {
      pDst = FX_Alloc(FX_WCHAR, nCount);
      FX_Unicode_GetNormalization(wChar, pDst);
      for (int nIndex = 0; nIndex < nCount; nIndex++) {
        PAGECHAR_INFO info2 = info;
        info2.m_Unicode = pDst[nIndex];
        info2.m_Flag = FPDFTEXT_CHAR_PIECE;
        m_TextBuf.AppendChar(info2.m_Unicode);
        m_CharList.push_back(info2);
      }
      FX_Free(pDst);
      return;
    }
    info.m_Unicode = wChar;
    m_TextBuf.AppendChar(info.m_Unicode);
  } else {
    info.m_Index = -1;
  }
  m_CharList.push_back(info);
}

void CPDF_TextPage::CloseTempLine() {
  if (m_TempCharList.empty())
    return;

  CFX_WideString str = m_TempTextBuf.GetWideString();
  FX_BOOL bPrevSpace = FALSE;
  for (int i = 0; i < str.GetLength(); i++) {
    if (str.GetAt(i) != ' ') {
      bPrevSpace = FALSE;
      continue;
    }
    if (bPrevSpace) {
      m_TempTextBuf.Delete(i, 1);
      m_TempCharList.erase(m_TempCharList.begin() + i);
      str.Delete(i);
      i--;
    }
    bPrevSpace = TRUE;
  }
  CFX_BidiString bidi(str);
  if (m_parserflag == FPDFTEXT_RLTB)
    bidi.SetOverallDirectionRight();
  CFX_BidiChar::Direction eCurrentDirection = bidi.OverallDirection();
  for (const auto& segment : bidi) {
    if (segment.direction == CFX_BidiChar::RIGHT ||
        (segment.direction == CFX_BidiChar::NEUTRAL &&
         eCurrentDirection == CFX_BidiChar::RIGHT)) {
      eCurrentDirection = CFX_BidiChar::RIGHT;
      for (int m = segment.start + segment.count; m > segment.start; --m)
        AddCharInfoByRLDirection(bidi.CharAt(m - 1), m_TempCharList[m - 1]);
    } else {
      eCurrentDirection = CFX_BidiChar::LEFT;
      for (int m = segment.start; m < segment.start + segment.count; m++)
        AddCharInfoByLRDirection(bidi.CharAt(m), m_TempCharList[m]);
    }
  }
  m_TempCharList.clear();
  m_TempTextBuf.Delete(0, m_TempTextBuf.GetLength());
}

void CPDF_TextPage::ProcessTextObject(
    CPDF_TextObject* pTextObj,
    const CFX_Matrix& formMatrix,
    const CPDF_PageObjectList* pObjList,
    CPDF_PageObjectList::const_iterator ObjPos) {
  CFX_FloatRect re(pTextObj->m_Left, pTextObj->m_Bottom, pTextObj->m_Right,
                   pTextObj->m_Top);
  if (FXSYS_fabs(pTextObj->m_Right - pTextObj->m_Left) < 0.01f) {
    return;
  }
  int count = m_LineObj.GetSize();
  PDFTEXT_Obj Obj;
  Obj.m_pTextObj = pTextObj;
  Obj.m_formMatrix = formMatrix;
  if (count == 0) {
    m_LineObj.Add(Obj);
    return;
  }
  if (IsSameAsPreTextObject(pTextObj, pObjList, ObjPos)) {
    return;
  }
  PDFTEXT_Obj prev_Obj = m_LineObj.GetAt(count - 1);
  CPDF_TextObjectItem item;
  int nItem = prev_Obj.m_pTextObj->CountItems();
  prev_Obj.m_pTextObj->GetItemInfo(nItem - 1, &item);
  FX_FLOAT prev_width =
      GetCharWidth(item.m_CharCode, prev_Obj.m_pTextObj->GetFont()) *
      prev_Obj.m_pTextObj->GetFontSize() / 1000;
  CFX_Matrix prev_matrix;
  prev_Obj.m_pTextObj->GetTextMatrix(&prev_matrix);
  prev_width = FXSYS_fabs(prev_width);
  prev_matrix.Concat(prev_Obj.m_formMatrix);
  prev_width = prev_matrix.TransformDistance(prev_width);
  pTextObj->GetItemInfo(0, &item);
  FX_FLOAT this_width = GetCharWidth(item.m_CharCode, pTextObj->GetFont()) *
                        pTextObj->GetFontSize() / 1000;
  this_width = FXSYS_fabs(this_width);
  CFX_Matrix this_matrix;
  pTextObj->GetTextMatrix(&this_matrix);
  this_width = FXSYS_fabs(this_width);
  this_matrix.Concat(formMatrix);
  this_width = this_matrix.TransformDistance(this_width);
  FX_FLOAT threshold =
      prev_width > this_width ? prev_width / 4 : this_width / 4;
  FX_FLOAT prev_x = prev_Obj.m_pTextObj->GetPosX(),
           prev_y = prev_Obj.m_pTextObj->GetPosY();
  prev_Obj.m_formMatrix.Transform(prev_x, prev_y);
  m_DisplayMatrix.Transform(prev_x, prev_y);
  FX_FLOAT this_x = pTextObj->GetPosX(), this_y = pTextObj->GetPosY();
  formMatrix.Transform(this_x, this_y);
  m_DisplayMatrix.Transform(this_x, this_y);
  if (FXSYS_fabs(this_y - prev_y) > threshold * 2) {
    for (int i = 0; i < count; i++) {
      ProcessTextObject(m_LineObj.GetAt(i));
    }
    m_LineObj.RemoveAll();
    m_LineObj.Add(Obj);
    return;
  }
  int i = 0;
  for (i = count - 1; i >= 0; i--) {
    PDFTEXT_Obj prev_Obj = m_LineObj.GetAt(i);
    CFX_Matrix prev_matrix;
    prev_Obj.m_pTextObj->GetTextMatrix(&prev_matrix);
    FX_FLOAT Prev_x = prev_Obj.m_pTextObj->GetPosX(),
             Prev_y = prev_Obj.m_pTextObj->GetPosY();
    prev_Obj.m_formMatrix.Transform(Prev_x, Prev_y);
    m_DisplayMatrix.Transform(Prev_x, Prev_y);
    if (this_x >= Prev_x) {
      if (i == count - 1) {
        m_LineObj.Add(Obj);
      } else {
        m_LineObj.InsertAt(i + 1, Obj);
      }
      break;
    }
  }
  if (i < 0) {
    m_LineObj.InsertAt(0, Obj);
  }
}

int32_t CPDF_TextPage::PreMarkedContent(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj;
  CPDF_ContentMarkData* pMarkData =
      (CPDF_ContentMarkData*)pTextObj->m_ContentMark.GetObject();
  if (!pMarkData)
    return FPDFTEXT_MC_PASS;

  int nContentMark = pMarkData->CountItems();
  if (nContentMark < 1)
    return FPDFTEXT_MC_PASS;
  CFX_WideString actText;
  FX_BOOL bExist = FALSE;
  CPDF_Dictionary* pDict = NULL;
  int n = 0;
  for (n = 0; n < nContentMark; n++) {
    CPDF_ContentMarkItem& item = pMarkData->GetItem(n);
    if (item.GetParamType() == CPDF_ContentMarkItem::ParamType::None)
      continue;
    pDict = item.GetParam();
    CPDF_String* temp =
        ToString(pDict ? pDict->GetObjectBy("ActualText") : nullptr);
    if (temp) {
      bExist = TRUE;
      actText = temp->GetUnicodeText();
    }
  }
  if (!bExist)
    return FPDFTEXT_MC_PASS;

  if (m_pPreTextObj) {
    CPDF_ContentMarkData* pPreMarkData =
        (CPDF_ContentMarkData*)m_pPreTextObj->m_ContentMark.GetObject();
    if (pPreMarkData && pPreMarkData->CountItems() == n &&
        pDict == pPreMarkData->GetItem(n - 1).GetParam()) {
      return FPDFTEXT_MC_DONE;
    }
  }
  FX_STRSIZE nItems = actText.GetLength();
  if (nItems < 1)
    return FPDFTEXT_MC_PASS;

  CPDF_Font* pFont = pTextObj->GetFont();
  bExist = FALSE;
  for (FX_STRSIZE i = 0; i < nItems; i++) {
    if (pFont->CharCodeFromUnicode(actText.GetAt(i)) !=
        CPDF_Font::kInvalidCharCode) {
      bExist = TRUE;
      break;
    }
  }
  if (!bExist)
    return FPDFTEXT_MC_PASS;

  bExist = FALSE;
  for (FX_STRSIZE i = 0; i < nItems; i++) {
    FX_WCHAR wChar = actText.GetAt(i);
    if ((wChar > 0x80 && wChar < 0xFFFD) || (wChar <= 0x80 && isprint(wChar))) {
      bExist = TRUE;
      break;
    }
  }
  if (!bExist)
    return FPDFTEXT_MC_DONE;

  return FPDFTEXT_MC_DELAY;
}

void CPDF_TextPage::ProcessMarkedContent(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj;
  CPDF_ContentMarkData* pMarkData =
      (CPDF_ContentMarkData*)pTextObj->m_ContentMark.GetObject();
  if (!pMarkData)
    return;

  int nContentMark = pMarkData->CountItems();
  if (nContentMark < 1)
    return;
  CFX_WideString actText;
  CPDF_Dictionary* pDict = NULL;
  for (int n = 0; n < nContentMark; n++) {
    CPDF_ContentMarkItem& item = pMarkData->GetItem(n);
    if (item.GetParamType() == CPDF_ContentMarkItem::ParamType::None)
      continue;
    pDict = item.GetParam();
    if (pDict)
      actText = pDict->GetUnicodeTextBy("ActualText");
  }
  FX_STRSIZE nItems = actText.GetLength();
  if (nItems < 1)
    return;

  CPDF_Font* pFont = pTextObj->GetFont();
  CFX_Matrix formMatrix = Obj.m_formMatrix;
  CFX_Matrix matrix;
  pTextObj->GetTextMatrix(&matrix);
  matrix.Concat(formMatrix);
  FX_FLOAT fPosX = pTextObj->GetPosX();
  FX_FLOAT fPosY = pTextObj->GetPosY();
  int nCharInfoIndex = m_TextBuf.GetLength();
  CFX_FloatRect charBox;
  charBox.top = pTextObj->m_Top;
  charBox.left = pTextObj->m_Left;
  charBox.right = pTextObj->m_Right;
  charBox.bottom = pTextObj->m_Bottom;
  for (FX_STRSIZE k = 0; k < nItems; k++) {
    FX_WCHAR wChar = actText.GetAt(k);
    if (wChar <= 0x80 && !isprint(wChar)) {
      wChar = 0x20;
    }
    if (wChar >= 0xFFFD) {
      continue;
    }
    PAGECHAR_INFO charinfo;
    charinfo.m_OriginX = fPosX;
    charinfo.m_OriginY = fPosY;
    charinfo.m_Index = nCharInfoIndex;
    charinfo.m_Unicode = wChar;
    charinfo.m_CharCode = pFont->CharCodeFromUnicode(wChar);
    charinfo.m_Flag = FPDFTEXT_CHAR_PIECE;
    charinfo.m_pTextObj = pTextObj;
    charinfo.m_CharBox.top = charBox.top;
    charinfo.m_CharBox.left = charBox.left;
    charinfo.m_CharBox.right = charBox.right;
    charinfo.m_CharBox.bottom = charBox.bottom;
    charinfo.m_Matrix.Copy(matrix);
    m_TempTextBuf.AppendChar(wChar);
    m_TempCharList.push_back(charinfo);
  }
}

void CPDF_TextPage::FindPreviousTextObject() {
  if (m_TempCharList.empty() && m_CharList.empty())
    return;

  PAGECHAR_INFO preChar =
      m_TempCharList.empty() ? m_CharList.back() : m_TempCharList.back();

  if (preChar.m_pTextObj)
    m_pPreTextObj = preChar.m_pTextObj;
}
void CPDF_TextPage::SwapTempTextBuf(int32_t iCharListStartAppend,
                                    int32_t iBufStartAppend) {
  int32_t i = iCharListStartAppend;
  int32_t j = pdfium::CollectionSize<int32_t>(m_TempCharList) - 1;
  for (; i < j; i++, j--) {
    std::swap(m_TempCharList[i], m_TempCharList[j]);
    std::swap(m_TempCharList[i].m_Index, m_TempCharList[j].m_Index);
  }
  FX_WCHAR* pTempBuffer = m_TempTextBuf.GetBuffer();
  i = iBufStartAppend;
  j = m_TempTextBuf.GetLength() - 1;
  for (; i < j; i++, j--) {
    std::swap(pTempBuffer[i], pTempBuffer[j]);
  }
}

FX_BOOL CPDF_TextPage::IsRightToLeft(const CPDF_TextObject* pTextObj,
                                     const CPDF_Font* pFont,
                                     int nItems) const {
  CFX_WideString str;
  for (int32_t i = 0; i < nItems; i++) {
    CPDF_TextObjectItem item;
    pTextObj->GetItemInfo(i, &item);
    if (item.m_CharCode == (uint32_t)-1) {
      continue;
    }
    CFX_WideString wstrItem = pFont->UnicodeFromCharCode(item.m_CharCode);
    FX_WCHAR wChar = wstrItem.GetAt(0);
    if ((wstrItem.IsEmpty() || wChar == 0) && item.m_CharCode) {
      wChar = (FX_WCHAR)item.m_CharCode;
    }
    if (wChar)
      str += wChar;
  }
  return CFX_BidiString(str).OverallDirection() == CFX_BidiChar::RIGHT;
}

void CPDF_TextPage::ProcessTextObject(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj;
  if (FXSYS_fabs(pTextObj->m_Right - pTextObj->m_Left) < 0.01f) {
    return;
  }
  CFX_Matrix formMatrix = Obj.m_formMatrix;
  CPDF_Font* pFont = pTextObj->GetFont();
  CFX_Matrix matrix;
  pTextObj->GetTextMatrix(&matrix);
  matrix.Concat(formMatrix);
  int32_t bPreMKC = PreMarkedContent(Obj);
  if (FPDFTEXT_MC_DONE == bPreMKC) {
    m_pPreTextObj = pTextObj;
    m_perMatrix.Copy(formMatrix);
    return;
  }
  int result = 0;
  if (m_pPreTextObj) {
    result = ProcessInsertObject(pTextObj, formMatrix);
    if (2 == result) {
      m_CurlineRect =
          CFX_FloatRect(Obj.m_pTextObj->m_Left, Obj.m_pTextObj->m_Bottom,
                        Obj.m_pTextObj->m_Right, Obj.m_pTextObj->m_Top);
    } else {
      m_CurlineRect.Union(
          CFX_FloatRect(Obj.m_pTextObj->m_Left, Obj.m_pTextObj->m_Bottom,
                        Obj.m_pTextObj->m_Right, Obj.m_pTextObj->m_Top));
    }
    PAGECHAR_INFO generateChar;
    if (result == 1) {
      if (GenerateCharInfo(TEXT_BLANK_CHAR, generateChar)) {
        if (!formMatrix.IsIdentity()) {
          generateChar.m_Matrix.Copy(formMatrix);
        }
        m_TempTextBuf.AppendChar(TEXT_BLANK_CHAR);
        m_TempCharList.push_back(generateChar);
      }
    } else if (result == 2) {
      CloseTempLine();
      if (m_TextBuf.GetSize()) {
        if (GenerateCharInfo(TEXT_RETURN_CHAR, generateChar)) {
          m_TextBuf.AppendChar(TEXT_RETURN_CHAR);
          if (!formMatrix.IsIdentity()) {
            generateChar.m_Matrix.Copy(formMatrix);
          }
          m_CharList.push_back(generateChar);
        }
        if (GenerateCharInfo(TEXT_LINEFEED_CHAR, generateChar)) {
          m_TextBuf.AppendChar(TEXT_LINEFEED_CHAR);
          if (!formMatrix.IsIdentity()) {
            generateChar.m_Matrix.Copy(formMatrix);
          }
          m_CharList.push_back(generateChar);
        }
      }
    } else if (result == 3) {
      int32_t nChars = pTextObj->CountChars();
      if (nChars == 1) {
        CPDF_TextObjectItem item;
        pTextObj->GetCharInfo(0, &item);
        CFX_WideString wstrItem =
            pTextObj->GetFont()->UnicodeFromCharCode(item.m_CharCode);
        if (wstrItem.IsEmpty()) {
          wstrItem += (FX_WCHAR)item.m_CharCode;
        }
        FX_WCHAR curChar = wstrItem.GetAt(0);
        if (0x2D == curChar || 0xAD == curChar) {
          return;
        }
      }
      while (m_TempTextBuf.GetSize() > 0 &&
             m_TempTextBuf.GetWideString().GetAt(m_TempTextBuf.GetLength() -
                                                 1) == 0x20) {
        m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
        m_TempCharList.pop_back();
      }
      PAGECHAR_INFO* charinfo = &m_TempCharList.back();
      m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
      charinfo->m_Unicode = 0x2;
      charinfo->m_Flag = FPDFTEXT_CHAR_HYPHEN;
      m_TempTextBuf.AppendChar(0xfffe);
    }
  } else {
    m_CurlineRect =
        CFX_FloatRect(Obj.m_pTextObj->m_Left, Obj.m_pTextObj->m_Bottom,
                      Obj.m_pTextObj->m_Right, Obj.m_pTextObj->m_Top);
  }
  if (FPDFTEXT_MC_DELAY == bPreMKC) {
    ProcessMarkedContent(Obj);
    m_pPreTextObj = pTextObj;
    m_perMatrix.Copy(formMatrix);
    return;
  }
  m_pPreTextObj = pTextObj;
  m_perMatrix.Copy(formMatrix);
  int nItems = pTextObj->CountItems();
  FX_FLOAT baseSpace = _CalculateBaseSpace(pTextObj, matrix);

  const FX_BOOL bR2L = IsRightToLeft(pTextObj, pFont, nItems);
  const FX_BOOL bIsBidiAndMirrorInverse =
      bR2L && (matrix.a * matrix.d - matrix.b * matrix.c) < 0;
  int32_t iBufStartAppend = m_TempTextBuf.GetLength();
  int32_t iCharListStartAppend =
      pdfium::CollectionSize<int32_t>(m_TempCharList);

  FX_FLOAT spacing = 0;
  for (int i = 0; i < nItems; i++) {
    CPDF_TextObjectItem item;
    PAGECHAR_INFO charinfo;
    charinfo.m_OriginX = 0;
    charinfo.m_OriginY = 0;
    pTextObj->GetItemInfo(i, &item);
    if (item.m_CharCode == (uint32_t)-1) {
      CFX_WideString str = m_TempTextBuf.GetWideString();
      if (str.IsEmpty()) {
        str = m_TextBuf.GetWideString();
      }
      if (str.IsEmpty() || str.GetAt(str.GetLength() - 1) == TEXT_BLANK_CHAR) {
        continue;
      }
      FX_FLOAT fontsize_h = pTextObj->m_TextState.GetFontSizeH();
      spacing = -fontsize_h * item.m_OriginX / 1000;
      continue;
    }
    FX_FLOAT charSpace = pTextObj->m_TextState.GetObject()->m_CharSpace;
    if (charSpace > 0.001) {
      spacing += matrix.TransformDistance(charSpace);
    } else if (charSpace < -0.001) {
      spacing -= matrix.TransformDistance(FXSYS_fabs(charSpace));
    }
    spacing -= baseSpace;
    if (spacing && i > 0) {
      int last_width = 0;
      FX_FLOAT fontsize_h = pTextObj->m_TextState.GetFontSizeH();
      uint32_t space_charcode = pFont->CharCodeFromUnicode(' ');
      FX_FLOAT threshold = 0;
      if (space_charcode != CPDF_Font::kInvalidCharCode) {
        threshold = fontsize_h * pFont->GetCharWidthF(space_charcode) / 1000;
      }
      if (threshold > fontsize_h / 3) {
        threshold = 0;
      } else {
        threshold /= 2;
      }
      if (threshold == 0) {
        threshold = fontsize_h;
        int this_width = FXSYS_abs(GetCharWidth(item.m_CharCode, pFont));
        threshold = this_width > last_width ? (FX_FLOAT)this_width
                                            : (FX_FLOAT)last_width;
        threshold = _NormalizeThreshold(threshold);
        threshold = fontsize_h * threshold / 1000;
      }
      if (threshold && (spacing && spacing >= threshold)) {
        charinfo.m_Unicode = TEXT_BLANK_CHAR;
        charinfo.m_Flag = FPDFTEXT_CHAR_GENERATED;
        charinfo.m_pTextObj = pTextObj;
        charinfo.m_Index = m_TextBuf.GetLength();
        m_TempTextBuf.AppendChar(TEXT_BLANK_CHAR);
        charinfo.m_CharCode = CPDF_Font::kInvalidCharCode;
        charinfo.m_Matrix.Copy(formMatrix);
        matrix.Transform(item.m_OriginX, item.m_OriginY, charinfo.m_OriginX,
                         charinfo.m_OriginY);
        charinfo.m_CharBox =
            CFX_FloatRect(charinfo.m_OriginX, charinfo.m_OriginY,
                          charinfo.m_OriginX, charinfo.m_OriginY);
        m_TempCharList.push_back(charinfo);
      }
      if (item.m_CharCode == CPDF_Font::kInvalidCharCode) {
        continue;
      }
    }
    spacing = 0;
    CFX_WideString wstrItem = pFont->UnicodeFromCharCode(item.m_CharCode);
    FX_BOOL bNoUnicode = FALSE;
    FX_WCHAR wChar = wstrItem.GetAt(0);
    if ((wstrItem.IsEmpty() || wChar == 0) && item.m_CharCode) {
      if (wstrItem.IsEmpty()) {
        wstrItem += (FX_WCHAR)item.m_CharCode;
      } else {
        wstrItem.SetAt(0, (FX_WCHAR)item.m_CharCode);
      }
      bNoUnicode = TRUE;
    }
    charinfo.m_Index = -1;
    charinfo.m_CharCode = item.m_CharCode;
    if (bNoUnicode) {
      charinfo.m_Flag = FPDFTEXT_CHAR_UNUNICODE;
    } else {
      charinfo.m_Flag = FPDFTEXT_CHAR_NORMAL;
    }
    charinfo.m_pTextObj = pTextObj;
    charinfo.m_OriginX = 0, charinfo.m_OriginY = 0;
    matrix.Transform(item.m_OriginX, item.m_OriginY, charinfo.m_OriginX,
                     charinfo.m_OriginY);
    FX_RECT rect =
        charinfo.m_pTextObj->GetFont()->GetCharBBox(charinfo.m_CharCode);
    charinfo.m_CharBox.top =
        rect.top * pTextObj->GetFontSize() / 1000 + item.m_OriginY;
    charinfo.m_CharBox.left =
        rect.left * pTextObj->GetFontSize() / 1000 + item.m_OriginX;
    charinfo.m_CharBox.right =
        rect.right * pTextObj->GetFontSize() / 1000 + item.m_OriginX;
    charinfo.m_CharBox.bottom =
        rect.bottom * pTextObj->GetFontSize() / 1000 + item.m_OriginY;
    if (fabsf(charinfo.m_CharBox.top - charinfo.m_CharBox.bottom) < 0.01f) {
      charinfo.m_CharBox.top =
          charinfo.m_CharBox.bottom + pTextObj->GetFontSize();
    }
    if (fabsf(charinfo.m_CharBox.right - charinfo.m_CharBox.left) < 0.01f) {
      charinfo.m_CharBox.right =
          charinfo.m_CharBox.left + pTextObj->GetCharWidth(charinfo.m_CharCode);
    }
    matrix.TransformRect(charinfo.m_CharBox);
    charinfo.m_Matrix.Copy(matrix);
    if (wstrItem.IsEmpty()) {
      charinfo.m_Unicode = 0;
      m_TempCharList.push_back(charinfo);
      m_TempTextBuf.AppendChar(0xfffe);
      continue;
    } else {
      int nTotal = wstrItem.GetLength();
      FX_BOOL bDel = FALSE;
      const int count =
          std::min(pdfium::CollectionSize<int>(m_TempCharList), 7);
      FX_FLOAT threshold = charinfo.m_Matrix.TransformXDistance(
          (FX_FLOAT)TEXT_CHARRATIO_GAPDELTA * pTextObj->GetFontSize());
      for (int n = pdfium::CollectionSize<int>(m_TempCharList);
           n > pdfium::CollectionSize<int>(m_TempCharList) - count; n--) {
        const PAGECHAR_INFO& charinfo1 = m_TempCharList[n - 1];
        if (charinfo1.m_CharCode == charinfo.m_CharCode &&
            charinfo1.m_pTextObj->GetFont() == charinfo.m_pTextObj->GetFont() &&
            FXSYS_fabs(charinfo1.m_OriginX - charinfo.m_OriginX) < threshold &&
            FXSYS_fabs(charinfo1.m_OriginY - charinfo.m_OriginY) < threshold) {
          bDel = TRUE;
          break;
        }
      }
      if (!bDel) {
        for (int nIndex = 0; nIndex < nTotal; nIndex++) {
          charinfo.m_Unicode = wstrItem.GetAt(nIndex);
          if (charinfo.m_Unicode) {
            charinfo.m_Index = m_TextBuf.GetLength();
            m_TempTextBuf.AppendChar(charinfo.m_Unicode);
          } else {
            m_TempTextBuf.AppendChar(0xfffe);
          }
          m_TempCharList.push_back(charinfo);
        }
      } else if (i == 0) {
        CFX_WideString str = m_TempTextBuf.GetWideString();
        if (!str.IsEmpty() &&
            str.GetAt(str.GetLength() - 1) == TEXT_BLANK_CHAR) {
          m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
          m_TempCharList.pop_back();
        }
      }
    }
  }
  if (bIsBidiAndMirrorInverse) {
    SwapTempTextBuf(iCharListStartAppend, iBufStartAppend);
  }
}

int32_t CPDF_TextPage::GetTextObjectWritingMode(
    const CPDF_TextObject* pTextObj) {
  int32_t nChars = pTextObj->CountChars();
  if (nChars == 1) {
    return m_TextlineDir;
  }
  CPDF_TextObjectItem first, last;
  pTextObj->GetCharInfo(0, &first);
  pTextObj->GetCharInfo(nChars - 1, &last);
  CFX_Matrix textMatrix;
  pTextObj->GetTextMatrix(&textMatrix);
  textMatrix.TransformPoint(first.m_OriginX, first.m_OriginY);
  textMatrix.TransformPoint(last.m_OriginX, last.m_OriginY);
  FX_FLOAT dX = FXSYS_fabs(last.m_OriginX - first.m_OriginX);
  FX_FLOAT dY = FXSYS_fabs(last.m_OriginY - first.m_OriginY);
  if (dX <= 0.0001f && dY <= 0.0001f) {
    return -1;
  }
  CFX_VectorF v(dX, dY);
  v.Normalize();
  if (v.y <= 0.0872f) {
    return v.x <= 0.0872f ? m_TextlineDir : 0;
  }
  if (v.x <= 0.0872f) {
    return 1;
  }
  return m_TextlineDir;
}
FX_BOOL CPDF_TextPage::IsHyphen(FX_WCHAR curChar) {
  CFX_WideString strCurText = m_TempTextBuf.GetWideString();
  if (strCurText.GetLength() == 0) {
    strCurText = m_TextBuf.GetWideString();
  }
  FX_STRSIZE nCount = strCurText.GetLength();
  int nIndex = nCount - 1;
  FX_WCHAR wcTmp = strCurText.GetAt(nIndex);
  while (wcTmp == 0x20 && nIndex <= nCount - 1 && nIndex >= 0) {
    wcTmp = strCurText.GetAt(--nIndex);
  }
  if (0x2D == wcTmp || 0xAD == wcTmp) {
    if (--nIndex > 0) {
      FX_WCHAR preChar = strCurText.GetAt((nIndex));
      if (((preChar >= L'A' && preChar <= L'Z') ||
           (preChar >= L'a' && preChar <= L'z')) &&
          ((curChar >= L'A' && curChar <= L'Z') ||
           (curChar >= L'a' && curChar <= L'z'))) {
        return TRUE;
      }
    }
    const PAGECHAR_INFO* preInfo;
    if (!m_TempCharList.empty()) {
      preInfo = &m_TempCharList.back();
    } else if (!m_CharList.empty()) {
      preInfo = &m_CharList.back();
    } else {
      return FALSE;
    }
    if (FPDFTEXT_CHAR_PIECE == preInfo->m_Flag &&
        (0xAD == preInfo->m_Unicode || 0x2D == preInfo->m_Unicode)) {
      return TRUE;
    }
  }
  return FALSE;
}

int CPDF_TextPage::ProcessInsertObject(const CPDF_TextObject* pObj,
                                       const CFX_Matrix& formMatrix) {
  FindPreviousTextObject();
  FX_BOOL bNewline = FALSE;
  int WritingMode = GetTextObjectWritingMode(pObj);
  if (WritingMode == -1) {
    WritingMode = GetTextObjectWritingMode(m_pPreTextObj);
  }
  CFX_FloatRect this_rect(pObj->m_Left, pObj->m_Bottom, pObj->m_Right,
                          pObj->m_Top);
  CFX_FloatRect prev_rect(m_pPreTextObj->m_Left, m_pPreTextObj->m_Bottom,
                          m_pPreTextObj->m_Right, m_pPreTextObj->m_Top);
  CPDF_TextObjectItem PrevItem, item;
  int nItem = m_pPreTextObj->CountItems();
  m_pPreTextObj->GetItemInfo(nItem - 1, &PrevItem);
  pObj->GetItemInfo(0, &item);
  CFX_WideString wstrItem =
      pObj->GetFont()->UnicodeFromCharCode(item.m_CharCode);
  if (wstrItem.IsEmpty()) {
    wstrItem += (FX_WCHAR)item.m_CharCode;
  }
  FX_WCHAR curChar = wstrItem.GetAt(0);
  if (WritingMode == 0) {
    if (this_rect.Height() > 4.5 && prev_rect.Height() > 4.5) {
      FX_FLOAT top =
          this_rect.top < prev_rect.top ? this_rect.top : prev_rect.top;
      FX_FLOAT bottom = this_rect.bottom > prev_rect.bottom ? this_rect.bottom
                                                            : prev_rect.bottom;
      if (bottom >= top) {
        if (IsHyphen(curChar)) {
          return 3;
        }
        return 2;
      }
    }
  } else if (WritingMode == 1) {
    if (this_rect.Width() > pObj->GetFontSize() * 0.1f &&
        prev_rect.Width() > m_pPreTextObj->GetFontSize() * 0.1f) {
      FX_FLOAT left = this_rect.left > m_CurlineRect.left ? this_rect.left
                                                          : m_CurlineRect.left;
      FX_FLOAT right = this_rect.right < m_CurlineRect.right
                           ? this_rect.right
                           : m_CurlineRect.right;
      if (right <= left) {
        if (IsHyphen(curChar)) {
          return 3;
        }
        return 2;
      }
    }
  }
  FX_FLOAT last_pos = PrevItem.m_OriginX;
  int nLastWidth = GetCharWidth(PrevItem.m_CharCode, m_pPreTextObj->GetFont());
  FX_FLOAT last_width = nLastWidth * m_pPreTextObj->GetFontSize() / 1000;
  last_width = FXSYS_fabs(last_width);
  int nThisWidth = GetCharWidth(item.m_CharCode, pObj->GetFont());
  FX_FLOAT this_width = nThisWidth * pObj->GetFontSize() / 1000;
  this_width = FXSYS_fabs(this_width);
  FX_FLOAT threshold =
      last_width > this_width ? last_width / 4 : this_width / 4;
  CFX_Matrix prev_matrix, prev_reverse;
  m_pPreTextObj->GetTextMatrix(&prev_matrix);
  prev_matrix.Concat(m_perMatrix);
  prev_reverse.SetReverse(prev_matrix);
  FX_FLOAT x = pObj->GetPosX();
  FX_FLOAT y = pObj->GetPosY();
  formMatrix.Transform(x, y);
  prev_reverse.Transform(x, y);
  if (last_width < this_width) {
    threshold = prev_reverse.TransformDistance(threshold);
  }
  CFX_FloatRect rect1(m_pPreTextObj->m_Left, pObj->m_Bottom,
                      m_pPreTextObj->m_Right, pObj->m_Top);
  CFX_FloatRect rect2(m_pPreTextObj->m_Left, m_pPreTextObj->m_Bottom,
                      m_pPreTextObj->m_Right, m_pPreTextObj->m_Top);
  CFX_FloatRect rect3 = rect1;
  rect1.Intersect(rect2);
  if (WritingMode == 0) {
    if ((rect1.IsEmpty() && rect2.Height() > 5 && rect3.Height() > 5) ||
        ((y > threshold * 2 || y < threshold * -3) &&
         (FXSYS_fabs(y) < 1 ? FXSYS_fabs(x) < FXSYS_fabs(y) : TRUE))) {
      bNewline = TRUE;
      if (nItem > 1) {
        CPDF_TextObjectItem tempItem;
        m_pPreTextObj->GetItemInfo(0, &tempItem);
        CFX_Matrix m;
        m_pPreTextObj->GetTextMatrix(&m);
        if (PrevItem.m_OriginX > tempItem.m_OriginX &&
            m_DisplayMatrix.a > 0.9 && m_DisplayMatrix.b < 0.1 &&
            m_DisplayMatrix.c < 0.1 && m_DisplayMatrix.d < -0.9 && m.b < 0.1 &&
            m.c < 0.1) {
          CFX_FloatRect re(0, m_pPreTextObj->m_Bottom, 1000,
                           m_pPreTextObj->m_Top);
          if (re.Contains(pObj->GetPosX(), pObj->GetPosY())) {
            bNewline = FALSE;
          } else {
            CFX_FloatRect re(0, pObj->m_Bottom, 1000, pObj->m_Top);
            if (re.Contains(m_pPreTextObj->GetPosX(),
                            m_pPreTextObj->GetPosY())) {
              bNewline = FALSE;
            }
          }
        }
      }
    }
  }
  if (bNewline)
    return IsHyphen(curChar) ? 3 : 2;

  int32_t nChars = pObj->CountChars();
  if (nChars == 1 && (0x2D == curChar || 0xAD == curChar) &&
      IsHyphen(curChar)) {
    return 3;
  }
  CFX_WideString PrevStr =
      m_pPreTextObj->GetFont()->UnicodeFromCharCode(PrevItem.m_CharCode);
  FX_WCHAR preChar = PrevStr.GetAt(PrevStr.GetLength() - 1);
  CFX_Matrix matrix;
  pObj->GetTextMatrix(&matrix);
  matrix.Concat(formMatrix);
  threshold = (FX_FLOAT)(nLastWidth > nThisWidth ? nLastWidth : nThisWidth);
  threshold = threshold > 400
                  ? (threshold < 700
                         ? threshold / 4
                         : (threshold > 800 ? threshold / 6 : threshold / 5))
                  : (threshold / 2);
  if (nLastWidth >= nThisWidth) {
    threshold *= FXSYS_fabs(m_pPreTextObj->GetFontSize());
  } else {
    threshold *= FXSYS_fabs(pObj->GetFontSize());
    threshold = matrix.TransformDistance(threshold);
    threshold = prev_reverse.TransformDistance(threshold);
  }
  threshold /= 1000;
  if ((threshold < 1.4881 && threshold > 1.4879) ||
      (threshold < 1.39001 && threshold > 1.38999)) {
    threshold *= 1.5;
  }
  if (FXSYS_fabs(last_pos + last_width - x) > threshold && curChar != L' ' &&
      preChar != L' ') {
    if (curChar != L' ' && preChar != L' ') {
      if ((x - last_pos - last_width) > threshold ||
          (last_pos - x - last_width) > threshold) {
        return 1;
      }
      if (x < 0 && (last_pos - x - last_width) > threshold) {
        return 1;
      }
      if ((x - last_pos - last_width) > this_width ||
          (x - last_pos - this_width) > last_width) {
        return 1;
      }
    }
  }
  return 0;
}

FX_BOOL CPDF_TextPage::IsSameTextObject(CPDF_TextObject* pTextObj1,
                                        CPDF_TextObject* pTextObj2) {
  if (!pTextObj1 || !pTextObj2) {
    return FALSE;
  }
  CFX_FloatRect rcPreObj(pTextObj2->m_Left, pTextObj2->m_Bottom,
                         pTextObj2->m_Right, pTextObj2->m_Top);
  CFX_FloatRect rcCurObj(pTextObj1->m_Left, pTextObj1->m_Bottom,
                         pTextObj1->m_Right, pTextObj1->m_Top);
  if (rcPreObj.IsEmpty() && rcCurObj.IsEmpty()) {
    FX_FLOAT dbXdif = FXSYS_fabs(rcPreObj.left - rcCurObj.left);
    size_t nCount = m_CharList.size();
    if (nCount >= 2) {
      PAGECHAR_INFO perCharTemp = m_CharList[nCount - 2];
      FX_FLOAT dbSpace = perCharTemp.m_CharBox.Width();
      if (dbXdif > dbSpace) {
        return FALSE;
      }
    }
  }
  if (!rcPreObj.IsEmpty() || !rcCurObj.IsEmpty()) {
    rcPreObj.Intersect(rcCurObj);
    if (rcPreObj.IsEmpty()) {
      return FALSE;
    }
    if (FXSYS_fabs(rcPreObj.Width() - rcCurObj.Width()) >
        rcCurObj.Width() / 2) {
      return FALSE;
    }
    if (pTextObj2->GetFontSize() != pTextObj1->GetFontSize()) {
      return FALSE;
    }
  }
  int nPreCount = pTextObj2->CountItems();
  int nCurCount = pTextObj1->CountItems();
  if (nPreCount != nCurCount) {
    return FALSE;
  }
  // If both objects have no items, consider them same.
  if (!nPreCount)
    return TRUE;

  CPDF_TextObjectItem itemPer = {0, 0.0f, 0.0f};
  CPDF_TextObjectItem itemCur = {0, 0.0f, 0.0f};
  for (int i = 0; i < nPreCount; i++) {
    pTextObj2->GetItemInfo(i, &itemPer);
    pTextObj1->GetItemInfo(i, &itemCur);
    if (itemCur.m_CharCode != itemPer.m_CharCode) {
      return FALSE;
    }
  }
  if (FXSYS_fabs(pTextObj1->GetPosX() - pTextObj2->GetPosX()) >
          GetCharWidth(itemPer.m_CharCode, pTextObj2->GetFont()) *
              pTextObj2->GetFontSize() / 1000 * 0.9 ||
      FXSYS_fabs(pTextObj1->GetPosY() - pTextObj2->GetPosY()) >
          std::max(std::max(rcPreObj.Height(), rcPreObj.Width()),
                   pTextObj2->GetFontSize()) /
              8) {
    return FALSE;
  }
  return TRUE;
}

FX_BOOL CPDF_TextPage::IsSameAsPreTextObject(
    CPDF_TextObject* pTextObj,
    const CPDF_PageObjectList* pObjList,
    CPDF_PageObjectList::const_iterator iter) {
  int i = 0;
  while (i < 5 && iter != pObjList->begin()) {
    --iter;
    CPDF_PageObject* pOtherObj = iter->get();
    if (pOtherObj == pTextObj || !pOtherObj->IsText())
      continue;
    if (IsSameTextObject(pOtherObj->AsText(), pTextObj))
      return TRUE;
    ++i;
  }
  return FALSE;
}

FX_BOOL CPDF_TextPage::GenerateCharInfo(FX_WCHAR unicode, PAGECHAR_INFO& info) {
  const PAGECHAR_INFO* preChar;
  if (!m_TempCharList.empty()) {
    preChar = &m_TempCharList.back();
  } else if (!m_CharList.empty()) {
    preChar = &m_CharList.back();
  } else {
    return FALSE;
  }
  info.m_Index = m_TextBuf.GetLength();
  info.m_Unicode = unicode;
  info.m_pTextObj = NULL;
  info.m_CharCode = CPDF_Font::kInvalidCharCode;
  info.m_Flag = FPDFTEXT_CHAR_GENERATED;
  int preWidth = 0;
  if (preChar->m_pTextObj && preChar->m_CharCode != -1) {
    preWidth =
        GetCharWidth(preChar->m_CharCode, preChar->m_pTextObj->GetFont());
  }

  FX_FLOAT fFontSize = preChar->m_pTextObj ? preChar->m_pTextObj->GetFontSize()
                                           : preChar->m_CharBox.Height();
  if (!fFontSize)
    fFontSize = kDefaultFontSize;

  info.m_OriginX = preChar->m_OriginX + preWidth * (fFontSize) / 1000;
  info.m_OriginY = preChar->m_OriginY;
  info.m_CharBox = CFX_FloatRect(info.m_OriginX, info.m_OriginY, info.m_OriginX,
                                 info.m_OriginY);
  return TRUE;
}

FX_BOOL CPDF_TextPage::IsRectIntersect(const CFX_FloatRect& rect1,
                                       const CFX_FloatRect& rect2) {
  CFX_FloatRect rect = rect1;
  rect.Intersect(rect2);
  return !rect.IsEmpty();
}

FX_BOOL CPDF_TextPage::IsLetter(FX_WCHAR unicode) {
  if (unicode < L'A') {
    return FALSE;
  }
  if (unicode > L'Z' && unicode < L'a') {
    return FALSE;
  }
  if (unicode > L'z') {
    return FALSE;
  }
  return TRUE;
}

CPDF_TextPageFind::CPDF_TextPageFind(const IPDF_TextPage* pTextPage)
    : m_pTextPage(pTextPage),
      m_flags(0),
      m_findNextStart(-1),
      m_findPreStart(-1),
      m_bMatchCase(FALSE),
      m_bMatchWholeWord(FALSE),
      m_resStart(0),
      m_resEnd(-1),
      m_IsFind(FALSE) {
  m_strText = m_pTextPage->GetPageText();
  int nCount = pTextPage->CountChars();
  if (nCount) {
    m_CharIndex.push_back(0);
  }
  for (int i = 0; i < nCount; i++) {
    FPDF_CHAR_INFO info;
    pTextPage->GetCharInfo(i, &info);
    int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
    if (info.m_Flag == FPDFTEXT_CHAR_NORMAL ||
        info.m_Flag == FPDFTEXT_CHAR_GENERATED) {
      if (indexSize % 2) {
        m_CharIndex.push_back(1);
      } else {
        if (indexSize <= 0) {
          continue;
        }
        m_CharIndex[indexSize - 1] += 1;
      }
    } else {
      if (indexSize % 2) {
        if (indexSize <= 0) {
          continue;
        }
        m_CharIndex[indexSize - 1] = i + 1;
      } else {
        m_CharIndex.push_back(i + 1);
      }
    }
  }
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  if (indexSize % 2) {
    m_CharIndex.erase(m_CharIndex.begin() + indexSize - 1);
  }
}

int CPDF_TextPageFind::GetCharIndex(int index) const {
  return m_pTextPage->CharIndexFromTextIndex(index);
}

FX_BOOL CPDF_TextPageFind::FindFirst(const CFX_WideString& findwhat,
                                     int flags,
                                     int startPos) {
  if (!m_pTextPage) {
    return FALSE;
  }
  if (m_strText.IsEmpty() || m_bMatchCase != (flags & FPDFTEXT_MATCHCASE)) {
    m_strText = m_pTextPage->GetPageText();
  }
  CFX_WideString findwhatStr = findwhat;
  m_findWhat = findwhatStr;
  m_flags = flags;
  m_bMatchCase = flags & FPDFTEXT_MATCHCASE;
  if (m_strText.IsEmpty()) {
    m_IsFind = FALSE;
    return TRUE;
  }
  FX_STRSIZE len = findwhatStr.GetLength();
  if (!m_bMatchCase) {
    findwhatStr.MakeLower();
    m_strText.MakeLower();
  }
  m_bMatchWholeWord = flags & FPDFTEXT_MATCHWHOLEWORD;
  m_findNextStart = startPos;
  if (startPos == -1) {
    m_findPreStart = m_strText.GetLength() - 1;
  } else {
    m_findPreStart = startPos;
  }
  m_csFindWhatArray.clear();
  int i = 0;
  while (i < len) {
    if (findwhatStr.GetAt(i) != ' ') {
      break;
    }
    i++;
  }
  if (i < len) {
    ExtractFindWhat(findwhatStr);
  } else {
    m_csFindWhatArray.push_back(findwhatStr);
  }
  if (m_csFindWhatArray.empty()) {
    return FALSE;
  }
  m_IsFind = TRUE;
  m_resStart = 0;
  m_resEnd = -1;
  return TRUE;
}

FX_BOOL CPDF_TextPageFind::FindNext() {
  if (!m_pTextPage) {
    return FALSE;
  }
  m_resArray.RemoveAll();
  if (m_findNextStart == -1) {
    return FALSE;
  }
  if (m_strText.IsEmpty()) {
    m_IsFind = FALSE;
    return m_IsFind;
  }
  int strLen = m_strText.GetLength();
  if (m_findNextStart > strLen - 1) {
    m_IsFind = FALSE;
    return m_IsFind;
  }
  int nCount = pdfium::CollectionSize<int>(m_csFindWhatArray);
  int nResultPos = 0;
  int nStartPos = 0;
  nStartPos = m_findNextStart;
  FX_BOOL bSpaceStart = FALSE;
  for (int iWord = 0; iWord < nCount; iWord++) {
    CFX_WideString csWord = m_csFindWhatArray[iWord];
    if (csWord.IsEmpty()) {
      if (iWord == nCount - 1) {
        FX_WCHAR strInsert = m_strText.GetAt(nStartPos);
        if (strInsert == TEXT_LINEFEED_CHAR || strInsert == TEXT_BLANK_CHAR ||
            strInsert == TEXT_RETURN_CHAR || strInsert == 160) {
          nResultPos = nStartPos + 1;
          break;
        }
        iWord = -1;
      } else if (iWord == 0) {
        bSpaceStart = TRUE;
      }
      continue;
    }
    int endIndex;
    nResultPos = m_strText.Find(csWord.c_str(), nStartPos);
    if (nResultPos == -1) {
      m_IsFind = FALSE;
      return m_IsFind;
    }
    endIndex = nResultPos + csWord.GetLength() - 1;
    if (iWord == 0) {
      m_resStart = nResultPos;
    }
    FX_BOOL bMatch = TRUE;
    if (iWord != 0 && !bSpaceStart) {
      int PreResEndPos = nStartPos;
      int curChar = csWord.GetAt(0);
      CFX_WideString lastWord = m_csFindWhatArray[iWord - 1];
      int lastChar = lastWord.GetAt(lastWord.GetLength() - 1);
      if (nStartPos == nResultPos &&
          !(_IsIgnoreSpaceCharacter(lastChar) ||
            _IsIgnoreSpaceCharacter(curChar))) {
        bMatch = FALSE;
      }
      for (int d = PreResEndPos; d < nResultPos; d++) {
        FX_WCHAR strInsert = m_strText.GetAt(d);
        if (strInsert != TEXT_LINEFEED_CHAR && strInsert != TEXT_BLANK_CHAR &&
            strInsert != TEXT_RETURN_CHAR && strInsert != 160) {
          bMatch = FALSE;
          break;
        }
      }
    } else if (bSpaceStart) {
      if (nResultPos > 0) {
        FX_WCHAR strInsert = m_strText.GetAt(nResultPos - 1);
        if (strInsert != TEXT_LINEFEED_CHAR && strInsert != TEXT_BLANK_CHAR &&
            strInsert != TEXT_RETURN_CHAR && strInsert != 160) {
          bMatch = FALSE;
          m_resStart = nResultPos;
        } else {
          m_resStart = nResultPos - 1;
        }
      }
    }
    if (m_bMatchWholeWord && bMatch) {
      bMatch = IsMatchWholeWord(m_strText, nResultPos, endIndex);
    }
    nStartPos = endIndex + 1;
    if (!bMatch) {
      iWord = -1;
      if (bSpaceStart) {
        nStartPos = m_resStart + m_csFindWhatArray[1].GetLength();
      } else {
        nStartPos = m_resStart + m_csFindWhatArray[0].GetLength();
      }
    }
  }
  m_resEnd = nResultPos + m_csFindWhatArray.back().GetLength() - 1;
  m_IsFind = TRUE;
  int resStart = GetCharIndex(m_resStart);
  int resEnd = GetCharIndex(m_resEnd);
  m_pTextPage->GetRectArray(resStart, resEnd - resStart + 1, m_resArray);
  if (m_flags & FPDFTEXT_CONSECUTIVE) {
    m_findNextStart = m_resStart + 1;
    m_findPreStart = m_resEnd - 1;
  } else {
    m_findNextStart = m_resEnd + 1;
    m_findPreStart = m_resStart - 1;
  }
  return m_IsFind;
}

FX_BOOL CPDF_TextPageFind::FindPrev() {
  if (!m_pTextPage) {
    return FALSE;
  }
  m_resArray.RemoveAll();
  if (m_strText.IsEmpty() || m_findPreStart < 0) {
    m_IsFind = FALSE;
    return m_IsFind;
  }
  CPDF_TextPageFind findEngine(m_pTextPage);
  FX_BOOL ret = findEngine.FindFirst(m_findWhat, m_flags);
  if (!ret) {
    m_IsFind = FALSE;
    return m_IsFind;
  }
  int order = -1, MatchedCount = 0;
  while (ret) {
    ret = findEngine.FindNext();
    if (ret) {
      int order1 = findEngine.GetCurOrder();
      int MatchedCount1 = findEngine.GetMatchedCount();
      if (((order1 + MatchedCount1) - 1) > m_findPreStart) {
        break;
      }
      order = order1;
      MatchedCount = MatchedCount1;
    }
  }
  if (order == -1) {
    m_IsFind = FALSE;
    return m_IsFind;
  }
  m_resStart = m_pTextPage->TextIndexFromCharIndex(order);
  m_resEnd = m_pTextPage->TextIndexFromCharIndex(order + MatchedCount - 1);
  m_IsFind = TRUE;
  m_pTextPage->GetRectArray(order, MatchedCount, m_resArray);
  if (m_flags & FPDFTEXT_CONSECUTIVE) {
    m_findNextStart = m_resStart + 1;
    m_findPreStart = m_resEnd - 1;
  } else {
    m_findNextStart = m_resEnd + 1;
    m_findPreStart = m_resStart - 1;
  }
  return m_IsFind;
}

void CPDF_TextPageFind::ExtractFindWhat(const CFX_WideString& findwhat) {
  if (findwhat.IsEmpty()) {
    return;
  }
  int index = 0;
  while (1) {
    CFX_WideString csWord = TEXT_EMPTY;
    int ret =
        ExtractSubString(csWord, findwhat.c_str(), index, TEXT_BLANK_CHAR);
    if (csWord.IsEmpty()) {
      if (ret) {
        m_csFindWhatArray.push_back(L"");
        index++;
        continue;
      } else {
        break;
      }
    }
    int pos = 0;
    while (pos < csWord.GetLength()) {
      CFX_WideString curStr = csWord.Mid(pos, 1);
      FX_WCHAR curChar = csWord.GetAt(pos);
      if (_IsIgnoreSpaceCharacter(curChar)) {
        if (pos > 0 && curChar == 0x2019) {
          pos++;
          continue;
        }
        if (pos > 0) {
          m_csFindWhatArray.push_back(csWord.Mid(0, pos));
        }
        m_csFindWhatArray.push_back(curStr);
        if (pos == csWord.GetLength() - 1) {
          csWord.Empty();
          break;
        }
        csWord = csWord.Right(csWord.GetLength() - pos - 1);
        pos = 0;
        continue;
      }
      pos++;
    }
    if (!csWord.IsEmpty()) {
      m_csFindWhatArray.push_back(csWord);
    }
    index++;
  }
}

FX_BOOL CPDF_TextPageFind::IsMatchWholeWord(const CFX_WideString& csPageText,
                                            int startPos,
                                            int endPos) {
  FX_WCHAR char_left = 0;
  FX_WCHAR char_right = 0;
  int char_count = endPos - startPos + 1;
  if (char_count < 1) {
    return FALSE;
  }
  if (char_count == 1 && csPageText.GetAt(startPos) > 255) {
    return TRUE;
  }
  if (startPos - 1 >= 0) {
    char_left = csPageText.GetAt(startPos - 1);
  }
  if (startPos + char_count < csPageText.GetLength()) {
    char_right = csPageText.GetAt(startPos + char_count);
  }
  if ((char_left > 'A' && char_left < 'a') ||
      (char_left > 'a' && char_left < 'z') ||
      (char_left > 0xfb00 && char_left < 0xfb06) || std::iswdigit(char_left) ||
      (char_right > 'A' && char_right < 'a') ||
      (char_right > 'a' && char_right < 'z') ||
      (char_right > 0xfb00 && char_right < 0xfb06) ||
      std::iswdigit(char_right)) {
    return FALSE;
  }
  if (!(('A' > char_left || char_left > 'Z') &&
        ('a' > char_left || char_left > 'z') &&
        ('A' > char_right || char_right > 'Z') &&
        ('a' > char_right || char_right > 'z'))) {
    return FALSE;
  }
  if (char_count > 0) {
    if (csPageText.GetAt(startPos) >= L'0' &&
        csPageText.GetAt(startPos) <= L'9' && char_left >= L'0' &&
        char_left <= L'9') {
      return FALSE;
    }
    if (csPageText.GetAt(endPos) >= L'0' && csPageText.GetAt(endPos) <= L'9' &&
        char_right >= L'0' && char_right <= L'9') {
      return FALSE;
    }
  }
  return TRUE;
}

FX_BOOL CPDF_TextPageFind::ExtractSubString(CFX_WideString& rString,
                                            const FX_WCHAR* lpszFullString,
                                            int iSubString,
                                            FX_WCHAR chSep) {
  if (!lpszFullString) {
    return FALSE;
  }
  while (iSubString--) {
    lpszFullString = FXSYS_wcschr(lpszFullString, chSep);
    if (!lpszFullString) {
      rString.Empty();
      return FALSE;
    }
    lpszFullString++;
    while (*lpszFullString == chSep) {
      lpszFullString++;
    }
  }
  const FX_WCHAR* lpchEnd = FXSYS_wcschr(lpszFullString, chSep);
  int nLen = lpchEnd ? (int)(lpchEnd - lpszFullString)
                     : (int)FXSYS_wcslen(lpszFullString);
  ASSERT(nLen >= 0);
  FXSYS_memcpy(rString.GetBuffer(nLen), lpszFullString,
               nLen * sizeof(FX_WCHAR));
  rString.ReleaseBuffer();
  return TRUE;
}

CFX_WideString CPDF_TextPageFind::MakeReverse(const CFX_WideString& str) {
  CFX_WideString str2;
  str2.Empty();
  int nlen = str.GetLength();
  for (int i = nlen - 1; i >= 0; i--) {
    str2 += str.GetAt(i);
  }
  return str2;
}

void CPDF_TextPageFind::GetRectArray(CFX_RectArray& rects) const {
  rects.Copy(m_resArray);
}

int CPDF_TextPageFind::GetCurOrder() const {
  return GetCharIndex(m_resStart);
}

int CPDF_TextPageFind::GetMatchedCount() const {
  int resStart = GetCharIndex(m_resStart);
  int resEnd = GetCharIndex(m_resEnd);
  return resEnd - resStart + 1;
}

CPDF_LinkExtract::CPDF_LinkExtract()
    : m_pTextPage(nullptr), m_bIsParsed(false) {}

CPDF_LinkExtract::~CPDF_LinkExtract() {
  DeleteLinkList();
}

FX_BOOL CPDF_LinkExtract::ExtractLinks(const IPDF_TextPage* pTextPage) {
  if (!pTextPage || !pTextPage->IsParsed())
    return FALSE;

  m_pTextPage = (const CPDF_TextPage*)pTextPage;
  m_strPageText = m_pTextPage->GetPageText(0, -1);
  DeleteLinkList();
  if (m_strPageText.IsEmpty()) {
    return FALSE;
  }
  ParseLink();
  m_bIsParsed = true;
  return TRUE;
}

void CPDF_LinkExtract::DeleteLinkList() {
  while (m_LinkList.GetSize()) {
    CPDF_LinkExt* linkinfo = NULL;
    linkinfo = m_LinkList.GetAt(0);
    m_LinkList.RemoveAt(0);
    delete linkinfo;
  }
  m_LinkList.RemoveAll();
}

int CPDF_LinkExtract::CountLinks() const {
  if (!m_bIsParsed) {
    return -1;
  }
  return m_LinkList.GetSize();
}

void CPDF_LinkExtract::ParseLink() {
  int start = 0, pos = 0;
  int TotalChar = m_pTextPage->CountChars();
  while (pos < TotalChar) {
    FPDF_CHAR_INFO pageChar;
    m_pTextPage->GetCharInfo(pos, &pageChar);
    if (pageChar.m_Flag == FPDFTEXT_CHAR_GENERATED ||
        pageChar.m_Unicode == 0x20 || pos == TotalChar - 1) {
      int nCount = pos - start;
      if (pos == TotalChar - 1) {
        nCount++;
      }
      CFX_WideString strBeCheck;
      strBeCheck = m_pTextPage->GetPageText(start, nCount);
      if (strBeCheck.GetLength() > 5) {
        while (strBeCheck.GetLength() > 0) {
          FX_WCHAR ch = strBeCheck.GetAt(strBeCheck.GetLength() - 1);
          if (ch == L')' || ch == L',' || ch == L'>' || ch == L'.') {
            strBeCheck = strBeCheck.Mid(0, strBeCheck.GetLength() - 1);
            nCount--;
          } else {
            break;
          }
        }
        if (nCount > 5 &&
            (CheckWebLink(strBeCheck) || CheckMailLink(strBeCheck))) {
          AppendToLinkList(start, nCount, strBeCheck);
        }
      }
      start = ++pos;
    } else {
      pos++;
    }
  }
}

FX_BOOL CPDF_LinkExtract::CheckWebLink(CFX_WideString& strBeCheck) {
  CFX_WideString str = strBeCheck;
  str.MakeLower();
  if (str.Find(L"http://www.") != -1) {
    strBeCheck = strBeCheck.Right(str.GetLength() - str.Find(L"http://www."));
    return TRUE;
  }
  if (str.Find(L"http://") != -1) {
    strBeCheck = strBeCheck.Right(str.GetLength() - str.Find(L"http://"));
    return TRUE;
  }
  if (str.Find(L"https://www.") != -1) {
    strBeCheck = strBeCheck.Right(str.GetLength() - str.Find(L"https://www."));
    return TRUE;
  }
  if (str.Find(L"https://") != -1) {
    strBeCheck = strBeCheck.Right(str.GetLength() - str.Find(L"https://"));
    return TRUE;
  }
  if (str.Find(L"www.") != -1) {
    strBeCheck = strBeCheck.Right(str.GetLength() - str.Find(L"www."));
    strBeCheck = L"http://" + strBeCheck;
    return TRUE;
  }
  return FALSE;
}

bool CPDF_LinkExtract::CheckMailLink(CFX_WideString& str) {
  int aPos = str.Find(L'@');
  // Invalid when no '@'.
  if (aPos < 1) {
    return FALSE;
  }

  // Check the local part.
  int pPos = aPos;  // Used to track the position of '@' or '.'.
  for (int i = aPos - 1; i >= 0; i--) {
    FX_WCHAR ch = str.GetAt(i);
    if (ch == L'_' || ch == L'-' || FXSYS_iswalnum(ch)) {
      continue;
    }
    if (ch != L'.' || i == pPos - 1 || i == 0) {
      if (i == aPos - 1) {
        // There is '.' or invalid char before '@'.
        return FALSE;
      }
      // End extracting for other invalid chars, '.' at the beginning, or
      // consecutive '.'.
      int removed_len = i == pPos - 1 ? i + 2 : i + 1;
      str = str.Right(str.GetLength() - removed_len);
      break;
    }
    // Found a valid '.'.
    pPos = i;
  }

  // Check the domain name part.
  aPos = str.Find(L'@');
  if (aPos < 1) {
    return FALSE;
  }
  str.TrimRight(L'.');
  // At least one '.' in domain name, but not at the beginning.
  // TODO(weili): RFC5322 allows domain names to be a local name without '.'.
  // Check whether we should remove this check.
  int ePos = str.Find(L'.', aPos + 1);
  if (ePos == -1 || ePos == aPos + 1) {
    return FALSE;
  }
  // Validate all other chars in domain name.
  int nLen = str.GetLength();
  pPos = 0;  // Used to track the position of '.'.
  for (int i = aPos + 1; i < nLen; i++) {
    FX_WCHAR wch = str.GetAt(i);
    if (wch == L'-' || FXSYS_iswalnum(wch)) {
      continue;
    }
    if (wch != L'.' || i == pPos + 1) {
      // Domain name should end before invalid char.
      int host_end = i == pPos + 1 ? i - 2 : i - 1;
      if (pPos > 0 && host_end - aPos >= 3) {
        // Trim the ending invalid chars if there is at least one '.' and name.
        str = str.Left(host_end + 1);
        break;
      }
      return FALSE;
    }
    pPos = i;
  }

  if (str.Find(L"mailto:") == -1) {
    str = L"mailto:" + str;
  }
  return TRUE;
}

void CPDF_LinkExtract::AppendToLinkList(int start,
                                        int count,
                                        const CFX_WideString& strUrl) {
  CPDF_LinkExt* linkInfo = new CPDF_LinkExt;
  linkInfo->m_strUrl = strUrl;
  linkInfo->m_Start = start;
  linkInfo->m_Count = count;
  m_LinkList.Add(linkInfo);
}

CFX_WideString CPDF_LinkExtract::GetURL(int index) const {
  if (!m_bIsParsed || index < 0 || index >= m_LinkList.GetSize()) {
    return L"";
  }
  CPDF_LinkExt* link = NULL;
  link = m_LinkList.GetAt(index);
  if (!link) {
    return L"";
  }
  return link->m_strUrl;
}
void CPDF_LinkExtract::GetBoundedSegment(int index,
                                         int& start,
                                         int& count) const {
  if (!m_bIsParsed || index < 0 || index >= m_LinkList.GetSize()) {
    return;
  }
  CPDF_LinkExt* link = NULL;
  link = m_LinkList.GetAt(index);
  if (!link) {
    return;
  }
  start = link->m_Start;
  count = link->m_Count;
}

void CPDF_LinkExtract::GetRects(int index, CFX_RectArray& rects) const {
  if (!m_bIsParsed || index < 0 || index >= m_LinkList.GetSize()) {
    return;
  }
  CPDF_LinkExt* link = NULL;
  link = m_LinkList.GetAt(index);
  if (!link) {
    return;
  }
  m_pTextPage->GetRectArray(link->m_Start, link->m_Count, rects);
}
