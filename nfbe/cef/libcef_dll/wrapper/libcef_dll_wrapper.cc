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

#include "include/cef_app.h"
#include "include/capi/cef_app_capi.h"
#include "include/cef_geolocation.h"
#include "include/capi/cef_geolocation_capi.h"
#include "include/cef_origin_whitelist.h"
#include "include/capi/cef_origin_whitelist_capi.h"
#include "include/cef_parser.h"
#include "include/capi/cef_parser_capi.h"
#include "include/cef_path_util.h"
#include "include/capi/cef_path_util_capi.h"
#include "include/cef_process_util.h"
#include "include/capi/cef_process_util_capi.h"
#include "include/cef_scheme.h"
#include "include/capi/cef_scheme_capi.h"
#include "include/cef_task.h"
#include "include/capi/cef_task_capi.h"
#include "include/cef_trace.h"
#include "include/capi/cef_trace_capi.h"
#include "include/cef_v8.h"
#include "include/capi/cef_v8_capi.h"
#include "include/cef_web_plugin.h"
#include "include/capi/cef_web_plugin_capi.h"
#include "include/cef_version.h"
#include "libcef_dll/cpptoc/app_cpptoc.h"
#include "libcef_dll/cpptoc/browser_process_handler_cpptoc.h"
#include "libcef_dll/cpptoc/views/browser_view_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/views/button_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/completion_callback_cpptoc.h"
#include "libcef_dll/cpptoc/context_menu_handler_cpptoc.h"
#include "libcef_dll/cpptoc/cookie_visitor_cpptoc.h"
#include "libcef_dll/cpptoc/domvisitor_cpptoc.h"
#include "libcef_dll/cpptoc/delete_cookies_callback_cpptoc.h"
#include "libcef_dll/cpptoc/dialog_handler_cpptoc.h"
#include "libcef_dll/cpptoc/display_handler_cpptoc.h"
#include "libcef_dll/cpptoc/download_handler_cpptoc.h"
#include "libcef_dll/cpptoc/download_image_callback_cpptoc.h"
#include "libcef_dll/cpptoc/drag_handler_cpptoc.h"
#include "libcef_dll/cpptoc/end_tracing_callback_cpptoc.h"
#include "libcef_dll/cpptoc/find_handler_cpptoc.h"
#include "libcef_dll/cpptoc/focus_handler_cpptoc.h"
#include "libcef_dll/cpptoc/geolocation_handler_cpptoc.h"
#include "libcef_dll/cpptoc/get_geolocation_callback_cpptoc.h"
#include "libcef_dll/cpptoc/jsdialog_handler_cpptoc.h"
#include "libcef_dll/cpptoc/keyboard_handler_cpptoc.h"
#include "libcef_dll/cpptoc/life_span_handler_cpptoc.h"
#include "libcef_dll/cpptoc/load_handler_cpptoc.h"
#include "libcef_dll/cpptoc/views/menu_button_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/menu_model_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/navigation_entry_visitor_cpptoc.h"
#include "libcef_dll/cpptoc/views/panel_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/pdf_print_callback_cpptoc.h"
#include "libcef_dll/cpptoc/print_handler_cpptoc.h"
#include "libcef_dll/cpptoc/read_handler_cpptoc.h"
#include "libcef_dll/cpptoc/render_handler_cpptoc.h"
#include "libcef_dll/cpptoc/render_process_handler_cpptoc.h"
#include "libcef_dll/cpptoc/request_handler_cpptoc.h"
#include "libcef_dll/cpptoc/resolve_callback_cpptoc.h"
#include "libcef_dll/cpptoc/resource_bundle_handler_cpptoc.h"
#include "libcef_dll/cpptoc/resource_handler_cpptoc.h"
#include "libcef_dll/cpptoc/response_filter_cpptoc.h"
#include "libcef_dll/cpptoc/run_file_dialog_callback_cpptoc.h"
#include "libcef_dll/cpptoc/scheme_handler_factory_cpptoc.h"
#include "libcef_dll/cpptoc/set_cookie_callback_cpptoc.h"
#include "libcef_dll/cpptoc/string_visitor_cpptoc.h"
#include "libcef_dll/cpptoc/task_cpptoc.h"
#include "libcef_dll/cpptoc/views/textfield_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/urlrequest_client_cpptoc.h"
#include "libcef_dll/cpptoc/v8accessor_cpptoc.h"
#include "libcef_dll/cpptoc/v8handler_cpptoc.h"
#include "libcef_dll/cpptoc/views/view_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/web_plugin_info_visitor_cpptoc.h"
#include "libcef_dll/cpptoc/web_plugin_unstable_callback_cpptoc.h"
#include "libcef_dll/cpptoc/views/window_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/write_handler_cpptoc.h"
#include "libcef_dll/ctocpp/auth_callback_ctocpp.h"
#include "libcef_dll/ctocpp/before_download_callback_ctocpp.h"
#include "libcef_dll/ctocpp/binary_value_ctocpp.h"
#include "libcef_dll/ctocpp/views/box_layout_ctocpp.h"
#include "libcef_dll/ctocpp/browser_ctocpp.h"
#include "libcef_dll/ctocpp/browser_host_ctocpp.h"
#include "libcef_dll/ctocpp/views/browser_view_ctocpp.h"
#include "libcef_dll/ctocpp/views/button_ctocpp.h"
#include "libcef_dll/ctocpp/callback_ctocpp.h"
#include "libcef_dll/ctocpp/command_line_ctocpp.h"
#include "libcef_dll/ctocpp/context_menu_params_ctocpp.h"
#include "libcef_dll/ctocpp/domdocument_ctocpp.h"
#include "libcef_dll/ctocpp/domnode_ctocpp.h"
#include "libcef_dll/ctocpp/dictionary_value_ctocpp.h"
#include "libcef_dll/ctocpp/views/display_ctocpp.h"
#include "libcef_dll/ctocpp/download_item_ctocpp.h"
#include "libcef_dll/ctocpp/download_item_callback_ctocpp.h"
#include "libcef_dll/ctocpp/drag_data_ctocpp.h"
#include "libcef_dll/ctocpp/file_dialog_callback_ctocpp.h"
#include "libcef_dll/ctocpp/views/fill_layout_ctocpp.h"
#include "libcef_dll/ctocpp/frame_ctocpp.h"
#include "libcef_dll/ctocpp/geolocation_callback_ctocpp.h"
#include "libcef_dll/ctocpp/hbbtvapp_mgr_ctocpp.h"
#include "libcef_dll/ctocpp/image_ctocpp.h"
#include "libcef_dll/ctocpp/jsdialog_callback_ctocpp.h"
#include "libcef_dll/ctocpp/views/label_button_ctocpp.h"
#include "libcef_dll/ctocpp/views/layout_ctocpp.h"
#include "libcef_dll/ctocpp/list_value_ctocpp.h"
#include "libcef_dll/ctocpp/views/menu_button_ctocpp.h"
#include "libcef_dll/ctocpp/menu_model_ctocpp.h"
#include "libcef_dll/ctocpp/navigation_entry_ctocpp.h"
#include "libcef_dll/ctocpp/views/panel_ctocpp.h"
#include "libcef_dll/ctocpp/print_dialog_callback_ctocpp.h"
#include "libcef_dll/ctocpp/print_job_callback_ctocpp.h"
#include "libcef_dll/ctocpp/print_settings_ctocpp.h"
#include "libcef_dll/ctocpp/process_message_ctocpp.h"
#include "libcef_dll/ctocpp/request_callback_ctocpp.h"
#include "libcef_dll/ctocpp/run_context_menu_callback_ctocpp.h"
#include "libcef_dll/ctocpp/sslcert_principal_ctocpp.h"
#include "libcef_dll/ctocpp/sslinfo_ctocpp.h"
#include "libcef_dll/ctocpp/scheme_registrar_ctocpp.h"
#include "libcef_dll/ctocpp/views/scroll_view_ctocpp.h"
#include "libcef_dll/ctocpp/stream_reader_ctocpp.h"
#include "libcef_dll/ctocpp/stream_writer_ctocpp.h"
#include "libcef_dll/ctocpp/task_runner_ctocpp.h"
#include "libcef_dll/ctocpp/views/textfield_ctocpp.h"
#include "libcef_dll/ctocpp/urlrequest_ctocpp.h"
#include "libcef_dll/ctocpp/v8context_ctocpp.h"
#include "libcef_dll/ctocpp/v8exception_ctocpp.h"
#include "libcef_dll/ctocpp/v8stack_frame_ctocpp.h"
#include "libcef_dll/ctocpp/v8stack_trace_ctocpp.h"
#include "libcef_dll/ctocpp/v8value_ctocpp.h"
#include "libcef_dll/ctocpp/value_ctocpp.h"
#include "libcef_dll/ctocpp/views/view_ctocpp.h"
#include "libcef_dll/ctocpp/web_plugin_info_ctocpp.h"
#include "libcef_dll/ctocpp/views/window_ctocpp.h"
#include "libcef_dll/ctocpp/xml_reader_ctocpp.h"
#include "libcef_dll/ctocpp/zip_reader_ctocpp.h"
#include "libcef_dll/transfer_util.h"

