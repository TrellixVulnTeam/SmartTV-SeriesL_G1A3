// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_DEPENDENCY_MANAGER_H_
#define COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_DEPENDENCY_MANAGER_H_

#include "base/callback_forward.h"
#include "base/callback_list.h"
#include "base/memory/scoped_ptr.h"
#include "components/keyed_service/core/dependency_manager.h"
#include "components/keyed_service/core/keyed_service_export.h"

class BrowserContextKeyedBaseFactory;

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}  // namespace base

namespace content {
class BrowserContext;
}

namespace user_prefs {
class PrefRegistrySyncable;
}

// A singleton that listens for context destruction notifications and
// rebroadcasts them to each BrowserContextKeyedBaseFactory in a safe order
// based on the stated dependencies by each service.
class KEYED_SERVICE_EXPORT BrowserContextDependencyManager
    : public DependencyManager {
 public:
  // Registers profile-specific preferences for all services via |registry|.
  // |context| should be the BrowserContext containing |registry| and is used as
  // a key to prevent multiple registrations on the same BrowserContext in
  // tests.
  void RegisterProfilePrefsForServices(
      content::BrowserContext* context,
      user_prefs::PrefRegistrySyncable* registry);

  // Called by each BrowserContext to alert us of its creation. Several
  // services want to be started when a context is created. If you want your
  // KeyedService to be started with the BrowserContext, override
  // BrowserContextKeyedBaseFactory::ServiceIsCreatedWithBrowserContext() to
  // return true. This method also registers any service-related preferences
  // for non-incognito profiles.
  void CreateBrowserContextServices(content::BrowserContext* context);

  // Similar to CreateBrowserContextServices(), except this is used for creating
  // test BrowserContexts - these contexts will not create services for any
  // BrowserContextKeyedBaseFactories that return true from
  // ServiceIsNULLWhileTesting().
  void CreateBrowserContextServicesForTest(content::BrowserContext* context);

  // Called by each BrowserContext to alert us that we should destroy services
  // associated with it.
  void DestroyBrowserContextServices(content::BrowserContext* context);

  // Registers a |callback| that will be called just before executing
  // CreateBrowserContextServices() or CreateBrowserContextServicesForTest().
  // This can be useful in browser tests which wish to substitute test or mock
  // builders for the keyed services.
  scoped_ptr<base::CallbackList<void(content::BrowserContext*)>::Subscription>
  RegisterWillCreateBrowserContextServicesCallbackForTesting(
      const base::Callback<void(content::BrowserContext*)>& callback);

#ifndef NDEBUG
  // Debugging assertion called as part of GetServiceForBrowserContext in debug
  // mode. This will NOTREACHED() whenever the user is trying to access a stale
  // BrowserContext*.
  void AssertBrowserContextWasntDestroyed(content::BrowserContext* context);

  // Marks |context| as live (i.e., not stale). This method can be called as a
  // safeguard against |AssertBrowserContextWasntDestroyed()| checks going off
  // due to |context| aliasing a BrowserContext instance from a prior test
  // (i.e., 0xWhatever might be created, be destroyed, and then a new
  // BrowserContext object might be created at 0xWhatever).
  void MarkBrowserContextLiveForTesting(content::BrowserContext* context);
#endif  // NDEBUG

  static BrowserContextDependencyManager* GetInstance();

 private:
  friend class BrowserContextDependencyManagerUnittests;
  friend struct base::DefaultSingletonTraits<BrowserContextDependencyManager>;

  // Helper function used by CreateBrowserContextServices[ForTest].
  void DoCreateBrowserContextServices(content::BrowserContext* context,
                                      bool is_testing_context);

  BrowserContextDependencyManager();
  ~BrowserContextDependencyManager() override;

#ifndef NDEBUG
  // DependencyManager:
  void DumpContextDependencies(base::SupportsUserData* context) const final;
#endif  // NDEBUG

  // A list of callbacks to call just before executing
  // CreateBrowserContextServices() or CreateBrowserContextServicesForTest().
  base::CallbackList<void(content::BrowserContext*)>
      will_create_browser_context_services_callbacks_;
};

#endif  // COMPONENTS_KEYED_SERVICE_CONTENT_BROWSER_CONTEXT_DEPENDENCY_MANAGER_H_
