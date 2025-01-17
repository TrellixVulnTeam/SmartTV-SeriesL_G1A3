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

#include "libcef_dll/cpptoc/hbbtvapp_mgr_cpptoc.h"


// GLOBAL FUNCTIONS - Body may be edited by hand.

CEF_EXPORT cef_hbbtvapp_mgr_t* cef_hbbtvapp_mgr_create() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefRefPtr<CefHBBTVAppMgr> _retval = CefHBBTVAppMgr::Create();

  // Return type: refptr_same
  return CefHBBTVAppMgrCppToC::Wrap(_retval);
}


namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

cef_hbbtv_on_aitupdate_return_value_t CEF_CALLBACK hbbtvapp_mgr_on_aitupdate(
    struct _cef_hbbtvapp_mgr_t* self,
    struct _cef_hbbtv_dvb_application_information_t* in_ai, int in_count,
    int in_isChannelChange, int in_isXMLAIT)
{
  DCHECK(self);
  if (!self)
    return ON_AITUPDATE_E_GENERIC;
  cef_hbbtv_on_aitupdate_return_value_t _retval = CefHBBTVAppMgrCppToC::Get(
      self)->OnAITUpdate(in_ai,
                         in_count,
                         in_isChannelChange?true:false,
                         in_isXMLAIT?true:false);
  return _retval;
}
cef_hbbtvapp_mgr_ptr_t CEF_CALLBACK hbbtvapp_mgr_get_hbbtvapp_mgr(
    struct _cef_hbbtvapp_mgr_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return NULL;

  // Execute
  cef_hbbtvapp_mgr_ptr_t _retval = CefHBBTVAppMgrCppToC::Get(
      self)->GetHBBTVAppMgr();

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK hbbtvapp_mgr_pre_handle_keyboard_event(
    struct _cef_hbbtvapp_mgr_t* self, const struct _cef_key_event_t* event,
    cef_event_handle_t os_event) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: event; type: struct_byref_const
  DCHECK(event);
  if (!event)
    return 0;

  // Translate param: event; type: struct_byref_const
  CefKeyEvent eventObj;
  if (event)
    eventObj.Set(*event, false);

  // Execute
  bool _retval = CefHBBTVAppMgrCppToC::Get(self)->PreHandleKeyboardEvent(
      eventObj,
      os_event);

  // Return type: bool
  return _retval;
}

}  // namespace


// CONSTRUCTOR - Do not edit by hand.

CefHBBTVAppMgrCppToC::CefHBBTVAppMgrCppToC() {
  GetStruct()->on_aitupdate = hbbtvapp_mgr_on_aitupdate;
  GetStruct()->get_hbbtvapp_mgr = hbbtvapp_mgr_get_hbbtvapp_mgr;
  GetStruct()->pre_handle_keyboard_event =
      hbbtvapp_mgr_pre_handle_keyboard_event;
}

template<> CefRefPtr<CefHBBTVAppMgr> CefCppToC<CefHBBTVAppMgrCppToC,
    CefHBBTVAppMgr, cef_hbbtvapp_mgr_t>::UnwrapDerived(CefWrapperType type,
    cef_hbbtvapp_mgr_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#ifndef NDEBUG
template<> base::AtomicRefCount CefCppToC<CefHBBTVAppMgrCppToC, CefHBBTVAppMgr,
    cef_hbbtvapp_mgr_t>::DebugObjCt = 0;
#endif

template<> CefWrapperType CefCppToC<CefHBBTVAppMgrCppToC, CefHBBTVAppMgr,
    cef_hbbtvapp_mgr_t>::kWrapperType = WT_HBBTVAPP_MGR;
