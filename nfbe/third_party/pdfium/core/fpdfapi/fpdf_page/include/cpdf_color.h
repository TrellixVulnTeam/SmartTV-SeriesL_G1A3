// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_COLOR_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_COLOR_H_

#include "core/fpdfapi/fpdf_page/include/cpdf_colorspace.h"
#include "core/fxcrt/include/fx_system.h"

class CPDF_Pattern;

class CPDF_Color {
 public:
  CPDF_Color() : m_pCS(nullptr), m_pBuffer(nullptr) {}
  explicit CPDF_Color(int family);
  ~CPDF_Color();

  FX_BOOL IsNull() const { return !m_pBuffer; }
  FX_BOOL IsEqual(const CPDF_Color& other) const;
  FX_BOOL IsPattern() const {
    return m_pCS && m_pCS->GetFamily() == PDFCS_PATTERN;
  }

  void Copy(const CPDF_Color* pSrc);

  void SetColorSpace(CPDF_ColorSpace* pCS);
  void SetValue(FX_FLOAT* comp);
  void SetValue(CPDF_Pattern* pPattern, FX_FLOAT* comp, int ncomps);

  FX_BOOL GetRGB(int& R, int& G, int& B) const;
  CPDF_Pattern* GetPattern() const;
  CPDF_ColorSpace* GetPatternCS() const;
  FX_FLOAT* GetPatternColor() const;

  CPDF_ColorSpace* m_pCS;

 protected:
  void ReleaseBuffer();
  void ReleaseColorSpace();

  FX_FLOAT* m_pBuffer;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_COLOR_H_
