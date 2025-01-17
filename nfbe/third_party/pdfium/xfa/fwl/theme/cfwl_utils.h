// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_UTILS_H_
#define XFA_FWL_THEME_CFWL_UTILS_H_

#include "core/fxcrt/include/fx_system.h"

#define THEME_XPSimilar
enum FWLTHEME_EDGE {
  FWLTHEME_EDGE_Flat = 0,
  FWLTHEME_EDGE_Raised,
  FWLTHEME_EDGE_Sunken
};
enum FWLTHEME_STATE {
  FWLTHEME_STATE_Normal = 1,
  FWLTHEME_STATE_Hover,
  FWLTHEME_STATE_Pressed,
  FWLTHEME_STATE_Disabale
};
enum FWLTHEME_DIRECTION {
  FWLTHEME_DIRECTION_Up = 0,
  FWLTHEME_DIRECTION_Down,
  FWLTHEME_DIRECTION_Left,
  FWLTHEME_DIRECTION_Right
};
struct FWLCOLOR {
  union {
    uint32_t color;
    struct {
      uint8_t b;
      uint8_t g;
      uint8_t r;
      uint8_t a;
    };
  };

  FWLCOLOR() : color(0) {}
  FWLCOLOR(uint32_t c) : color(c) {}
  FWLCOLOR(const FWLCOLOR& c) : color(c.color) {}

  bool operator==(const FWLCOLOR& frColor) { return color == frColor.color; }
  operator uint32_t() { return color; }
};
#define FWLTHEME_BEZIER FX_BEZIER
#define FWLTHEME_PI FX_PI
#define FWLTHEME_PI_2_1 (FX_PI / 2.0f)
#define FWLTHEME_PI_2_3 (3.0f * FX_PI / 2.0f)
#define FWLTHEME_COLOR_EDGELT1 (ArgbEncode(255, 172, 168, 153))
#define FWLTHEME_COLOR_EDGELT2 (ArgbEncode(255, 113, 111, 100))
#define FWLTHEME_COLOR_EDGERB1 (ArgbEncode(255, 241, 239, 226))
#define FWLTHEME_COLOR_EDGERB2 (ArgbEncode(255, 255, 255, 255))
#define FWLTHEME_COLOR_Background (ArgbEncode(255, 236, 233, 216))
#define FWLTHEME_COLOR_BKSelected (ArgbEncode(255, 153, 193, 218))
#define FWLTHEME_COLOR_Green_BKSelected (ArgbEncode(255, 147, 160, 112))
#ifdef THEME_XPSimilar
#define FWLTHEME_CAPACITY_EdgeFlat 2.0f
#else
#define FWLTHEME_CAPACITY_EdgeFlat 0.0f
#endif
#define FWLTHEME_CAPACITY_EdgeRaised 2.0f
#define FWLTHEME_CAPACITY_EdgeSunken 2.0f
#define FWLTHEME_CAPACITY_FontSize 12.0f
#define FWLTHEME_CAPACITY_LineHeight 12.0f
#define FWLTHEME_CAPACITY_TextColor (ArgbEncode(255, 0, 0, 0))
#define FWLTHEME_CAPACITY_TextSelColor (ArgbEncode(255, 153, 193, 218))
#define FWLTHEME_CAPACITY_TextDisColor (ArgbEncode(255, 172, 168, 153))
#define FWLTHEME_CAPACITY_ScrollBarWidth 17.0f
#define FWLTHEME_CAPACITY_CXBorder 1.0f
#define FWLTHEME_CAPACITY_CYBorder 1.0f

#endif  // XFA_FWL_THEME_CFWL_UTILS_H_
