
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     content/common/service_worker/embedded_worker_setup.mojom
//

package org.chromium.mojom.content.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;


class EmbeddedWorkerSetup_Internal {

    public static final org.chromium.mojo.bindings.Interface.Manager<EmbeddedWorkerSetup, EmbeddedWorkerSetup.Proxy> MANAGER =
            new org.chromium.mojo.bindings.Interface.Manager<EmbeddedWorkerSetup, EmbeddedWorkerSetup.Proxy>() {
    
        public String getName() {
            return "content::mojom::EmbeddedWorkerSetup";
        }
    
        public int getVersion() {
          return 0;
        }
    
        public Proxy buildProxy(org.chromium.mojo.system.Core core,
                                org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            return new Proxy(core, messageReceiver);
        }
    
        public Stub buildStub(org.chromium.mojo.system.Core core, EmbeddedWorkerSetup impl) {
            return new Stub(core, impl);
        }
    
        public EmbeddedWorkerSetup[] buildArray(int size) {
          return new EmbeddedWorkerSetup[size];
        }
    };


    private static final int EXCHANGE_INTERFACE_PROVIDERS_ORDINAL = 0;


    static final class Proxy extends org.chromium.mojo.bindings.Interface.AbstractProxy implements EmbeddedWorkerSetup.Proxy {

        Proxy(org.chromium.mojo.system.Core core,
              org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            super(core, messageReceiver);
        }


        @Override
        public void exchangeInterfaceProviders(
int threadId, org.chromium.mojo.bindings.InterfaceRequest<org.chromium.mojom.mojo.shell.mojom.InterfaceProvider> remoteInterfaces, org.chromium.mojom.mojo.shell.mojom.InterfaceProvider localInterfaces) {

            EmbeddedWorkerSetupExchangeInterfaceProvidersParams _message = new EmbeddedWorkerSetupExchangeInterfaceProvidersParams();

            _message.threadId = threadId;

            _message.remoteInterfaces = remoteInterfaces;

            _message.localInterfaces = localInterfaces;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(EXCHANGE_INTERFACE_PROVIDERS_ORDINAL)));

        }


    }

    static final class Stub extends org.chromium.mojo.bindings.Interface.Stub<EmbeddedWorkerSetup> {

        Stub(org.chromium.mojo.system.Core core, EmbeddedWorkerSetup impl) {
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
                                EmbeddedWorkerSetup_Internal.MANAGER, messageWithHeader);
            
            
            
            
            
                    case EXCHANGE_INTERFACE_PROVIDERS_ORDINAL: {
            
                        EmbeddedWorkerSetupExchangeInterfaceProvidersParams data =
                                EmbeddedWorkerSetupExchangeInterfaceProvidersParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().exchangeInterfaceProviders(data.threadId, data.remoteInterfaces, data.localInterfaces);
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
                                getCore(), EmbeddedWorkerSetup_Internal.MANAGER, messageWithHeader, receiver);
            
            
            
            
                    default:
                        return false;
                }
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                System.err.println(e.toString());
                return false;
            }
        }
    }


    
    static final class EmbeddedWorkerSetupExchangeInterfaceProvidersParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 24;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(24, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public int threadId;
    
        public org.chromium.mojo.bindings.InterfaceRequest<org.chromium.mojom.mojo.shell.mojom.InterfaceProvider> remoteInterfaces;
    
        public org.chromium.mojom.mojo.shell.mojom.InterfaceProvider localInterfaces;
    
    
    
        private EmbeddedWorkerSetupExchangeInterfaceProvidersParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
    
    
    
    
        }
    
        public EmbeddedWorkerSetupExchangeInterfaceProvidersParams() {
            this(0);
        }
    
        public static EmbeddedWorkerSetupExchangeInterfaceProvidersParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static EmbeddedWorkerSetupExchangeInterfaceProvidersParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            EmbeddedWorkerSetupExchangeInterfaceProvidersParams result = new EmbeddedWorkerSetupExchangeInterfaceProvidersParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.threadId = decoder0.readInt(8);
                
            }
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.remoteInterfaces = decoder0.readInterfaceRequest(12, false);
                
            }
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.localInterfaces = decoder0.readServiceInterface(16, false, org.chromium.mojom.mojo.shell.mojom.InterfaceProvider.MANAGER);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(threadId, 8);
            
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(remoteInterfaces, 12, false);
            
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(localInterfaces, 16, false, org.chromium.mojom.mojo.shell.mojom.InterfaceProvider.MANAGER);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            EmbeddedWorkerSetupExchangeInterfaceProvidersParams other = (EmbeddedWorkerSetupExchangeInterfaceProvidersParams) object;
    
            if (this.threadId != other.threadId)
                return false;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.remoteInterfaces, other.remoteInterfaces))
                return false;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.localInterfaces, other.localInterfaces))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(threadId);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(remoteInterfaces);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(localInterfaces);
    
            return result;
        }
    }



}
