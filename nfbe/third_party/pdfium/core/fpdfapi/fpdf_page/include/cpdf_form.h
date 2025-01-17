// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_FORM_H_
#define CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_FORM_H_

#include "core/fpdfapi/fpdf_page/include/cpdf_pageobjectholder.h"

class CPDF_Document;
class CPDF_Dictionary;
class CPDF_Stream;
class CPDF_AllStates;
class CFX_Matrix;
class CPDF_Type3Char;
class CPDF_ParseOptions;

class CPDF_Form : public CPDF_PageObjectHolder {
 public:
  CPDF_Form(CPDF_Document* pDocument,
            CPDF_Dictionary* pPageResources,
            CPDF_Stream* pFormStream,
            CPDF_Dictionary* pParentResources = NULL);

  ~CPDF_Form();

  void StartParse(CPDF_AllStates* pGraphicStates,
                  CFX_Matrix* pParentMatrix,
                  CPDF_Type3Char* pType3Char,
                  CPDF_ParseOptions* pOptions,
                  int level = 0);

  void ParseContent(CPDF_AllStates* pGraphicStates,
                    CFX_Matrix* pParentMatrix,
                    CPDF_Type3Char* pType3Char,
                    CPDF_ParseOptions* pOptions,
                    int level = 0);

  CPDF_Form* Clone() const;
};

#endif  // CORE_FPDFAPI_FPDF_PAGE_INCLUDE_CPDF_FORM_H_
