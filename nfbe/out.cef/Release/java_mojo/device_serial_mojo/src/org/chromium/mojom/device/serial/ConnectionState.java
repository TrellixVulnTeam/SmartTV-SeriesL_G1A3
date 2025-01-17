
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     device/serial/serial_serialization.mojom
//

package org.chromium.mojom.device.serial;

import org.chromium.base.annotations.SuppressFBWarnings;


public final class ConnectionState extends org.chromium.mojo.bindings.Struct {

    private static final int STRUCT_SIZE = 40;
    private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(40, 0)};
    private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];





    public int connectionId;

    public boolean paused;

    public boolean persistent;

    public String name;

    public int receiveTimeout;

    public int sendTimeout;

    public int bufferSize;



    private ConnectionState(int version) {
        super(STRUCT_SIZE, version);




        paused = (boolean) false;



        persistent = (boolean) false;



        name = (String) "";



        receiveTimeout = (int) 0L;



        sendTimeout = (int) 0L;



        bufferSize = (int) 4096L;


    }

    public ConnectionState() {
        this(0);
    }

    public static ConnectionState deserialize(org.chromium.mojo.bindings.Message message) {
        return decode(new org.chromium.mojo.bindings.Decoder(message));
    }

    @SuppressWarnings("unchecked")
    public static ConnectionState decode(org.chromium.mojo.bindings.Decoder decoder0) {
        if (decoder0 == null) {
            return null;
        }
        org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
        ConnectionState result = new ConnectionState(mainDataHeader.elementsOrVersion);


        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.connectionId = decoder0.readInt(8);
            
        }









        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.paused = decoder0.readBoolean(12, 0);
            
        }

        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.persistent = decoder0.readBoolean(12, 1);
            
        }









        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.name = decoder0.readString(16, false);
            
        }

















        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.receiveTimeout = decoder0.readInt(24);
            
        }









        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.sendTimeout = decoder0.readInt(28);
            
        }









        if (mainDataHeader.elementsOrVersion >= 0) {
            
            
            result.bufferSize = decoder0.readInt(32);
            
        }
















        return result;
    }

    @SuppressWarnings("unchecked")
    @Override
    protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {

        org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);



        
        
        encoder0.encode(connectionId, 8);
        









        
        
        encoder0.encode(paused, 12, 0);
        

        
        
        encoder0.encode(persistent, 12, 1);
        









        
        
        encoder0.encode(name, 16, false);
        

















        
        
        encoder0.encode(receiveTimeout, 24);
        









        
        
        encoder0.encode(sendTimeout, 28);
        









        
        
        encoder0.encode(bufferSize, 32);
        
















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

        ConnectionState other = (ConnectionState) object;

        if (this.connectionId != other.connectionId)
            return false;

        if (this.paused != other.paused)
            return false;

        if (this.persistent != other.persistent)
            return false;

        if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.name, other.name))
            return false;

        if (this.receiveTimeout != other.receiveTimeout)
            return false;

        if (this.sendTimeout != other.sendTimeout)
            return false;

        if (this.bufferSize != other.bufferSize)
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

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(connectionId);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(paused);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(persistent);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(name);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(receiveTimeout);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(sendTimeout);

        result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(bufferSize);

        return result;
    }
}
