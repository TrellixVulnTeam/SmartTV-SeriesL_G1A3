// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/app/xfa_ffsubform.h"

#include "xfa/include/fxfa/xfa_ffapp.h"
#include "xfa/include/fxfa/xfa_ffdoc.h"
#include "xfa/include/fxfa/xfa_ffpageview.h"
#include "xfa/include/fxfa/xfa_ffwidget.h"

CXFA_FFSubForm::CXFA_FFSubForm(CXFA_FFPageView* pPageView,
                               CXFA_WidgetAcc* pDataAcc)
    : CXFA_FFWidget(pPageView, pDataAcc) {}
CXFA_FFSubForm::~CXFA_FFSubForm() {}
