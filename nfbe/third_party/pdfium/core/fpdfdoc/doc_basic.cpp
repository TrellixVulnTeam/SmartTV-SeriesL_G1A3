// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfdoc/doc_utils.h"
#include "core/include/fpdfdoc/fpdf_doc.h"

namespace {

const int nMaxRecursion = 32;
const FX_CHAR* const g_sZoomModes[] = {"XYZ",  "Fit",   "FitH",  "FitV", "FitR",
                                       "FitB", "FitBH", "FitBV", nullptr};

}  // namespace

int CPDF_Dest::GetPageIndex(CPDF_Document* pDoc) {
  CPDF_Array* pArray = ToArray(m_pObj);
  if (!pArray)
    return 0;

  CPDF_Object* pPage = pArray->GetDirectObjectAt(0);
  if (!pPage)
    return 0;
  if (pPage->IsNumber())
    return pPage->GetInteger();
  if (!pPage->IsDictionary())
    return 0;
  return pDoc->GetPageIndex(pPage->GetObjNum());
}
uint32_t CPDF_Dest::GetPageObjNum() {
  CPDF_Array* pArray = ToArray(m_pObj);
  if (!pArray)
    return 0;

  CPDF_Object* pPage = pArray->GetDirectObjectAt(0);
  if (!pPage)
    return 0;
  if (pPage->IsNumber())
    return pPage->GetInteger();
  if (pPage->IsDictionary())
    return pPage->GetObjNum();
  return 0;
}

int CPDF_Dest::GetZoomMode() {
  CPDF_Array* pArray = ToArray(m_pObj);
  if (!pArray)
    return 0;

  CPDF_Object* pObj = pArray->GetDirectObjectAt(1);
  if (!pObj)
    return 0;

  CFX_ByteString mode = pObj->GetString();
  for (int i = 0; g_sZoomModes[i]; ++i) {
    if (mode == g_sZoomModes[i])
      return i + 1;
  }

  return 0;
}

