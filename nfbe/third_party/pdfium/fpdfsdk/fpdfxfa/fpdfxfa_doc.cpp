// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_document.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream_acc.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_app.h"
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_doc.h"
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_page.h"
#include "fpdfsdk/include/fpdfxfa/fpdfxfa_util.h"
#include "fpdfsdk/include/fsdk_define.h"
#include "fpdfsdk/include/fsdk_mgr.h"
#include "fpdfsdk/javascript/ijs_runtime.h"
#include "public/fpdf_formfill.h"
#include "xfa/include/fxfa/xfa_ffapp.h"
#include "xfa/include/fxfa/xfa_ffdoc.h"
#include "xfa/include/fxfa/xfa_ffdocview.h"
#include "xfa/include/fxfa/xfa_ffpageview.h"
#include "xfa/include/fxfa/xfa_ffwidget.h"

#define IDS_XFA_Validate_Input                                          \
  "At least one required field was empty. Please fill in the required " \
  "fields\r\n(highlighted) before continuing."

// submit
#define FXFA_CONFIG 0x00000001
#define FXFA_TEMPLATE 0x00000010
#define FXFA_LOCALESET 0x00000100
#define FXFA_DATASETS 0x00001000
#define FXFA_XMPMETA 0x00010000
#define FXFA_XFDF 0x00100000
#define FXFA_FORM 0x01000000
#define FXFA_PDF 0x10000000

#ifndef _WIN32
extern void SetLastError(int err);

extern int GetLastError();
#endif

CPDFXFA_Document::CPDFXFA_Document(CPDF_Document* pPDFDoc,
                                   CPDFXFA_App* pProvider)
    : m_iDocType(DOCTYPE_PDF),
      m_pPDFDoc(pPDFDoc),
      m_pSDKDoc(nullptr),
      m_pXFADoc(nullptr),
      m_pXFADocView(nullptr),
      m_pApp(pProvider),
      m_pJSContext(nullptr),
      m_nLoadStatus(FXFA_LOADSTATUS_PRELOAD),
      m_nPageCount(0) {}

CPDFXFA_Document::~CPDFXFA_Document() {
  m_nLoadStatus = FXFA_LOADSTATUS_CLOSING;

  if (m_pXFADoc) {
    CXFA_FFApp* pApp = m_pApp->GetXFAApp();
    if (pApp) {
      CXFA_FFDocHandler* pDocHandler = pApp->GetDocHandler();
      if (pDocHandler) {
        CloseXFADoc(pDocHandler);
      }
    }
    delete m_pXFADoc;
  }
  if (m_pJSContext && m_pSDKDoc && m_pSDKDoc->GetEnv())
    m_pSDKDoc->GetEnv()->GetJSRuntime()->ReleaseContext(m_pJSContext);
  delete m_pSDKDoc;
  if (m_pPDFDoc) {
    CPDF_Parser* pParser = m_pPDFDoc->GetParser();
    if (pParser)
      delete pParser;
    else
      delete m_pPDFDoc;
  }

  m_nLoadStatus = FXFA_LOADSTATUS_CLOSED;
}

FX_BOOL CPDFXFA_Document::LoadXFADoc() {
  m_nLoadStatus = FXFA_LOADSTATUS_LOADING;

  if (!m_pPDFDoc)
    return FALSE;

  m_XFAPageList.RemoveAll();

  CXFA_FFApp* pApp = m_pApp->GetXFAApp();
  if (!pApp)
    return FALSE;

  m_pXFADoc = pApp->CreateDoc(this, m_pPDFDoc);
  if (!m_pXFADoc) {
    SetLastError(FPDF_ERR_XFALOAD);
    return FALSE;
  }

  CXFA_FFDocHandler* pDocHandler = pApp->GetDocHandler();
  if (!pDocHandler) {
    SetLastError(FPDF_ERR_XFALOAD);
    return FALSE;
  }

  m_pXFADoc->StartLoad();
  int iStatus = m_pXFADoc->DoLoad(nullptr);
  if (iStatus != XFA_PARSESTATUS_Done) {
    CloseXFADoc(pDocHandler);
    SetLastError(FPDF_ERR_XFALOAD);
    return FALSE;
  }
  m_pXFADoc->StopLoad();
  m_pXFADoc->GetXFADoc()->InitScriptContext(m_pApp->GetJSERuntime());

  if (m_pXFADoc->GetDocType() == XFA_DOCTYPE_Dynamic)
    m_iDocType = DOCTYPE_DYNAMIC_XFA;
  else
    m_iDocType = DOCTYPE_STATIC_XFA;

  m_pXFADocView = m_pXFADoc->CreateDocView(XFA_DOCVIEW_View);
  if (m_pXFADocView->StartLayout() < 0) {
    CloseXFADoc(pDocHandler);
    SetLastError(FPDF_ERR_XFALAYOUT);
    return FALSE;
  }

  m_pXFADocView->DoLayout(NULL);
  m_pXFADocView->StopLayout();
  m_nLoadStatus = FXFA_LOADSTATUS_LOADED;

  return TRUE;
}

int CPDFXFA_Document::GetPageCount() {
  if (!m_pPDFDoc && !m_pXFADoc)
    return 0;

  switch (m_iDocType) {
    case DOCTYPE_PDF:
    case DOCTYPE_STATIC_XFA:
      if (m_pPDFDoc)
        return m_pPDFDoc->GetPageCount();
    case DOCTYPE_DYNAMIC_XFA:
      if (m_pXFADoc)
        return m_pXFADocView->CountPageViews();
    default:
      return 0;
  }
}

CPDFXFA_Page* CPDFXFA_Document::GetPage(int page_index) {
  if (page_index < 0)
    return nullptr;
  CPDFXFA_Page* pPage = nullptr;
  int nCount = m_XFAPageList.GetSize();
  if (nCount > 0 && page_index < nCount) {
    pPage = m_XFAPageList.GetAt(page_index);
    if (pPage)
      pPage->AddRef();
  } else {
    m_nPageCount = GetPageCount();
    m_XFAPageList.SetSize(m_nPageCount);
  }
  if (pPage)
    return pPage;
  pPage = new CPDFXFA_Page(this, page_index);
  if (!pPage->LoadPage()) {
    delete pPage;
    return nullptr;
  }
  m_XFAPageList.SetAt(page_index, pPage);
  return pPage;
}

