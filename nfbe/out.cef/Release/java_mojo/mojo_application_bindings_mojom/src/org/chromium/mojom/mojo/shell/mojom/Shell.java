
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     mojo/shell/public/interfaces/shell.mojom
//

package org.chromium.mojom.mojo.shell.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;


public interface Shell extends org.chromium.mojo.bindings.Interface {



    public interface Proxy extends Shell, org.chromium.mojo.bindings.Interface.Proxy {
    }

    Manager<Shell, Shell.Proxy> MANAGER = Shell_Internal.MANAGER;


    void addInstanceListener(
InstanceListener listener);


}
