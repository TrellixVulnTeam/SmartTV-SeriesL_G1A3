
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     mojo/services/tracing/public/interfaces/tracing.mojom
//

package org.chromium.mojom.tracing;

import org.chromium.base.annotations.SuppressFBWarnings;


public interface StartupPerformanceDataCollector extends org.chromium.mojo.bindings.Interface {



    public interface Proxy extends StartupPerformanceDataCollector, org.chromium.mojo.bindings.Interface.Proxy {
    }

    Manager<StartupPerformanceDataCollector, StartupPerformanceDataCollector.Proxy> MANAGER = StartupPerformanceDataCollector_Internal.MANAGER;


    void setShellProcessCreationTime(
long time);



    void setShellMainEntryPointTime(
long time);



    void setBrowserMessageLoopStartTicks(
long ticks);



    void setBrowserWindowDisplayTicks(
long ticks);



    void setBrowserOpenTabsTimeDelta(
long delta);



    void setFirstWebContentsMainFrameLoadTicks(
long ticks);



    void setFirstVisuallyNonEmptyLayoutTicks(
long ticks);



    void getStartupPerformanceTimes(

GetStartupPerformanceTimesResponse callback);

    interface GetStartupPerformanceTimesResponse extends org.chromium.mojo.bindings.Callbacks.Callback1<StartupPerformanceTimes> { }


}
