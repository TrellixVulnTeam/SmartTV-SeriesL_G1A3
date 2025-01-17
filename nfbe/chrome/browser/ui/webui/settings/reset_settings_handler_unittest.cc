// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include "base/macros.h"
#include "base/values.h"
#include "chrome/browser/profile_resetter/profile_resetter.h"
#include "chrome/browser/ui/webui/settings/reset_settings_handler.h"
#include "chrome/test/base/testing_profile.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/test/test_browser_thread_bundle.h"
#include "content/public/test/test_web_ui.h"
#include "testing/gtest/include/gtest/gtest.h"

using settings::ResetSettingsHandler;

namespace {

// Mock version of ProfileResetter to be used in tests.
class MockProfileResetter : public ProfileResetter {
 public:
  explicit MockProfileResetter(TestingProfile* profile)
      : ProfileResetter(profile) {
  }

  bool IsActive() const override {
    return false;
  }

  void Reset(ResettableFlags resettable_flags,
             std::unique_ptr<BrandcodedDefaultSettings> master_settings,
             const base::Closure& callback) override {
    resets_++;
    callback.Run();
  }

  size_t resets() const {
    return resets_;
  }

 private:
  size_t resets_ = 0;
};

class TestingResetSettingsHandler : public ResetSettingsHandler {
 public:
  TestingResetSettingsHandler(
      TestingProfile* profile, bool allow_powerwash, content::WebUI* web_ui)
      : ResetSettingsHandler(profile, allow_powerwash),
        resetter_(profile) {
    set_web_ui(web_ui);
  }

  size_t resets() const { return resetter_.resets(); }

  using settings::ResetSettingsHandler::HandleResetProfileSettings;

 protected:
  ProfileResetter* GetResetter() override {
    return &resetter_;
  }

private:
  MockProfileResetter resetter_;

  DISALLOW_COPY_AND_ASSIGN(TestingResetSettingsHandler);
};

class ResetSettingsHandlerTest : public testing::Test {
 public:
  ResetSettingsHandlerTest() : handler_(&profile_, false, &web_ui_) {
  }

  TestingResetSettingsHandler* handler() { return &handler_; }
  content::TestWebUI* web_ui() { return &web_ui_; }

 private:
  // The order here matters.
  content::TestBrowserThreadBundle thread_bundle_;
  TestingProfile profile_;
  content::TestWebUI web_ui_;
  TestingResetSettingsHandler handler_;
};

TEST_F(ResetSettingsHandlerTest, HandleResetProfileSettings) {
  base::ListValue list;
  std::string expected_callback_id("dummyCallbackId");
  list.AppendString(expected_callback_id);
  list.AppendBoolean(false);
  handler()->HandleResetProfileSettings(&list);
  // Check that the delegate ProfileResetter was called.
  EXPECT_EQ(1u, handler()->resets());
  // Check that Javascript side is notified after resetting is done.
  EXPECT_EQ("cr.webUIResponse",
            web_ui()->call_data()[0]->function_name());
  std::string callback_id;
  EXPECT_TRUE(web_ui()->call_data()[0]->arg1()->GetAsString(&callback_id));
  EXPECT_EQ(expected_callback_id, callback_id);
}

}  // namespace
