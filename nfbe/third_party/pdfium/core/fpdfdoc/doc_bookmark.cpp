// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <memory>
#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "core/include/fpdfdoc/fpdf_doc.h"

CPDF_Bookmark CPDF_BookmarkTree::GetFirstChild(
    const CPDF_Bookmark& parent) const {
  if (!parent.GetDict()) {
    CPDF_Dictionary* pRoot = m_pDocument->GetRoot()->GetDictBy("Outlines");
    if (!pRoot)
      return CPDF_Bookmark();
    return CPDF_Bookmark(pRoot->GetDictBy("First"));
  }
  return CPDF_Bookmark(parent.GetDict()->GetDictBy("First"));
}

CPDF_Bookmark CPDF_BookmarkTree::GetNextSibling(
    const CPDF_Bookmark& bookmark) const {
  if (!bookmark.GetDict())
    return CPDF_Bookmark();

  CPDF_Dictionary* pNext = bookmark.GetDict()->GetDictBy("Next");
  return pNext == bookmark.GetDict() ? CPDF_Bookmark() : CPDF_Bookmark(pNext);
}

uint32_t CPDF_Bookmark::GetColorRef() const {
  if (!m_pDict) {
    return 0;
  }
  CPDF_Array* pColor = m_pDict->GetArrayBy("C");
  if (!pColor) {
    return FXSYS_RGB(0, 0, 0);
  }
  int r = FXSYS_round(pColor->GetNumberAt(0) * 255);
  int g = FXSYS_round(pColor->GetNumberAt(1) * 255);
  int b = FXSYS_round(pColor->GetNumberAt(2) * 255);
  return FXSYS_RGB(r, g, b);
}
uint32_t CPDF_Bookmark::GetFontStyle() const {
  if (!m_pDict) {
    return 0;
  }
  return m_pDict->GetIntegerBy("F");
}
CFX_WideString CPDF_Bookmark::GetTitle() const {
  if (!m_pDict) {
    return CFX_WideString();
  }
  CPDF_String* pString = ToString(m_pDict->GetDirectObjectBy("Title"));
  if (!pString)
    return CFX_WideString();

  CFX_WideString title = pString->GetUnicodeText();
  int len = title.GetLength();
  if (!len) {
    return CFX_WideString();
  }
  std::unique_ptr<FX_WCHAR[]> buf(new FX_WCHAR[len]);
  for (int i = 0; i < len; i++) {
    FX_WCHAR w = title[i];
    buf[i] = w > 0x20 ? w : 0x20;
  }
  return CFX_WideString(buf.get(), len);
}
CPDF_Dest CPDF_Bookmark::GetDest(CPDF_Document* pDocument) const {
  if (!m_pDict)
    return CPDF_Dest();

  CPDF_Object* pDest = m_pDict->GetDirectObjectBy("Dest");
  if (!pDest)
    return CPDF_Dest();
  if (pDest->IsString() || pDest->IsName()) {
    CPDF_NameTree name_tree(pDocument, "Dests");
    return CPDF_Dest(name_tree.LookupNamedDest(
        pDocument, pDest->GetString().AsByteStringC()));
  }
  if (CPDF_Array* pArray = pDest->AsArray())
    return CPDF_Dest(pArray);
  return CPDF_Dest();
}
CPDF_Action CPDF_Bookmark::GetAction() const {
  if (!m_pDict) {
    return CPDF_Action();
  }
  return CPDF_Action(m_pDict->GetDictBy("A"));
}
