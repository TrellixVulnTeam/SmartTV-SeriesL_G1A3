// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CSS_FDE_CSSDECLARATION_H_
#define XFA_FDE_CSS_FDE_CSSDECLARATION_H_

#include "xfa/fde/css/fde_cssdatatable.h"

class FDE_CSSPropertyHolder : public CFX_Target {
 public:
  int16_t eProperty;
  int16_t bImportant;
  IFDE_CSSValue* pValue;
  FDE_CSSPropertyHolder* pNext;
};

class FDE_CSSCustomProperty : public CFX_Target {
 public:
  const FX_WCHAR* pwsName;
  const FX_WCHAR* pwsValue;
  FDE_CSSCustomProperty* pNext;
};

struct FDE_CSSPROPERTYARGS {
  IFX_MEMAllocator* pStaticStore;
  CFX_MapPtrToPtr* pStringCache;
  FDE_LPCCSSPROPERTYTABLE pProperty;
};

class CFDE_CSSDeclaration : public IFDE_CSSDeclaration, public CFX_Target {
 public:
  CFDE_CSSDeclaration()
      : m_pFirstProperty(NULL),
        m_pLastProperty(NULL),
        m_pFirstCustom(NULL),
        m_pLastCustom(NULL) {}
  virtual IFDE_CSSValue* GetProperty(FDE_CSSPROPERTY eProperty,
                                     FX_BOOL& bImportant) const;
  virtual FX_POSITION GetStartPosition() const;
  virtual void GetNextProperty(FX_POSITION& pos,
                               FDE_CSSPROPERTY& eProperty,
                               IFDE_CSSValue*& pValue,
                               FX_BOOL& bImportant) const;
  virtual FX_POSITION GetStartCustom() const;
  virtual void GetNextCustom(FX_POSITION& pos,
                             CFX_WideString& wsName,
                             CFX_WideString& wsValue) const;
  FX_BOOL AddProperty(const FDE_CSSPROPERTYARGS* pArgs,
                      const FX_WCHAR* pszValue,
                      int32_t iValueLen);
  FX_BOOL AddProperty(const FDE_CSSPROPERTYARGS* pArgs,
                      const FX_WCHAR* pszName,
                      int32_t iNameLen,
                      const FX_WCHAR* pszValue,
                      int32_t iValueLen);

 protected:
  FX_BOOL ParseTextEmphasisProperty(FDE_CSSPROPERTYARGS* pArgs,
                                    const FX_WCHAR* pszValue,
                                    int32_t iValueLen,
                                    FX_BOOL bImportant);
  FX_BOOL ParseColumnsProperty(const FDE_CSSPROPERTYARGS* pArgs,
                               const FX_WCHAR* pszValue,
                               int32_t iValueLen,
                               FX_BOOL bImportant);
  FX_BOOL ParseColumnRuleProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                  const FX_WCHAR* pszValue,
                                  int32_t iValueLen,
                                  FX_BOOL bImportant);
  FX_BOOL ParseOverflowProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                const FX_WCHAR* pszValue,
                                int32_t iValueLen,
                                FX_BOOL bImportant);
  FX_BOOL ParseFontProperty(const FDE_CSSPROPERTYARGS* pArgs,
                            const FX_WCHAR* pszValue,
                            int32_t iValueLen,
                            FX_BOOL bImportant);
  FX_BOOL ParseBackgroundProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                  const FX_WCHAR* pszValue,
                                  int32_t iValueLen,
                                  FX_BOOL bImportant);
  FX_BOOL ParseListStyleProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                 const FX_WCHAR* pszValue,
                                 int32_t iValueLen,
                                 FX_BOOL bImportant);
  FX_BOOL ParseBorderPropoerty(IFX_MEMAllocator* pStaticStore,
                               const FX_WCHAR* pszValue,
                               int32_t iValueLen,
                               IFDE_CSSValue*& pColor,
                               IFDE_CSSValue*& pStyle,
                               IFDE_CSSValue*& pWidth) const;
  void AddBorderProperty(IFX_MEMAllocator* pStaticStore,
                         IFDE_CSSValue* pColor,
                         IFDE_CSSValue* pStyle,
                         IFDE_CSSValue* pWidth,
                         FX_BOOL bImportant,
                         FDE_CSSPROPERTY eColor,
                         FDE_CSSPROPERTY eStyle,
                         FDE_CSSPROPERTY eWidth);
  FX_BOOL ParseContentProperty(const FDE_CSSPROPERTYARGS* pArgs,
                               const FX_WCHAR* pszValue,
                               int32_t iValueLen,
                               FX_BOOL bImportant);
  FX_BOOL ParseCounterProperty(const FDE_CSSPROPERTYARGS* pArgs,
                               const FX_WCHAR* pszValue,
                               int32_t iValueLen,
                               FX_BOOL bImportant);
  FX_BOOL ParseValueListProperty(const FDE_CSSPROPERTYARGS* pArgs,
                                 const FX_WCHAR* pszValue,
                                 int32_t iValueLen,
                                 FX_BOOL bImportant);
  FX_BOOL Add4ValuesProperty(IFX_MEMAllocator* pStaticStore,
                             const CFDE_CSSValueArray& list,
                             FX_BOOL bImportant,
                             FDE_CSSPROPERTY eLeft,
                             FDE_CSSPROPERTY eTop,
                             FDE_CSSPROPERTY eRight,
                             FDE_CSSPROPERTY eBottom);
  IFDE_CSSValue* ParseNumber(const FDE_CSSPROPERTYARGS* pArgs,
                             const FX_WCHAR* pszValue,
                             int32_t iValueLen);
  IFDE_CSSValue* ParseEnum(const FDE_CSSPROPERTYARGS* pArgs,
                           const FX_WCHAR* pszValue,
                           int32_t iValueLen);
  IFDE_CSSValue* ParseColor(const FDE_CSSPROPERTYARGS* pArgs,
                            const FX_WCHAR* pszValue,
                            int32_t iValueLen);
  IFDE_CSSValue* ParseURI(const FDE_CSSPROPERTYARGS* pArgs,
                          const FX_WCHAR* pszValue,
                          int32_t iValueLen);
  IFDE_CSSValue* ParseString(const FDE_CSSPROPERTYARGS* pArgs,
                             const FX_WCHAR* pszValue,
                             int32_t iValueLen);
  IFDE_CSSValue* ParseFunction(const FDE_CSSPROPERTYARGS* pArgs,
                               const FX_WCHAR* pszValue,
                               int32_t iValueLen);
  const FX_WCHAR* CopyToLocal(const FDE_CSSPROPERTYARGS* pArgs,
                              const FX_WCHAR* pszValue,
                              int32_t iValueLen);
  void AddPropertyHolder(IFX_MEMAllocator* pStaticStore,
                         FDE_CSSPROPERTY eProperty,
                         IFDE_CSSValue* pValue,
                         FX_BOOL bImportant);
  IFDE_CSSPrimitiveValue* NewNumberValue(IFX_MEMAllocator* pStaticStore,
                                         FDE_CSSPRIMITIVETYPE eUnit,
                                         FX_FLOAT fValue) const;
  IFDE_CSSPrimitiveValue* NewEnumValue(IFX_MEMAllocator* pStaticStore,
                                       FDE_CSSPROPERTYVALUE eValue) const;
  FDE_CSSPropertyHolder* m_pFirstProperty;
  FDE_CSSPropertyHolder* m_pLastProperty;
  FDE_CSSCustomProperty* m_pFirstCustom;
  FDE_CSSCustomProperty* m_pLastCustom;
};

#endif  // XFA_FDE_CSS_FDE_CSSDECLARATION_H_
