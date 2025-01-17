
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     device/usb/public/interfaces/device_manager.mojom
//

package org.chromium.mojom.device.usb;

import org.chromium.base.annotations.SuppressFBWarnings;


public final class EnumerationOptions extends org.chromium.mojo.bindings.Struct {

    private static final int STRUCT_SIZE = 16;
    private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
    private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];





    public DeviceFilter[] filters;



    private EnumerationOptions(int version) {
        super(STRUCT_SIZE, version);



    }

    public EnumerationOptions() {
        this(0);
    }

    public static EnumerationOptions deserialize(org.chromium.mojo.bindings.Message message) {
        return decode(new org.chromium.mojo.bindings.Decoder(message));
    }

    @SuppressWarnings("unchecked")
    public static EnumerationOptions decode(org.chromium.mojo.bindings.Decoder decoder0) {
        if (decoder0 == null) {
            return null;
        }
        org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
        EnumerationOptions result = new EnumerationOptions(mainDataHeader.elementsOrVersion);


        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(8, true);
            
            
            if (decoder1 == null) {
                result.filters = null;
            } else {
            
            
                org.chromium.mojo.bindings.DataHeader si1 = decoder1.readDataHeaderForPointerArray(org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
                result.filters = new DeviceFilter[si1.elementsOrVersion];
                for (int i1 = 0; i1 < si1.elementsOrVersion; ++i1) {
                    
                    
                    org.chromium.mojo.bindings.Decoder decoder2 = decoder1.readPointer(org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i1, false);
                    
                    result.filters[i1] = DeviceFilter.decode(decoder2);
                    
                    
                }
            
            }
            
            
        }
















        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {

        org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);



        
        
        
        
        if (filters == null) {
            encoder0.encodeNullPointer(8, true);
        } else {
        
        
        
        
            org.chromium.mojo.bindings.Encoder encoder1 = encoder0.encodePointerArray(filters.length, 8, org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
            for (int i0 = 0; i0 < filters.length; ++i0) {
                
                
                encoder1.encode(filters[i0], org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i0, false);
                
            }
        }
        
















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

        EnumerationOptions other = (EnumerationOptions) object;

        if (!java.util.Arrays.deepEquals(this.filters, other.filters))
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

        result = prime * result + java.util.Arrays.deepHashCode(filters);

        return result;
    }
}
