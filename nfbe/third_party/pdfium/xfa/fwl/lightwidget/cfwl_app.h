// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_LIGHTWIDGET_CFWL_APP_H_
#define XFA_FWL_LIGHTWIDGET_CFWL_APP_H_

#include "xfa/fwl/core/fwl_error.h"

class CFWL_Theme;
class IFWL_App;

class CFWL_App {
 public:
  CFWL_App();
  virtual ~CFWL_App();

  FWL_ERR Initialize();
  FWL_ERR Exit(int32_t iExitCode);

  CFWL_Theme* GetTheme() const { return m_pTheme; }
  IFWL_App* GetInterface() const { return m_pIface; }

 private:
  IFWL_App* m_pIface;
  CFWL_Theme* m_pTheme;
};

#endif  // XFA_FWL_LIGHTWIDGET_CFWL_APP_H_
