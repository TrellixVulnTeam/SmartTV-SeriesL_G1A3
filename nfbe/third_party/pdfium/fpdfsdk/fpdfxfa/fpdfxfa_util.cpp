// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/include/fpdfxfa/fpdfxfa_util.h"

#include <vector>

#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"

std::vector<CFWL_TimerInfo*>* CXFA_FWLAdapterTimerMgr::s_TimerArray = nullptr;

FWL_ERR CXFA_FWLAdapterTimerMgr::Start(IFWL_Timer* pTimer,
                                       uint32_t dwElapse,
                                       FWL_HTIMER& hTimer,
                                       FX_BOOL bImmediately) {
  if (!m_pEnv)
    return FWL_ERR_Indefinite;

  int32_t id_event = m_pEnv->FFI_SetTimer(dwElapse, TimerProc);
  if (!s_TimerArray)
    s_TimerArray = new std::vector<CFWL_TimerInfo*>;
  s_TimerArray->push_back(new CFWL_TimerInfo(id_event, pTimer));
  hTimer = reinterpret_cast<FWL_HTIMER>(s_TimerArray->back());
  return FWL_ERR_Succeeded;
}

FWL_ERR CXFA_FWLAdapterTimerMgr::Stop(FWL_HTIMER hTimer) {
  if (!hTimer || !m_pEnv)
    return FWL_ERR_Indefinite;

  CFWL_TimerInfo* pInfo = reinterpret_cast<CFWL_TimerInfo*>(hTimer);
  m_pEnv->FFI_KillTimer(pInfo->idEvent);
  if (s_TimerArray) {
    auto it = std::find(s_TimerArray->begin(), s_TimerArray->end(), pInfo);
    if (it != s_TimerArray->end()) {
      s_TimerArray->erase(it);
      delete pInfo;
    }
  }
  return FWL_ERR_Succeeded;
}

// static
void CXFA_FWLAdapterTimerMgr::TimerProc(int32_t idEvent) {
  if (!s_TimerArray)
    return;

  for (CFWL_TimerInfo* pInfo : *s_TimerArray) {
    if (pInfo->idEvent == idEvent) {
      pInfo->pTimer->Run(reinterpret_cast<FWL_HTIMER>(pInfo));
      break;
    }
  }
}
