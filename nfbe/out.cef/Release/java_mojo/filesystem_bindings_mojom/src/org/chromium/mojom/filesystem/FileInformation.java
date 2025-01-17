
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     components/filesystem/public/interfaces/types.mojom
//

package org.chromium.mojom.filesystem;

import org.chromium.base.annotations.SuppressFBWarnings;


public final class FileInformation extends org.chromium.mojo.bindings.Struct {

    private static final int STRUCT_SIZE = 48;
    private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(48, 0)};
    private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];





    public int type;

    public long size;

    public double atime;

    public double mtime;

    public double ctime;



    private FileInformation(int version) {
        super(STRUCT_SIZE, version);











    }

    public FileInformation() {
        this(0);
    }

    public static FileInformation deserialize(org.chromium.mojo.bindings.Message message) {
        return decode(new org.chromium.mojo.bindings.Decoder(message));
    }

    @SuppressWarnings("unchecked")
    public static FileInformation decode(org.chromium.mojo.bindings.Decoder decoder0) {
        if (decoder0 == null) {
            return null;
        }
        org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
        FileInformation result = new FileInformation(mainDataHeader.elementsOrVersion);


        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.type = decoder0.readInt(8);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.size = decoder0.readLong(16);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.atime = decoder0.readDouble(24);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.mtime = decoder0.readDouble(32);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.ctime = decoder0.readDouble(40);
            
        }
















        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {

        org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);



        
        
        encoder0.encode(type, 8);
        

















        
        
        encoder0.encode(size, 16);
        

















        
        
        encoder0.encode(atime, 24);
        

















        
        
        encoder0.encode(mtime, 32);
        

















        
        
        encoder0.encode(ctime, 40);
        
















    }

    /**
     * @see Object#equals(Object)
     */
    @Override
    public boolean equals(Object object) {
        if (object == this)
            return true;
        if (object == null)
            return false;
        if (getClass() != object.getClass())
            return false;

        FileInformation other = (FileInformation) object;

        if (this.type != other.type)
            return false;

        if (this.size != other.size)
            return false;

        if (this.atime != other.atime)
            return false;

        if (this.mtime != other.mtime)
            return false;

        if (this.ctime != other.ctime)
            return false;


        return true;
    }

    /**
     * @see Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = prime + getClass().hashCode();

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(type);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(size);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(atime);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(mtime);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(ctime);

        return result;
    }
}
