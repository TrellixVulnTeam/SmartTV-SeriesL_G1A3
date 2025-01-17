// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/android/shell_manager.h"

#include "base/android/jni_android.h"
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#include "base/bind.h"
#include "base/lazy_instance.h"
#include "content/public/browser/web_contents.h"
#include "content/shell/browser/shell.h"
#include "content/shell/browser/shell_browser_context.h"
#if defined(HBBTV_ON)
#include "content/shell/browser/shell_browser_main_parts.h"
#endif
#include "content/shell/browser/shell_content_browser_client.h"
#if defined(HBBTV_ON)
#include "content/shell/browser/acs/hbbtv/hbbtv_app_mgr.h"
#endif
#include "jni/ShellManager_jni.h"
#include "url/gurl.h"

using base::android::ScopedJavaLocalRef;

namespace {

struct GlobalState {
  GlobalState() {}
  base::android::ScopedJavaGlobalRef<jobject> j_shell_manager;
};

base::LazyInstance<GlobalState> g_global_state = LAZY_INSTANCE_INITIALIZER;

}  // namespace

namespace content {

ScopedJavaLocalRef<jobject> CreateShellView(Shell* shell) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_shell_manager = g_global_state.Get().j_shell_manager.obj();
  return Java_ShellManager_createShell(env, j_shell_manager,
                                       reinterpret_cast<intptr_t>(shell));
}

void RemoveShellView(jobject shell_view) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_shell_manager = g_global_state.Get().j_shell_manager.obj();
  Java_ShellManager_removeShell(env, j_shell_manager, shell_view);
}

void Focus(Shell* shell) {
  JNIEnv* env = base::android::AttachCurrentThread();
  jobject j_shell_manager = g_global_state.Get().j_shell_manager.obj();
  Java_ShellManager_focus(env, j_shell_manager, reinterpret_cast<intptr_t>(shell));
}

// Register native methods
bool RegisterShellManager(JNIEnv* env) {
  return RegisterNativesImpl(env);
}

static void Init(JNIEnv* env,
                 const JavaParamRef<jclass>& clazz,
                 const JavaParamRef<jobject>& obj) {
  g_global_state.Get().j_shell_manager.Reset(obj);
}

void LaunchShell(JNIEnv* env,
                 const JavaParamRef<jclass>& clazz,
                 const JavaParamRef<jstring>& jurl) {
#if defined(HBBTV_ON)
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  hbbtv::HBBTVAppMgr* mgr = ShellContentBrowserClient::Get()->shell_browser_main_parts()->hbbtv_app_mgr();
  // load the given start page as an application and show it
  // Browser process should be launched before launching shell.
  // The initialization process creates HBBTVAppMgr.
  CHECK(mgr);
  mgr->LoadApp(url);
#else
  ShellBrowserContext* browserContext =
      ShellContentBrowserClient::Get()->browser_context();
  GURL url(base::android::ConvertJavaStringToUTF8(env, jurl));
  Shell::CreateNewWindow(browserContext,
                         url,
                         NULL,
                         gfx::Size());
#endif
}

}  // namespace content
