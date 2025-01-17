// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_text.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_page.h"
#include "core/fpdftext/include/ipdf_linkextract.h"
#include "core/fpdftext/include/ipdf_textpage.h"
#include "core/fpdftext/include/ipdf_textpagefind.h"
#include "core/include/fpdfdoc/fpdf_doc.h"
#include "fpdfsdk/include/fsdk_define.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_doc.h"
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_page.h"
#endif  // PDF_ENABLE_XFA

#ifdef _WIN32
#include <tchar.h>
#endif

DLLEXPORT FPDF_TEXTPAGE STDCALL FPDFText_LoadPage(FPDF_PAGE page) {
  CPDF_Page* pPDFPage = CPDFPageFromFPDFPage(page);
  if (!pPDFPage)
    return nullptr;
#ifdef PDF_ENABLE_XFA
  CPDFXFA_Page* pPage = (CPDFXFA_Page*)page;
  CPDFXFA_Document* pDoc = pPage->GetDocument();
  CPDF_ViewerPreferences viewRef(pDoc->GetPDFDoc());
#else  // PDF_ENABLE_XFA
  CPDF_ViewerPreferences viewRef(pPDFPage->m_pDocument);
#endif  // PDF_ENABLE_XFA
  IPDF_TextPage* textpage =
      IPDF_TextPage::CreateTextPage(pPDFPage, viewRef.IsDirectionR2L());
  textpage->ParseTextPage();
  return textpage;
}
DLLEXPORT void STDCALL FPDFText_ClosePage(FPDF_TEXTPAGE text_page) {
  delete (IPDF_TextPage*)text_page;
}
DLLEXPORT int STDCALL FPDFText_CountChars(FPDF_TEXTPAGE text_page) {
  if (!text_page)
    return -1;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;
  return textpage->CountChars();
}

DLLEXPORT unsigned int STDCALL FPDFText_GetUnicode(FPDF_TEXTPAGE text_page,
                                                   int index) {
  if (!text_page)
    return 0;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;

  if (index < 0 || index >= textpage->CountChars())
    return 0;

  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  return charinfo.m_Unicode;
}

DLLEXPORT double STDCALL FPDFText_GetFontSize(FPDF_TEXTPAGE text_page,
                                              int index) {
  if (!text_page)
    return 0;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;

  if (index < 0 || index >= textpage->CountChars())
    return 0;

  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  return charinfo.m_FontSize;
}

DLLEXPORT void STDCALL FPDFText_GetCharBox(FPDF_TEXTPAGE text_page,
                                           int index,
                                           double* left,
                                           double* right,
                                           double* bottom,
                                           double* top) {
  if (!text_page)
    return;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;

  if (index < 0 || index >= textpage->CountChars())
    return;
  FPDF_CHAR_INFO charinfo;
  textpage->GetCharInfo(index, &charinfo);
  *left = charinfo.m_CharBox.left;
  *right = charinfo.m_CharBox.right;
  *bottom = charinfo.m_CharBox.bottom;
  *top = charinfo.m_CharBox.top;
}

// select
DLLEXPORT int STDCALL FPDFText_GetCharIndexAtPos(FPDF_TEXTPAGE text_page,
                                                 double x,
                                                 double y,
                                                 double xTolerance,
                                                 double yTolerance) {
  if (!text_page)
    return -3;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;
  return textpage->GetIndexAtPos((FX_FLOAT)x, (FX_FLOAT)y, (FX_FLOAT)xTolerance,
                                 (FX_FLOAT)yTolerance);
}

DLLEXPORT int STDCALL FPDFText_GetText(FPDF_TEXTPAGE text_page,
                                       int start,
                                       int count,
                                       unsigned short* result) {
  if (!text_page)
    return 0;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;

  if (start >= textpage->CountChars())
    return 0;

  CFX_WideString str = textpage->GetPageText(start, count);
  if (str.GetLength() > count)
    str = str.Left(count);

  CFX_ByteString cbUTF16str = str.UTF16LE_Encode();
  FXSYS_memcpy(result, cbUTF16str.GetBuffer(cbUTF16str.GetLength()),
               cbUTF16str.GetLength());
  cbUTF16str.ReleaseBuffer(cbUTF16str.GetLength());

  return cbUTF16str.GetLength() / sizeof(unsigned short);
}

DLLEXPORT int STDCALL FPDFText_CountRects(FPDF_TEXTPAGE text_page,
                                          int start,
                                          int count) {
  if (!text_page)
    return 0;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;
  return textpage->CountRects(start, count);
}
DLLEXPORT void STDCALL FPDFText_GetRect(FPDF_TEXTPAGE text_page,
                                        int rect_index,
                                        double* left,
                                        double* top,
                                        double* right,
                                        double* bottom) {
  if (!text_page)
    return;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;
  CFX_FloatRect rect;
  textpage->GetRect(rect_index, rect.left, rect.top, rect.right, rect.bottom);
  *left = rect.left;
  *top = rect.top;
  *right = rect.right;
  *bottom = rect.bottom;
}

DLLEXPORT int STDCALL FPDFText_GetBoundedText(FPDF_TEXTPAGE text_page,
                                              double left,
                                              double top,
                                              double right,
                                              double bottom,
                                              unsigned short* buffer,
                                              int buflen) {
  if (!text_page)
    return 0;
  IPDF_TextPage* textpage = (IPDF_TextPage*)text_page;
  CFX_FloatRect rect((FX_FLOAT)left, (FX_FLOAT)bottom, (FX_FLOAT)right,
                     (FX_FLOAT)top);
  CFX_WideString str = textpage->GetTextByRect(rect);

  if (buflen <= 0 || !buffer) {
    return str.GetLength();
  }

  CFX_ByteString cbUTF16Str = str.UTF16LE_Encode();
  int len = cbUTF16Str.GetLength() / sizeof(unsigned short);
  int size = buflen > len ? len : buflen;
  FXSYS_memcpy(buffer, cbUTF16Str.GetBuffer(size * sizeof(unsigned short)),
               size * sizeof(unsigned short));
  cbUTF16Str.ReleaseBuffer(size * sizeof(unsigned short));

  return size;
}