CPDFXFA_Page* CPDFXFA_Document::GetPage(CXFA_FFPageView* pPage) {
  if (!pPage)
    return NULL;

  if (!m_pXFADoc)
    return NULL;

  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return NULL;

  int nSize = m_XFAPageList.GetSize();
  for (int i = 0; i < nSize; i++) {
    CPDFXFA_Page* pTempPage = m_XFAPageList.GetAt(i);
    if (!pTempPage)
      continue;
    if (pTempPage->GetXFAPageView() && pTempPage->GetXFAPageView() == pPage)
      return pTempPage;
  }

  return NULL;
}

void CPDFXFA_Document::DeletePage(int page_index) {
  if (page_index < 0 || page_index >= m_XFAPageList.GetSize())
    return;

  if (CPDFXFA_Page* pPage = m_XFAPageList.GetAt(page_index))
    pPage->Release();
}

void CPDFXFA_Document::RemovePage(CPDFXFA_Page* page) {
  m_XFAPageList.SetAt(page->GetPageIndex(), NULL);
}

CPDFSDK_Document* CPDFXFA_Document::GetSDKDocument(
    CPDFDoc_Environment* pFormFillEnv) {
  if (!m_pSDKDoc && pFormFillEnv)
    m_pSDKDoc = new CPDFSDK_Document(this, pFormFillEnv);
  return m_pSDKDoc;
}

void CPDFXFA_Document::FXRect2PDFRect(const CFX_RectF& fxRectF,
                                      CFX_FloatRect& pdfRect) {
  pdfRect.left = fxRectF.left;
  pdfRect.top = fxRectF.bottom();
  pdfRect.right = fxRectF.right();
  pdfRect.bottom = fxRectF.top;
}

void CPDFXFA_Document::SetChangeMark(CXFA_FFDoc* hDoc) {
  if (hDoc == m_pXFADoc && m_pSDKDoc) {
    m_pSDKDoc->SetChangeMark();
  }
}

FX_BOOL CPDFXFA_Document::GetChangeMark(CXFA_FFDoc* hDoc) {
  if (hDoc == m_pXFADoc && m_pSDKDoc)
    return m_pSDKDoc->GetChangeMark();
  return FALSE;
}

void CPDFXFA_Document::InvalidateRect(CXFA_FFPageView* pPageView,
                                      const CFX_RectF& rt,
                                      uint32_t dwFlags /* = 0 */) {
  if (!m_pXFADoc || !m_pSDKDoc)
    return;

  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return;

  CFX_FloatRect rcPage;
  FXRect2PDFRect(rt, rcPage);

  CPDFXFA_Page* pPage = GetPage(pPageView);

  if (pPage == NULL)
    return;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return;

  pEnv->FFI_Invalidate((FPDF_PAGE)pPage, rcPage.left, rcPage.bottom,
                       rcPage.right, rcPage.top);
}

void CPDFXFA_Document::InvalidateRect(CXFA_FFWidget* hWidget,
                                      uint32_t dwFlags /* = 0 */) {
  if (!hWidget)
    return;

  if (!m_pXFADoc || !m_pSDKDoc || !m_pXFADocView)
    return;

  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return;

  CXFA_FFWidgetHandler* pWidgetHandler = m_pXFADocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  CFX_RectF rect;
  hWidget->GetRect(rect);
  InvalidateRect(pPageView, rect, dwFlags);
}

void CPDFXFA_Document::DisplayCaret(CXFA_FFWidget* hWidget,
                                    FX_BOOL bVisible,
                                    const CFX_RectF* pRtAnchor) {
  if (!hWidget || pRtAnchor == NULL)
    return;

  if (!m_pXFADoc || !m_pSDKDoc || !m_pXFADocView)
    return;

  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return;

  CXFA_FFWidgetHandler* pWidgetHandler = m_pXFADocView->GetWidgetHandler();
  if (!pWidgetHandler)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  CPDFXFA_Page* pPage = GetPage(pPageView);

  if (pPage == NULL)
    return;

  CFX_FloatRect rcCaret;
  FXRect2PDFRect(*pRtAnchor, rcCaret);

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return;

  pEnv->FFI_DisplayCaret((FPDF_PAGE)pPage, bVisible, rcCaret.left, rcCaret.top,
                         rcCaret.right, rcCaret.bottom);
}

