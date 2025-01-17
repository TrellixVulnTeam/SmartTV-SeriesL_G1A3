// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/basewidget/fwl_formproxyimp.h"

#include "xfa/fwl/core/fwl_formimp.h"
#include "xfa/fwl/core/fwl_noteimp.h"
#include "xfa/fwl/core/fwl_panelimp.h"
#include "xfa/fwl/core/fwl_targetimp.h"
#include "xfa/fwl/core/fwl_widgetimp.h"

CFWL_FormProxyImp::CFWL_FormProxyImp(const CFWL_WidgetImpProperties& properties,
                                     IFWL_Widget* pOuter)
    : CFWL_FormImp(properties, pOuter) {}
CFWL_FormProxyImp::~CFWL_FormProxyImp() {}
FWL_ERR CFWL_FormProxyImp::GetClassName(CFX_WideString& wsClass) const {
  wsClass = FWL_CLASS_FormProxy;
  return FWL_ERR_Succeeded;
}
uint32_t CFWL_FormProxyImp::GetClassID() const {
  return FWL_CLASSHASH_FormProxy;
}
FX_BOOL CFWL_FormProxyImp::IsInstance(const CFX_WideStringC& wsClass) const {
  if (wsClass == CFX_WideStringC(FWL_CLASS_FormProxy)) {
    return TRUE;
  }
  return CFWL_FormImp::IsInstance(wsClass);
}
FWL_ERR CFWL_FormProxyImp::Initialize() {
  if (CFWL_WidgetImp::Initialize() != FWL_ERR_Succeeded)
    return FWL_ERR_Indefinite;
  m_pDelegate = new CFWL_FormProxyImpDelegate(this);
  return FWL_ERR_Succeeded;
}
FWL_ERR CFWL_FormProxyImp::Finalize() {
  delete m_pDelegate;
  m_pDelegate = nullptr;
  return CFWL_WidgetImp::Finalize();
}
FWL_ERR CFWL_FormProxyImp::Update() {
  return FWL_ERR_Succeeded;
}
FWL_ERR CFWL_FormProxyImp::DrawWidget(CFX_Graphics* pGraphics,
                                      const CFX_Matrix* pMatrix) {
  return FWL_ERR_Succeeded;
}
CFWL_FormProxyImpDelegate::CFWL_FormProxyImpDelegate(CFWL_FormProxyImp* pOwner)
    : m_pOwner(pOwner) {}
int32_t CFWL_FormProxyImpDelegate::OnProcessMessage(CFWL_Message* pMessage) {
  IFWL_WidgetDelegate* pDelegate = m_pOwner->m_pOuter->SetDelegate(NULL);
  return pDelegate->OnProcessMessage(pMessage);
}
