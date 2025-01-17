// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CSS_FDE_CSSCACHE_H_
#define XFA_FDE_CSS_FDE_CSSCACHE_H_

#include <map>

#include "xfa/fde/css/fde_css.h"
#include "xfa/fgas/crt/fgas_memory.h"

class FDE_CSSCacheItem : public CFX_Target {
 public:
  explicit FDE_CSSCacheItem(IFDE_CSSStyleSheet* p);
  ~FDE_CSSCacheItem();

  IFDE_CSSStyleSheet* pStylesheet;
  uint32_t dwActivity;
};

class CFDE_CSSStyleSheetCache : public IFDE_CSSStyleSheetCache,
                                public CFX_Target {
 public:
  CFDE_CSSStyleSheetCache();
  ~CFDE_CSSStyleSheetCache();
  virtual void Release() { delete this; }

  virtual void SetMaxItems(int32_t iMaxCount = 5) {
    FXSYS_assert(iMaxCount >= 3);
    m_iMaxItems = iMaxCount;
  }

  virtual void AddStyleSheet(const CFX_ByteStringC& szKey,
                             IFDE_CSSStyleSheet* pStyleSheet);
  virtual IFDE_CSSStyleSheet* GetStyleSheet(const CFX_ByteStringC& szKey) const;
  virtual void RemoveStyleSheet(const CFX_ByteStringC& szKey);

 protected:
  void RemoveLowestActivityItem();
  std::map<CFX_ByteString, FDE_CSSCacheItem*> m_Stylesheets;
  IFX_MEMAllocator* m_pFixedStore;
  int32_t m_iMaxItems;
};

class FDE_CSSTagCache : public CFX_Target {
 public:
  FDE_CSSTagCache(FDE_CSSTagCache* parent, IFDE_CSSTagProvider* tag);
  FDE_CSSTagCache(const FDE_CSSTagCache& it);
  FDE_CSSTagCache* GetParent() const { return pParent; }
  IFDE_CSSTagProvider* GetTag() const { return pTag; }
  uint32_t HashID() const { return dwIDHash; }
  uint32_t HashTag() const { return dwTagHash; }
  int32_t CountHashClass() const { return dwClassHashs.GetSize(); }
  void SetClassIndex(int32_t index) { iClassIndex = index; }
  uint32_t HashClass() const {
    return iClassIndex < dwClassHashs.GetSize()
               ? dwClassHashs.GetAt(iClassIndex)
               : 0;
  }

 protected:
  IFDE_CSSTagProvider* pTag;
  FDE_CSSTagCache* pParent;
  uint32_t dwIDHash;
  uint32_t dwTagHash;
  int32_t iClassIndex;
  CFDE_DWordArray dwClassHashs;
};
typedef CFX_ObjectStackTemplate<FDE_CSSTagCache> CFDE_CSSTagStack;

class CFDE_CSSAccelerator : public IFDE_CSSAccelerator, public CFX_Target {
 public:
  virtual void OnEnterTag(IFDE_CSSTagProvider* pTag);
  virtual void OnLeaveTag(IFDE_CSSTagProvider* pTag);
  void Clear() { m_Stack.RemoveAll(); }
  FDE_CSSTagCache* GetTopElement() const { return m_Stack.GetTopElement(); }

 protected:
  CFDE_CSSTagStack m_Stack;
};

#endif  // XFA_FDE_CSS_FDE_CSSCACHE_H_