// Define used to facilitate parsing.
#define CEF_GLOBAL


// GLOBAL METHODS - Body may be edited by hand.

CEF_GLOBAL int CefExecuteProcess(const CefMainArgs& args,
    CefRefPtr<CefApp> application, void* windows_sandbox_info) {
  const char* api_hash = cef_api_hash(0);
  if (strcmp(api_hash, CEF_API_HASH_PLATFORM)) {
    // The libcef API hash does not match the current header API hash.
    NOTREACHED();
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: application, windows_sandbox_info

  // Execute
  int _retval = cef_execute_process(
      &args,
      CefAppCppToC::Wrap(application),
      windows_sandbox_info);

  // Return type: simple
  return _retval;
}

CEF_GLOBAL bool CefInitialize(const CefMainArgs& args,
    const CefSettings& settings, CefRefPtr<CefApp> application,
    void* windows_sandbox_info) {
  const char* api_hash = cef_api_hash(0);
  if (strcmp(api_hash, CEF_API_HASH_PLATFORM)) {
    // The libcef API hash does not match the current header API hash.
    NOTREACHED();
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: application, windows_sandbox_info

  // Execute
  int _retval = cef_initialize(
      &args,
      &settings,
      CefAppCppToC::Wrap(application),
      windows_sandbox_info);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL void CefShutdown() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_shutdown();

#ifndef NDEBUG
  // Check that all wrapper objects have been destroyed
  DCHECK(base::AtomicRefCountIsZero(&CefAuthCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefBeforeDownloadCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBinaryValueCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBoxLayoutCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBrowserCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBrowserHostCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefBrowserProcessHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBrowserViewCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefBrowserViewDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefButtonCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefButtonDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefCompletionCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefContextMenuHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefContextMenuParamsCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefCookieVisitorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDOMDocumentCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDOMNodeCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDOMVisitorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefDeleteCookiesCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDialogHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDictionaryValueCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDisplayCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDisplayHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDownloadHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefDownloadImageCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDownloadItemCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefDownloadItemCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDragDataCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefDragHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefEndTracingCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefFileDialogCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefFillLayoutCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefFindHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefFocusHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefFrameCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefGeolocationCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefGeolocationHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefGetGeolocationCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefHBBTVAppMgrCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefImageCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefJSDialogCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefJSDialogHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefKeyboardHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefLabelButtonCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefLayoutCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefLifeSpanHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefListValueCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefLoadHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefMenuButtonCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefMenuButtonDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefMenuModelCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefMenuModelDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefNavigationEntryCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefNavigationEntryVisitorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPanelCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPanelDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPdfPrintCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPrintDialogCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPrintHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPrintJobCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefPrintSettingsCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefProcessMessageCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefReadHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefRenderHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefRenderProcessHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefRequestCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefRequestHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefResolveCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefResourceBundleHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefResourceHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefResponseFilterCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefRunContextMenuCallbackCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefRunFileDialogCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefSSLCertPrincipalCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefSSLInfoCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefSchemeHandlerFactoryCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefSchemeRegistrarCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefScrollViewCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefSetCookieCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefStreamReaderCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefStreamWriterCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefStringVisitorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefTaskCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefTaskRunnerCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefTextfieldCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefTextfieldDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefURLRequestCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefURLRequestClientCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8AccessorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8ContextCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8ExceptionCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8HandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8StackFrameCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8StackTraceCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefV8ValueCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefValueCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefViewCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefViewDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefWebPluginInfoCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefWebPluginInfoVisitorCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(
      &CefWebPluginUnstableCallbackCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefWindowCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefWindowDelegateCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefWriteHandlerCppToC::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefXmlReaderCToCpp::DebugObjCt));
  DCHECK(base::AtomicRefCountIsZero(&CefZipReaderCToCpp::DebugObjCt));
#endif  // !NDEBUG
}

CEF_GLOBAL void CefDoMessageLoopWork() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_do_message_loop_work();
}

CEF_GLOBAL void CefRunMessageLoop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_run_message_loop();
}

CEF_GLOBAL void CefQuitMessageLoop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_quit_message_loop();
}

