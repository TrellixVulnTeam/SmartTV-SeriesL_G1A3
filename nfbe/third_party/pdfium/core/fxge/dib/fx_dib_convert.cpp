// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/include/fxcodec/fx_codec.h"
#include "core/include/fxge/fx_dib.h"
#include "core/include/fxge/fx_ge.h"

class CFX_Palette {
 public:
  CFX_Palette();
  ~CFX_Palette();

  FX_BOOL BuildPalette(const CFX_DIBSource* pBitmap);
  uint32_t* GetPalette() const { return m_pPalette; }
  uint32_t* GetColorLut() const { return m_cLut; }
  uint32_t* GetAmountLut() const { return m_aLut; }
  int32_t Getlut() const { return m_lut; }

 protected:
  uint32_t* m_pPalette;
  uint32_t* m_cLut;
  uint32_t* m_aLut;
  int m_lut;
};
int _Partition(uint32_t* alut, uint32_t* clut, int l, int r) {
  uint32_t p_a = alut[l];
  uint32_t p_c = clut[l];
  while (l < r) {
    while (l < r && alut[r] >= p_a) {
      r--;
    }
    if (l < r) {
      alut[l] = alut[r];
      clut[l++] = clut[r];
    }
    while (l < r && alut[l] <= p_a) {
      l++;
    }
    if (l < r) {
      alut[r] = alut[l];
      clut[r--] = clut[l];
    }
  }
  alut[l] = p_a;
  clut[l] = p_c;
  return l;
}
void _Qsort(uint32_t* alut, uint32_t* clut, int l, int r) {
  if (l < r) {
    int pI = _Partition(alut, clut, l, r);
    _Qsort(alut, clut, l, pI - 1);
    _Qsort(alut, clut, pI + 1, r);
  }
}
void _ColorDecode(uint32_t pal_v, uint8_t& r, uint8_t& g, uint8_t& b) {
  r = (uint8_t)((pal_v & 0xf00) >> 4);
  g = (uint8_t)(pal_v & 0x0f0);
  b = (uint8_t)((pal_v & 0x00f) << 4);
}
void _Obtain_Pal(uint32_t* aLut,
                 uint32_t* cLut,
                 uint32_t* dest_pal,
                 uint32_t lut) {
  uint32_t lut_1 = lut - 1;
  for (int row = 0; row < 256; row++) {
    int lut_offset = lut_1 - row;
    if (lut_offset < 0) {
      lut_offset += 256;
    }
    uint32_t color = cLut[lut_offset];
    uint8_t r;
    uint8_t g;
    uint8_t b;
    _ColorDecode(color, r, g, b);
    dest_pal[row] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b | 0xff000000;
    aLut[lut_offset] = row;
  }
}

