// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_APPLE_APPLE_INT_H_
#define CORE_FXGE_APPLE_APPLE_INT_H_

#include "core/fxcrt/include/fx_system.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_APPLE_

#include "core/include/fxge/fx_ge.h"

#if _FX_OS_ == _FX_MACOSX_
#include <Carbon/Carbon.h>
#endif

typedef enum eFXIOSFONTCHARSET {
  eFXFontCharsetDEFAULT = 0,
  eFXFontCharsetANSI = 1,
  eFXFontCharsetSYMBOL = 1 << 1,
  eFXFontCharsetSHIFTJIS = 1 << 2,
  eFXFontCharsetHANGEUL = 1 << 3,
  eFXFontCharsetGB2312 = 1 << 4,
  eFXFontCharsetCHINESEBIG5 = 1 << 5,
  eFXFontCharsetTHAI = 1 << 6,
  eFXFontCharsetEASTEUROPE = 1 << 7,
  eFXFontCharsetRUSSIAN = 1 << 8,
  eFXFontCharsetGREEK = 1 << 9,
  eFXFontCharsetTURKISH = 1 << 10,
  eFXFontCharsetHEBREW = 1 << 11,
  eFXFontCharsetARABIC = 1 << 12,
  eFXFontCharsetBALTIC = 1 << 13,
} FX_IOSCHARSET;

FX_IOSCHARSET FX_GetiOSCharset(int charset);
typedef enum eFXIOSFONTFLAG {
  eFXFontFlagBold = 1,
  eFXFontFlagItalic = 1 << 1,
  eFXFontFlagFixedPitch = 1 << 2,
  eFXFontFlagSerif = 1 << 3,
  eFXFontFlagScript = 1 << 4,
} FX_IOSFONTFLAG;

typedef struct IOS_FONTDATA_ {
  uint32_t nHashCode;
  const char* psName;
  uint32_t charsets;
  uint32_t styles;
} IOS_FONTDATA;

class CQuartz2D {
 public:
  void* createGraphics(CFX_DIBitmap* bitmap);
  void destroyGraphics(void* graphics);

  void* CreateFont(const uint8_t* pFontData, uint32_t dwFontSize);
  void DestroyFont(void* pFont);
  void setGraphicsTextMatrix(void* graphics, CFX_Matrix* matrix);
  FX_BOOL drawGraphicsString(void* graphics,
                             void* font,
                             FX_FLOAT fontSize,
                             uint16_t* glyphIndices,
                             CGPoint* glyphPositions,
                             int32_t chars,
                             FX_ARGB argb,
                             CFX_Matrix* matrix = NULL);
  void saveGraphicsState(void* graphics);
  void restoreGraphicsState(void* graphics);
};

class CApplePlatform {
 public:
  CApplePlatform() {}
  ~CApplePlatform() {}

  CQuartz2D m_quartz2d;
};

