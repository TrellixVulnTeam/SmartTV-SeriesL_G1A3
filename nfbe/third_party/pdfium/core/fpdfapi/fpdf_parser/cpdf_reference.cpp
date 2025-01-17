// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"

#include "core/fpdfapi/fpdf_parser/include/cpdf_indirect_object_holder.h"

CPDF_Reference::CPDF_Reference(CPDF_IndirectObjectHolder* pDoc, int objnum)
    : m_pObjList(pDoc), m_RefObjNum(objnum) {}

CPDF_Reference::~CPDF_Reference() {}

CPDF_Object::Type CPDF_Reference::GetType() const {
  return REFERENCE;
}

CFX_ByteString CPDF_Reference::GetString() const {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetString() : CFX_ByteString();
}

CFX_ByteStringC CPDF_Reference::GetConstString() const {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetConstString() : CFX_ByteStringC();
}

FX_FLOAT CPDF_Reference::GetNumber() const {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetNumber() : 0;
}

int CPDF_Reference::GetInteger() const {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetInteger() : 0;
}

CPDF_Dictionary* CPDF_Reference::GetDict() const {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetDict() : nullptr;
}

bool CPDF_Reference::IsReference() const {
  return true;
}

CPDF_Reference* CPDF_Reference::AsReference() {
  return this;
}

const CPDF_Reference* CPDF_Reference::AsReference() const {
  return this;
}

CPDF_Object* CPDF_Reference::Clone(FX_BOOL bDirect) const {
  if (bDirect) {
    auto* pDirect = GetDirect();
    return pDirect ? pDirect->Clone(TRUE) : nullptr;
  }
  return new CPDF_Reference(m_pObjList, m_RefObjNum);
}

void CPDF_Reference::SetRef(CPDF_IndirectObjectHolder* pDoc, uint32_t objnum) {
  m_pObjList = pDoc;
  m_RefObjNum = objnum;
}

CPDF_Object* CPDF_Reference::GetDirect() const {
  return m_pObjList ? m_pObjList->GetIndirectObject(m_RefObjNum) : nullptr;
}
