// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_CHECKBOXTP_H_
#define XFA_FWL_THEME_CFWL_CHECKBOXTP_H_

#include "xfa/fwl/theme/cfwl_utils.h"
#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_CheckBoxTP : public CFWL_WidgetTP {
 public:
  CFWL_CheckBoxTP();
  virtual ~CFWL_CheckBoxTP();
  virtual FX_BOOL IsValidWidget(IFWL_Widget* pWidget);
  virtual uint32_t SetThemeID(IFWL_Widget* pWidget,
                              uint32_t dwThemeID,
                              FX_BOOL bChildren = TRUE);
  virtual FX_BOOL DrawText(CFWL_ThemeText* pParams);
  virtual FX_BOOL DrawBackground(CFWL_ThemeBackground* pParams);
  virtual FWL_ERR Initialize();
  virtual FWL_ERR Finalize();

 protected:
  void DrawBoxBk(IFWL_Widget* pWidget,
                 CFX_Graphics* pGraphics,
                 const CFX_RectF* pRect,
                 uint32_t dwStates,
                 CFX_Matrix* pMatrix = NULL);
  void DrawSign(IFWL_Widget* pWidget,
                CFX_Graphics* pGraphics,
                const CFX_RectF* pRtBox,
                uint32_t dwStates,
                CFX_Matrix* pMatrix = NULL);
  void DrawSignNeutral(CFX_Graphics* pGraphics,
                       const CFX_RectF* pRtSign,
                       CFX_Matrix* pMatrix = NULL);
  void DrawSignCheck(CFX_Graphics* pGraphics,
                     const CFX_RectF* pRtSign,
                     FX_ARGB argbFill,
                     CFX_Matrix* pMatrix = NULL);
  void DrawSignCircle(CFX_Graphics* pGraphics,
                      const CFX_RectF* pRtSign,
                      FX_ARGB argbFill,
                      CFX_Matrix* pMatrix = NULL);
  void DrawSignCross(CFX_Graphics* pGraphics,
                     const CFX_RectF* pRtSign,
                     FX_ARGB argbFill,
                     CFX_Matrix* pMatrix = NULL);
  void DrawSignDiamond(CFX_Graphics* pGraphics,
                       const CFX_RectF* pRtSign,
                       FX_ARGB argbFill,
                       CFX_Matrix* pMatrix = NULL);
  void DrawSignSquare(CFX_Graphics* pGraphics,
                      const CFX_RectF* pRtSign,
                      FX_ARGB argbFill,
                      CFX_Matrix* pMatrix = NULL);
  void DrawSignStar(CFX_Graphics* pGraphics,
                    const CFX_RectF* pRtSign,
                    FX_ARGB argbFill,
                    CFX_Matrix* pMatrix = NULL);
  void DrawSignBorder(IFWL_Widget* pWidget,
                      CFX_Graphics* pGraphics,
                      const CFX_RectF* pRtBox,
                      FX_BOOL bDisable = FALSE,
                      CFX_Matrix* pMatrix = NULL);
  void SetThemeData(uint32_t dwID);
  void initCheckPath(FX_FLOAT fCheckLen);
  struct CKBThemeData {
    FX_ARGB clrBoxBk[13][2];
    FX_ARGB clrSignBorderNormal;
    FX_ARGB clrSignBorderDisable;
    FX_ARGB clrSignCheck;
    FX_ARGB clrSignNeutral;
    FX_ARGB clrSignNeutralNormal;
    FX_ARGB clrSignNeutralHover;
    FX_ARGB clrSignNeutralPressed;
  } * m_pThemeData;
  CFX_Path* m_pCheckPath;
};

#endif  // XFA_FWL_THEME_CFWL_CHECKBOXTP_H_