FX_BOOL CPDFXFA_Document::GetPopupPos(CXFA_FFWidget* hWidget,
                                      FX_FLOAT fMinPopup,
                                      FX_FLOAT fMaxPopup,
                                      const CFX_RectF& rtAnchor,
                                      CFX_RectF& rtPopup) {
  if (!hWidget)
    return FALSE;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return FALSE;

  CPDFXFA_Page* pPage = GetPage(pXFAPageView);
  if (!pPage)
    return FALSE;

  CXFA_WidgetAcc* pWidgetAcc = hWidget->GetDataAcc();
  int nRotate = 0;
#ifdef PDF_ENABLE_XFA
  nRotate = pWidgetAcc->GetRotate();
#endif

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return FALSE;
  FS_RECTF pageViewRect;
  pEnv->FFI_GetPageViewRect(pPage, pageViewRect);

  CFX_FloatRect rcAnchor;

  rcAnchor.left = rtAnchor.left;
  rcAnchor.top = rtAnchor.bottom();
  rcAnchor.right = rtAnchor.right();
  rcAnchor.bottom = rtAnchor.top;

  int t1, t2, t;
  uint32_t dwPos;
  FX_FLOAT fPoupHeight;
  switch (nRotate) {
    case 90: {
      t1 = (int)(pageViewRect.right - rcAnchor.right);
      t2 = (int)(rcAnchor.left - pageViewRect.left);
      if (rcAnchor.bottom < pageViewRect.bottom) {
        rtPopup.left += rcAnchor.bottom - pageViewRect.bottom;
      }

      break;
    }

    case 180: {
      t2 = (int)(pageViewRect.top - rcAnchor.top);
      t1 = (int)(rcAnchor.bottom - pageViewRect.bottom);
      if (rcAnchor.left < pageViewRect.left) {
        rtPopup.left += rcAnchor.left - pageViewRect.left;
      }
      break;
    }
    case 270: {
      t1 = (int)(rcAnchor.left - pageViewRect.left);
      t2 = (int)(pageViewRect.right - rcAnchor.right);

      if (rcAnchor.top > pageViewRect.top) {
        rtPopup.left -= rcAnchor.top - pageViewRect.top;
      }
      break;
    }
    case 0:
    default: {
      t1 = (int)(pageViewRect.top - rcAnchor.top);
      t2 = (int)(rcAnchor.bottom - pageViewRect.bottom);
      if (rcAnchor.right > pageViewRect.right) {
        rtPopup.left -= rcAnchor.right - pageViewRect.right;
      }
      break;
    }
  }

  if (t1 <= 0 && t2 <= 0) {
    return FALSE;
  }
  if (t1 <= 0) {
    t = t2;
    dwPos = 1;
  } else if (t2 <= 0) {
    t = t1;
    dwPos = 0;
  } else if (t1 > t2) {
    t = t1;
    dwPos = 0;
  } else {
    t = t2;
    dwPos = 1;
  }
  if (t < fMinPopup) {
    fPoupHeight = fMinPopup;
  } else if (t > fMaxPopup) {
    fPoupHeight = fMaxPopup;
  } else {
    fPoupHeight = (FX_FLOAT)t;
  }

  switch (nRotate) {
    case 0:
    case 180: {
      if (dwPos == 0) {
        rtPopup.top = rtAnchor.height;
        rtPopup.height = fPoupHeight;
      } else {
        rtPopup.top = -fPoupHeight;
        rtPopup.height = fPoupHeight;
      }
      break;
    }
    case 90:
    case 270: {
      if (dwPos == 0) {
        rtPopup.top = rtAnchor.width;
        rtPopup.height = fPoupHeight;
      } else {
        rtPopup.top = -fPoupHeight;
        rtPopup.height = fPoupHeight;
      }
      break;
    }
    default:
      break;
  }

  return TRUE;
}

FX_BOOL CPDFXFA_Document::PopupMenu(CXFA_FFWidget* hWidget,
                                    CFX_PointF ptPopup,
                                    const CFX_RectF* pRectExclude) {
  if (!hWidget)
    return FALSE;

  CXFA_FFPageView* pXFAPageView = hWidget->GetPageView();
  if (!pXFAPageView)
    return FALSE;

  CPDFXFA_Page* pPage = GetPage(pXFAPageView);
  if (!pPage)
    return FALSE;

  int menuFlag = 0;

  if (hWidget->CanUndo())
    menuFlag |= FXFA_MEMU_UNDO;
  if (hWidget->CanRedo())
    menuFlag |= FXFA_MEMU_REDO;
  if (hWidget->CanPaste())
    menuFlag |= FXFA_MEMU_PASTE;
  if (hWidget->CanCopy())
    menuFlag |= FXFA_MEMU_COPY;
  if (hWidget->CanCut())
    menuFlag |= FXFA_MEMU_CUT;
  if (hWidget->CanSelectAll())
    menuFlag |= FXFA_MEMU_SELECTALL;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  return pEnv && pEnv->FFI_PopupMenu(pPage, hWidget, menuFlag, ptPopup, NULL);
}

void CPDFXFA_Document::PageViewEvent(CXFA_FFPageView* pPageView,
                                     uint32_t dwFlags) {
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return;

  if (m_nLoadStatus != FXFA_LOADSTATUS_LOADING &&
      m_nLoadStatus != FXFA_LOADSTATUS_CLOSING &&
      XFA_PAGEVIEWEVENT_StopLayout == dwFlags) {
    int nNewCount = GetPageCount();
    if (nNewCount == m_nPageCount)
      return;

    CXFA_FFDocView* pXFADocView = GetXFADocView();
    if (!pXFADocView)
      return;
    for (int iPageIter = 0; iPageIter < m_nPageCount; iPageIter++) {
      CPDFXFA_Page* pPage = m_XFAPageList.GetAt(iPageIter);
      if (!pPage)
        continue;
      m_pSDKDoc->RemovePageView(pPage);
      CXFA_FFPageView* pXFAPageView = pXFADocView->GetPageView(iPageIter);
      pPage->SetXFAPageView(pXFAPageView);
      if (pXFAPageView)
        pXFAPageView->LoadPageView(nullptr);
    }

    int flag = (nNewCount < m_nPageCount) ? FXFA_PAGEVIEWEVENT_POSTREMOVED
                                          : FXFA_PAGEVIEWEVENT_POSTADDED;
    int count = FXSYS_abs(nNewCount - m_nPageCount);
    m_nPageCount = nNewCount;
    m_XFAPageList.SetSize(nNewCount);
    pEnv->FFI_PageEvent(count, flag);
  }
}

void CPDFXFA_Document::WidgetEvent(CXFA_FFWidget* hWidget,
                                   CXFA_WidgetAcc* pWidgetData,
                                   uint32_t dwEvent,
                                   void* pParam,
                                   void* pAdditional) {
  if (m_iDocType != DOCTYPE_DYNAMIC_XFA || !hWidget)
    return;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return;

  CXFA_FFPageView* pPageView = hWidget->GetPageView();
  if (!pPageView)
    return;

  CPDFXFA_Page* pXFAPage = GetPage(pPageView);
  if (!pXFAPage)
    return;

  CPDFSDK_PageView* pSdkPageView = m_pSDKDoc->GetPageView(pXFAPage);
  if (dwEvent == XFA_WIDGETEVENT_PostAdded) {
    pSdkPageView->AddAnnot(hWidget);
  } else if (dwEvent == XFA_WIDGETEVENT_PreRemoved) {
    CPDFSDK_Annot* pAnnot = pSdkPageView->GetAnnotByXFAWidget(hWidget);
    if (pAnnot)
      pSdkPageView->DeleteAnnot(pAnnot);
  }
}

int32_t CPDFXFA_Document::CountPages(CXFA_FFDoc* hDoc) {
  if (hDoc == m_pXFADoc && m_pSDKDoc)
    return GetPageCount();
  return 0;
}

int32_t CPDFXFA_Document::GetCurrentPage(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc)
    return -1;
  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return -1;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return -1;

  return pEnv->FFI_GetCurrentPageIndex(this);
}