CEF_GLOBAL void CefSetOSModalLoop(bool osModalLoop) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_set_osmodal_loop(
      osModalLoop);
}

CEF_GLOBAL void CefEnableHighDPISupport() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_enable_highdpi_support();
}

CEF_GLOBAL bool CefGetGeolocation(
    CefRefPtr<CefGetGeolocationCallback> callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get())
    return false;

  // Execute
  int _retval = cef_get_geolocation(
      CefGetGeolocationCallbackCppToC::Wrap(callback));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefAddCrossOriginWhitelistEntry(const CefString& source_origin,
    const CefString& target_protocol, const CefString& target_domain,
    bool allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(!source_origin.empty());
  if (source_origin.empty())
    return false;
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(!target_protocol.empty());
  if (target_protocol.empty())
    return false;
  // Unverified params: target_domain

  // Execute
  int _retval = cef_add_cross_origin_whitelist_entry(
      source_origin.GetStruct(),
      target_protocol.GetStruct(),
      target_domain.GetStruct(),
      allow_target_subdomains);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefRemoveCrossOriginWhitelistEntry(
    const CefString& source_origin, const CefString& target_protocol,
    const CefString& target_domain, bool allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(!source_origin.empty());
  if (source_origin.empty())
    return false;
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(!target_protocol.empty());
  if (target_protocol.empty())
    return false;
  // Unverified params: target_domain

  // Execute
  int _retval = cef_remove_cross_origin_whitelist_entry(
      source_origin.GetStruct(),
      target_protocol.GetStruct(),
      target_domain.GetStruct(),
      allow_target_subdomains);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefClearCrossOriginWhitelist() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = cef_clear_cross_origin_whitelist();

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefParseURL(const CefString& url, CefURLParts& parts) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: url; type: string_byref_const
  DCHECK(!url.empty());
  if (url.empty())
    return false;

  // Execute
  int _retval = cef_parse_url(
      url.GetStruct(),
      &parts);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefCreateURL(const CefURLParts& parts, CefString& url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = cef_create_url(
      &parts,
      url.GetWritableStruct());

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL CefString CefFormatUrlForSecurityDisplay(
    const CefString& origin_url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: origin_url; type: string_byref_const
  DCHECK(!origin_url.empty());
  if (origin_url.empty())
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_format_url_for_security_display(
      origin_url.GetStruct());

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL CefString CefGetMimeType(const CefString& extension) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: extension; type: string_byref_const
  DCHECK(!extension.empty());
  if (extension.empty())
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_get_mime_type(
      extension.GetStruct());

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL void CefGetExtensionsForMimeType(const CefString& mime_type,
    std::vector<CefString>& extensions) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: mime_type; type: string_byref_const
  DCHECK(!mime_type.empty());
  if (mime_type.empty())
    return;

  // Translate param: extensions; type: string_vec_byref
  cef_string_list_t extensionsList = cef_string_list_alloc();
  DCHECK(extensionsList);
  if (extensionsList)
    transfer_string_list_contents(extensions, extensionsList);

  // Execute
  cef_get_extensions_for_mime_type(
      mime_type.GetStruct(),
      extensionsList);

  // Restore param:extensions; type: string_vec_byref
  if (extensionsList) {
    extensions.clear();
    transfer_string_list_contents(extensionsList, extensions);
    cef_string_list_free(extensionsList);
  }
}

CEF_GLOBAL CefString CefBase64Encode(const void* data, size_t data_size) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data)
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_base64encode(
      data,
      data_size);

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL CefRefPtr<CefBinaryValue> CefBase64Decode(const CefString& data) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: string_byref_const
  DCHECK(!data.empty());
  if (data.empty())
    return NULL;

  // Execute
  cef_binary_value_t* _retval = cef_base64decode(
      data.GetStruct());

  // Return type: refptr_same
  return CefBinaryValueCToCpp::Wrap(_retval);
}

CEF_GLOBAL CefString CefURIEncode(const CefString& text, bool use_plus) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: text; type: string_byref_const
  DCHECK(!text.empty());
  if (text.empty())
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_uriencode(
      text.GetStruct(),
      use_plus);

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL CefString CefURIDecode(const CefString& text, bool convert_to_utf8,
    cef_uri_unescape_rule_t unescape_rule) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: text; type: string_byref_const
  DCHECK(!text.empty());
  if (text.empty())
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_uridecode(
      text.GetStruct(),
      convert_to_utf8,
      unescape_rule);

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL CefRefPtr<CefValue> CefParseJSON(const CefString& json_string,
    cef_json_parser_options_t options) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: json_string; type: string_byref_const
  DCHECK(!json_string.empty());
  if (json_string.empty())
    return NULL;

  // Execute
  cef_value_t* _retval = cef_parse_json(
      json_string.GetStruct(),
      options);

  // Return type: refptr_same
  return CefValueCToCpp::Wrap(_retval);
}

