
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     mojo/shell/public/interfaces/shell_client.mojom
//

package org.chromium.mojom.mojo.shell.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;


public interface ShellClient extends org.chromium.mojo.bindings.Interface {



    public interface Proxy extends ShellClient, org.chromium.mojo.bindings.Interface.Proxy {
    }

    Manager<ShellClient, ShellClient.Proxy> MANAGER = ShellClient_Internal.MANAGER;


    void initialize(
Identity identity, int id, 
InitializeResponse callback);

    interface InitializeResponse extends org.chromium.mojo.bindings.Callbacks.Callback1<org.chromium.mojo.bindings.InterfaceRequest<Connector>> { }



    void acceptConnection(
Identity source, int sourceId, org.chromium.mojo.bindings.InterfaceRequest<InterfaceProvider> localInterfaces, InterfaceProvider remoteInterfaces, CapabilityRequest allowedCapabilities, String resolvedName);


}