void CPDFXFA_Document::SetCurrentPage(CXFA_FFDoc* hDoc, int32_t iCurPage) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc || m_iDocType != DOCTYPE_DYNAMIC_XFA ||
      iCurPage < 0 || iCurPage >= m_pSDKDoc->GetPageCount()) {
    return;
  }
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return;
  pEnv->FFI_SetCurrentPage(this, iCurPage);
}
FX_BOOL CPDFXFA_Document::IsCalculationsEnabled(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc)
    return FALSE;
  if (m_pSDKDoc->GetInterForm())
    return m_pSDKDoc->GetInterForm()->IsXfaCalculateEnabled();

  return FALSE;
}
void CPDFXFA_Document::SetCalculationsEnabled(CXFA_FFDoc* hDoc,
                                              FX_BOOL bEnabled) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc)
    return;
  if (m_pSDKDoc->GetInterForm())
    m_pSDKDoc->GetInterForm()->XfaEnableCalculate(bEnabled);
}

void CPDFXFA_Document::GetTitle(CXFA_FFDoc* hDoc, CFX_WideString& wsTitle) {
  if (hDoc != m_pXFADoc)
    return;
  if (m_pPDFDoc == NULL)
    return;
  CPDF_Dictionary* pInfoDict = m_pPDFDoc->GetInfo();

  if (pInfoDict == NULL)
    return;

  CFX_ByteString csTitle = pInfoDict->GetStringBy("Title");
  wsTitle = wsTitle.FromLocal(csTitle.GetBuffer(csTitle.GetLength()));
  csTitle.ReleaseBuffer(csTitle.GetLength());
}
void CPDFXFA_Document::SetTitle(CXFA_FFDoc* hDoc,
                                const CFX_WideStringC& wsTitle) {
  if (hDoc != m_pXFADoc)
    return;
  if (m_pPDFDoc == NULL)
    return;
  CPDF_Dictionary* pInfoDict = m_pPDFDoc->GetInfo();

  if (pInfoDict == NULL)
    return;
  pInfoDict->SetAt("Title", new CPDF_String(wsTitle));
}
void CPDFXFA_Document::ExportData(CXFA_FFDoc* hDoc,
                                  const CFX_WideStringC& wsFilePath,
                                  FX_BOOL bXDP) {
  if (hDoc != m_pXFADoc)
    return;
  if (m_iDocType != DOCTYPE_DYNAMIC_XFA && m_iDocType != DOCTYPE_STATIC_XFA)
    return;
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (pEnv == NULL)
    return;
  int fileType = bXDP ? FXFA_SAVEAS_XDP : FXFA_SAVEAS_XML;
  CFX_ByteString bs = CFX_WideString(wsFilePath).UTF16LE_Encode();

  if (wsFilePath.IsEmpty()) {
    if (!pEnv->GetFormFillInfo() ||
        pEnv->GetFormFillInfo()->m_pJsPlatform == NULL)
      return;
    CFX_WideString filepath = pEnv->JS_fieldBrowse();
    bs = filepath.UTF16LE_Encode();
  }
  int len = bs.GetLength() / sizeof(unsigned short);
  FPDF_FILEHANDLER* pFileHandler = pEnv->FFI_OpenFile(
      bXDP ? FXFA_SAVEAS_XDP : FXFA_SAVEAS_XML,
      (FPDF_WIDESTRING)bs.GetBuffer(len * sizeof(unsigned short)), "wb");
  bs.ReleaseBuffer(len * sizeof(unsigned short));

  if (pFileHandler == NULL)
    return;

  CFPDF_FileStream fileWrite(pFileHandler);

  CFX_ByteString content;
  if (fileType == FXFA_SAVEAS_XML) {
    content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
    fileWrite.WriteBlock((const FX_CHAR*)content, fileWrite.GetSize(),
                         content.GetLength());
    CFX_WideStringC data(L"data");
    if (m_pXFADocView->GetDoc()->SavePackage(data, &fileWrite)) {
      // Ignoring error.
    }
  } else if (fileType == FXFA_SAVEAS_XDP) {
    if (m_pPDFDoc == NULL)
      return;
    CPDF_Dictionary* pRoot = m_pPDFDoc->GetRoot();
    if (pRoot == NULL)
      return;
    CPDF_Dictionary* pAcroForm = pRoot->GetDictBy("AcroForm");
    if (NULL == pAcroForm)
      return;
    CPDF_Object* pXFA = pAcroForm->GetObjectBy("XFA");
    if (pXFA == NULL)
      return;
    if (!pXFA->IsArray())
      return;
    CPDF_Array* pArray = pXFA->GetArray();
    if (NULL == pArray)
      return;
    int size = pArray->GetCount();
    for (int i = 1; i < size; i += 2) {
      CPDF_Object* pPDFObj = pArray->GetObjectAt(i);
      CPDF_Object* pPrePDFObj = pArray->GetObjectAt(i - 1);
      if (!pPrePDFObj->IsString())
        continue;
      if (!pPDFObj->IsReference())
        continue;
      CPDF_Object* pDirectObj = pPDFObj->GetDirect();
      if (!pDirectObj->IsStream())
        continue;
      if (pPrePDFObj->GetString() == "form") {
        CFX_WideStringC form(L"form");
        m_pXFADocView->GetDoc()->SavePackage(form, &fileWrite);
      } else if (pPrePDFObj->GetString() == "datasets") {
        CFX_WideStringC datasets(L"datasets");
        m_pXFADocView->GetDoc()->SavePackage(datasets, &fileWrite);
      } else {
        if (i == size - 1) {
          CFX_WideString wPath = CFX_WideString::FromUTF16LE(
              (unsigned short*)(const FX_CHAR*)bs,
              bs.GetLength() / sizeof(unsigned short));
          CFX_ByteString bPath = wPath.UTF8Encode();
          CFX_ByteString szFormat =
              "\n<pdf href=\"%s\" xmlns=\"http://ns.adobe.com/xdp/pdf/\"/>";
          content.Format(szFormat, (char*)(const FX_CHAR*)bPath);
          fileWrite.WriteBlock((const FX_CHAR*)content, fileWrite.GetSize(),
                               content.GetLength());
        }

        CPDF_Stream* pStream = (CPDF_Stream*)pDirectObj;
        CPDF_StreamAcc* pAcc = new CPDF_StreamAcc;
        pAcc->LoadAllData(pStream);
        fileWrite.WriteBlock(pAcc->GetData(), fileWrite.GetSize(),
                             pAcc->GetSize());
        delete pAcc;
      }
    }
  }
  if (!fileWrite.Flush()) {
    // Ignoring flush error.
  }
}
void CPDFXFA_Document::ImportData(CXFA_FFDoc* hDoc,
                                  const CFX_WideStringC& wsFilePath) {}

