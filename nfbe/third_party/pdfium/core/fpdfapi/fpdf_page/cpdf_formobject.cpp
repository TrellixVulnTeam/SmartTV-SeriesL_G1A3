// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/include/cpdf_formobject.h"

#include "core/fpdfapi/fpdf_page/include/cpdf_form.h"

CPDF_FormObject::CPDF_FormObject() : m_pForm(nullptr) {}

CPDF_FormObject::~CPDF_FormObject() {
  delete m_pForm;
}

void CPDF_FormObject::Transform(const CFX_Matrix& matrix) {
  m_FormMatrix.Concat(matrix);
  CalcBoundingBox();
}

CPDF_FormObject* CPDF_FormObject::Clone() const {
  CPDF_FormObject* obj = new CPDF_FormObject;
  obj->CopyData(this);

  obj->m_pForm = m_pForm->Clone();
  obj->m_FormMatrix = m_FormMatrix;
  return obj;
}

void CPDF_FormObject::CalcBoundingBox() {
  CFX_FloatRect form_rect = m_pForm->CalcBoundingBox();
  form_rect.Transform(&m_FormMatrix);
  m_Left = form_rect.left;
  m_Bottom = form_rect.bottom;
  m_Right = form_rect.right;
  m_Top = form_rect.top;
}
