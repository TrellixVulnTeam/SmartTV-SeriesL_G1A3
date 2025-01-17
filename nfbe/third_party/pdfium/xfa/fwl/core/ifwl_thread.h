// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CORE_IFWL_THREAD_H_
#define XFA_FWL_CORE_IFWL_THREAD_H_

// The FWL thread/app code contains three parallel inheritance hierarchies,
// which reference each other via pointers as follows:
//
//                                           m_pImpl
//      (nonesuch)              IFWL_Thread ----------> CFWL_ThreadImp
//                                   |      <----------      |
//                                   A       m_pIface        A
//                                   |                       |
//      (nonesuch)              IFWL_NoteThread         CFWL_NoteThreadImp
//                                   |                       |
//                                   A                       A
//                m_pIface           |                       |
//      CFWL_App --------------> IFWL_App                CFWL_AppImp
//

#include "xfa/fwl/core/fwl_error.h"

class CFWL_ThreadImp;
class IFWL_NoteDriver;

typedef struct FWL_HTHREAD_ { void* pData; } * FWL_HTHREAD;

class IFWL_Thread {
 public:
  // These call into polymorphic methods in the impl; no need to override.
  void Release();
  FWL_ERR Run(FWL_HTHREAD hThread);

  CFWL_ThreadImp* GetImpl() const { return m_pImpl; }
  void SetImpl(CFWL_ThreadImp* pImpl) { m_pImpl = pImpl; }

 protected:
  virtual ~IFWL_Thread() {}

 private:
  CFWL_ThreadImp* m_pImpl;
};

#endif  // XFA_FWL_CORE_IFWL_THREAD_H_