void CPDFXFA_Document::GotoURL(CXFA_FFDoc* hDoc,
                               const CFX_WideStringC& bsURL,
                               FX_BOOL bAppend) {
  if (hDoc != m_pXFADoc)
    return;

  if (m_iDocType != DOCTYPE_DYNAMIC_XFA)
    return;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (pEnv == NULL)
    return;

  CFX_WideStringC str(bsURL.raw_str());

  pEnv->FFI_GotoURL(this, str, bAppend);
}

FX_BOOL CPDFXFA_Document::IsValidationsEnabled(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc)
    return FALSE;
  if (m_pSDKDoc->GetInterForm())
    return m_pSDKDoc->GetInterForm()->IsXfaValidationsEnabled();

  return TRUE;
}
void CPDFXFA_Document::SetValidationsEnabled(CXFA_FFDoc* hDoc,
                                             FX_BOOL bEnabled) {
  if (hDoc != m_pXFADoc || !m_pSDKDoc)
    return;
  if (m_pSDKDoc->GetInterForm())
    m_pSDKDoc->GetInterForm()->XfaSetValidationsEnabled(bEnabled);
}
void CPDFXFA_Document::SetFocusWidget(CXFA_FFDoc* hDoc,
                                      CXFA_FFWidget* hWidget) {
  if (hDoc != m_pXFADoc)
    return;

  if (NULL == hWidget) {
    m_pSDKDoc->SetFocusAnnot(NULL);
    return;
  }

  int pageViewCount = m_pSDKDoc->GetPageViewCount();
  for (int i = 0; i < pageViewCount; i++) {
    CPDFSDK_PageView* pPageView = m_pSDKDoc->GetPageView(i);
    if (pPageView == NULL)
      continue;
    CPDFSDK_Annot* pAnnot = pPageView->GetAnnotByXFAWidget(hWidget);
    if (pAnnot) {
      m_pSDKDoc->SetFocusAnnot(pAnnot);
      break;
    }
  }
}
void CPDFXFA_Document::Print(CXFA_FFDoc* hDoc,
                             int32_t nStartPage,
                             int32_t nEndPage,
                             uint32_t dwOptions) {
  if (hDoc != m_pXFADoc)
    return;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (pEnv == NULL)
    return;

  if (!pEnv->GetFormFillInfo() ||
      pEnv->GetFormFillInfo()->m_pJsPlatform == NULL)
    return;
  if (pEnv->GetFormFillInfo()->m_pJsPlatform->Doc_print == NULL)
    return;
  pEnv->GetFormFillInfo()->m_pJsPlatform->Doc_print(
      pEnv->GetFormFillInfo()->m_pJsPlatform,
      dwOptions & XFA_PRINTOPT_ShowDialog, nStartPage, nEndPage,
      dwOptions & XFA_PRINTOPT_CanCancel, dwOptions & XFA_PRINTOPT_ShrinkPage,
      dwOptions & XFA_PRINTOPT_AsImage, dwOptions & XFA_PRINTOPT_ReverseOrder,
      dwOptions & XFA_PRINTOPT_PrintAnnot);
}

void CPDFXFA_Document::GetURL(CXFA_FFDoc* hDoc, CFX_WideString& wsDocURL) {
  if (hDoc != m_pXFADoc)
    return;

  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (pEnv == NULL)
    return;

  pEnv->FFI_GetURL(this, wsDocURL);
}

FX_ARGB CPDFXFA_Document::GetHighlightColor(CXFA_FFDoc* hDoc) {
  if (hDoc != m_pXFADoc)
    return 0;
  if (m_pSDKDoc) {
    if (CPDFSDK_InterForm* pInterForm = m_pSDKDoc->GetInterForm()) {
      FX_COLORREF color = pInterForm->GetHighlightColor(FPDF_FORMFIELD_XFA);
      uint8_t alpha = pInterForm->GetHighlightAlpha();
      FX_ARGB argb = ArgbEncode((int)alpha, color);
      return argb;
    }
  }
  return 0;
}

FX_BOOL CPDFXFA_Document::_NotifySubmit(FX_BOOL bPrevOrPost) {
  if (bPrevOrPost)
    return _OnBeforeNotifySumbit();

  _OnAfterNotifySumbit();
  return TRUE;
}

