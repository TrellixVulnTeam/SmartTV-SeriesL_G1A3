// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_MEMORY_ACS_LOW_MEMORY_LISTENER_H_
#define CONTENT_MEMORY_ACS_LOW_MEMORY_LISTENER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"

namespace content {

class LowMemoryListenerDelegate;
class LowMemoryListenerImpl;

////////////////////////////////////////////////////////////////////////////////
// LowMemoryListener
//
// Class to handle observation of low memory device for changes so that we
// can get a signal from the kernel about low memory conditions and notify
// our delegate, who can take actions such as discarding tabs.
//
// This object is intended to be created and used on the UI thread. All
// notifications are dispatched on the UI thread.
class LowMemoryListener {
 public:
  // explicit LowMemoryListener(LowMemoryListenerDelegate* delegate);
  explicit LowMemoryListener(LowMemoryListenerDelegate* delegate, int oomEventFd);
  ~LowMemoryListener();

  void Start();
  void Stop();

 private:
  // Callback from LowMemoryListenerImpl when memory is low. Called on the UI
  // thread.
  void OnMemoryLow();

  scoped_refptr<LowMemoryListenerImpl> observer_;
  LowMemoryListenerDelegate* delegate_;
  base::WeakPtrFactory<LowMemoryListener> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(LowMemoryListener);
};

}  // namespace content

#endif  // CONTENT_MEMORY_ACS_LOW_MEMORY_LISTENER_H_
