// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_searchex.h"

#include "core/fpdftext/include/ipdf_textpage.h"

DLLEXPORT int STDCALL
FPDFText_GetCharIndexFromTextIndex(FPDF_TEXTPAGE text_page, int nTextIndex) {
  if (!text_page)
    return -1;
  return ((IPDF_TextPage*)text_page)->CharIndexFromTextIndex(nTextIndex);
}
