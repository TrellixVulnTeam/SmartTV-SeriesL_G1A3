// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_fwladapter.h"

#include "xfa/fxfa/app/xfa_fffield.h"
#include "xfa/include/fxfa/xfa_ffdoc.h"

FWL_ERR CXFA_FWLAdapterWidgetMgr::RepaintWidget(IFWL_Widget* pWidget,
                                                const CFX_RectF* pRect) {
  if (!pWidget)
    return FWL_ERR_Indefinite;
  CXFA_FFField* pField = (CXFA_FFField*)pWidget->GetPrivateData(pWidget);
  if (!pField)
    return FWL_ERR_Indefinite;
  pField->AddInvalidateRect(nullptr);
  return FWL_ERR_Succeeded;
}

FX_BOOL CXFA_FWLAdapterWidgetMgr::GetPopupPos(IFWL_Widget* pWidget,
                                              FX_FLOAT fMinHeight,
                                              FX_FLOAT fMaxHeight,
                                              const CFX_RectF& rtAnchor,
                                              CFX_RectF& rtPopup) {
  CXFA_FFWidget* pFFWidget = (CXFA_FFWidget*)(pWidget->GetPrivateData(pWidget));
  CFX_Matrix mt;
  pFFWidget->GetRotateMatrix(mt);
  CFX_RectF rtRotateAnchor(rtAnchor);
  mt.TransformRect(rtRotateAnchor);
  pFFWidget->GetDoc()->GetDocProvider()->GetPopupPos(
      pFFWidget, fMinHeight, fMaxHeight, rtRotateAnchor, rtPopup);
  return TRUE;
}