FX_FLOAT CPDF_Dest::GetParam(int index) {
  CPDF_Array* pArray = ToArray(m_pObj);
  return pArray ? pArray->GetNumberAt(2 + index) : 0;
}
CFX_ByteString CPDF_Dest::GetRemoteName() {
  return m_pObj ? m_pObj->GetString() : CFX_ByteString();
}
CPDF_NameTree::CPDF_NameTree(CPDF_Document* pDoc,
                             const CFX_ByteStringC& category) {
  if (pDoc->GetRoot() && pDoc->GetRoot()->GetDictBy("Names"))
    m_pRoot = pDoc->GetRoot()->GetDictBy("Names")->GetDictBy(category);
  else
    m_pRoot = NULL;
}
static CPDF_Object* SearchNameNode(CPDF_Dictionary* pNode,
                                   const CFX_ByteString& csName,
                                   int& nIndex,
                                   CPDF_Array** ppFind,
                                   int nLevel = 0) {
  if (nLevel > nMaxRecursion) {
    return NULL;
  }
  CPDF_Array* pLimits = pNode->GetArrayBy("Limits");
  if (pLimits) {
    CFX_ByteString csLeft = pLimits->GetStringAt(0);
    CFX_ByteString csRight = pLimits->GetStringAt(1);
    if (csLeft.Compare(csRight.AsByteStringC()) > 0) {
      CFX_ByteString csTmp = csRight;
      csRight = csLeft;
      csLeft = csTmp;
    }
    if (csName.Compare(csLeft.AsByteStringC()) < 0 ||
        csName.Compare(csRight.AsByteStringC()) > 0) {
      return NULL;
    }
  }
  CPDF_Array* pNames = pNode->GetArrayBy("Names");
  if (pNames) {
    uint32_t dwCount = pNames->GetCount() / 2;
    for (uint32_t i = 0; i < dwCount; i++) {
      CFX_ByteString csValue = pNames->GetStringAt(i * 2);
      int32_t iCompare = csValue.Compare(csName.AsByteStringC());
      if (iCompare <= 0) {
        if (ppFind) {
          *ppFind = pNames;
        }
        if (iCompare < 0) {
          continue;
        }
      } else {
        break;
      }
      nIndex += i;
      return pNames->GetDirectObjectAt(i * 2 + 1);
    }
    nIndex += dwCount;
    return NULL;
  }
  CPDF_Array* pKids = pNode->GetArrayBy("Kids");
  if (!pKids) {
    return NULL;
  }
  for (uint32_t i = 0; i < pKids->GetCount(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid) {
      continue;
    }
    CPDF_Object* pFound =
        SearchNameNode(pKid, csName, nIndex, ppFind, nLevel + 1);
    if (pFound) {
      return pFound;
    }
  }
  return NULL;
}
static CPDF_Object* SearchNameNode(CPDF_Dictionary* pNode,
                                   int nIndex,
                                   int& nCurIndex,
                                   CFX_ByteString& csName,
                                   CPDF_Array** ppFind,
                                   int nLevel = 0) {
  if (nLevel > nMaxRecursion) {
    return NULL;
  }
  CPDF_Array* pNames = pNode->GetArrayBy("Names");
  if (pNames) {
    int nCount = pNames->GetCount() / 2;
    if (nIndex >= nCurIndex + nCount) {
      nCurIndex += nCount;
      return NULL;
    }
    if (ppFind) {
      *ppFind = pNames;
    }
    csName = pNames->GetStringAt((nIndex - nCurIndex) * 2);
    return pNames->GetDirectObjectAt((nIndex - nCurIndex) * 2 + 1);
  }
  CPDF_Array* pKids = pNode->GetArrayBy("Kids");
  if (!pKids) {
    return NULL;
  }
  for (uint32_t i = 0; i < pKids->GetCount(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid) {
      continue;
    }
    CPDF_Object* pFound =
        SearchNameNode(pKid, nIndex, nCurIndex, csName, ppFind, nLevel + 1);
    if (pFound) {
      return pFound;
    }
  }
  return NULL;
}
static int CountNames(CPDF_Dictionary* pNode, int nLevel = 0) {
  if (nLevel > nMaxRecursion) {
    return 0;
  }
  CPDF_Array* pNames = pNode->GetArrayBy("Names");
  if (pNames) {
    return pNames->GetCount() / 2;
  }
  CPDF_Array* pKids = pNode->GetArrayBy("Kids");
  if (!pKids) {
    return 0;
  }
  int nCount = 0;
  for (uint32_t i = 0; i < pKids->GetCount(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid) {
      continue;
    }
    nCount += CountNames(pKid, nLevel + 1);
  }
  return nCount;
}
int CPDF_NameTree::GetCount() const {
  if (!m_pRoot) {
    return 0;
  }
  return ::CountNames(m_pRoot);
}
int CPDF_NameTree::GetIndex(const CFX_ByteString& csName) const {
  if (!m_pRoot) {
    return -1;
  }
  int nIndex = 0;
  if (!SearchNameNode(m_pRoot, csName, nIndex, NULL)) {
    return -1;
  }
  return nIndex;
}
CPDF_Object* CPDF_NameTree::LookupValue(int nIndex,
                                        CFX_ByteString& csName) const {
  if (!m_pRoot) {
    return NULL;
  }
  int nCurIndex = 0;
  return SearchNameNode(m_pRoot, nIndex, nCurIndex, csName, NULL);
}
CPDF_Object* CPDF_NameTree::LookupValue(const CFX_ByteString& csName) const {
  if (!m_pRoot) {
    return NULL;
  }
  int nIndex = 0;
  return SearchNameNode(m_pRoot, csName, nIndex, NULL);
}
CPDF_Array* CPDF_NameTree::LookupNamedDest(CPDF_Document* pDoc,
                                           const CFX_ByteStringC& sName) {
  CPDF_Object* pValue = LookupValue(sName);
  if (!pValue) {
    CPDF_Dictionary* pDests = pDoc->GetRoot()->GetDictBy("Dests");
    if (!pDests)
      return nullptr;
    pValue = pDests->GetDirectObjectBy(sName);
  }
  if (!pValue)
    return nullptr;
  if (CPDF_Array* pArray = pValue->AsArray())
    return pArray;
  if (CPDF_Dictionary* pDict = pValue->AsDictionary())
    return pDict->GetArrayBy("D");
  return nullptr;
}

#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_ || \
    _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
static CFX_WideString ChangeSlashToPlatform(const FX_WCHAR* str) {
  CFX_WideString result;
  while (*str) {
    if (*str == '/') {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
      result += ':';
#else
      result += '\\';
#endif
    } else {
      result += *str;
    }
    str++;
  }
  return result;
}

static CFX_WideString ChangeSlashToPDF(const FX_WCHAR* str) {
  CFX_WideString result;
  while (*str) {
    if (*str == '\\' || *str == ':') {
      result += '/';
    } else {
      result += *str;
    }
    str++;
  }
  return result;
}
#endif  // _FXM_PLATFORM_APPLE_ || _FXM_PLATFORM_WINDOWS_