class CFX_QuartzDeviceDriver : public IFX_RenderDeviceDriver {
 public:
  CFX_QuartzDeviceDriver(CGContextRef context, int32_t deviceClass);
  ~CFX_QuartzDeviceDriver() override;

  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) override;
  CFX_Matrix GetCTM() const override;
  FX_BOOL IsPSPrintDriver() override { return FALSE; }
  FX_BOOL StartRendering() override { return TRUE; }
  void EndRendering() override {}
  void SaveState() override;
  void RestoreState(FX_BOOL bKeepSaved) override;
  FX_BOOL SetClip_PathFill(const CFX_PathData* pPathData,
                           const CFX_Matrix* pObject2Device,
                           int fill_mode) override;
  FX_BOOL SetClip_PathStroke(const CFX_PathData* pPathData,
                             const CFX_Matrix* pObject2Device,
                             const CFX_GraphStateData* pGraphState) override;
  FX_BOOL DrawPath(const CFX_PathData* pPathData,
                   const CFX_Matrix* pObject2Device,
                   const CFX_GraphStateData* pGraphState,
                   uint32_t fill_color,
                   uint32_t stroke_color,
                   int fill_mode,
                   int alpha_flag = 0,
                   void* pIccTransform = NULL,
                   int blend_type = FXDIB_BLEND_NORMAL) override;
  FX_BOOL SetPixel(int x,
                   int y,
                   uint32_t color,
                   int alpha_flag = 0,
                   void* pIccTransform = NULL) override {
    return FALSE;
  }
  FX_BOOL FillRect(const FX_RECT* pRect,
                   uint32_t fill_color,
                   int alpha_flag = 0,
                   void* pIccTransform = NULL,
                   int blend_type = FXDIB_BLEND_NORMAL) override;
  FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           uint32_t color,
                           int alpha_flag = 0,
                           void* pIccTransform = NULL,
                           int blend_type = FXDIB_BLEND_NORMAL) override;
  FX_BOOL GetClipBox(FX_RECT* pRect) override;
  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap,
                    int left,
                    int top,
                    void* pIccTransform = NULL,
                    FX_BOOL bDEdge = FALSE) override;
  CFX_DIBitmap* GetBackDrop() override { return NULL; }
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int dest_left,
                    int dest_top,
                    int blend_type,
                    int alpha_flag = 0,
                    void* pIccTransform = NULL) override;
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        uint32_t color,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        uint32_t flags,
                        int alpha_flag = 0,
                        void* pIccTransform = NULL,
                        int blend_type = FXDIB_BLEND_NORMAL) override;
  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t flags,
                      void*& handle,
                      int alpha_flag = 0,
                      void* pIccTransform = NULL,
                      int blend_type = FXDIB_BLEND_NORMAL) override {
    return FALSE;
  }
  FX_BOOL ContinueDIBits(void* handle, IFX_Pause* pPause) override {
    return FALSE;
  }
  void CancelDIBits(void* handle) override {}
  FX_BOOL DrawDeviceText(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         const CFX_Matrix* pObject2Device,
                         FX_FLOAT font_size,
                         uint32_t color,
                         int alpha_flag = 0,
                         void* pIccTransform = NULL) override;
  void* GetPlatformSurface() const override { return NULL; }
  void ClearDriver() override;

 protected:
  void setStrokeInfo(const CFX_GraphStateData* graphState,
                     FX_ARGB argb,
                     FX_FLOAT lineWidth);
  void setFillInfo(FX_ARGB argb);
  void setPathToContext(const CFX_PathData* pathData);
  FX_FLOAT getLineWidth(const CFX_GraphStateData* graphState,
                        CGAffineTransform ctm);
  FX_BOOL CG_DrawGlypRun(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         const CFX_Matrix* pGlyphMatrix,
                         const CFX_Matrix* pObject2Device,
                         FX_FLOAT font_size,
                         uint32_t argb,
                         int alpha_flag,
                         void* pIccTransform);
  void CG_SetImageTransform(int dest_left,
                            int dest_top,
                            int dest_width,
                            int dest_height,
                            CGRect* rect = NULL);

  CGContextRef m_context;
  CGAffineTransform m_foxitDevice2User;
  CGAffineTransform m_user2FoxitDevice;
  int32_t m_saveCount;

  int32_t m_width;
  int32_t m_height;
  int32_t m_bitsPerPixel;
  int32_t m_deviceClass;
  int32_t m_renderCaps;
  int32_t m_horzSize;
  int32_t m_vertSize;
};

uint32_t FX_GetHashCode(const FX_CHAR* pStr);
uint32_t FX_IOSGetMatchFamilyNameHashcode(const FX_CHAR* pFontName);
uint32_t FX_IOSGetFamilyNamesCount();
const FX_CHAR* FX_IOSGetFamilyName(uint32_t uIndex);
#endif

#endif  // CORE_FXGE_APPLE_APPLE_INT_H_