CEF_GLOBAL CefRefPtr<CefValue> CefParseJSONAndReturnError(
    const CefString& json_string, cef_json_parser_options_t options,
    cef_json_parser_error_t& error_code_out, CefString& error_msg_out) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: json_string; type: string_byref_const
  DCHECK(!json_string.empty());
  if (json_string.empty())
    return NULL;

  // Execute
  cef_value_t* _retval = cef_parse_jsonand_return_error(
      json_string.GetStruct(),
      options,
      &error_code_out,
      error_msg_out.GetWritableStruct());

  // Return type: refptr_same
  return CefValueCToCpp::Wrap(_retval);
}

CEF_GLOBAL CefString CefWriteJSON(CefRefPtr<CefValue> node,
    cef_json_writer_options_t options) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: node; type: refptr_same
  DCHECK(node.get());
  if (!node.get())
    return CefString();

  // Execute
  cef_string_userfree_t _retval = cef_write_json(
      CefValueCToCpp::Unwrap(node),
      options);

  // Return type: string
  CefString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

CEF_GLOBAL bool CefGetPath(PathKey key, CefString& path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = cef_get_path(
      key,
      path.GetWritableStruct());

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefLaunchProcess(CefRefPtr<CefCommandLine> command_line) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: command_line; type: refptr_same
  DCHECK(command_line.get());
  if (!command_line.get())
    return false;

  // Execute
  int _retval = cef_launch_process(
      CefCommandLineCToCpp::Unwrap(command_line));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefRegisterSchemeHandlerFactory(const CefString& scheme_name,
    const CefString& domain_name,
    CefRefPtr<CefSchemeHandlerFactory> factory) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(!scheme_name.empty());
  if (scheme_name.empty())
    return false;
  // Unverified params: domain_name, factory

  // Execute
  int _retval = cef_register_scheme_handler_factory(
      scheme_name.GetStruct(),
      domain_name.GetStruct(),
      CefSchemeHandlerFactoryCppToC::Wrap(factory));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefClearSchemeHandlerFactories() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = cef_clear_scheme_handler_factories();

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefCurrentlyOn(CefThreadId threadId) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = cef_currently_on(
      threadId);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefPostTask(CefThreadId threadId, CefRefPtr<CefTask> task) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task.get());
  if (!task.get())
    return false;

  // Execute
  int _retval = cef_post_task(
      threadId,
      CefTaskCppToC::Wrap(task));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefPostDelayedTask(CefThreadId threadId,
    CefRefPtr<CefTask> task, int64 delay_ms) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task.get());
  if (!task.get())
    return false;

  // Execute
  int _retval = cef_post_delayed_task(
      threadId,
      CefTaskCppToC::Wrap(task),
      delay_ms);

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefBeginTracing(const CefString& categories,
    CefRefPtr<CefCompletionCallback> callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: categories, callback

  // Execute
  int _retval = cef_begin_tracing(
      categories.GetStruct(),
      CefCompletionCallbackCppToC::Wrap(callback));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL bool CefEndTracing(const CefString& tracing_file,
    CefRefPtr<CefEndTracingCallback> callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: tracing_file, callback

  // Execute
  int _retval = cef_end_tracing(
      tracing_file.GetStruct(),
      CefEndTracingCallbackCppToC::Wrap(callback));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL int64 CefNowFromSystemTraceTime() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int64 _retval = cef_now_from_system_trace_time();

  // Return type: simple
  return _retval;
}

CEF_GLOBAL bool CefRegisterExtension(const CefString& extension_name,
    const CefString& javascript_code, CefRefPtr<CefV8Handler> handler) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: extension_name; type: string_byref_const
  DCHECK(!extension_name.empty());
  if (extension_name.empty())
    return false;
  // Verify param: javascript_code; type: string_byref_const
  DCHECK(!javascript_code.empty());
  if (javascript_code.empty())
    return false;
  // Unverified params: handler

  // Execute
  int _retval = cef_register_extension(
      extension_name.GetStruct(),
      javascript_code.GetStruct(),
      CefV8HandlerCppToC::Wrap(handler));

  // Return type: bool
  return _retval?true:false;
}

