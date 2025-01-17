
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     device/usb/public/interfaces/device.mojom
//

package org.chromium.mojom.device.usb;

import org.chromium.base.annotations.SuppressFBWarnings;


public final class AlternateInterfaceInfo extends org.chromium.mojo.bindings.Struct {

    private static final int STRUCT_SIZE = 32;
    private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(32, 0)};
    private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];





    public byte alternateSetting;

    public byte classCode;

    public byte subclassCode;

    public byte protocolCode;

    public String interfaceName;

    public EndpointInfo[] endpoints;



    private AlternateInterfaceInfo(int version) {
        super(STRUCT_SIZE, version);













    }

    public AlternateInterfaceInfo() {
        this(0);
    }

    public static AlternateInterfaceInfo deserialize(org.chromium.mojo.bindings.Message message) {
        return decode(new org.chromium.mojo.bindings.Decoder(message));
    }

    @SuppressWarnings("unchecked")
    public static AlternateInterfaceInfo decode(org.chromium.mojo.bindings.Decoder decoder0) {
        if (decoder0 == null) {
            return null;
        }
        org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
        AlternateInterfaceInfo result = new AlternateInterfaceInfo(mainDataHeader.elementsOrVersion);


        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.alternateSetting = decoder0.readByte(8);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.classCode = decoder0.readByte(9);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.subclassCode = decoder0.readByte(10);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.protocolCode = decoder0.readByte(11);
            
        }











        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.interfaceName = decoder0.readString(16, true);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(24, false);
            
            
            {
            
            
                org.chromium.mojo.bindings.DataHeader si1 = decoder1.readDataHeaderForPointerArray(org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
                result.endpoints = new EndpointInfo[si1.elementsOrVersion];
                for (int i1 = 0; i1 < si1.elementsOrVersion; ++i1) {
                    
                    
                    org.chromium.mojo.bindings.Decoder decoder2 = decoder1.readPointer(org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i1, false);
                    
                    result.endpoints[i1] = EndpointInfo.decode(decoder2);
                    
                    
                }
            
            }
            
            
        }
















        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {

        org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);



        
        
        encoder0.encode(alternateSetting, 8);
        



        
        
        encoder0.encode(classCode, 9);
        



        
        
        encoder0.encode(subclassCode, 10);
        



        
        
        encoder0.encode(protocolCode, 11);
        











        
        
        encoder0.encode(interfaceName, 16, true);
        

















        
        
        
        
        if (endpoints == null) {
            encoder0.encodeNullPointer(24, false);
        } else {
        
        
        
        
            org.chromium.mojo.bindings.Encoder encoder1 = encoder0.encodePointerArray(endpoints.length, 24, org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
            for (int i0 = 0; i0 < endpoints.length; ++i0) {
                
                
                encoder1.encode(endpoints[i0], org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i0, false);
                
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

        AlternateInterfaceInfo other = (AlternateInterfaceInfo) object;

        if (this.alternateSetting != other.alternateSetting)
            return false;

        if (this.classCode != other.classCode)
            return false;

        if (this.subclassCode != other.subclassCode)
            return false;

        if (this.protocolCode != other.protocolCode)
            return false;

        if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.interfaceName, other.interfaceName))
            return false;

        if (!java.util.Arrays.deepEquals(this.endpoints, other.endpoints))
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

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(alternateSetting);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(classCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(subclassCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(protocolCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(interfaceName);

        result = prime * result + java.util.Arrays.deepHashCode(endpoints);

        return result;
    }
}
