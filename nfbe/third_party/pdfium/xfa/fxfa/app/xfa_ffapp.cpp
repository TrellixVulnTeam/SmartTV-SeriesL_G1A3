// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/include/fxfa/xfa_ffapp.h"

#include <algorithm>

#include "xfa/fwl/core/ifwl_widgetmgrdelegate.h"
#include "xfa/fxfa/app/xfa_fwladapter.h"
#include "xfa/fxfa/app/xfa_fwltheme.h"
#include "xfa/include/fxfa/xfa_ffdoc.h"
#include "xfa/include/fxfa/xfa_ffdochandler.h"
#include "xfa/include/fxfa/xfa_ffwidgethandler.h"
#include "xfa/include/fxfa/xfa_fontmgr.h"

CXFA_FileRead::CXFA_FileRead(const CFX_ArrayTemplate<CPDF_Stream*>& streams) {
  int32_t iCount = streams.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CPDF_StreamAcc& acc = m_Data.Add();
    acc.LoadAllData(streams[i]);
  }
}
FX_FILESIZE CXFA_FileRead::GetSize() {
  uint32_t dwSize = 0;
  int32_t iCount = m_Data.GetSize();
  for (int32_t i = 0; i < iCount; i++) {
    CPDF_StreamAcc& acc = m_Data[i];
    dwSize += acc.GetSize();
  }
  return dwSize;
}
FX_BOOL CXFA_FileRead::ReadBlock(void* buffer,
                                 FX_FILESIZE offset,
                                 size_t size) {
  int32_t iCount = m_Data.GetSize();
  int32_t index = 0;
  while (index < iCount) {
    CPDF_StreamAcc& acc = m_Data[index];
    FX_FILESIZE dwSize = acc.GetSize();
    if (offset < dwSize) {
      break;
    }
    offset -= dwSize;
    index++;
  }
  while (index < iCount) {
    CPDF_StreamAcc& acc = m_Data[index];
    uint32_t dwSize = acc.GetSize();
    size_t dwRead = std::min(size, static_cast<size_t>(dwSize - offset));
    FXSYS_memcpy(buffer, acc.GetData() + offset, dwRead);
    size -= dwRead;
    if (size == 0) {
      return TRUE;
    }
    buffer = (uint8_t*)buffer + dwRead;
    offset = 0;
    index++;
  }
  return FALSE;
}

CXFA_FFApp::CXFA_FFApp(IXFA_AppProvider* pProvider)
    : m_pDocHandler(nullptr),
      m_pFWLTheme(nullptr),
      m_pProvider(pProvider),
      m_pFontMgr(nullptr),
#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
      m_pFontSource(nullptr),
#endif
      m_pAdapterWidgetMgr(nullptr),
      m_pWidgetMgrDelegate(nullptr),
      m_pFDEFontMgr(nullptr),
      m_pAdapterThreadMgr(nullptr) {
  m_pFWLApp = IFWL_App::Create(this);
  FWL_SetApp(m_pFWLApp);
  m_pFWLApp->Initialize();
  CXFA_TimeZoneProvider::Create();
}
CXFA_FFApp::~CXFA_FFApp() {
  delete m_pDocHandler;
  if (m_pFWLApp) {
    m_pFWLApp->Finalize();
    m_pFWLApp->Release();
    delete m_pFWLApp;
  }
  if (m_pFWLTheme)
    m_pFWLTheme->Release();
  delete m_pAdapterWidgetMgr;
  delete m_pAdapterThreadMgr;

  CXFA_TimeZoneProvider::Destroy();
  delete m_pFontMgr;
#if _FXM_PLATFORM_ != _FXM_PLATFORM_WINDOWS_
  if (m_pFontSource)
    m_pFontSource->Release();
#endif
  if (m_pFDEFontMgr)
    m_pFDEFontMgr->Release();
}

CXFA_FFDocHandler* CXFA_FFApp::GetDocHandler() {
  if (!m_pDocHandler) {
    m_pDocHandler = new CXFA_FFDocHandler;
  }
  return m_pDocHandler;
}
CXFA_FFDoc* CXFA_FFApp::CreateDoc(IXFA_DocProvider* pProvider,
                                  IFX_FileRead* pStream,
                                  FX_BOOL bTakeOverFile) {
  CXFA_FFDoc* pDoc = new CXFA_FFDoc(this, pProvider);
  FX_BOOL bSuccess = pDoc->OpenDoc(pStream, bTakeOverFile);
  if (!bSuccess) {
    delete pDoc;
    pDoc = NULL;
  }
  return pDoc;
}
CXFA_FFDoc* CXFA_FFApp::CreateDoc(IXFA_DocProvider* pProvider,
                                  CPDF_Document* pPDFDoc) {
  if (pPDFDoc == NULL) {
    return NULL;
  }
  CXFA_FFDoc* pDoc = new CXFA_FFDoc(this, pProvider);
  FX_BOOL bSuccess = pDoc->OpenDoc(pPDFDoc);
  if (!bSuccess) {
    delete pDoc;
    pDoc = NULL;
  }
  return pDoc;
}

void CXFA_FFApp::SetDefaultFontMgr(CXFA_DefFontMgr* pFontMgr) {
  if (!m_pFontMgr) {
    m_pFontMgr = new CXFA_FontMgr();
  }
  m_pFontMgr->SetDefFontMgr(pFontMgr);
}
CXFA_FontMgr* CXFA_FFApp::GetXFAFontMgr() {
  return m_pFontMgr;
}
IFX_FontMgr* CXFA_FFApp::GetFDEFontMgr() {
  if (!m_pFDEFontMgr) {
#if _FXM_PLATFORM_ == _FXM_PLATFORM_WINDOWS_
    m_pFDEFontMgr = IFX_FontMgr::Create(FX_GetDefFontEnumerator());
#else
    m_pFontSource = FX_CreateDefaultFontSourceEnum();
    m_pFDEFontMgr = IFX_FontMgr::Create(m_pFontSource);
#endif
  }
  return m_pFDEFontMgr;
}
CXFA_FWLTheme* CXFA_FFApp::GetFWLTheme() {
  if (!m_pFWLTheme) {
    m_pFWLTheme = new CXFA_FWLTheme(this);
  }
  return m_pFWLTheme;
}
IFWL_AdapterWidgetMgr* CXFA_FFApp::GetWidgetMgr(
    IFWL_WidgetMgrDelegate* pDelegate) {
  if (!m_pAdapterWidgetMgr) {
    m_pAdapterWidgetMgr = new CXFA_FWLAdapterWidgetMgr;
    pDelegate->OnSetCapability(FWL_WGTMGR_DisableThread |
                               FWL_WGTMGR_DisableForm);
    m_pWidgetMgrDelegate = pDelegate;
  }
  return m_pAdapterWidgetMgr;
}
IFWL_AdapterThreadMgr* CXFA_FFApp::GetThreadMgr() {
  if (!m_pAdapterThreadMgr) {
    m_pAdapterThreadMgr = new CFWL_SDAdapterThreadMgr;
  }
  return m_pAdapterThreadMgr;
}
IFWL_AdapterTimerMgr* CXFA_FFApp::GetTimerMgr() {
  return m_pProvider->GetTimerMgr();
}
