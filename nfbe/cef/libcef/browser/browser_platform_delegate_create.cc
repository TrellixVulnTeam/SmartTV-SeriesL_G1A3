// Copyright 2015 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/browser_platform_delegate.h"

#include <utility>

#include "build/build_config.h"

#if defined(OS_WIN)
#include "libcef/browser/native/browser_platform_delegate_native_win.h"
#include "libcef/browser/osr/browser_platform_delegate_osr_win.h"
#elif defined(OS_MACOSX)
#include "libcef/browser/native/browser_platform_delegate_native_mac.h"
#include "libcef/browser/osr/browser_platform_delegate_osr_mac.h"
#elif defined(OS_LINUX)
#include "libcef/browser/native/browser_platform_delegate_native_linux.h"
#include "libcef/browser/osr/browser_platform_delegate_osr_linux.h"
#include "libcef/browser/native/browser_platform_delegate_native_aura.h"
#else
#error A delegate implementation is not available for your platform.
#endif

#if defined(USE_AURA) && defined(TOOKIT_VIEWS)
#include "libcef/browser/views/browser_platform_delegate_views.h"
#endif

namespace {

std::unique_ptr<CefBrowserPlatformDelegateNative> CreateNativeDelegate(
    const CefWindowInfo& window_info) {
#if defined(OS_WIN)
  return make_scoped_ptr(new CefBrowserPlatformDelegateNativeWin(window_info));
#elif defined(OS_MACOSX)
  return make_scoped_ptr(new CefBrowserPlatformDelegateNativeMac(window_info));
#elif defined(OS_LINUX)
#if !defined(USE_AURA)
  return make_scoped_ptr(
      new CefBrowserPlatformDelegateNativeLinux(window_info));
#else
  return make_scoped_ptr(
      new CefBrowserPlatformDelegateNativeAura(window_info));
#endif
#endif
}

std::unique_ptr<CefBrowserPlatformDelegateOsr> CreateOSRDelegate(
    std::unique_ptr<CefBrowserPlatformDelegateNative> native_delegate) {
#if defined(OS_WIN)
  return make_scoped_ptr(
        new CefBrowserPlatformDelegateOsrWin(std::move(native_delegate)));
#elif defined(OS_MACOSX)
  return make_scoped_ptr(
        new CefBrowserPlatformDelegateOsrMac(std::move(native_delegate)));
#elif defined(OS_LINUX)
  return make_scoped_ptr(
        new CefBrowserPlatformDelegateOsrLinux(std::move(native_delegate)));
#endif
}

}  // namespace

// static
std::unique_ptr<CefBrowserPlatformDelegate> CefBrowserPlatformDelegate::Create(
    CefBrowserHostImpl::CreateParams& create_params) {
  if (create_params.window_info) {
    std::unique_ptr<CefBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(*create_params.window_info.get());
    if (create_params.window_info->windowless_rendering_enabled &&
        create_params.client &&
        create_params.client->GetRenderHandler().get()) {
      return CreateOSRDelegate(std::move(native_delegate));
    }
    return std::move(native_delegate);
  }
#if defined(USE_AURA) && defined(TOOKIT_VIEWS)
  else {
    // CefWindowInfo is not used in this case.
    std::unique_ptr<CefBrowserPlatformDelegateNative> native_delegate =
        CreateNativeDelegate(CefWindowInfo());
    return make_scoped_ptr(new CefBrowserPlatformDelegateViews(
        std::move(native_delegate),
        static_cast<CefBrowserViewImpl*>(create_params.browser_view.get())));
  }
#endif  // defined(USE_AURA)

  NOTREACHED();
  return nullptr;
}