FX_BOOL CPDFXFA_Document::_OnBeforeNotifySumbit() {
#ifdef PDF_ENABLE_XFA
  if (m_iDocType != DOCTYPE_DYNAMIC_XFA && m_iDocType != DOCTYPE_STATIC_XFA)
    return TRUE;
  if (m_pXFADocView == NULL)
    return TRUE;
  CXFA_FFWidgetHandler* pWidgetHandler = m_pXFADocView->GetWidgetHandler();
  if (pWidgetHandler == NULL)
    return TRUE;
  CXFA_WidgetAccIterator* pWidgetAccIterator =
      m_pXFADocView->CreateWidgetAccIterator();
  if (pWidgetAccIterator) {
    CXFA_EventParam Param;
    Param.m_eType = XFA_EVENT_PreSubmit;
    CXFA_WidgetAcc* pWidgetAcc = pWidgetAccIterator->MoveToNext();
    while (pWidgetAcc) {
      pWidgetHandler->ProcessEvent(pWidgetAcc, &Param);
      pWidgetAcc = pWidgetAccIterator->MoveToNext();
    }
    pWidgetAccIterator->Release();
  }
  pWidgetAccIterator = m_pXFADocView->CreateWidgetAccIterator();
  if (pWidgetAccIterator) {
    CXFA_WidgetAcc* pWidgetAcc = pWidgetAccIterator->MoveToNext();
    pWidgetAcc = pWidgetAccIterator->MoveToNext();
    while (pWidgetAcc) {
      int fRet = pWidgetAcc->ProcessValidate(-1);
      if (fRet == XFA_EVENTERROR_Error) {
        CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
        if (pEnv == NULL)
          return FALSE;
        CFX_WideString ws;
        ws.FromLocal(IDS_XFA_Validate_Input);
        CFX_ByteString bs = ws.UTF16LE_Encode();
        int len = bs.GetLength() / sizeof(unsigned short);
        pEnv->FFI_Alert(
            (FPDF_WIDESTRING)bs.GetBuffer(len * sizeof(unsigned short)),
            (FPDF_WIDESTRING)L"", 0, 1);
        bs.ReleaseBuffer(len * sizeof(unsigned short));
        pWidgetAccIterator->Release();
        return FALSE;
      }
      pWidgetAcc = pWidgetAccIterator->MoveToNext();
    }
    pWidgetAccIterator->Release();
    m_pXFADocView->UpdateDocView();
  }
#endif
  return TRUE;
}
void CPDFXFA_Document::_OnAfterNotifySumbit() {
  if (m_iDocType != DOCTYPE_DYNAMIC_XFA && m_iDocType != DOCTYPE_STATIC_XFA)
    return;
  if (m_pXFADocView == NULL)
    return;
  CXFA_FFWidgetHandler* pWidgetHandler = m_pXFADocView->GetWidgetHandler();
  if (pWidgetHandler == NULL)
    return;
  CXFA_WidgetAccIterator* pWidgetAccIterator =
      m_pXFADocView->CreateWidgetAccIterator();
  if (pWidgetAccIterator == NULL)
    return;
  CXFA_EventParam Param;
  Param.m_eType = XFA_EVENT_PostSubmit;

  CXFA_WidgetAcc* pWidgetAcc = pWidgetAccIterator->MoveToNext();
  while (pWidgetAcc) {
    pWidgetHandler->ProcessEvent(pWidgetAcc, &Param);
    pWidgetAcc = pWidgetAccIterator->MoveToNext();
  }
  pWidgetAccIterator->Release();
  m_pXFADocView->UpdateDocView();
}

FX_BOOL CPDFXFA_Document::SubmitData(CXFA_FFDoc* hDoc, CXFA_Submit submit) {
  if (!_NotifySubmit(TRUE))
    return FALSE;
  if (NULL == m_pXFADocView)
    return FALSE;
  m_pXFADocView->UpdateDocView();

  FX_BOOL ret = _SubmitData(hDoc, submit);
  _NotifySubmit(FALSE);
  return ret;
}

IFX_FileRead* CPDFXFA_Document::OpenLinkedFile(CXFA_FFDoc* hDoc,
                                               const CFX_WideString& wsLink) {
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (pEnv == NULL)
    return FALSE;
  CFX_ByteString bs = wsLink.UTF16LE_Encode();
  int len = bs.GetLength() / sizeof(unsigned short);
  FPDF_FILEHANDLER* pFileHandler = pEnv->FFI_OpenFile(
      0, (FPDF_WIDESTRING)bs.GetBuffer(len * sizeof(unsigned short)), "rb");
  bs.ReleaseBuffer(len * sizeof(unsigned short));

  if (pFileHandler == NULL)
    return NULL;
  return new CFPDF_FileStream(pFileHandler);
}
FX_BOOL CPDFXFA_Document::_ExportSubmitFile(FPDF_FILEHANDLER* pFileHandler,
                                            int fileType,
                                            FPDF_DWORD encodeType,
                                            FPDF_DWORD flag) {
  if (!m_pXFADocView)
    return FALSE;

  CFX_ByteString content;
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return FALSE;

  CFPDF_FileStream fileStream(pFileHandler);

  if (fileType == FXFA_SAVEAS_XML) {
    CFX_WideString ws;
    ws.FromLocal("data");
    CFX_ByteString content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
    fileStream.WriteBlock((const FX_CHAR*)content, 0, content.GetLength());
    m_pXFADoc->SavePackage(ws.AsWideStringC(), &fileStream);
  } else if (fileType == FXFA_SAVEAS_XDP) {
    if (flag == 0)
      flag = FXFA_CONFIG | FXFA_TEMPLATE | FXFA_LOCALESET | FXFA_DATASETS |
             FXFA_XMPMETA | FXFA_XFDF | FXFA_FORM;
    if (m_pPDFDoc == NULL) {
      fileStream.Flush();
      return FALSE;
    }
    CPDF_Dictionary* pRoot = m_pPDFDoc->GetRoot();
    if (pRoot == NULL) {
      fileStream.Flush();
      return FALSE;
    }
    CPDF_Dictionary* pAcroForm = pRoot->GetDictBy("AcroForm");
    if (NULL == pAcroForm) {
      fileStream.Flush();
      return FALSE;
    }
    CPDF_Object* pXFA = pAcroForm->GetObjectBy("XFA");
    if (pXFA == NULL) {
      fileStream.Flush();
      return FALSE;
    }
    if (!pXFA->IsArray()) {
      fileStream.Flush();
      return FALSE;
    }
    CPDF_Array* pArray = pXFA->GetArray();
    if (NULL == pArray) {
      fileStream.Flush();
      return FALSE;
    }
    int size = pArray->GetCount();
    for (int i = 1; i < size; i += 2) {
      CPDF_Object* pPDFObj = pArray->GetObjectAt(i);
      CPDF_Object* pPrePDFObj = pArray->GetObjectAt(i - 1);
      if (!pPrePDFObj->IsString())
        continue;
      if (!pPDFObj->IsReference())
        continue;
      CPDF_Object* pDirectObj = pPDFObj->GetDirect();
      if (!pDirectObj->IsStream())
        continue;
      if (pPrePDFObj->GetString() == "config" && !(flag & FXFA_CONFIG))
        continue;
      if (pPrePDFObj->GetString() == "template" && !(flag & FXFA_TEMPLATE))
        continue;
      if (pPrePDFObj->GetString() == "localeSet" && !(flag & FXFA_LOCALESET))
        continue;
      if (pPrePDFObj->GetString() == "datasets" && !(flag & FXFA_DATASETS))
        continue;
      if (pPrePDFObj->GetString() == "xmpmeta" && !(flag & FXFA_XMPMETA))
        continue;
      if (pPrePDFObj->GetString() == "xfdf" && !(flag & FXFA_XFDF))
        continue;
      if (pPrePDFObj->GetString() == "form" && !(flag & FXFA_FORM))
        continue;
      if (pPrePDFObj->GetString() == "form") {
        CFX_WideString ws;
        ws.FromLocal("form");
        m_pXFADoc->SavePackage(ws.AsWideStringC(), &fileStream);
      } else if (pPrePDFObj->GetString() == "datasets") {
        CFX_WideString ws;
        ws.FromLocal("datasets");
        m_pXFADoc->SavePackage(ws.AsWideStringC(), &fileStream);
      } else {
        // PDF,creator.
      }
    }
  }
  return TRUE;
}