CFX_WideString CPDF_FileSpec::DecodeFileName(const CFX_WideStringC& filepath) {
  if (filepath.GetLength() <= 1)
    return CFX_WideString();

#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  if (filepath.Left(sizeof("/Mac") - 1) == CFX_WideStringC(L"/Mac"))
    return ChangeSlashToPlatform(filepath.raw_str() + 1);
  return ChangeSlashToPlatform(filepath.raw_str());
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  if (filepath.GetAt(0) != '/')
    return ChangeSlashToPlatform(filepath.raw_str());
  if (filepath.GetAt(1) == '/')
    return ChangeSlashToPlatform(filepath.raw_str() + 1);
  if (filepath.GetAt(2) == '/') {
    CFX_WideString result;
    result += filepath.GetAt(1);
    result += ':';
    result += ChangeSlashToPlatform(filepath.raw_str() + 2);
    return result;
  }
  CFX_WideString result;
  result += '\\';
  result += ChangeSlashToPlatform(filepath.raw_str());
  return result;
#else
  return filepath;
#endif
}

bool CPDF_FileSpec::GetFileName(CFX_WideString* csFileName) const {
  if (CPDF_Dictionary* pDict = m_pObj->AsDictionary()) {
    *csFileName = pDict->GetUnicodeTextBy("UF");
    if (csFileName->IsEmpty()) {
      *csFileName = CFX_WideString::FromLocal(pDict->GetStringBy("F"));
    }
    if (pDict->GetStringBy("FS") == "URL")
      return true;
    if (csFileName->IsEmpty()) {
      if (pDict->KeyExist("DOS")) {
        *csFileName = CFX_WideString::FromLocal(pDict->GetStringBy("DOS"));
      } else if (pDict->KeyExist("Mac")) {
        *csFileName = CFX_WideString::FromLocal(pDict->GetStringBy("Mac"));
      } else if (pDict->KeyExist("Unix")) {
        *csFileName = CFX_WideString::FromLocal(pDict->GetStringBy("Unix"));
      } else {
        return false;
      }
    }
  } else if (m_pObj->IsString()) {
    *csFileName = CFX_WideString::FromLocal(m_pObj->GetString());
  } else {
    return false;
  }
  *csFileName = DecodeFileName(csFileName->AsWideStringC());
  return true;
}

CPDF_FileSpec::CPDF_FileSpec() {
  m_pObj = new CPDF_Dictionary;
  m_pObj->AsDictionary()->SetAtName("Type", "Filespec");
}

CFX_WideString CPDF_FileSpec::EncodeFileName(const CFX_WideStringC& filepath) {
  if (filepath.GetLength() <= 1) {
    return CFX_WideString();
  }
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
  if (filepath.GetAt(1) == ':') {
    CFX_WideString result;
    result = '/';
    result += filepath.GetAt(0);
    if (filepath.GetAt(2) != '\\') {
      result += '/';
    }
    result += ChangeSlashToPDF(filepath.raw_str() + 2);
    return result;
  }
  if (filepath.GetAt(0) == '\\' && filepath.GetAt(1) == '\\') {
    return ChangeSlashToPDF(filepath.raw_str() + 1);
  }
  if (filepath.GetAt(0) == '\\') {
    CFX_WideString result;
    result = '/';
    result += ChangeSlashToPDF(filepath.raw_str());
    return result;
  }
  return ChangeSlashToPDF(filepath.raw_str());
#elif _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_
  if (filepath.Left(sizeof("Mac") - 1) == FX_WSTRC(L"Mac")) {
    CFX_WideString result;
    result = '/';
    result += ChangeSlashToPDF(filepath.raw_str());
    return result;
  }
  return ChangeSlashToPDF(filepath.raw_str());
#else
  return filepath;
#endif
}

void CPDF_FileSpec::SetFileName(const CFX_WideStringC& wsFileName) {
  if (!m_pObj)
    return;

  CFX_WideString wsStr = EncodeFileName(wsFileName);
  if (m_pObj->IsString()) {
    m_pObj->SetString(CFX_ByteString::FromUnicode(wsStr));
  } else if (CPDF_Dictionary* pDict = m_pObj->AsDictionary()) {
    pDict->SetAtString("F", CFX_ByteString::FromUnicode(wsStr));
    pDict->SetAtString("UF", PDF_EncodeText(wsStr));
  }
}

