// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_INCLUDE_FSDK_DEFINE_H_
#define FPDFSDK_INCLUDE_FSDK_DEFINE_H_

#include "core/fpdfapi/fpdf_parser/include/cpdf_parser.h"
#include "core/include/fpdfdoc/fpdf_doc.h"
#include "core/include/fxge/fx_ge.h"
#include "core/include/fxge/fx_ge_win32.h"
#include "public/fpdfview.h"

#ifdef PDF_ENABLE_XFA
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_doc.h"
#endif  // PDF_ENABLE_XFA

#ifdef _WIN32
#include <math.h>
#include <tchar.h>
#endif

// Convert a #FX_ARGB to a #FX_COLORREF.
#define FX_ARGBTOCOLORREF(argb)                                            \
  ((((uint32_t)argb & 0x00FF0000) >> 16) | ((uint32_t)argb & 0x0000FF00) | \
   (((uint32_t)argb & 0x000000FF) << 16))

// Convert a #FX_COLORREF to a #FX_ARGB.
#define FX_COLORREFTOARGB(rgb)                                   \
  ((uint32_t)0xFF000000 | (((uint32_t)rgb & 0x000000FF) << 16) | \
   ((uint32_t)rgb & 0x0000FF00) | (((uint32_t)rgb & 0x00FF0000) >> 16))

typedef unsigned int FX_UINT;
class CRenderContext;
class IFSDK_PAUSE_Adapter;

class CPDF_CustomAccess final : public IFX_FileRead {
 public:
  explicit CPDF_CustomAccess(FPDF_FILEACCESS* pFileAccess);
  ~CPDF_CustomAccess() override {}

  // IFX_FileRead
  FX_FILESIZE GetSize() override { return m_FileAccess.m_FileLen; }
  void Release() override { delete this; }
  FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size) override;

#ifdef PDF_ENABLE_XFA
  virtual CFX_ByteString GetFullPath() { return ""; }
  virtual FX_BOOL GetByte(uint32_t pos, uint8_t& ch);
  virtual FX_BOOL GetBlock(uint32_t pos, uint8_t* pBuf, uint32_t size);
#endif  // PDF_ENABLE_XFA

 private:
  FPDF_FILEACCESS m_FileAccess;
#ifdef PDF_ENABLE_XFA
  uint8_t m_Buffer[512];
  uint32_t m_BufferOffset;
#endif  // PDF_ENABLE_XFA
};

#ifdef PDF_ENABLE_XFA
class CFPDF_FileStream : public IFX_FileStream {
 public:
  CFPDF_FileStream(FPDF_FILEHANDLER* pFS);
  virtual ~CFPDF_FileStream() {}

  virtual IFX_FileStream* Retain();
  virtual void Release();

  virtual FX_FILESIZE GetSize();
  virtual FX_BOOL IsEOF();
  virtual FX_FILESIZE GetPosition() { return m_nCurPos; }
  virtual void SetPosition(FX_FILESIZE pos) { m_nCurPos = pos; }
  virtual FX_BOOL ReadBlock(void* buffer, FX_FILESIZE offset, size_t size);
  virtual size_t ReadBlock(void* buffer, size_t size);
  virtual FX_BOOL WriteBlock(const void* buffer,
                             FX_FILESIZE offset,
                             size_t size);
  virtual FX_BOOL Flush();

 protected:
  FPDF_FILEHANDLER* m_pFS;
  FX_FILESIZE m_nCurPos;
};
#endif  // PDF_ENABLE_XFA

// Object types for public FPDF_ types; these correspond to next layer down
// from fpdfsdk. For master, these are CPDF_ types, but for XFA, these are
// CPDFXFA_ types.
#ifndef PDF_ENABLE_XFA
using UnderlyingDocumentType = CPDF_Document;
using UnderlyingPageType = CPDF_Page;
#else   // PDF_ENABLE_XFA
using UnderlyingDocumentType = CPDFXFA_Document;
using UnderlyingPageType = CPDFXFA_Page;
#endif  // PDF_ENABLE_XFA

// Conversions to/from underlying types.
UnderlyingDocumentType* UnderlyingFromFPDFDocument(FPDF_DOCUMENT doc);
FPDF_DOCUMENT FPDFDocumentFromUnderlying(UnderlyingDocumentType* doc);

UnderlyingPageType* UnderlyingFromFPDFPage(FPDF_PAGE page);

// Conversions to/from FPDF_ types.
CPDF_Document* CPDFDocumentFromFPDFDocument(FPDF_DOCUMENT doc);
FPDF_DOCUMENT FPDFDocumentFromCPDFDocument(CPDF_Document* doc);

CPDF_Page* CPDFPageFromFPDFPage(FPDF_PAGE page);

void DropContext(void* data);
void FSDK_SetSandBoxPolicy(FPDF_DWORD policy, FPDF_BOOL enable);
FPDF_BOOL FSDK_IsSandBoxPolicyEnabled(FPDF_DWORD policy);
void FPDF_RenderPage_Retail(CRenderContext* pContext,
                            FPDF_PAGE page,
                            int start_x,
                            int start_y,
                            int size_x,
                            int size_y,
                            int rotate,
                            int flags,
                            FX_BOOL bNeedToRestore,
                            IFSDK_PAUSE_Adapter* pause);

void CheckUnSupportError(CPDF_Document* pDoc, uint32_t err_code);
void CheckUnSupportAnnot(CPDF_Document* pDoc, const CPDF_Annot* pPDFAnnot);
void ProcessParseError(CPDF_Parser::Error err);

#endif  // FPDFSDK_INCLUDE_FSDK_DEFINE_H_
