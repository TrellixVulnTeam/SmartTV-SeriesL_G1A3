// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_INCLUDE_FXFA_XFA_FFDOCVIEW_H_
#define XFA_INCLUDE_FXFA_XFA_FFDOCVIEW_H_

#include "xfa/include/fxfa/xfa_ffdoc.h"

class CXFA_FFWidgetHandler;
class CXFA_FFDoc;
class CXFA_FFWidget;
class CXFA_WidgetAccIterator;

extern const XFA_ATTRIBUTEENUM gs_EventActivity[];
enum XFA_DOCVIEW_LAYOUTSTATUS {
  XFA_DOCVIEW_LAYOUTSTATUS_None,
  XFA_DOCVIEW_LAYOUTSTATUS_Start,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitialize,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitCalculate,
  XFA_DOCVIEW_LAYOUTSTATUS_FormInitValidate,
  XFA_DOCVIEW_LAYOUTSTATUS_FormFormReady,
  XFA_DOCVIEW_LAYOUTSTATUS_Doing,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitialize,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitCalculate,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetInitValidate,
  XFA_DOCVIEW_LAYOUTSTATUS_PagesetFormReady,
  XFA_DOCVIEW_LAYOUTSTATUS_LayoutReady,
  XFA_DOCVIEW_LAYOUTSTATUS_DocReady,
  XFA_DOCVIEW_LAYOUTSTATUS_End
};
class CXFA_FFDocView {
 public:
  CXFA_FFDocView(CXFA_FFDoc* pDoc);
  ~CXFA_FFDocView();

  CXFA_FFDoc* GetDoc() { return m_pDoc; }
  int32_t StartLayout(int32_t iStartPage = 0);
  int32_t DoLayout(IFX_Pause* pPause = NULL);
  void StopLayout();
  int32_t GetLayoutStatus();
  void UpdateDocView();
  int32_t CountPageViews();
  CXFA_FFPageView* GetPageView(int32_t nIndex);

  void ResetWidgetData(CXFA_WidgetAcc* pWidgetAcc = NULL);
  int32_t ProcessWidgetEvent(CXFA_EventParam* pParam,
                             CXFA_WidgetAcc* pWidgetAcc = NULL);
  CXFA_FFWidgetHandler* GetWidgetHandler();
  IXFA_WidgetIterator* CreateWidgetIterator();
  CXFA_WidgetAccIterator* CreateWidgetAccIterator(
      XFA_WIDGETORDER eOrder = XFA_WIDGETORDER_PreOrder);
  CXFA_FFWidget* GetFocusWidget();
  void KillFocus();
  FX_BOOL SetFocus(CXFA_FFWidget* hWidget);
  CXFA_FFWidget* GetWidgetByName(const CFX_WideStringC& wsName,
                                 CXFA_FFWidget* pRefWidget = NULL);
  CXFA_WidgetAcc* GetWidgetAccByName(const CFX_WideStringC& wsName,
                                     CXFA_WidgetAcc* pRefWidgetAcc = NULL);
  CXFA_LayoutProcessor* GetXFALayout() const;
  void OnPageEvent(CXFA_ContainerLayoutItem* pSender,
                   XFA_PAGEEVENT eEvent,
                   int32_t iPageIndex);
  void LockUpdate();
  void UnlockUpdate();
  FX_BOOL IsUpdateLocked();
  void ClearInvalidateList();
  void AddInvalidateRect(CXFA_FFWidget* pWidget, const CFX_RectF& rtInvalidate);
  void AddInvalidateRect(CXFA_FFPageView* pPageView,
                         const CFX_RectF& rtInvalidate);
  void RunInvalidate();
  void RunDocClose();
  void DestroyDocView();

  FX_BOOL InitValidate(CXFA_Node* pNode);
  FX_BOOL RunValidate();

  void SetChangeMark();

