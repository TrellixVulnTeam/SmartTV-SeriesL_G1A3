
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     device/serial/data_stream.mojom
//

package org.chromium.mojom.device.serial;

import org.chromium.base.annotations.SuppressFBWarnings;


class DataSourceClient_Internal {

    public static final org.chromium.mojo.bindings.Interface.Manager<DataSourceClient, DataSourceClient.Proxy> MANAGER =
            new org.chromium.mojo.bindings.Interface.Manager<DataSourceClient, DataSourceClient.Proxy>() {
    
        public String getName() {
            return "device::serial::DataSourceClient";
        }
    
        public int getVersion() {
          return 0;
        }
    
        public Proxy buildProxy(org.chromium.mojo.system.Core core,
                                org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            return new Proxy(core, messageReceiver);
        }
    
        public Stub buildStub(org.chromium.mojo.system.Core core, DataSourceClient impl) {
            return new Stub(core, impl);
        }
    
        public DataSourceClient[] buildArray(int size) {
          return new DataSourceClient[size];
        }
    };


    private static final int ON_ERROR_ORDINAL = 0;

    private static final int ON_DATA_ORDINAL = 1;


    static final class Proxy extends org.chromium.mojo.bindings.Interface.AbstractProxy implements DataSourceClient.Proxy {

        Proxy(org.chromium.mojo.system.Core core,
              org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            super(core, messageReceiver);
        }


        @Override
        public void onError(
int error) {

            DataSourceClientOnErrorParams _message = new DataSourceClientOnErrorParams();

            _message.error = error;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_ERROR_ORDINAL)));

        }


        @Override
        public void onData(
byte[] data) {

            DataSourceClientOnDataParams _message = new DataSourceClientOnDataParams();

            _message.data = data;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_DATA_ORDINAL)));

        }


    }

    static final class Stub extends org.chromium.mojo.bindings.Interface.Stub<DataSourceClient> {

        Stub(org.chromium.mojo.system.Core core, DataSourceClient impl) {
            super(core, impl);
        }

        @Override
        public boolean accept(org.chromium.mojo.bindings.Message message) {
            try {
                org.chromium.mojo.bindings.ServiceMessage messageWithHeader =
                        message.asServiceMessage();
                org.chromium.mojo.bindings.MessageHeader header = messageWithHeader.getHeader();
                if (!header.validateHeader(org.chromium.mojo.bindings.MessageHeader.NO_FLAG)) {
                    return false;
                }
                switch(header.getType()) {
            
                    case org.chromium.mojo.bindings.InterfaceControlMessagesConstants.RUN_OR_CLOSE_PIPE_MESSAGE_ID:
                        return org.chromium.mojo.bindings.InterfaceControlMessagesHelper.handleRunOrClosePipe(
                                DataSourceClient_Internal.MANAGER, messageWithHeader);
            
            
            
            
            
                    case ON_ERROR_ORDINAL: {
            
                        DataSourceClientOnErrorParams data =
                                DataSourceClientOnErrorParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onError(data.error);
                        return true;
                    }
            
            
            
            
            
                    case ON_DATA_ORDINAL: {
            
                        DataSourceClientOnDataParams data =
                                DataSourceClientOnDataParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onData(data.data);
                        return true;
                    }
            
            
                    default:
                        return false;
                }
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                System.err.println(e.toString());
                return false;
            }
        }

        @Override
        public boolean acceptWithResponder(org.chromium.mojo.bindings.Message message, org.chromium.mojo.bindings.MessageReceiver receiver) {
            try {
                org.chromium.mojo.bindings.ServiceMessage messageWithHeader =
                        message.asServiceMessage();
                org.chromium.mojo.bindings.MessageHeader header = messageWithHeader.getHeader();
                if (!header.validateHeader(org.chromium.mojo.bindings.MessageHeader.MESSAGE_EXPECTS_RESPONSE_FLAG)) {
                    return false;
                }
                switch(header.getType()) {
            
                    case org.chromium.mojo.bindings.InterfaceControlMessagesConstants.RUN_MESSAGE_ID:
                        return org.chromium.mojo.bindings.InterfaceControlMessagesHelper.handleRun(
                                getCore(), DataSourceClient_Internal.MANAGER, messageWithHeader, receiver);
            
            
            
            
            
            
                    default:
                        return false;
                }
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                System.err.println(e.toString());
                return false;
            }
        }
    }


    
    static final class DataSourceClientOnErrorParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public int error;
    
    
    
        private DataSourceClientOnErrorParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public DataSourceClientOnErrorParams() {
            this(0);
        }
    
        public static DataSourceClientOnErrorParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static DataSourceClientOnErrorParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            DataSourceClientOnErrorParams result = new DataSourceClientOnErrorParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.error = decoder0.readInt(8);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(error, 8);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            DataSourceClientOnErrorParams other = (DataSourceClientOnErrorParams) object;
    
            if (this.error != other.error)
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(error);
    
            return result;
        }
    }



    
    static final class DataSourceClientOnDataParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public byte[] data;
    
    
    
        private DataSourceClientOnDataParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public DataSourceClientOnDataParams() {
            this(0);
        }
    
        public static DataSourceClientOnDataParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static DataSourceClientOnDataParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            DataSourceClientOnDataParams result = new DataSourceClientOnDataParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.data = decoder0.readBytes(8, org.chromium.mojo.bindings.BindingsHelper.NOTHING_NULLABLE, org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(data, 8, org.chromium.mojo.bindings.BindingsHelper.NOTHING_NULLABLE, org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            DataSourceClientOnDataParams other = (DataSourceClientOnDataParams) object;
    
            if (!java.util.Arrays.equals(this.data, other.data))
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
    
            result = prime * result + java.util.Arrays.hashCode(data);
    
            return result;
        }
    }



}
