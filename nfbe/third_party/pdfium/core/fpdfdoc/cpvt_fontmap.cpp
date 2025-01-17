// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpvt_fontmap.h"

#include "core/fpdfapi/fpdf_font/include/cpdf_font.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfdoc/doc_utils.h"

CPVT_FontMap::CPVT_FontMap(CPDF_Document* pDoc,
                           CPDF_Dictionary* pResDict,
                           CPDF_Font* pDefFont,
                           const CFX_ByteString& sDefFontAlias)
    : m_pDocument(pDoc),
      m_pResDict(pResDict),
      m_pDefFont(pDefFont),
      m_sDefFontAlias(sDefFontAlias),
      m_pSysFont(nullptr),
      m_sSysFontAlias() {}

CPVT_FontMap::~CPVT_FontMap() {}

void CPVT_FontMap::GetAnnotSysPDFFont(CPDF_Document* pDoc,
                                      const CPDF_Dictionary* pResDict,
                                      CPDF_Font*& pSysFont,
                                      CFX_ByteString& sSysFontAlias) {
  if (!pDoc || !pResDict)
    return;

  CFX_ByteString sFontAlias;
  CPDF_Dictionary* pFormDict = pDoc->GetRoot()->GetDictBy("AcroForm");
  CPDF_Font* pPDFFont = AddNativeInterFormFont(pFormDict, pDoc, sSysFontAlias);
  if (!pPDFFont)
    return;

  if (CPDF_Dictionary* pFontList = pResDict->GetDictBy("Font")) {
    if (!pFontList->KeyExist(sSysFontAlias.AsByteStringC()))
      pFontList->SetAtReference(sSysFontAlias.AsByteStringC(), pDoc,
                                pPDFFont->GetFontDict());
  }
  pSysFont = pPDFFont;
}

CPDF_Font* CPVT_FontMap::GetPDFFont(int32_t nFontIndex) {
  switch (nFontIndex) {
    case 0:
      return m_pDefFont;
    case 1:
      if (!m_pSysFont) {
        GetAnnotSysPDFFont(m_pDocument, m_pResDict, m_pSysFont,
                           m_sSysFontAlias);
      }
      return m_pSysFont;
    default:
      return nullptr;
  }
}

CFX_ByteString CPVT_FontMap::GetPDFFontAlias(int32_t nFontIndex) {
  switch (nFontIndex) {
    case 0:
      return m_sDefFontAlias;
    case 1:
      if (!m_pSysFont) {
        GetAnnotSysPDFFont(m_pDocument, m_pResDict, m_pSysFont,
                           m_sSysFontAlias);
      }
      return m_sSysFontAlias;
    default:
      return "";
  }
}

int32_t CPVT_FontMap::GetWordFontIndex(uint16_t word,
                                       int32_t charset,
                                       int32_t nFontIndex) {
  ASSERT(false);
  return 0;
}

int32_t CPVT_FontMap::CharCodeFromUnicode(int32_t nFontIndex, uint16_t word) {
  ASSERT(false);
  return 0;
}

int32_t CPVT_FontMap::CharSetFromUnicode(uint16_t word, int32_t nOldCharset) {
  ASSERT(false);
  return 0;
}