  void AddValidateWidget(CXFA_WidgetAcc* pWidget);
  void AddCalculateNodeNotify(CXFA_Node* pNodeChange);
  void AddCalculateWidgetAcc(CXFA_WidgetAcc* pWidgetAcc);
  int32_t RunCalculateWidgets();
  FX_BOOL IsStaticNotify();
  FX_BOOL RunLayout();
  void RunSubformIndexChange();
  void AddNewFormNode(CXFA_Node* pNode);
  void AddIndexChangedSubform(CXFA_Node* pNode);
  CXFA_WidgetAcc* GetFocusWidgetAcc();
  void SetFocusWidgetAcc(CXFA_WidgetAcc* pWidgetAcc);
  void DeleteLayoutItem(CXFA_FFWidget* pWidget);
  int32_t ExecEventActivityByDeepFirst(CXFA_Node* pFormNode,
                                       XFA_EVENTTYPE eEventType,
                                       FX_BOOL bIsFormReady = FALSE,
                                       FX_BOOL bRecursive = TRUE,
                                       CXFA_Node* pExclude = NULL);
  FX_BOOL m_bLayoutEvent;
  CFX_WideStringArray m_arrNullTestMsg;
  CXFA_FFWidget* m_pListFocusWidget;
  FX_BOOL m_bInLayoutStatus;

 protected:
  FX_BOOL RunEventLayoutReady();
  void RunBindItems();
  FX_BOOL InitCalculate(CXFA_Node* pNode);
  void InitLayout(CXFA_Node* pNode);
  void RunCalculateRecursive(int32_t& iIndex);
  void ShowNullTestMsg();
  FX_BOOL ResetSingleWidgetAccData(CXFA_WidgetAcc* pWidgetAcc);
  CXFA_Node* GetRootSubform();

  CXFA_FFDoc* m_pDoc;
  CXFA_FFWidgetHandler* m_pWidgetHandler;
  CXFA_LayoutProcessor* m_pXFADocLayout;
  CXFA_WidgetAcc* m_pFocusAcc;
  CXFA_FFWidget* m_pFocusWidget;
  CXFA_FFWidget* m_pOldFocusWidget;
  CFX_MapPtrToPtr m_mapPageInvalidate;
  CFX_PtrArray m_ValidateAccs;
  CFX_PtrArray m_bindItems;
  CFX_PtrArray m_CalculateAccs;

  CFX_PtrArray m_NewAddedNodes;
  CFX_PtrArray m_IndexChangedSubforms;
  XFA_DOCVIEW_LAYOUTSTATUS m_iStatus;
  int32_t m_iLock;
  friend class CXFA_FFNotify;
};
class CXFA_FFDocWidgetIterator : public IXFA_WidgetIterator {
 public:
  CXFA_FFDocWidgetIterator(CXFA_FFDocView* pDocView, CXFA_Node* pTravelRoot);
  virtual ~CXFA_FFDocWidgetIterator();

  virtual void Release() { delete this; }

  virtual void Reset();
  virtual CXFA_FFWidget* MoveToFirst();
  virtual CXFA_FFWidget* MoveToLast();
  virtual CXFA_FFWidget* MoveToNext();
  virtual CXFA_FFWidget* MoveToPrevious();
  virtual CXFA_FFWidget* GetCurrentWidget();
  virtual FX_BOOL SetCurrentWidget(CXFA_FFWidget* hWidget);

 protected:
  CXFA_ContainerIterator m_ContentIterator;
  CXFA_FFDocView* m_pDocView;
  CXFA_FFWidget* m_pCurWidget;
};
class CXFA_WidgetAccIterator {
 public:
  CXFA_WidgetAccIterator(CXFA_FFDocView* pDocView, CXFA_Node* pTravelRoot);
  ~CXFA_WidgetAccIterator();

  void Release() { delete this; }
  void Reset();
  CXFA_WidgetAcc* MoveToFirst();
  CXFA_WidgetAcc* MoveToLast();
  CXFA_WidgetAcc* MoveToNext();
  CXFA_WidgetAcc* MoveToPrevious();
  CXFA_WidgetAcc* GetCurrentWidgetAcc();
  FX_BOOL SetCurrentWidgetAcc(CXFA_WidgetAcc* hWidget);
  void SkipTree();

 protected:
  CXFA_ContainerIterator m_ContentIterator;
  CXFA_FFDocView* m_pDocView;
  CXFA_WidgetAcc* m_pCurWidgetAcc;
};

#endif  // XFA_INCLUDE_FXFA_XFA_FFDOCVIEW_H_
