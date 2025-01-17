
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


public final class DeviceFilter extends org.chromium.mojo.bindings.Struct {

    private static final int STRUCT_SIZE = 16;
    private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
    private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];





    public boolean hasVendorId;

    public short vendorId;

    public boolean hasProductId;

    public short productId;

    public boolean hasClassCode;

    public byte classCode;

    public boolean hasSubclassCode;

    public byte subclassCode;

    public boolean hasProtocolCode;

    public byte protocolCode;



    private DeviceFilter(int version) {
        super(STRUCT_SIZE, version);





















    }

    public DeviceFilter() {
        this(0);
    }

    public static DeviceFilter deserialize(org.chromium.mojo.bindings.Message message) {
        return decode(new org.chromium.mojo.bindings.Decoder(message));
    }

    @SuppressWarnings("unchecked")
    public static DeviceFilter decode(org.chromium.mojo.bindings.Decoder decoder0) {
        if (decoder0 == null) {
            return null;
        }
        org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
        DeviceFilter result = new DeviceFilter(mainDataHeader.elementsOrVersion);


        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.hasVendorId = decoder0.readBoolean(8, 0);
            
        }

        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.hasProductId = decoder0.readBoolean(8, 1);
            
        }

        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.hasClassCode = decoder0.readBoolean(8, 2);
            
        }

        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.hasSubclassCode = decoder0.readBoolean(8, 3);
            
        }

        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.hasProtocolCode = decoder0.readBoolean(8, 4);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.classCode = decoder0.readByte(9);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.vendorId = decoder0.readShort(10);
            
        }





        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.productId = decoder0.readShort(12);
            
        }





        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.subclassCode = decoder0.readByte(14);
            
        }



        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.protocolCode = decoder0.readByte(15);
            
        }


        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {

        org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);



        
        
        encoder0.encode(hasVendorId, 8, 0);
        

        
        
        encoder0.encode(hasProductId, 8, 1);
        

        
        
        encoder0.encode(hasClassCode, 8, 2);
        

        
        
        encoder0.encode(hasSubclassCode, 8, 3);
        

        
        
        encoder0.encode(hasProtocolCode, 8, 4);
        



        
        
        encoder0.encode(classCode, 9);
        



        
        
        encoder0.encode(vendorId, 10);
        





        
        
        encoder0.encode(productId, 12);
        





        
        
        encoder0.encode(subclassCode, 14);
        



        
        
        encoder0.encode(protocolCode, 15);
        


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

        DeviceFilter other = (DeviceFilter) object;

        if (this.hasVendorId != other.hasVendorId)
            return false;

        if (this.vendorId != other.vendorId)
            return false;

        if (this.hasProductId != other.hasProductId)
            return false;

        if (this.productId != other.productId)
            return false;

        if (this.hasClassCode != other.hasClassCode)
            return false;

        if (this.classCode != other.classCode)
            return false;

        if (this.hasSubclassCode != other.hasSubclassCode)
            return false;

        if (this.subclassCode != other.subclassCode)
            return false;

        if (this.hasProtocolCode != other.hasProtocolCode)
            return false;

        if (this.protocolCode != other.protocolCode)
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

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(hasVendorId);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(vendorId);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(hasProductId);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(productId);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(hasClassCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(classCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(hasSubclassCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(subclassCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(hasProtocolCode);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(protocolCode);

        return result;
    }
}