CEF_GLOBAL void CefVisitWebPluginInfo(
    CefRefPtr<CefWebPluginInfoVisitor> visitor) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: visitor; type: refptr_diff
  DCHECK(visitor.get());
  if (!visitor.get())
    return;

  // Execute
  cef_visit_web_plugin_info(
      CefWebPluginInfoVisitorCppToC::Wrap(visitor));
}

CEF_GLOBAL void CefRefreshWebPlugins() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_refresh_web_plugins();
}

CEF_GLOBAL void CefUnregisterInternalWebPlugin(const CefString& path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(!path.empty());
  if (path.empty())
    return;

  // Execute
  cef_unregister_internal_web_plugin(
      path.GetStruct());
}

CEF_GLOBAL void CefRegisterWebPluginCrash(const CefString& path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(!path.empty());
  if (path.empty())
    return;

  // Execute
  cef_register_web_plugin_crash(
      path.GetStruct());
}

CEF_GLOBAL void CefIsWebPluginUnstable(const CefString& path,
    CefRefPtr<CefWebPluginUnstableCallback> callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(!path.empty());
  if (path.empty())
    return;
  // Verify param: callback; type: refptr_diff
  DCHECK(callback.get());
  if (!callback.get())
    return;

  // Execute
  cef_is_web_plugin_unstable(
      path.GetStruct(),
      CefWebPluginUnstableCallbackCppToC::Wrap(callback));
}

