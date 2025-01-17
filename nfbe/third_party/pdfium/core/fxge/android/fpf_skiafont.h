// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_FPF_SKIAFONT_H_
#define CORE_FXGE_ANDROID_FPF_SKIAFONT_H_

#include "core/fxcrt/include/fx_system.h"

#if _FX_OS_ == _FX_ANDROID_

#include "core/include/fxge/fpf.h"
#include "core/include/fxge/fx_font.h"

class CFPF_SkiaFontDescriptor;
class CFPF_SkiaFontMgr;
class CFPF_SkiaFont : public IFPF_Font {
 public:
  CFPF_SkiaFont();
  ~CFPF_SkiaFont() override;

  // IFPF_Font
  void Release() override;
  IFPF_Font* Retain() override;
  FPF_HFONT GetHandle() override;
  CFX_ByteString GetFamilyName() override;
  CFX_WideString GetPsName() override;
  uint32_t GetFontStyle() const override { return m_dwStyle; }
  uint8_t GetCharset() const override { return m_uCharset; }
  int32_t GetGlyphIndex(FX_WCHAR wUnicode) override;
  int32_t GetGlyphWidth(int32_t iGlyphIndex) override;
  int32_t GetAscent() const override;
  int32_t GetDescent() const override;
  FX_BOOL GetGlyphBBox(int32_t iGlyphIndex, FX_RECT& rtBBox) override;
  FX_BOOL GetBBox(FX_RECT& rtBBox) override;
  int32_t GetHeight() const override;
  int32_t GetItalicAngle() const override;
  uint32_t GetFontData(uint32_t dwTable,
                       uint8_t* pBuffer,
                       uint32_t dwSize) override;

  FX_BOOL InitFont(CFPF_SkiaFontMgr* pFontMgr,
                   CFPF_SkiaFontDescriptor* pFontDes,
                   const CFX_ByteStringC& bsFamily,
                   uint32_t dwStyle,
                   uint8_t uCharset);

 protected:
  CFPF_SkiaFontMgr* m_pFontMgr;
  CFPF_SkiaFontDescriptor* m_pFontDes;
  FXFT_Face m_Face;
  uint32_t m_dwStyle;
  uint8_t m_uCharset;
  uint32_t m_dwRefCount;
};
#endif

#endif  // CORE_FXGE_ANDROID_FPF_SKIAFONT_H_
