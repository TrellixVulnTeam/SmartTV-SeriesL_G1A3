// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_WIDGETDELEGATE_H_
#define XFA_FWL_CORE_IFWL_WIDGETDELEGATE_H_

#include "xfa/fwl/core/fwl_error.h"

class CFWL_Event;
class CFWL_Message;
class CFX_Graphics;
class CFX_Matrix;

class IFWL_WidgetDelegate {
 public:
  virtual ~IFWL_WidgetDelegate() {}
  virtual int32_t OnProcessMessage(CFWL_Message* pMessage) = 0;
  virtual FWL_ERR OnProcessEvent(CFWL_Event* pEvent) = 0;
  virtual FWL_ERR OnDrawWidget(CFX_Graphics* pGraphics,
                               const CFX_Matrix* pMatrix = nullptr) = 0;
};

#endif  // XFA_FWL_CORE_IFWL_WIDGETDELEGATE_H_
