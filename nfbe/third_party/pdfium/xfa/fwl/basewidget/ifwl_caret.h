// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_BASEWIDGET_IFWL_CARET_H_
#define XFA_FWL_BASEWIDGET_IFWL_CARET_H_

#include "xfa/fwl/core/ifwl_widget.h"
#include "xfa/fwl/core/cfwl_widgetimpproperties.h"

#define FWL_CLASS_Caret L"FWL_CARET"
#define FWL_CLASSHASH_Caret 671181879
#define FWL_STATE_CAT_HightLight 1
#define FWL_PART_CAT_Background 1
#define FWL_PARTSTATE_CAT_HightLight 1

class IFWL_Caret : public IFWL_Widget {
 public:
  static IFWL_Caret* Create(const CFWL_WidgetImpProperties& properties,
                            IFWL_Widget* pOuter);

  FWL_ERR ShowCaret(FX_BOOL bFlag = TRUE);
  FWL_ERR GetFrequency(uint32_t& elapse);
  FWL_ERR SetFrequency(uint32_t elapse);
  FWL_ERR SetColor(CFX_Color crFill);

 protected:
  IFWL_Caret();
};

#endif  // XFA_FWL_BASEWIDGET_IFWL_CARET_H_
