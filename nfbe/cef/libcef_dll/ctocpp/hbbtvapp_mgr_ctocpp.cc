// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//

#include "libcef_dll/ctocpp/hbbtvapp_mgr_ctocpp.h"


// STATIC METHODS - Body may be edited by hand.

CefRefPtr<CefHBBTVAppMgr> CefHBBTVAppMgr::Create() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_hbbtvapp_mgr_t* _retval = cef_hbbtvapp_mgr_create();

  // Return type: refptr_same
  return CefHBBTVAppMgrCToCpp::Wrap(_retval);
}


// VIRTUAL METHODS - Body may be edited by hand.

cef_hbbtv_on_aitupdate_return_value_t CefHBBTVAppMgrCToCpp::OnAITUpdate(
    cef_hbbtv_dvb_application_information_t* in_ai, int in_count,
    bool in_isChannelChange, bool in_isXMLAIT)
{
  cef_hbbtvapp_mgr_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_aitupdate))
    return ON_AITUPDATE_E_GENERIC;

  cef_hbbtv_on_aitupdate_return_value_t _retval = _struct->on_aitupdate(
      _struct,
      in_ai,
      in_count,
      in_isChannelChange,
      in_isXMLAIT);
  return _retval;
}
CefHBBTVAppMgrPtr CefHBBTVAppMgrCToCpp::GetHBBTVAppMgr() {
  cef_hbbtvapp_mgr_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, get_hbbtvapp_mgr))
    return NULL;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_hbbtvapp_mgr_ptr_t _retval = _struct->get_hbbtvapp_mgr(_struct);

  // Return type: simple
  return _retval;
}

bool CefHBBTVAppMgrCToCpp::PreHandleKeyboardEvent(const CefKeyEvent& event,
    CefEventHandle os_event) {
  cef_hbbtvapp_mgr_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, pre_handle_keyboard_event))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->pre_handle_keyboard_event(_struct,
      &event,
      os_event);

  // Return type: bool
  return _retval?true:false;
}


// CONSTRUCTOR - Do not edit by hand.

CefHBBTVAppMgrCToCpp::CefHBBTVAppMgrCToCpp() {
}

template<> cef_hbbtvapp_mgr_t* CefCToCpp<CefHBBTVAppMgrCToCpp, CefHBBTVAppMgr,
    cef_hbbtvapp_mgr_t>::UnwrapDerived(CefWrapperType type,
    CefHBBTVAppMgr* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#ifndef NDEBUG
template<> base::AtomicRefCount CefCToCpp<CefHBBTVAppMgrCToCpp, CefHBBTVAppMgr,
    cef_hbbtvapp_mgr_t>::DebugObjCt = 0;
#endif

template<> CefWrapperType CefCToCpp<CefHBBTVAppMgrCToCpp, CefHBBTVAppMgr,
    cef_hbbtvapp_mgr_t>::kWrapperType = WT_HBBTVAPP_MGR;
