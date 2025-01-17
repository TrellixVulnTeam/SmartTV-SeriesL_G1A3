
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     content/common/wake_lock_service.mojom
//

package org.chromium.mojom.content.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;


class WakeLockService_Internal {

    public static final org.chromium.mojo.bindings.Interface.Manager<WakeLockService, WakeLockService.Proxy> MANAGER =
            new org.chromium.mojo.bindings.Interface.Manager<WakeLockService, WakeLockService.Proxy>() {
    
        public String getName() {
            return "content::mojom::WakeLockService";
        }
    
        public int getVersion() {
          return 0;
        }
    
        public Proxy buildProxy(org.chromium.mojo.system.Core core,
                                org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            return new Proxy(core, messageReceiver);
        }
    
        public Stub buildStub(org.chromium.mojo.system.Core core, WakeLockService impl) {
            return new Stub(core, impl);
        }
    
        public WakeLockService[] buildArray(int size) {
          return new WakeLockService[size];
        }
    };


    private static final int REQUEST_WAKE_LOCK_ORDINAL = 0;

    private static final int CANCEL_WAKE_LOCK_ORDINAL = 1;


    static final class Proxy extends org.chromium.mojo.bindings.Interface.AbstractProxy implements WakeLockService.Proxy {

        Proxy(org.chromium.mojo.system.Core core,
              org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            super(core, messageReceiver);
        }


        @Override
        public void requestWakeLock(
) {

            WakeLockServiceRequestWakeLockParams _message = new WakeLockServiceRequestWakeLockParams();


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(REQUEST_WAKE_LOCK_ORDINAL)));

        }


        @Override
        public void cancelWakeLock(
) {

            WakeLockServiceCancelWakeLockParams _message = new WakeLockServiceCancelWakeLockParams();


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(CANCEL_WAKE_LOCK_ORDINAL)));

        }


    }

    static final class Stub extends org.chromium.mojo.bindings.Interface.Stub<WakeLockService> {

        Stub(org.chromium.mojo.system.Core core, WakeLockService impl) {
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
                                WakeLockService_Internal.MANAGER, messageWithHeader);
            
            
            
            
            
                    case REQUEST_WAKE_LOCK_ORDINAL: {
            
                        WakeLockServiceRequestWakeLockParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().requestWakeLock();
                        return true;
                    }
            
            
            
            
            
                    case CANCEL_WAKE_LOCK_ORDINAL: {
            
                        WakeLockServiceCancelWakeLockParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().cancelWakeLock();
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
                                getCore(), WakeLockService_Internal.MANAGER, messageWithHeader, receiver);
            
            
            
            
            
            
                    default:
                        return false;
                }
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                System.err.println(e.toString());
                return false;
            }
        }
    }


    
    static final class WakeLockServiceRequestWakeLockParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 8;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(8, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
        private WakeLockServiceRequestWakeLockParams(int version) {
            super(STRUCT_SIZE, version);
    
        }
    
        public WakeLockServiceRequestWakeLockParams() {
            this(0);
        }
    
        public static WakeLockServiceRequestWakeLockParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static WakeLockServiceRequestWakeLockParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            WakeLockServiceRequestWakeLockParams result = new WakeLockServiceRequestWakeLockParams(mainDataHeader.elementsOrVersion);
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
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
    
            return true;
        }
    
        /**
         * @see Object#hashCode()
         */
        @Override
        public int hashCode() {
            final int prime = 31;
            int result = prime + getClass().hashCode();
    
            return result;
        }
    }



    
    static final class WakeLockServiceCancelWakeLockParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 8;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(8, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
        private WakeLockServiceCancelWakeLockParams(int version) {
            super(STRUCT_SIZE, version);
    
        }
    
        public WakeLockServiceCancelWakeLockParams() {
            this(0);
        }
    
        public static WakeLockServiceCancelWakeLockParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static WakeLockServiceCancelWakeLockParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            WakeLockServiceCancelWakeLockParams result = new WakeLockServiceCancelWakeLockParams(mainDataHeader.elementsOrVersion);
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
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
    
            return true;
        }
    
        /**
         * @see Object#hashCode()
         */
        @Override
        public int hashCode() {
            final int prime = 31;
            int result = prime + getClass().hashCode();
    
            return result;
        }
    }



}