void CPDFXFA_Document::_ClearChangeMark() {
  if (m_pSDKDoc)
    m_pSDKDoc->ClearChangeMark();
}

void CPDFXFA_Document::_ToXFAContentFlags(CFX_WideString csSrcContent,
                                          FPDF_DWORD& flag) {
  if (csSrcContent.Find(L" config ", 0) != -1)
    flag |= FXFA_CONFIG;
  if (csSrcContent.Find(L" template ", 0) != -1)
    flag |= FXFA_TEMPLATE;
  if (csSrcContent.Find(L" localeSet ", 0) != -1)
    flag |= FXFA_LOCALESET;
  if (csSrcContent.Find(L" datasets ", 0) != -1)
    flag |= FXFA_DATASETS;
  if (csSrcContent.Find(L" xmpmeta ", 0) != -1)
    flag |= FXFA_XMPMETA;
  if (csSrcContent.Find(L" xfdf ", 0) != -1)
    flag |= FXFA_XFDF;
  if (csSrcContent.Find(L" form ", 0) != -1)
    flag |= FXFA_FORM;
  if (flag == 0)
    flag = FXFA_CONFIG | FXFA_TEMPLATE | FXFA_LOCALESET | FXFA_DATASETS |
           FXFA_XMPMETA | FXFA_XFDF | FXFA_FORM;
}
FX_BOOL CPDFXFA_Document::_MailToInfo(CFX_WideString& csURL,
                                      CFX_WideString& csToAddress,
                                      CFX_WideString& csCCAddress,
                                      CFX_WideString& csBCCAddress,
                                      CFX_WideString& csSubject,
                                      CFX_WideString& csMsg) {
  CFX_WideString srcURL = csURL;
  srcURL.TrimLeft();
  if (0 != srcURL.Left(7).CompareNoCase(L"mailto:"))
    return FALSE;
  int pos = srcURL.Find(L'?', 0);
  CFX_WideString tmp;
  if (pos == -1) {
    pos = srcURL.Find(L'@', 0);
    if (pos == -1)
      return FALSE;

    tmp = srcURL.Right(csURL.GetLength() - 7);
    tmp.TrimLeft();
    tmp.TrimRight();
  } else {
    tmp = srcURL.Left(pos);
    tmp = tmp.Right(tmp.GetLength() - 7);
    tmp.TrimLeft();
    tmp.TrimRight();
  }

  csToAddress = tmp;

  srcURL = srcURL.Right(srcURL.GetLength() - (pos + 1));
  while (!srcURL.IsEmpty()) {
    srcURL.TrimLeft();
    srcURL.TrimRight();
    pos = srcURL.Find(L'&', 0);
    if (pos == -1)
      tmp = srcURL;
    else
      tmp = srcURL.Left(pos);

    tmp.TrimLeft();
    tmp.TrimRight();
    if (tmp.GetLength() >= 3 && 0 == tmp.Left(3).CompareNoCase(L"cc=")) {
      tmp = tmp.Right(tmp.GetLength() - 3);
      if (!csCCAddress.IsEmpty())
        csCCAddress += L';';
      csCCAddress += tmp;

    } else if (tmp.GetLength() >= 4 &&
               0 == tmp.Left(4).CompareNoCase(L"bcc=")) {
      tmp = tmp.Right(tmp.GetLength() - 4);
      if (!csBCCAddress.IsEmpty())
        csBCCAddress += L';';
      csBCCAddress += tmp;
    } else if (tmp.GetLength() >= 8 &&
               0 == tmp.Left(8).CompareNoCase(L"subject=")) {
      tmp = tmp.Right(tmp.GetLength() - 8);
      csSubject += tmp;
    } else if (tmp.GetLength() >= 5 &&
               0 == tmp.Left(5).CompareNoCase(L"body=")) {
      tmp = tmp.Right(tmp.GetLength() - 5);
      csMsg += tmp;
    }
    if (pos == -1)
      srcURL = L"";
    else
      srcURL = srcURL.Right(csURL.GetLength() - (pos + 1));
  }
  csToAddress.Replace(L",", L";");
  csCCAddress.Replace(L",", L";");
  csBCCAddress.Replace(L",", L";");
  return TRUE;
}

