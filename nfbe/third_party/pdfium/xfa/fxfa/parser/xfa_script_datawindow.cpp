// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_script_datawindow.h"

#include "xfa/fxfa/fm2js/xfa_fm2jsapi.h"
#include "xfa/fxfa/parser/xfa_docdata.h"
#include "xfa/fxfa/parser/xfa_doclayout.h"
#include "xfa/fxfa/parser/xfa_document.h"
#include "xfa/fxfa/parser/xfa_localemgr.h"
#include "xfa/fxfa/parser/xfa_object.h"
#include "xfa/fxfa/parser/xfa_parser.h"
#include "xfa/fxfa/parser/xfa_script.h"
#include "xfa/fxfa/parser/xfa_utils.h"
#include "xfa/fxjse/cfxjse_arguments.h"

CScript_DataWindow::CScript_DataWindow(CXFA_Document* pDocument)
    : CXFA_OrdinaryObject(pDocument, XFA_ELEMENT_DataWindow) {
  m_uScriptHash = XFA_HASHCODE_DataWindow;
}
CScript_DataWindow::~CScript_DataWindow() {}
void CScript_DataWindow::Script_DataWindow_MoveCurrentRecord(
    CFXJSE_Arguments* pArguments) {}
void CScript_DataWindow::Script_DataWindow_Record(
    CFXJSE_Arguments* pArguments) {}
void CScript_DataWindow::Script_DataWindow_GotoRecord(
    CFXJSE_Arguments* pArguments) {}
void CScript_DataWindow::Script_DataWindow_IsRecordGroup(
    CFXJSE_Arguments* pArguments) {}
void CScript_DataWindow::Script_DataWindow_RecordsBefore(
    FXJSE_HVALUE hValue,
    FX_BOOL bSetting,
    XFA_ATTRIBUTE eAttribute) {}
void CScript_DataWindow::Script_DataWindow_CurrentRecordNumber(
    FXJSE_HVALUE hValue,
    FX_BOOL bSetting,
    XFA_ATTRIBUTE eAttribute) {}
void CScript_DataWindow::Script_DataWindow_RecordsAfter(
    FXJSE_HVALUE hValue,
    FX_BOOL bSetting,
    XFA_ATTRIBUTE eAttribute) {}
void CScript_DataWindow::Script_DataWindow_IsDefined(FXJSE_HVALUE hValue,
                                                     FX_BOOL bSetting,
                                                     XFA_ATTRIBUTE eAttribute) {
}
