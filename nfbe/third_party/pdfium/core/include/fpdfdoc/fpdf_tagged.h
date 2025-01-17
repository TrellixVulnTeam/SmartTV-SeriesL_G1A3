// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_INCLUDE_FPDFDOC_FPDF_TAGGED_H_
#define CORE_INCLUDE_FPDFDOC_FPDF_TAGGED_H_

#include "core/include/fxge/fx_dib.h"

class CPDF_Document;
class CPDF_StructElement;
class CPDF_StructTree;
struct CPDF_StructKid;

class CPDF_StructTree {
 public:
  static CPDF_StructTree* LoadDoc(const CPDF_Document* pDoc);

  static CPDF_StructTree* LoadPage(const CPDF_Document* pDoc,
                                   const CPDF_Dictionary* pPageDict);

  virtual ~CPDF_StructTree() {}

  virtual int CountTopElements() const = 0;

  virtual CPDF_StructElement* GetTopElement(int i) const = 0;
};
struct CPDF_StructKid {
  enum { Invalid, Element, PageContent, StreamContent, Object } m_Type;

  union {
    struct {
      CPDF_StructElement* m_pElement;

      CPDF_Dictionary* m_pDict;
    } m_Element;
    struct {
      uint32_t m_PageObjNum;

      uint32_t m_ContentId;
    } m_PageContent;
    struct {
      uint32_t m_PageObjNum;

      uint32_t m_ContentId;

      uint32_t m_RefObjNum;
    } m_StreamContent;
    struct {
      uint32_t m_PageObjNum;

      uint32_t m_RefObjNum;
    } m_Object;
  };
};
class CPDF_StructElement {
 public:
  virtual ~CPDF_StructElement() {}

  virtual CPDF_StructTree* GetTree() const = 0;

  virtual const CFX_ByteString& GetType() const = 0;

  virtual CPDF_StructElement* GetParent() const = 0;

  virtual CPDF_Dictionary* GetDict() const = 0;

  virtual int CountKids() const = 0;

  virtual const CPDF_StructKid& GetKid(int index) const = 0;

  virtual CPDF_Object* GetAttr(const CFX_ByteStringC& owner,
                               const CFX_ByteStringC& name,
                               FX_BOOL bInheritable = FALSE,
                               FX_FLOAT fLevel = 0.0F) = 0;

  virtual CFX_ByteString GetName(const CFX_ByteStringC& owner,
                                 const CFX_ByteStringC& name,
                                 const CFX_ByteStringC& default_value,
                                 FX_BOOL bInheritable = FALSE,
                                 int subindex = -1) = 0;

  virtual FX_ARGB GetColor(const CFX_ByteStringC& owner,
                           const CFX_ByteStringC& name,
                           FX_ARGB default_value,
                           FX_BOOL bInheritable = FALSE,
                           int subindex = -1) = 0;

  virtual FX_FLOAT GetNumber(const CFX_ByteStringC& owner,
                             const CFX_ByteStringC& name,
                             FX_FLOAT default_value,
                             FX_BOOL bInheritable = FALSE,
                             int subindex = -1) = 0;

  virtual int GetInteger(const CFX_ByteStringC& owner,
                         const CFX_ByteStringC& name,
                         int default_value,
                         FX_BOOL bInheritable = FALSE,
                         int subindex = -1) = 0;
};

#endif  // CORE_INCLUDE_FPDFDOC_FPDF_TAGGED_H_
