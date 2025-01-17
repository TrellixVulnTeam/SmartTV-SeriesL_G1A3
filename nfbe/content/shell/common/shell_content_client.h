// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_COMMON_SHELL_CONTENT_CLIENT_H_
#define CONTENT_SHELL_COMMON_SHELL_CONTENT_CLIENT_H_

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "content/public/common/content_client.h"

#if defined(ENABLE_PLUGINS)
#include "content/public/common/pepper_plugin_info.h"
#if defined(HBBTV_ON)
#include "content/renderer/cbip_plugin/cbip_constants.h"
#include "content/renderer/cbip_plugin/cbip_plugin.h"
#endif
#include "ppapi/shared_impl/ppapi_permissions.h"
#endif

namespace content {

std::string GetShellUserAgent();

class ShellContentClient : public ContentClient {
 public:
  ShellContentClient();
  ~ShellContentClient() override;

#if defined(ENABLE_PLUGINS)
#if defined(HBBTV_ON)
  static void SetCBIPEntryFunctions(
      content::PepperPluginInfo::GetInterfaceFunc get_interface,
      content::PepperPluginInfo::PPP_InitializeModuleFunc initialize_module,
      content::PepperPluginInfo::PPP_ShutdownModuleFunc shutdown_module);
#endif
  void AddPepperPlugins(
      std::vector<content::PepperPluginInfo>* plugins) override;
#endif
  std::string GetUserAgent() const override;
  base::string16 GetLocalizedString(int message_id) const override;
  base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const override;
  base::RefCountedStaticMemory* GetDataResourceBytes(
      int resource_id) const override;
  gfx::Image& GetNativeImageNamed(int resource_id) const override;
  bool IsSupplementarySiteIsolationModeEnabled() override;
  base::StringPiece GetOriginTrialPublicKey() override;

 private:
  base::StringPiece origin_trial_public_key_;
};

}  // namespace content

#endif  // CONTENT_SHELL_COMMON_SHELL_CONTENT_CLIENT_H_
