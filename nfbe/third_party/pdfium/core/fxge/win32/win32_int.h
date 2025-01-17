// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_WIN32_WIN32_INT_H_
#define CORE_FXGE_WIN32_WIN32_INT_H_

#include "core/fxge/win32/dwrite_int.h"
#include "core/include/fxge/fx_ge.h"

struct WINDIB_Open_Args_;
class CGdiplusExt {
 public:
  CGdiplusExt();
  ~CGdiplusExt();
  void Load();
  FX_BOOL IsAvailable() { return m_hModule != NULL; }
  FX_BOOL StretchBitMask(HDC hDC,
                         BOOL bMonoDevice,
                         const CFX_DIBitmap* pBitmap,
                         int dest_left,
                         int dest_top,
                         int dest_width,
                         int dest_height,
                         uint32_t argb,
                         const FX_RECT* pClipRect,
                         int flags);
  FX_BOOL StretchDIBits(HDC hDC,
                        const CFX_DIBitmap* pBitmap,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        int flags);
  FX_BOOL DrawPath(HDC hDC,
                   const CFX_PathData* pPathData,
                   const CFX_Matrix* pObject2Device,
                   const CFX_GraphStateData* pGraphState,
                   uint32_t fill_argb,
                   uint32_t stroke_argb,
                   int fill_mode);

  void* LoadMemFont(uint8_t* pData, uint32_t size);
  void DeleteMemFont(void* pFontCollection);
  FX_BOOL GdipCreateFromImage(void* bitmap, void** graphics);
  void GdipDeleteGraphics(void* graphics);
  void GdipSetTextRenderingHint(void* graphics, int mode);
  void GdipSetPageUnit(void* graphics, uint32_t unit);
  void GdipSetWorldTransform(void* graphics, void* pMatrix);
  FX_BOOL GdipDrawDriverString(void* graphics,
                               unsigned short* text,
                               int length,
                               void* font,
                               void* brush,
                               void* positions,
                               int flags,
                               const void* matrix);
  void GdipCreateBrush(uint32_t fill_argb, void** pBrush);
  void GdipDeleteBrush(void* pBrush);
  void GdipCreateMatrix(FX_FLOAT a,
                        FX_FLOAT b,
                        FX_FLOAT c,
                        FX_FLOAT d,
                        FX_FLOAT e,
                        FX_FLOAT f,
                        void** matrix);
  void GdipDeleteMatrix(void* matrix);
  FX_BOOL GdipCreateFontFamilyFromName(const FX_WCHAR* name,
                                       void* pFontCollection,
                                       void** pFamily);
  void GdipDeleteFontFamily(void* pFamily);
  FX_BOOL GdipCreateFontFromFamily(void* pFamily,
                                   FX_FLOAT font_size,
                                   int fontstyle,
                                   int flag,
                                   void** pFont);
  void* GdipCreateFontFromCollection(void* pFontCollection,
                                     FX_FLOAT font_size,
                                     int fontstyle);
  void GdipDeleteFont(void* pFont);
  FX_BOOL GdipCreateBitmap(CFX_DIBitmap* pBitmap, void** bitmap);
  void GdipDisposeImage(void* bitmap);
  void GdipGetFontSize(void* pFont, FX_FLOAT* size);
  void* GdiAddFontMemResourceEx(void* pFontdata,
                                uint32_t size,
                                void* pdv,
                                uint32_t* num_face);
  FX_BOOL GdiRemoveFontMemResourceEx(void* handle);
  void* m_Functions[100];
  void* m_pGdiAddFontMemResourceEx;
  void* m_pGdiRemoveFontMemResourseEx;
  CFX_DIBitmap* LoadDIBitmap(WINDIB_Open_Args_ args);

 protected:
  HMODULE m_hModule;
  HMODULE m_GdiModule;
};
class CWin32Platform {
 public:
  FX_BOOL m_bHalfTone;
  CGdiplusExt m_GdiplusExt;
  CDWriteExt m_DWriteExt;
};

class CGdiDeviceDriver : public IFX_RenderDeviceDriver {
 protected:
  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) override;
  void SaveState() override { SaveDC(m_hDC); }
  void RestoreState(FX_BOOL bKeepSaved = FALSE) override {
    RestoreDC(m_hDC, -1);
    if (bKeepSaved) {
      SaveDC(m_hDC);
    }
  }
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
                   int alpha_flag,
                   void* pIccTransform,
                   int blend_type) override;
  FX_BOOL FillRect(const FX_RECT* pRect,
                   uint32_t fill_color,
                   int alpha_flag,
                   void* pIccTransform,
                   int blend_type) override;
  FX_BOOL DrawCosmeticLine(FX_FLOAT x1,
                           FX_FLOAT y1,
                           FX_FLOAT x2,
                           FX_FLOAT y2,
                           uint32_t color,
                           int alpha_flag,
                           void* pIccTransform,
                           int blend_type) override;
  FX_BOOL GetClipBox(FX_RECT* pRect) override;
  void* GetPlatformSurface() const override { return (void*)m_hDC; }

  virtual void* GetClipRgn();
  virtual FX_BOOL SetClipRgn(void* pRgn);
  virtual FX_BOOL DeleteDeviceRgn(void* pRgn);
  virtual void DrawLine(FX_FLOAT x1, FX_FLOAT y1, FX_FLOAT x2, FX_FLOAT y2);

  FX_BOOL GDI_SetDIBits(const CFX_DIBitmap* pBitmap,
                        const FX_RECT* pSrcRect,
                        int left,
                        int top,
                        void* pIccTransform);
  FX_BOOL GDI_StretchDIBits(const CFX_DIBitmap* pBitmap,
                            int dest_left,
                            int dest_top,
                            int dest_width,
                            int dest_height,
                            uint32_t flags,
                            void* pIccTransform);
  FX_BOOL GDI_StretchBitMask(const CFX_DIBitmap* pBitmap,
                             int dest_left,
                             int dest_top,
                             int dest_width,
                             int dest_height,
                             uint32_t bitmap_color,
                             uint32_t flags,
                             int alpha_flag,
                             void* pIccTransform);
  HDC m_hDC;
  int m_Width, m_Height, m_nBitsPerPixel;
  int m_DeviceClass, m_RenderCaps;
  CGdiDeviceDriver(HDC hDC, int device_class);
  ~CGdiDeviceDriver() override {}
};

