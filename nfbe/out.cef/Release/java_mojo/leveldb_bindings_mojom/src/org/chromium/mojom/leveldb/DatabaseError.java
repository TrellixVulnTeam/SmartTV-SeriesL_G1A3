
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     components/leveldb/public/interfaces/leveldb.mojom
//

package org.chromium.mojom.leveldb;

import org.chromium.base.annotations.SuppressFBWarnings;

public final class DatabaseError {


    public static final int OK = 0;

    public static final int NOT_FOUND = OK + 1;

    public static final int CORRUPTION = NOT_FOUND + 1;

    public static final int NOT_SUPPORTED = CORRUPTION + 1;

    public static final int INVALID_ARGUMENT = NOT_SUPPORTED + 1;

    public static final int IO_ERROR = INVALID_ARGUMENT + 1;


    private DatabaseError() {}

}