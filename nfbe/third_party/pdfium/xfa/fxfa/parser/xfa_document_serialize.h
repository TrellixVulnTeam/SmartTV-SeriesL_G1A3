// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_DOCUMENT_SERIALIZE_H_
#define XFA_FXFA_PARSER_XFA_DOCUMENT_SERIALIZE_H_

#include "xfa/fxfa/parser/xfa_docdata.h"

class CXFA_DataImporter {
 public:
  explicit CXFA_DataImporter(CXFA_Document* pDocument);

  void Release() { delete this; }
  FX_BOOL ImportData(IFX_FileRead* pDataDocument);

 protected:
  CXFA_Document* const m_pDocument;
};

class CXFA_DataExporter {
 public:
  explicit CXFA_DataExporter(CXFA_Document* pDocument);

  void Release() { delete this; }
  FX_BOOL Export(IFX_FileWrite* pWrite);
  FX_BOOL Export(IFX_FileWrite* pWrite,
                 CXFA_Node* pNode,
                 uint32_t dwFlag = 0,
                 const FX_CHAR* pChecksum = NULL);

 protected:
  FX_BOOL Export(IFX_Stream* pStream,
                 CXFA_Node* pNode,
                 uint32_t dwFlag,
                 const FX_CHAR* pChecksum);
  CXFA_Document* const m_pDocument;
};

#endif  // XFA_FXFA_PARSER_XFA_DOCUMENT_SERIALIZE_H_