class CGdiDisplayDriver : public CGdiDeviceDriver {
 public:
  CGdiDisplayDriver(HDC hDC);

 protected:
  FX_BOOL GetDIBits(CFX_DIBitmap* pBitmap,
                    int left,
                    int top,
                    void* pIccTransform = NULL,
                    FX_BOOL bDEdge = FALSE) override;
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int left,
                    int top,
                    int blend_type,
                    int alpha_flag,
                    void* pIccTransform) override;
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        uint32_t color,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        uint32_t flags,
                        int alpha_flag,
                        void* pIccTransform,
                        int blend_type) override;
  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t render_flags,
                      void*& handle,
                      int alpha_flag,
                      void* pIccTransform,
                      int blend_type) override {
    return FALSE;
  }
  FX_BOOL UseFoxitStretchEngine(const CFX_DIBSource* pSource,
                                uint32_t color,
                                int dest_left,
                                int dest_top,
                                int dest_width,
                                int dest_height,
                                const FX_RECT* pClipRect,
                                int render_flags,
                                int alpha_flag = 0,
                                void* pIccTransform = NULL,
                                int blend_type = FXDIB_BLEND_NORMAL);
};
class CGdiPrinterDriver : public CGdiDeviceDriver {
 public:
  CGdiPrinterDriver(HDC hDC);

 protected:
  int GetDeviceCaps(int caps_id) override;
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int left,
                    int top,
                    int blend_type,
                    int alpha_flag,
                    void* pIccTransform) override;
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        uint32_t color,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        uint32_t flags,
                        int alpha_flag,
                        void* pIccTransform,
                        int blend_type) override;
  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t render_flags,
                      void*& handle,
                      int alpha_flag,
                      void* pIccTransform,
                      int blend_type) override;
  int m_HorzSize, m_VertSize;
  FX_BOOL m_bSupportROP;
};

class CPSOutput : public IFX_PSOutput {
 public:
  explicit CPSOutput(HDC hDC);
  ~CPSOutput() override;

  // IFX_PSOutput
  void Release() override { delete this; }
  void OutputPS(const FX_CHAR* str, int len) override;

  void Init();

  HDC m_hDC;
  FX_CHAR* m_pBuf;
};

class CPSPrinterDriver : public IFX_RenderDeviceDriver {
 public:
  CPSPrinterDriver();
  FX_BOOL Init(HDC hDC, int ps_level, FX_BOOL bCmykOutput);
  ~CPSPrinterDriver() override;

 protected:
  // IFX_RenderDeviceDriver
  int GetDeviceCaps(int caps_id) override;
  FX_BOOL IsPSPrintDriver() override { return TRUE; }
  FX_BOOL StartRendering() override;
  void EndRendering() override;
  void SaveState() override;
  void RestoreState(FX_BOOL bKeepSaved = FALSE) override;
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
                   int alpha_flag,
                   void* pIccTransform,
                   int blend_type) override;
  FX_BOOL GetClipBox(FX_RECT* pRect) override;
  FX_BOOL SetDIBits(const CFX_DIBSource* pBitmap,
                    uint32_t color,
                    const FX_RECT* pSrcRect,
                    int left,
                    int top,
                    int blend_type,
                    int alpha_flag,
                    void* pIccTransform) override;
  FX_BOOL StretchDIBits(const CFX_DIBSource* pBitmap,
                        uint32_t color,
                        int dest_left,
                        int dest_top,
                        int dest_width,
                        int dest_height,
                        const FX_RECT* pClipRect,
                        uint32_t flags,
                        int alpha_flag,
                        void* pIccTransform,
                        int blend_type) override;
  FX_BOOL StartDIBits(const CFX_DIBSource* pBitmap,
                      int bitmap_alpha,
                      uint32_t color,
                      const CFX_Matrix* pMatrix,
                      uint32_t render_flags,
                      void*& handle,
                      int alpha_flag,
                      void* pIccTransform,
                      int blend_type) override;
  FX_BOOL DrawDeviceText(int nChars,
                         const FXTEXT_CHARPOS* pCharPos,
                         CFX_Font* pFont,
                         CFX_FontCache* pCache,
                         const CFX_Matrix* pObject2Device,
                         FX_FLOAT font_size,
                         uint32_t color,
                         int alpha_flag,
                         void* pIccTransform) override;
  void* GetPlatformSurface() const override { return (void*)m_hDC; }

  HDC m_hDC;
  FX_BOOL m_bCmykOutput;
  int m_Width, m_Height, m_nBitsPerPixel;
  int m_HorzSize, m_VertSize;
  CPSOutput* m_pPSOutput;
  CFX_PSRenderer m_PSRenderer;
};
void _Color2Argb(FX_ARGB& argb,
                 uint32_t color,
                 int alpha_flag,
                 void* pIccTransform);

#endif  // CORE_FXGE_WIN32_WIN32_INT_H_