FX_BOOL CPDFXFA_Document::_SubmitData(CXFA_FFDoc* hDoc, CXFA_Submit submit) {
#ifdef PDF_ENABLE_XFA
  CPDFDoc_Environment* pEnv = m_pSDKDoc->GetEnv();
  if (!pEnv)
    return FALSE;
  CFX_WideStringC csURLC;
  submit.GetSubmitTarget(csURLC);
  CFX_WideString csURL = csURLC;
  if (csURL.IsEmpty()) {
    CFX_WideString ws;
    ws.FromLocal("Submit cancelled.");
    CFX_ByteString bs = ws.UTF16LE_Encode();
    int len = bs.GetLength() / sizeof(unsigned short);
    pEnv->FFI_Alert((FPDF_WIDESTRING)bs.GetBuffer(len * sizeof(unsigned short)),
                    (FPDF_WIDESTRING)L"", 0, 4);
    bs.ReleaseBuffer(len * sizeof(unsigned short));
    return FALSE;
  }
  FPDF_BOOL bRet = TRUE;
  FPDF_FILEHANDLER* pFileHandler = nullptr;
  int fileFlag = -1;
  switch (submit.GetSubmitFormat()) {
    case XFA_ATTRIBUTEENUM_Xdp: {
      CFX_WideStringC csContentC;
      submit.GetSubmitXDPContent(csContentC);
      CFX_WideString csContent;
      csContent = csContentC;
      csContent.TrimLeft();
      csContent.TrimRight();
      CFX_WideString space;
      space.FromLocal(" ");
      csContent = space + csContent + space;
      FPDF_DWORD flag = 0;
      if (submit.IsSubmitEmbedPDF())
        flag |= FXFA_PDF;
      _ToXFAContentFlags(csContent, flag);
      pFileHandler = pEnv->FFI_OpenFile(FXFA_SAVEAS_XDP, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XDP;
      _ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XDP, 0, flag);
      break;
    }
    case XFA_ATTRIBUTEENUM_Xml:
      pFileHandler = pEnv->FFI_OpenFile(FXFA_SAVEAS_XML, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XML;
      _ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XML, 0);
      break;
    case XFA_ATTRIBUTEENUM_Pdf:
      break;
    case XFA_ATTRIBUTEENUM_Urlencoded:
      pFileHandler = pEnv->FFI_OpenFile(FXFA_SAVEAS_XML, nullptr, "wb");
      fileFlag = FXFA_SAVEAS_XML;
      _ExportSubmitFile(pFileHandler, FXFA_SAVEAS_XML, 0);
      break;
    default:
      return false;
  }
  if (!pFileHandler)
    return FALSE;
  if (0 == csURL.Left(7).CompareNoCase(L"mailto:")) {
    CFX_WideString csToAddress;
    CFX_WideString csCCAddress;
    CFX_WideString csBCCAddress;
    CFX_WideString csSubject;
    CFX_WideString csMsg;
    bRet = _MailToInfo(csURL, csToAddress, csCCAddress, csBCCAddress, csSubject,
                       csMsg);
    if (!bRet)
      return FALSE;
    CFX_ByteString bsTo = CFX_WideString(csToAddress).UTF16LE_Encode();
    CFX_ByteString bsCC = CFX_WideString(csCCAddress).UTF16LE_Encode();
    CFX_ByteString bsBcc = CFX_WideString(csBCCAddress).UTF16LE_Encode();
    CFX_ByteString bsSubject = CFX_WideString(csSubject).UTF16LE_Encode();
    CFX_ByteString bsMsg = CFX_WideString(csMsg).UTF16LE_Encode();
    FPDF_WIDESTRING pTo = (FPDF_WIDESTRING)bsTo.GetBuffer(bsTo.GetLength());
    FPDF_WIDESTRING pCC = (FPDF_WIDESTRING)bsCC.GetBuffer(bsCC.GetLength());
    FPDF_WIDESTRING pBcc = (FPDF_WIDESTRING)bsBcc.GetBuffer(bsBcc.GetLength());
    FPDF_WIDESTRING pSubject =
        (FPDF_WIDESTRING)bsSubject.GetBuffer(bsSubject.GetLength());
    FPDF_WIDESTRING pMsg = (FPDF_WIDESTRING)bsMsg.GetBuffer(bsMsg.GetLength());
    pEnv->FFI_EmailTo(pFileHandler, pTo, pSubject, pCC, pBcc, pMsg);
    bsTo.ReleaseBuffer();
    bsCC.ReleaseBuffer();
    bsBcc.ReleaseBuffer();
    bsSubject.ReleaseBuffer();
    bsMsg.ReleaseBuffer();
  } else {
    // HTTP or FTP
    CFX_WideString ws;
    CFX_ByteString bs = csURL.UTF16LE_Encode();
    int len = bs.GetLength() / sizeof(unsigned short);
    pEnv->FFI_UploadTo(
        pFileHandler, fileFlag,
        (FPDF_WIDESTRING)bs.GetBuffer(len * sizeof(unsigned short)));
    bs.ReleaseBuffer(len * sizeof(unsigned short));
  }
  return bRet;
#else
  return TRUE;
#endif
}

FX_BOOL CPDFXFA_Document::SetGlobalProperty(CXFA_FFDoc* hDoc,
                                            const CFX_ByteStringC& szPropName,
                                            FXJSE_HVALUE hValue) {
  if (hDoc != m_pXFADoc)
    return FALSE;

  if (m_pSDKDoc && m_pSDKDoc->GetEnv()->GetJSRuntime())
    return m_pSDKDoc->GetEnv()->GetJSRuntime()->SetHValueByName(szPropName,
                                                                hValue);
  return FALSE;
}
FX_BOOL CPDFXFA_Document::GetPDFScriptObject(CXFA_FFDoc* hDoc,
                                             const CFX_ByteStringC& utf8Name,
                                             FXJSE_HVALUE hValue) {
  if (hDoc != m_pXFADoc)
    return FALSE;

  if (!m_pSDKDoc || !m_pSDKDoc->GetEnv()->GetJSRuntime())
    return FALSE;

  if (!m_pJSContext) {
    m_pSDKDoc->GetEnv()->GetJSRuntime()->SetReaderDocument(m_pSDKDoc);
    m_pJSContext = m_pSDKDoc->GetEnv()->GetJSRuntime()->NewContext();
  }

  return _GetHValueByName(utf8Name, hValue,
                          m_pSDKDoc->GetEnv()->GetJSRuntime());
}
FX_BOOL CPDFXFA_Document::GetGlobalProperty(CXFA_FFDoc* hDoc,
                                            const CFX_ByteStringC& szPropName,
                                            FXJSE_HVALUE hValue) {
  if (hDoc != m_pXFADoc)
    return FALSE;
  if (!m_pSDKDoc || !m_pSDKDoc->GetEnv()->GetJSRuntime())
    return FALSE;

  if (!m_pJSContext) {
    m_pSDKDoc->GetEnv()->GetJSRuntime()->SetReaderDocument(m_pSDKDoc);
    m_pJSContext = m_pSDKDoc->GetEnv()->GetJSRuntime()->NewContext();
  }

  return _GetHValueByName(szPropName, hValue,
                          m_pSDKDoc->GetEnv()->GetJSRuntime());
}
FX_BOOL CPDFXFA_Document::_GetHValueByName(const CFX_ByteStringC& utf8Name,
                                           FXJSE_HVALUE hValue,
                                           IJS_Runtime* runTime) {
  return runTime->GetHValueByName(utf8Name, hValue);
}
