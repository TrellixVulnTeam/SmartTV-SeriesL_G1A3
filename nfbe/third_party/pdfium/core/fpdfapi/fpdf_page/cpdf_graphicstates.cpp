// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_page/cpdf_graphicstates.h"

void CPDF_GraphicStates::DefaultStates() {
  m_ColorState.New()->Default();
}

void CPDF_GraphicStates::CopyStates(const CPDF_GraphicStates& src) {
  m_ClipPath = src.m_ClipPath;
  m_GraphState = src.m_GraphState;
  m_ColorState = src.m_ColorState;
  m_TextState = src.m_TextState;
  m_GeneralState = src.m_GeneralState;
}