// Search
// -1 for end
DLLEXPORT FPDF_SCHHANDLE STDCALL FPDFText_FindStart(FPDF_TEXTPAGE text_page,
                                                    FPDF_WIDESTRING findwhat,
                                                    unsigned long flags,
                                                    int start_index) {
  if (!text_page)
    return NULL;
  IPDF_TextPageFind* textpageFind = NULL;
  textpageFind = IPDF_TextPageFind::CreatePageFind((IPDF_TextPage*)text_page);
  FX_STRSIZE len = CFX_WideString::WStringLength(findwhat);
  textpageFind->FindFirst(CFX_WideString::FromUTF16LE(findwhat, len), flags,
                          start_index);
  return textpageFind;
}
DLLEXPORT FPDF_BOOL STDCALL FPDFText_FindNext(FPDF_SCHHANDLE handle) {
  if (!handle)
    return FALSE;
  IPDF_TextPageFind* textpageFind = (IPDF_TextPageFind*)handle;
  return textpageFind->FindNext();
}
DLLEXPORT FPDF_BOOL STDCALL FPDFText_FindPrev(FPDF_SCHHANDLE handle) {
  if (!handle)
    return FALSE;
  IPDF_TextPageFind* textpageFind = (IPDF_TextPageFind*)handle;
  return textpageFind->FindPrev();
}
DLLEXPORT int STDCALL FPDFText_GetSchResultIndex(FPDF_SCHHANDLE handle) {
  if (!handle)
    return 0;
  IPDF_TextPageFind* textpageFind = (IPDF_TextPageFind*)handle;
  return textpageFind->GetCurOrder();
}
DLLEXPORT int STDCALL FPDFText_GetSchCount(FPDF_SCHHANDLE handle) {
  if (!handle)
    return 0;
  IPDF_TextPageFind* textpageFind = (IPDF_TextPageFind*)handle;
  return textpageFind->GetMatchedCount();
}
DLLEXPORT void STDCALL FPDFText_FindClose(FPDF_SCHHANDLE handle) {
  if (!handle)
    return;
  IPDF_TextPageFind* textpageFind = (IPDF_TextPageFind*)handle;
  delete textpageFind;
  handle = NULL;
}

// web link
DLLEXPORT FPDF_PAGELINK STDCALL FPDFLink_LoadWebLinks(FPDF_TEXTPAGE text_page) {
  if (!text_page)
    return NULL;
  IPDF_LinkExtract* pageLink = NULL;
  pageLink = IPDF_LinkExtract::CreateLinkExtract();
  pageLink->ExtractLinks((IPDF_TextPage*)text_page);
  return pageLink;
}
DLLEXPORT int STDCALL FPDFLink_CountWebLinks(FPDF_PAGELINK link_page) {
  if (!link_page)
    return 0;
  IPDF_LinkExtract* pageLink = (IPDF_LinkExtract*)link_page;
  return pageLink->CountLinks();
}
DLLEXPORT int STDCALL FPDFLink_GetURL(FPDF_PAGELINK link_page,
                                      int link_index,
                                      unsigned short* buffer,
                                      int buflen) {
  if (!link_page)
    return 0;
  IPDF_LinkExtract* pageLink = (IPDF_LinkExtract*)link_page;
  CFX_WideString url = pageLink->GetURL(link_index);

  CFX_ByteString cbUTF16URL = url.UTF16LE_Encode();
  int len = cbUTF16URL.GetLength() / sizeof(unsigned short);
  if (!buffer || buflen <= 0)
    return len;
  int size = len < buflen ? len : buflen;
  if (size > 0) {
    FXSYS_memcpy(buffer, cbUTF16URL.GetBuffer(size * sizeof(unsigned short)),
                 size * sizeof(unsigned short));
    cbUTF16URL.ReleaseBuffer(size * sizeof(unsigned short));
  }
  return size;
}
DLLEXPORT int STDCALL FPDFLink_CountRects(FPDF_PAGELINK link_page,
                                          int link_index) {
  if (!link_page)
    return 0;
  IPDF_LinkExtract* pageLink = (IPDF_LinkExtract*)link_page;
  CFX_RectArray rectArray;
  pageLink->GetRects(link_index, rectArray);
  return rectArray.GetSize();
}
DLLEXPORT void STDCALL FPDFLink_GetRect(FPDF_PAGELINK link_page,
                                        int link_index,
                                        int rect_index,
                                        double* left,
                                        double* top,
                                        double* right,
                                        double* bottom) {
  if (!link_page)
    return;
  IPDF_LinkExtract* pageLink = (IPDF_LinkExtract*)link_page;
  CFX_RectArray rectArray;
  pageLink->GetRects(link_index, rectArray);
  if (rect_index >= 0 && rect_index < rectArray.GetSize()) {
    CFX_FloatRect rect = rectArray.GetAt(rect_index);
    *left = rect.left;
    *right = rect.right;
    *top = rect.top;
    *bottom = rect.bottom;
  }
}
DLLEXPORT void STDCALL FPDFLink_CloseWebLinks(FPDF_PAGELINK link_page) {
  delete (IPDF_LinkExtract*)link_page;
}