CFX_Palette::CFX_Palette() {
  m_pPalette = NULL;
  m_cLut = NULL;
  m_aLut = NULL;
  m_lut = 0;
}
CFX_Palette::~CFX_Palette() {
  FX_Free(m_pPalette);
  FX_Free(m_cLut);
  FX_Free(m_aLut);
  m_lut = 0;
}
FX_BOOL CFX_Palette::BuildPalette(const CFX_DIBSource* pBitmap) {
  if (!pBitmap) {
    return FALSE;
  }
  FX_Free(m_pPalette);
  m_pPalette = FX_Alloc(uint32_t, 256);
  int bpp = pBitmap->GetBPP() / 8;
  int width = pBitmap->GetWidth();
  int height = pBitmap->GetHeight();
  FX_Free(m_cLut);
  m_cLut = NULL;
  FX_Free(m_aLut);
  m_aLut = NULL;
  m_cLut = FX_Alloc(uint32_t, 4096);
  m_aLut = FX_Alloc(uint32_t, 4096);
  int row, col;
  m_lut = 0;
  for (row = 0; row < height; row++) {
    uint8_t* scan_line = (uint8_t*)pBitmap->GetScanline(row);
    for (col = 0; col < width; col++) {
      uint8_t* src_port = scan_line + col * bpp;
      uint32_t b = src_port[0] & 0xf0;
      uint32_t g = src_port[1] & 0xf0;
      uint32_t r = src_port[2] & 0xf0;
      uint32_t index = (r << 4) + g + (b >> 4);
      m_aLut[index]++;
    }
  }
  for (row = 0; row < 4096; row++) {
    if (m_aLut[row] != 0) {
      m_aLut[m_lut] = m_aLut[row];
      m_cLut[m_lut] = row;
      m_lut++;
    }
  }
  _Qsort(m_aLut, m_cLut, 0, m_lut - 1);
  _Obtain_Pal(m_aLut, m_cLut, m_pPalette, m_lut);
  return TRUE;
}
FX_BOOL ConvertBuffer_1bppMask2Gray(uint8_t* dest_buf,
                                    int dest_pitch,
                                    int width,
                                    int height,
                                    const CFX_DIBSource* pSrcBitmap,
                                    int src_left,
                                    int src_top) {
  uint8_t set_gray, reset_gray;
  set_gray = 0xff;
  reset_gray = 0x00;
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    FXSYS_memset(dest_scan, reset_gray, width);
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
    for (int col = src_left; col < src_left + width; col++) {
      if (src_scan[col / 8] & (1 << (7 - col % 8))) {
        *dest_scan = set_gray;
      }
      dest_scan++;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_8bppMask2Gray(uint8_t* dest_buf,
                                    int dest_pitch,
                                    int width,
                                    int height,
                                    const CFX_DIBSource* pSrcBitmap,
                                    int src_left,
                                    int src_top) {
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row) + src_left;
    FXSYS_memcpy(dest_scan, src_scan, width);
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_1bppPlt2Gray(uint8_t* dest_buf,
                                   int dest_pitch,
                                   int width,
                                   int height,
                                   const CFX_DIBSource* pSrcBitmap,
                                   int src_left,
                                   int src_top,
                                   void* pIccTransform) {
  uint32_t* src_plt = pSrcBitmap->GetPalette();
  uint8_t gray[2];
  if (pIccTransform) {
    uint32_t plt[2];
    if (pSrcBitmap->IsCmykImage()) {
      plt[0] = FXCMYK_TODIB(src_plt[0]);
      plt[1] = FXCMYK_TODIB(src_plt[1]);
    } else {
      uint8_t* bgr_ptr = (uint8_t*)plt;
      bgr_ptr[0] = FXARGB_B(src_plt[0]);
      bgr_ptr[1] = FXARGB_G(src_plt[0]);
      bgr_ptr[2] = FXARGB_R(src_plt[0]);
      bgr_ptr[3] = FXARGB_B(src_plt[1]);
      bgr_ptr[4] = FXARGB_G(src_plt[1]);
      bgr_ptr[5] = FXARGB_R(src_plt[1]);
    }
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    pIccModule->TranslateScanline(pIccTransform, gray, (const uint8_t*)plt, 2);
  } else {
    uint8_t reset_r, reset_g, reset_b, set_r, set_g, set_b;
    if (pSrcBitmap->IsCmykImage()) {
      AdobeCMYK_to_sRGB1(
          FXSYS_GetCValue(src_plt[0]), FXSYS_GetMValue(src_plt[0]),
          FXSYS_GetYValue(src_plt[0]), FXSYS_GetKValue(src_plt[0]), reset_r,
          reset_g, reset_b);
      AdobeCMYK_to_sRGB1(FXSYS_GetCValue(src_plt[1]),
                         FXSYS_GetMValue(src_plt[1]),
                         FXSYS_GetYValue(src_plt[1]),
                         FXSYS_GetKValue(src_plt[1]), set_r, set_g, set_b);
    } else {
      reset_r = FXARGB_R(src_plt[0]);
      reset_g = FXARGB_G(src_plt[0]);
      reset_b = FXARGB_B(src_plt[0]);
      set_r = FXARGB_R(src_plt[1]);
      set_g = FXARGB_G(src_plt[1]);
      set_b = FXARGB_B(src_plt[1]);
    }
    gray[0] = FXRGB2GRAY(reset_r, reset_g, reset_b);
    gray[1] = FXRGB2GRAY(set_r, set_g, set_b);
  }
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    FXSYS_memset(dest_scan, gray[0], width);
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
    for (int col = src_left; col < src_left + width; col++) {
      if (src_scan[col / 8] & (1 << (7 - col % 8))) {
        *dest_scan = gray[1];
      }
      dest_scan++;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_8bppPlt2Gray(uint8_t* dest_buf,
                                   int dest_pitch,
                                   int width,
                                   int height,
                                   const CFX_DIBSource* pSrcBitmap,
                                   int src_left,
                                   int src_top,
                                   void* pIccTransform) {
  uint32_t* src_plt = pSrcBitmap->GetPalette();
  uint8_t gray[256];
  if (pIccTransform) {
    uint32_t plt[256];
    if (pSrcBitmap->IsCmykImage()) {
      for (int i = 0; i < 256; i++) {
        plt[i] = FXCMYK_TODIB(src_plt[i]);
      }
    } else {
      uint8_t* bgr_ptr = (uint8_t*)plt;
      for (int i = 0; i < 256; i++) {
        *bgr_ptr++ = FXARGB_B(src_plt[i]);
        *bgr_ptr++ = FXARGB_G(src_plt[i]);
        *bgr_ptr++ = FXARGB_R(src_plt[i]);
      }
    }
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    pIccModule->TranslateScanline(pIccTransform, gray, (const uint8_t*)plt,
                                  256);
  } else {
    if (pSrcBitmap->IsCmykImage()) {
      uint8_t r, g, b;
      for (int i = 0; i < 256; i++) {
        AdobeCMYK_to_sRGB1(
            FXSYS_GetCValue(src_plt[i]), FXSYS_GetMValue(src_plt[i]),
            FXSYS_GetYValue(src_plt[i]), FXSYS_GetKValue(src_plt[i]), r, g, b);
        gray[i] = FXRGB2GRAY(r, g, b);
      }
    } else {
      for (int i = 0; i < 256; i++) {
        gray[i] = FXRGB2GRAY(FXARGB_R(src_plt[i]), FXARGB_G(src_plt[i]),
                             FXARGB_B(src_plt[i]));
      }
    }
  }
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row) + src_left;
    for (int col = 0; col < width; col++) {
      *dest_scan++ = gray[*src_scan++];
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_RgbOrCmyk2Gray(uint8_t* dest_buf,
                                     int dest_pitch,
                                     int width,
                                     int height,
                                     const CFX_DIBSource* pSrcBitmap,
                                     int src_left,
                                     int src_top,
                                     void* pIccTransform) {
  int Bpp = pSrcBitmap->GetBPP() / 8;
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    if (Bpp == 3 || pSrcBitmap->IsCmykImage()) {
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan = dest_buf + row * dest_pitch;
        const uint8_t* src_scan =
            pSrcBitmap->GetScanline(src_top + row) + src_left * Bpp;
        pIccModule->TranslateScanline(pIccTransform, dest_scan, src_scan,
                                      width);
      }
    } else {
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan = dest_buf + row * dest_pitch;
        const uint8_t* src_scan =
            pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
        for (int col = 0; col < width; col++) {
          pIccModule->TranslateScanline(pIccTransform, dest_scan, src_scan, 1);
          dest_scan++;
          src_scan += 4;
        }
      }
    }
  } else {
    if (pSrcBitmap->IsCmykImage()) {
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan = dest_buf + row * dest_pitch;
        const uint8_t* src_scan =
            pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
        for (int col = 0; col < width; col++) {
          uint8_t r, g, b;
          AdobeCMYK_to_sRGB1(FXSYS_GetCValue((uint32_t)src_scan[0]),
                             FXSYS_GetMValue((uint32_t)src_scan[1]),
                             FXSYS_GetYValue((uint32_t)src_scan[2]),
                             FXSYS_GetKValue((uint32_t)src_scan[3]), r, g, b);
          *dest_scan++ = FXRGB2GRAY(r, g, b);
          src_scan += 4;
        }
      }
    } else {
      for (int row = 0; row < height; row++) {
        uint8_t* dest_scan = dest_buf + row * dest_pitch;
        const uint8_t* src_scan =
            pSrcBitmap->GetScanline(src_top + row) + src_left * Bpp;
        for (int col = 0; col < width; col++) {
          *dest_scan++ = FXRGB2GRAY(src_scan[2], src_scan[1], src_scan[0]);
          src_scan += Bpp;
        }
      }
    }
  }
  return TRUE;
}
inline void ConvertBuffer_IndexCopy(uint8_t* dest_buf,
                                    int dest_pitch,
                                    int width,
                                    int height,
                                    const CFX_DIBSource* pSrcBitmap,
                                    int src_left,
                                    int src_top) {
  if (pSrcBitmap->GetBPP() == 1) {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      FXSYS_memset(dest_scan, 0, width);
      const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
      for (int col = src_left; col < src_left + width; col++) {
        if (src_scan[col / 8] & (1 << (7 - col % 8))) {
          *dest_scan = 1;
        }
        dest_scan++;
      }
    }
  } else {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left;
      FXSYS_memcpy(dest_scan, src_scan, width);
    }
  }
}
FX_BOOL ConvertBuffer_Plt2PltRgb8(uint8_t* dest_buf,
                                  int dest_pitch,
                                  int width,
                                  int height,
                                  const CFX_DIBSource* pSrcBitmap,
                                  int src_left,
                                  int src_top,
                                  uint32_t* dst_plt,
                                  void* pIccTransform) {
  ConvertBuffer_IndexCopy(dest_buf, dest_pitch, width, height, pSrcBitmap,
                          src_left, src_top);
  uint32_t* src_plt = pSrcBitmap->GetPalette();
  int plt_size = pSrcBitmap->GetPaletteSize();
  if (pIccTransform) {
    uint32_t plt[256];
    uint8_t* bgr_ptr = (uint8_t*)plt;
    if (pSrcBitmap->IsCmykImage()) {
      for (int i = 0; i < plt_size; i++) {
        plt[i] = FXCMYK_TODIB(src_plt[i]);
      }
    } else {
      for (int i = 0; i < plt_size; i++) {
        *bgr_ptr++ = FXARGB_B(src_plt[i]);
        *bgr_ptr++ = FXARGB_G(src_plt[i]);
        *bgr_ptr++ = FXARGB_R(src_plt[i]);
      }
      bgr_ptr = (uint8_t*)plt;
    }
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    pIccModule->TranslateScanline(pIccTransform, (uint8_t*)plt,
                                  (const uint8_t*)plt, plt_size);
    for (int i = 0; i < plt_size; i++) {
      dst_plt[i] = FXARGB_MAKE(0xff, bgr_ptr[2], bgr_ptr[1], bgr_ptr[0]);
      bgr_ptr += 3;
    }
  } else {
    if (pSrcBitmap->IsCmykImage()) {
      for (int i = 0; i < plt_size; i++) {
        uint8_t r, g, b;
        AdobeCMYK_to_sRGB1(
            FXSYS_GetCValue(src_plt[i]), FXSYS_GetMValue(src_plt[i]),
            FXSYS_GetYValue(src_plt[i]), FXSYS_GetKValue(src_plt[i]), r, g, b);
        dst_plt[i] = FXARGB_MAKE(0xff, r, g, b);
      }
    } else {
      FXSYS_memcpy(dst_plt, src_plt, plt_size * 4);
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_Rgb2PltRgb8_NoTransform(uint8_t* dest_buf,
                                              int dest_pitch,
                                              int width,
                                              int height,
                                              const CFX_DIBSource* pSrcBitmap,
                                              int src_left,
                                              int src_top,
                                              uint32_t* dst_plt) {
  int bpp = pSrcBitmap->GetBPP() / 8;
  int row, col;
  CFX_Palette palette;
  palette.BuildPalette(pSrcBitmap);
  uint32_t* cLut = palette.GetColorLut();
  uint32_t* aLut = palette.GetAmountLut();
  if (!cLut || !aLut) {
    return FALSE;
  }
  int lut = palette.Getlut();
  uint32_t* pPalette = palette.GetPalette();
  if (lut > 256) {
    int err, min_err;
    int lut_256 = lut - 256;
    for (row = 0; row < lut_256; row++) {
      min_err = 1000000;
      uint8_t r, g, b;
      _ColorDecode(cLut[row], r, g, b);
      int clrindex = 0;
      for (int col = 0; col < 256; col++) {
        uint32_t p_color = *(pPalette + col);
        int d_r = r - (uint8_t)(p_color >> 16);
        int d_g = g - (uint8_t)(p_color >> 8);
        int d_b = b - (uint8_t)(p_color);
        err = d_r * d_r + d_g * d_g + d_b * d_b;
        if (err < min_err) {
          min_err = err;
          clrindex = col;
        }
      }
      aLut[row] = clrindex;
    }
  }
  int32_t lut_1 = lut - 1;
  for (row = 0; row < height; row++) {
    uint8_t* src_scan =
        (uint8_t*)pSrcBitmap->GetScanline(src_top + row) + src_left;
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    for (col = 0; col < width; col++) {
      uint8_t* src_port = src_scan + col * bpp;
      int r = src_port[2] & 0xf0;
      int g = src_port[1] & 0xf0;
      int b = src_port[0] & 0xf0;
      uint32_t clrindex = (r << 4) + g + (b >> 4);
      for (int i = lut_1; i >= 0; i--)
        if (clrindex == cLut[i]) {
          *(dest_scan + col) = (uint8_t)(aLut[i]);
          break;
        }
    }
  }
  FXSYS_memcpy(dst_plt, pPalette, sizeof(uint32_t) * 256);
  return TRUE;
}
FX_BOOL ConvertBuffer_Rgb2PltRgb8(uint8_t* dest_buf,
                                  int dest_pitch,
                                  int width,
                                  int height,
                                  const CFX_DIBSource* pSrcBitmap,
                                  int src_left,
                                  int src_top,
                                  uint32_t* dst_plt,
                                  void* pIccTransform) {
  FX_BOOL ret = ConvertBuffer_Rgb2PltRgb8_NoTransform(
      dest_buf, dest_pitch, width, height, pSrcBitmap, src_left, src_top,
      dst_plt);
  if (ret && pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    for (int i = 0; i < 256; i++) {
      FX_ARGB* plt = dst_plt + i;
      FX_ARGB plt_entry = FXARGB_TODIB(*plt);
      pIccModule->TranslateScanline(pIccTransform, (uint8_t*)&plt_entry,
                                    (const uint8_t*)&plt_entry, 1);
      *plt = FXARGB_TODIB(plt_entry);
    }
  }
  return ret;
}
FX_BOOL ConvertBuffer_1bppMask2Rgb(FXDIB_Format dst_format,
                                   uint8_t* dest_buf,
                                   int dest_pitch,
                                   int width,
                                   int height,
                                   const CFX_DIBSource* pSrcBitmap,
                                   int src_left,
                                   int src_top) {
  int comps = (dst_format & 0xff) / 8;
  uint8_t set_gray, reset_gray;
  set_gray = 0xff;
  reset_gray = 0x00;
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
    for (int col = src_left; col < src_left + width; col++) {
      if (src_scan[col / 8] & (1 << (7 - col % 8))) {
        dest_scan[0] = set_gray;
        dest_scan[1] = set_gray;
        dest_scan[2] = set_gray;
      } else {
        dest_scan[0] = reset_gray;
        dest_scan[1] = reset_gray;
        dest_scan[2] = reset_gray;
      }
      dest_scan += comps;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_8bppMask2Rgb(FXDIB_Format dst_format,
                                   uint8_t* dest_buf,
                                   int dest_pitch,
                                   int width,
                                   int height,
                                   const CFX_DIBSource* pSrcBitmap,
                                   int src_left,
                                   int src_top) {
  int comps = (dst_format & 0xff) / 8;
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row) + src_left;
    uint8_t src_pixel;
    for (int col = 0; col < width; col++) {
      src_pixel = *src_scan++;
      *dest_scan++ = src_pixel;
      *dest_scan++ = src_pixel;
      *dest_scan = src_pixel;
      dest_scan += comps - 2;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_1bppPlt2Rgb(FXDIB_Format dst_format,
                                  uint8_t* dest_buf,
                                  int dest_pitch,
                                  int width,
                                  int height,
                                  const CFX_DIBSource* pSrcBitmap,
                                  int src_left,
                                  int src_top,
                                  void* pIccTransform) {
  int comps = (dst_format & 0xff) / 8;
  uint32_t* src_plt = pSrcBitmap->GetPalette();
  uint32_t plt[2];
  uint8_t* bgr_ptr = (uint8_t*)plt;
  if (pSrcBitmap->IsCmykImage()) {
    plt[0] = FXCMYK_TODIB(src_plt[0]);
    plt[1] = FXCMYK_TODIB(src_plt[1]);
  } else {
    bgr_ptr[0] = FXARGB_B(src_plt[0]);
    bgr_ptr[1] = FXARGB_G(src_plt[0]);
    bgr_ptr[2] = FXARGB_R(src_plt[0]);
    bgr_ptr[3] = FXARGB_B(src_plt[1]);
    bgr_ptr[4] = FXARGB_G(src_plt[1]);
    bgr_ptr[5] = FXARGB_R(src_plt[1]);
  }
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    pIccModule->TranslateScanline(pIccTransform, (uint8_t*)plt,
                                  (const uint8_t*)plt, 2);
  } else {
    if (pSrcBitmap->IsCmykImage()) {
      AdobeCMYK_to_sRGB1(
          FXSYS_GetCValue(src_plt[0]), FXSYS_GetMValue(src_plt[0]),
          FXSYS_GetYValue(src_plt[0]), FXSYS_GetKValue(src_plt[0]), bgr_ptr[2],
          bgr_ptr[1], bgr_ptr[0]);
      AdobeCMYK_to_sRGB1(
          FXSYS_GetCValue(src_plt[1]), FXSYS_GetMValue(src_plt[1]),
          FXSYS_GetYValue(src_plt[1]), FXSYS_GetKValue(src_plt[1]), bgr_ptr[5],
          bgr_ptr[4], bgr_ptr[3]);
    }
  }
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
    for (int col = src_left; col < src_left + width; col++) {
      if (src_scan[col / 8] & (1 << (7 - col % 8))) {
        *dest_scan++ = bgr_ptr[3];
        *dest_scan++ = bgr_ptr[4];
        *dest_scan = bgr_ptr[5];
      } else {
        *dest_scan++ = bgr_ptr[0];
        *dest_scan++ = bgr_ptr[1];
        *dest_scan = bgr_ptr[2];
      }
      dest_scan += comps - 2;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_8bppPlt2Rgb(FXDIB_Format dst_format,
                                  uint8_t* dest_buf,
                                  int dest_pitch,
                                  int width,
                                  int height,
                                  const CFX_DIBSource* pSrcBitmap,
                                  int src_left,
                                  int src_top,
                                  void* pIccTransform) {
  int comps = (dst_format & 0xff) / 8;
  uint32_t* src_plt = pSrcBitmap->GetPalette();
  uint32_t plt[256];
  uint8_t* bgr_ptr = (uint8_t*)plt;
  if (!pSrcBitmap->IsCmykImage()) {
    for (int i = 0; i < 256; i++) {
      *bgr_ptr++ = FXARGB_B(src_plt[i]);
      *bgr_ptr++ = FXARGB_G(src_plt[i]);
      *bgr_ptr++ = FXARGB_R(src_plt[i]);
    }
    bgr_ptr = (uint8_t*)plt;
  }
  if (pIccTransform) {
    if (pSrcBitmap->IsCmykImage()) {
      for (int i = 0; i < 256; i++) {
        plt[i] = FXCMYK_TODIB(src_plt[i]);
      }
    }
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    pIccModule->TranslateScanline(pIccTransform, (uint8_t*)plt,
                                  (const uint8_t*)plt, 256);
  } else {
    if (pSrcBitmap->IsCmykImage()) {
      for (int i = 0; i < 256; i++) {
        AdobeCMYK_to_sRGB1(
            FXSYS_GetCValue(src_plt[i]), FXSYS_GetMValue(src_plt[i]),
            FXSYS_GetYValue(src_plt[i]), FXSYS_GetKValue(src_plt[i]),
            bgr_ptr[2], bgr_ptr[1], bgr_ptr[0]);
        bgr_ptr += 3;
      }
      bgr_ptr = (uint8_t*)plt;
    }
  }
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row) + src_left;
    for (int col = 0; col < width; col++) {
      uint8_t* src_pixel = bgr_ptr + 3 * (*src_scan++);
      *dest_scan++ = *src_pixel++;
      *dest_scan++ = *src_pixel++;
      *dest_scan = *src_pixel++;
      dest_scan += comps - 2;
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_24bppRgb2Rgb24(uint8_t* dest_buf,
                                     int dest_pitch,
                                     int width,
                                     int height,
                                     const CFX_DIBSource* pSrcBitmap,
                                     int src_left,
                                     int src_top,
                                     void* pIccTransform) {
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 3;
      pIccModule->TranslateScanline(pIccTransform, dest_scan, src_scan, width);
    }
  } else {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 3;
      FXSYS_memcpy(dest_scan, src_scan, width * 3);
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_32bppRgb2Rgb24(uint8_t* dest_buf,
                                     int dest_pitch,
                                     int width,
                                     int height,
                                     const CFX_DIBSource* pSrcBitmap,
                                     int src_left,
                                     int src_top,
                                     void* pIccTransform) {
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan = dest_buf + row * dest_pitch;
    const uint8_t* src_scan =
        pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
    for (int col = 0; col < width; col++) {
      *dest_scan++ = *src_scan++;
      *dest_scan++ = *src_scan++;
      *dest_scan++ = *src_scan++;
      src_scan++;
    }
  }
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      pIccModule->TranslateScanline(pIccTransform, dest_scan, dest_scan, width);
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_Rgb2Rgb32(uint8_t* dest_buf,
                                int dest_pitch,
                                int width,
                                int height,
                                const CFX_DIBSource* pSrcBitmap,
                                int src_left,
                                int src_top,
                                void* pIccTransform) {
  int comps = pSrcBitmap->GetBPP() / 8;
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * comps;
      for (int col = 0; col < width; col++) {
        pIccModule->TranslateScanline(pIccTransform, dest_scan, src_scan, 1);
        dest_scan += 4;
        src_scan += comps;
      }
    }
  } else {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * comps;
      for (int col = 0; col < width; col++) {
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        *dest_scan++ = *src_scan++;
        dest_scan++;
        src_scan += comps - 3;
      }
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer_32bppCmyk2Rgb32(uint8_t* dest_buf,
                                      int dest_pitch,
                                      int width,
                                      int height,
                                      const CFX_DIBSource* pSrcBitmap,
                                      int src_left,
                                      int src_top,
                                      void* pIccTransform) {
  if (pIccTransform) {
    ICodec_IccModule* pIccModule =
        CFX_GEModule::Get()->GetCodecModule()->GetIccModule();
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
      for (int col = 0; col < width; col++) {
        pIccModule->TranslateScanline(pIccTransform, dest_scan, src_scan, 1);
        dest_scan += 4;
        src_scan += 4;
      }
    }
  } else {
    for (int row = 0; row < height; row++) {
      uint8_t* dest_scan = dest_buf + row * dest_pitch;
      const uint8_t* src_scan =
          pSrcBitmap->GetScanline(src_top + row) + src_left * 4;
      for (int col = 0; col < width; col++) {
        AdobeCMYK_to_sRGB1(src_scan[0], src_scan[1], src_scan[2], src_scan[3],
                           dest_scan[2], dest_scan[1], dest_scan[0]);
        dest_scan += 4;
        src_scan += 4;
      }
    }
  }
  return TRUE;
}
FX_BOOL ConvertBuffer(FXDIB_Format dest_format,
                      uint8_t* dest_buf,
                      int dest_pitch,
                      int width,
                      int height,
                      const CFX_DIBSource* pSrcBitmap,
                      int src_left,
                      int src_top,
                      uint32_t*& d_pal,
                      void* pIccTransform) {
  FXDIB_Format src_format = pSrcBitmap->GetFormat();
  if (!CFX_GEModule::Get()->GetCodecModule() ||
      !CFX_GEModule::Get()->GetCodecModule()->GetIccModule()) {
    pIccTransform = NULL;
  }
  switch (dest_format) {
    case FXDIB_Invalid:
    case FXDIB_1bppCmyk:
    case FXDIB_1bppMask:
    case FXDIB_1bppRgb:
      ASSERT(FALSE);
      return FALSE;
    case FXDIB_8bppMask: {
      if ((src_format & 0xff) == 1) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_1bppPlt2Gray(dest_buf, dest_pitch, width, height,
                                            pSrcBitmap, src_left, src_top,
                                            pIccTransform);
        }
        return ConvertBuffer_1bppMask2Gray(dest_buf, dest_pitch, width, height,
                                           pSrcBitmap, src_left, src_top);
      }
      if ((src_format & 0xff) == 8) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_8bppPlt2Gray(dest_buf, dest_pitch, width, height,
                                            pSrcBitmap, src_left, src_top,
                                            pIccTransform);
        }
        return ConvertBuffer_8bppMask2Gray(dest_buf, dest_pitch, width, height,
                                           pSrcBitmap, src_left, src_top);
      }
      if ((src_format & 0xff) >= 24) {
        return ConvertBuffer_RgbOrCmyk2Gray(dest_buf, dest_pitch, width, height,
                                            pSrcBitmap, src_left, src_top,
                                            pIccTransform);
      }
      return FALSE;
    }
    case FXDIB_8bppRgb:
    case FXDIB_8bppRgba: {
      if ((src_format & 0xff) == 8 && !pSrcBitmap->GetPalette()) {
        return ConvertBuffer(FXDIB_8bppMask, dest_buf, dest_pitch, width,
                             height, pSrcBitmap, src_left, src_top, d_pal,
                             pIccTransform);
      }
      d_pal = FX_Alloc(uint32_t, 256);
      if (((src_format & 0xff) == 1 || (src_format & 0xff) == 8) &&
          pSrcBitmap->GetPalette()) {
        return ConvertBuffer_Plt2PltRgb8(dest_buf, dest_pitch, width, height,
                                         pSrcBitmap, src_left, src_top, d_pal,
                                         pIccTransform);
      }
      if ((src_format & 0xff) >= 24) {
        return ConvertBuffer_Rgb2PltRgb8(dest_buf, dest_pitch, width, height,
                                         pSrcBitmap, src_left, src_top, d_pal,
                                         pIccTransform);
      }
      return FALSE;
    }
    case FXDIB_Rgb:
    case FXDIB_Rgba: {
      if ((src_format & 0xff) == 1) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_1bppPlt2Rgb(dest_format, dest_buf, dest_pitch,
                                           width, height, pSrcBitmap, src_left,
                                           src_top, pIccTransform);
        }
        return ConvertBuffer_1bppMask2Rgb(dest_format, dest_buf, dest_pitch,
                                          width, height, pSrcBitmap, src_left,
                                          src_top);
      }
      if ((src_format & 0xff) == 8) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_8bppPlt2Rgb(dest_format, dest_buf, dest_pitch,
                                           width, height, pSrcBitmap, src_left,
                                           src_top, pIccTransform);
        }
        return ConvertBuffer_8bppMask2Rgb(dest_format, dest_buf, dest_pitch,
                                          width, height, pSrcBitmap, src_left,
                                          src_top);
      }
      if ((src_format & 0xff) == 24) {
        return ConvertBuffer_24bppRgb2Rgb24(dest_buf, dest_pitch, width, height,
                                            pSrcBitmap, src_left, src_top,
                                            pIccTransform);
      }
      if ((src_format & 0xff) == 32) {
        return ConvertBuffer_32bppRgb2Rgb24(dest_buf, dest_pitch, width, height,
                                            pSrcBitmap, src_left, src_top,
                                            pIccTransform);
      }
      return FALSE;
    }
    case FXDIB_Argb:
    case FXDIB_Rgb32: {
      if ((src_format & 0xff) == 1) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_1bppPlt2Rgb(dest_format, dest_buf, dest_pitch,
                                           width, height, pSrcBitmap, src_left,
                                           src_top, pIccTransform);
        }
        return ConvertBuffer_1bppMask2Rgb(dest_format, dest_buf, dest_pitch,
                                          width, height, pSrcBitmap, src_left,
                                          src_top);
      }
      if ((src_format & 0xff) == 8) {
        if (pSrcBitmap->GetPalette()) {
          return ConvertBuffer_8bppPlt2Rgb(dest_format, dest_buf, dest_pitch,
                                           width, height, pSrcBitmap, src_left,
                                           src_top, pIccTransform);
        }
        return ConvertBuffer_8bppMask2Rgb(dest_format, dest_buf, dest_pitch,
                                          width, height, pSrcBitmap, src_left,
                                          src_top);
      }
      if ((src_format & 0xff) >= 24) {
        if (src_format & 0x0400) {
          return ConvertBuffer_32bppCmyk2Rgb32(dest_buf, dest_pitch, width,
                                               height, pSrcBitmap, src_left,
                                               src_top, pIccTransform);
        }
        return ConvertBuffer_Rgb2Rgb32(dest_buf, dest_pitch, width, height,
                                       pSrcBitmap, src_left, src_top,
                                       pIccTransform);
      }
      return FALSE;
    }
    default:
      return FALSE;
  }
}
CFX_DIBitmap* CFX_DIBSource::CloneConvert(FXDIB_Format dest_format,
                                          const FX_RECT* pClip,
                                          void* pIccTransform) const {
  if (dest_format == GetFormat() && !pIccTransform) {
    return Clone(pClip);
  }
  if (pClip) {
    CFX_DIBitmap* pClone = Clone(pClip);
    if (!pClone) {
      return NULL;
    }
    if (!pClone->ConvertFormat(dest_format, pIccTransform)) {
      delete pClone;
      return NULL;
    }
    return pClone;
  }
  CFX_DIBitmap* pClone = new CFX_DIBitmap;
  if (!pClone->Create(m_Width, m_Height, dest_format)) {
    delete pClone;
    return NULL;
  }
  FX_BOOL ret = TRUE;
  CFX_DIBitmap* pSrcAlpha = NULL;
  if (HasAlpha()) {
    pSrcAlpha = (GetFormat() == FXDIB_Argb) ? GetAlphaMask() : m_pAlphaMask;
    if (!pSrcAlpha) {
      delete pClone;
      return NULL;
    }
  }
  if (dest_format & 0x0200) {
    if (dest_format == FXDIB_Argb) {
      ret = pSrcAlpha ? pClone->LoadChannel(FXDIB_Alpha, pSrcAlpha, FXDIB_Alpha)
                      : pClone->LoadChannel(FXDIB_Alpha, 0xff);
    } else {
      ret = pClone->CopyAlphaMask(pSrcAlpha);
    }
  }
  if (pSrcAlpha && pSrcAlpha != m_pAlphaMask) {
    delete pSrcAlpha;
    pSrcAlpha = NULL;
  }
  if (!ret) {
    delete pClone;
    return NULL;
  }
  uint32_t* pal_8bpp = NULL;
  ret = ConvertBuffer(dest_format, pClone->GetBuffer(), pClone->GetPitch(),
                      m_Width, m_Height, this, 0, 0, pal_8bpp, pIccTransform);
  if (!ret) {
    FX_Free(pal_8bpp);
    delete pClone;
    return NULL;
  }
  if (pal_8bpp) {
    pClone->CopyPalette(pal_8bpp);
    FX_Free(pal_8bpp);
    pal_8bpp = NULL;
  }
  return pClone;
}
FX_BOOL CFX_DIBitmap::ConvertFormat(FXDIB_Format dest_format,
                                    void* pIccTransform) {
  FXDIB_Format src_format = GetFormat();
  if (dest_format == src_format && !pIccTransform) {
    return TRUE;
  }
  if (dest_format == FXDIB_8bppMask && src_format == FXDIB_8bppRgb &&
      !m_pPalette) {
    m_AlphaFlag = 1;
    return TRUE;
  }
  if (dest_format == FXDIB_Argb && src_format == FXDIB_Rgb32 &&
      !pIccTransform) {
    m_AlphaFlag = 2;
    for (int row = 0; row < m_Height; row++) {
      uint8_t* scanline = m_pBuffer + row * m_Pitch + 3;
      for (int col = 0; col < m_Width; col++) {
        *scanline = 0xff;
        scanline += 4;
      }
    }
    return TRUE;
  }
  int dest_bpp = dest_format & 0xff;
  int dest_pitch = (dest_bpp * m_Width + 31) / 32 * 4;
  uint8_t* dest_buf = FX_TryAlloc(uint8_t, dest_pitch * m_Height + 4);
  if (!dest_buf) {
    return FALSE;
  }
  CFX_DIBitmap* pAlphaMask = NULL;
  if (dest_format == FXDIB_Argb) {
    FXSYS_memset(dest_buf, 0xff, dest_pitch * m_Height + 4);
    if (m_pAlphaMask) {
      for (int row = 0; row < m_Height; row++) {
        uint8_t* pDstScanline = dest_buf + row * dest_pitch + 3;
        const uint8_t* pSrcScanline = m_pAlphaMask->GetScanline(row);
        for (int col = 0; col < m_Width; col++) {
          *pDstScanline = *pSrcScanline++;
          pDstScanline += 4;
        }
      }
    }
  } else if (dest_format & 0x0200) {
    if (src_format == FXDIB_Argb) {
      pAlphaMask = GetAlphaMask();
      if (!pAlphaMask) {
        FX_Free(dest_buf);
        return FALSE;
      }
    } else {
      if (!m_pAlphaMask) {
        if (!BuildAlphaMask()) {
          FX_Free(dest_buf);
          return FALSE;
        }
        pAlphaMask = m_pAlphaMask;
        m_pAlphaMask = NULL;
      } else {
        pAlphaMask = m_pAlphaMask;
      }
    }
  }
  FX_BOOL ret = FALSE;
  uint32_t* pal_8bpp = NULL;
  ret = ConvertBuffer(dest_format, dest_buf, dest_pitch, m_Width, m_Height,
                      this, 0, 0, pal_8bpp, pIccTransform);
  if (!ret) {
    FX_Free(pal_8bpp);
    if (pAlphaMask != m_pAlphaMask) {
      delete pAlphaMask;
    }
    FX_Free(dest_buf);
    return FALSE;
  }
  if (m_pAlphaMask && pAlphaMask != m_pAlphaMask) {
    delete m_pAlphaMask;
  }
  m_pAlphaMask = pAlphaMask;
  FX_Free(m_pPalette);
  m_pPalette = pal_8bpp;
  if (!m_bExtBuf) {
    FX_Free(m_pBuffer);
  }
  m_bExtBuf = FALSE;
  m_pBuffer = dest_buf;
  m_bpp = (uint8_t)dest_format;
  m_AlphaFlag = (uint8_t)(dest_format >> 8);
  m_Pitch = dest_pitch;
  return TRUE;
}