static CFX_WideString _MakeRoman(int num) {
  const int arabic[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
  const CFX_WideString roman[] = {L"m",  L"cm", L"d",  L"cd", L"c",
                                  L"xc", L"l",  L"xl", L"x",  L"ix",
                                  L"v",  L"iv", L"i"};
  const int nMaxNum = 1000000;
  num %= nMaxNum;
  int i = 0;
  CFX_WideString wsRomanNumber;
  while (num > 0) {
    while (num >= arabic[i]) {
      num = num - arabic[i];
      wsRomanNumber += roman[i];
    }
    i = i + 1;
  }
  return wsRomanNumber;
}

static CFX_WideString _MakeLetters(int num) {
  if (num == 0) {
    return CFX_WideString();
  }
  CFX_WideString wsLetters;
  const int nMaxCount = 1000;
  const int nLetterCount = 26;
  num -= 1;
  int count = num / nLetterCount + 1;
  count %= nMaxCount;
  FX_WCHAR ch = L'a' + num % nLetterCount;
  for (int i = 0; i < count; i++) {
    wsLetters += ch;
  }
  return wsLetters;
}
static CFX_WideString _GetLabelNumPortion(int num,
                                          const CFX_ByteString& bsStyle) {
  CFX_WideString wsNumPortion;
  if (bsStyle.IsEmpty()) {
    return wsNumPortion;
  }
  if (bsStyle == "D") {
    wsNumPortion.Format(L"%d", num);
  } else if (bsStyle == "R") {
    wsNumPortion = _MakeRoman(num);
    wsNumPortion.MakeUpper();
  } else if (bsStyle == "r") {
    wsNumPortion = _MakeRoman(num);
  } else if (bsStyle == "A") {
    wsNumPortion = _MakeLetters(num);
    wsNumPortion.MakeUpper();
  } else if (bsStyle == "a") {
    wsNumPortion = _MakeLetters(num);
  }
  return wsNumPortion;
}
CFX_WideString CPDF_PageLabel::GetLabel(int nPage) const {
  CFX_WideString wsLabel;
  if (!m_pDocument) {
    return wsLabel;
  }
  CPDF_Dictionary* pPDFRoot = m_pDocument->GetRoot();
  if (!pPDFRoot) {
    return wsLabel;
  }
  CPDF_Dictionary* pLabels = pPDFRoot->GetDictBy("PageLabels");
  CPDF_NumberTree numberTree(pLabels);
  CPDF_Object* pValue = NULL;
  int n = nPage;
  while (n >= 0) {
    pValue = numberTree.LookupValue(n);
    if (pValue) {
      break;
    }
    n--;
  }
  if (pValue) {
    pValue = pValue->GetDirect();
    if (CPDF_Dictionary* pLabel = pValue->AsDictionary()) {
      if (pLabel->KeyExist("P")) {
        wsLabel += pLabel->GetUnicodeTextBy("P");
      }
      CFX_ByteString bsNumberingStyle = pLabel->GetStringBy("S", NULL);
      int nLabelNum = nPage - n + pLabel->GetIntegerBy("St", 1);
      CFX_WideString wsNumPortion =
          _GetLabelNumPortion(nLabelNum, bsNumberingStyle);
      wsLabel += wsNumPortion;
      return wsLabel;
    }
  }
  wsLabel.Format(L"%d", nPage + 1);
  return wsLabel;
}
int32_t CPDF_PageLabel::GetPageByLabel(const CFX_ByteStringC& bsLabel) const {
  if (!m_pDocument) {
    return -1;
  }
  CPDF_Dictionary* pPDFRoot = m_pDocument->GetRoot();
  if (!pPDFRoot) {
    return -1;
  }
  int nPages = m_pDocument->GetPageCount();
  CFX_ByteString bsLbl;
  CFX_ByteString bsOrig = bsLabel;
  for (int i = 0; i < nPages; i++) {
    bsLbl = PDF_EncodeText(GetLabel(i));
    if (!bsLbl.Compare(bsOrig.AsByteStringC())) {
      return i;
    }
  }
  bsLbl = bsOrig;
  int nPage = FXSYS_atoi(bsLbl);
  if (nPage > 0 && nPage <= nPages) {
    return nPage;
  }
  return -1;
}
int32_t CPDF_PageLabel::GetPageByLabel(const CFX_WideStringC& wsLabel) const {
  return GetPageByLabel(PDF_EncodeText(wsLabel.raw_str()).AsByteStringC());
}
