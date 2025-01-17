// Copyright (c) 2016 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//

#ifndef CEF_INCLUDE_CAPI_CEF_HBBTV_APP_MGR_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_HBBTV_APP_MGR_CAPI_H_
#pragma once

#include "include/capi/cef_base_capi.h"

#ifdef __cplusplus
extern "C" {
#endif


///
// Structure wrapping the single instance of HBBTV Application Manager. Can be
// used on the browser host process.
///
typedef struct _cef_hbbtvapp_mgr_t {
  ///
  // Base structure.
  ///
  cef_base_t base;

  // n.b. If "default_retval" contains '-' (e.g. -1, 0-1), translator.py does
  // not generate wrappers.  there are 2 ways to overcome.
  //   1. even though our internal implementation uses negative default value,
  //      let the corresponding cef interface uses positive default value.
  //      (i.e. content::hbbtv:E_GENERIC==-1 but default_retval=1)
  //   2. create a cef return value type, and use a symbolic name.
  // we have chosen 2.
  ///
  // Call this when sending an table with updated AIT to the HBBTV Application
  // manager. It is also called internally when an XML AIT Update is incoming.
  // This function will parse each AIT one by one and create applications
  // accordingly. This will also kill or start applications depending on their
  // signaling.
  //
  // The param |in_ai| is an array of Application Information Table structures.
  // The param |in_count| is the length of the AIT array. The param
  // |in_isChannelChange| is true (1) if the AIT update was generated by a
  // broadcast channel change. The param |in_isXMLAIT| is true (1) if the AIT
  // update was generated after downloading an XML AIT. This returns E_OK in
  // case of success and  E_GENERIC or E_NOTFOUND otherwise.
  ///
  cef_hbbtv_on_aitupdate_return_value_t (CEF_CALLBACK *on_aitupdate)(
      struct _cef_hbbtvapp_mgr_t* self,
      struct _cef_hbbtv_dvb_application_information_t* in_ai, int in_count,
      int in_isChannelChange, int in_isXMLAIT);

  ///
  // Returns a pointer to the single HBBTV App Manager instance or NULL if this
  // was not yet initialized.
  ///
  cef_hbbtvapp_mgr_ptr_t (CEF_CALLBACK *get_hbbtvapp_mgr)(
      struct _cef_hbbtvapp_mgr_t* self);

  ///
  // Handling some events for test purposes. Not needed for development purposes
  // Return false (0) if the event should be sent further to the core and true
  // (1) if the event was handled here.
  ///
  int (CEF_CALLBACK *pre_handle_keyboard_event)(
      struct _cef_hbbtvapp_mgr_t* self, const struct _cef_key_event_t* event,
      cef_event_handle_t os_event);
} cef_hbbtvapp_mgr_t;


///
// Create a new cef_hbbtvapp_mgr_t object if none was ever created. Otherwise
// return a reference to the existing object.
///
CEF_EXPORT cef_hbbtvapp_mgr_t* cef_hbbtvapp_mgr_create();


#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_HBBTV_APP_MGR_CAPI_H_
