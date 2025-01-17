
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     content/common/background_sync_service.mojom
//

package org.chromium.mojom.content.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;

public final class BackgroundSyncState {


    public static final int PENDING = 0;

    public static final int FIRING = PENDING + 1;

    public static final int REREGISTERED_WHILE_FIRING = FIRING + 1;


    private BackgroundSyncState() {}

}