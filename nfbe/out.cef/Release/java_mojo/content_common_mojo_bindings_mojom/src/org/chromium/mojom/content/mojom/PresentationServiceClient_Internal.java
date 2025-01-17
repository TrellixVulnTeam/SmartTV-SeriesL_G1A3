
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     content/common/presentation/presentation_service.mojom
//

package org.chromium.mojom.content.mojom;

import org.chromium.base.annotations.SuppressFBWarnings;


class PresentationServiceClient_Internal {

    public static final org.chromium.mojo.bindings.Interface.Manager<PresentationServiceClient, PresentationServiceClient.Proxy> MANAGER =
            new org.chromium.mojo.bindings.Interface.Manager<PresentationServiceClient, PresentationServiceClient.Proxy>() {
    
        public String getName() {
            return "content::mojom::PresentationServiceClient";
        }
    
        public int getVersion() {
          return 0;
        }
    
        public Proxy buildProxy(org.chromium.mojo.system.Core core,
                                org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            return new Proxy(core, messageReceiver);
        }
    
        public Stub buildStub(org.chromium.mojo.system.Core core, PresentationServiceClient impl) {
            return new Stub(core, impl);
        }
    
        public PresentationServiceClient[] buildArray(int size) {
          return new PresentationServiceClient[size];
        }
    };


    private static final int ON_SCREEN_AVAILABILITY_NOT_SUPPORTED_ORDINAL = 0;

    private static final int ON_SCREEN_AVAILABILITY_UPDATED_ORDINAL = 1;

    private static final int ON_CONNECTION_STATE_CHANGED_ORDINAL = 2;

    private static final int ON_CONNECTION_CLOSED_ORDINAL = 3;

    private static final int ON_SESSION_MESSAGES_RECEIVED_ORDINAL = 4;

    private static final int ON_DEFAULT_SESSION_STARTED_ORDINAL = 5;


    static final class Proxy extends org.chromium.mojo.bindings.Interface.AbstractProxy implements PresentationServiceClient.Proxy {

        Proxy(org.chromium.mojo.system.Core core,
              org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            super(core, messageReceiver);
        }


        @Override
        public void onScreenAvailabilityNotSupported(
String url) {

            PresentationServiceClientOnScreenAvailabilityNotSupportedParams _message = new PresentationServiceClientOnScreenAvailabilityNotSupportedParams();

            _message.url = url;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_SCREEN_AVAILABILITY_NOT_SUPPORTED_ORDINAL)));

        }


        @Override
        public void onScreenAvailabilityUpdated(
String url, boolean available) {

            PresentationServiceClientOnScreenAvailabilityUpdatedParams _message = new PresentationServiceClientOnScreenAvailabilityUpdatedParams();

            _message.url = url;

            _message.available = available;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_SCREEN_AVAILABILITY_UPDATED_ORDINAL)));

        }


        @Override
        public void onConnectionStateChanged(
PresentationSessionInfo connection, int newState) {

            PresentationServiceClientOnConnectionStateChangedParams _message = new PresentationServiceClientOnConnectionStateChangedParams();

            _message.connection = connection;

            _message.newState = newState;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_CONNECTION_STATE_CHANGED_ORDINAL)));

        }


        @Override
        public void onConnectionClosed(
PresentationSessionInfo connection, int reason, String message) {

            PresentationServiceClientOnConnectionClosedParams _message = new PresentationServiceClientOnConnectionClosedParams();

            _message.connection = connection;

            _message.reason = reason;

            _message.message = message;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_CONNECTION_CLOSED_ORDINAL)));

        }


        @Override
        public void onSessionMessagesReceived(
PresentationSessionInfo sessionInfo, SessionMessage[] messages) {

            PresentationServiceClientOnSessionMessagesReceivedParams _message = new PresentationServiceClientOnSessionMessagesReceivedParams();

            _message.sessionInfo = sessionInfo;

            _message.messages = messages;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_SESSION_MESSAGES_RECEIVED_ORDINAL)));

        }


        @Override
        public void onDefaultSessionStarted(
PresentationSessionInfo sessionInfo) {

            PresentationServiceClientOnDefaultSessionStartedParams _message = new PresentationServiceClientOnDefaultSessionStartedParams();

            _message.sessionInfo = sessionInfo;


            getProxyHandler().getMessageReceiver().accept(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(ON_DEFAULT_SESSION_STARTED_ORDINAL)));

        }


    }

    static final class Stub extends org.chromium.mojo.bindings.Interface.Stub<PresentationServiceClient> {

        Stub(org.chromium.mojo.system.Core core, PresentationServiceClient impl) {
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
                                PresentationServiceClient_Internal.MANAGER, messageWithHeader);
            
            
            
            
            
                    case ON_SCREEN_AVAILABILITY_NOT_SUPPORTED_ORDINAL: {
            
                        PresentationServiceClientOnScreenAvailabilityNotSupportedParams data =
                                PresentationServiceClientOnScreenAvailabilityNotSupportedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onScreenAvailabilityNotSupported(data.url);
                        return true;
                    }
            
            
            
            
            
                    case ON_SCREEN_AVAILABILITY_UPDATED_ORDINAL: {
            
                        PresentationServiceClientOnScreenAvailabilityUpdatedParams data =
                                PresentationServiceClientOnScreenAvailabilityUpdatedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onScreenAvailabilityUpdated(data.url, data.available);
                        return true;
                    }
            
            
            
            
            
                    case ON_CONNECTION_STATE_CHANGED_ORDINAL: {
            
                        PresentationServiceClientOnConnectionStateChangedParams data =
                                PresentationServiceClientOnConnectionStateChangedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onConnectionStateChanged(data.connection, data.newState);
                        return true;
                    }
            
            
            
            
            
                    case ON_CONNECTION_CLOSED_ORDINAL: {
            
                        PresentationServiceClientOnConnectionClosedParams data =
                                PresentationServiceClientOnConnectionClosedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onConnectionClosed(data.connection, data.reason, data.message);
                        return true;
                    }
            
            
            
            
            
                    case ON_SESSION_MESSAGES_RECEIVED_ORDINAL: {
            
                        PresentationServiceClientOnSessionMessagesReceivedParams data =
                                PresentationServiceClientOnSessionMessagesReceivedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onSessionMessagesReceived(data.sessionInfo, data.messages);
                        return true;
                    }
            
            
            
            
            
                    case ON_DEFAULT_SESSION_STARTED_ORDINAL: {
            
                        PresentationServiceClientOnDefaultSessionStartedParams data =
                                PresentationServiceClientOnDefaultSessionStartedParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().onDefaultSessionStarted(data.sessionInfo);
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
                                getCore(), PresentationServiceClient_Internal.MANAGER, messageWithHeader, receiver);
            
            
            
            
            
            
            
            
            
            
            
            
            
            
                    default:
                        return false;
                }
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                System.err.println(e.toString());
                return false;
            }
        }
    }


    
    static final class PresentationServiceClientOnScreenAvailabilityNotSupportedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public String url;
    
    
    
        private PresentationServiceClientOnScreenAvailabilityNotSupportedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public PresentationServiceClientOnScreenAvailabilityNotSupportedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnScreenAvailabilityNotSupportedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnScreenAvailabilityNotSupportedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnScreenAvailabilityNotSupportedParams result = new PresentationServiceClientOnScreenAvailabilityNotSupportedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.url = decoder0.readString(8, false);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(url, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            PresentationServiceClientOnScreenAvailabilityNotSupportedParams other = (PresentationServiceClientOnScreenAvailabilityNotSupportedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.url, other.url))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(url);
    
            return result;
        }
    }



    
    static final class PresentationServiceClientOnScreenAvailabilityUpdatedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 24;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(24, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public String url;
    
        public boolean available;
    
    
    
        private PresentationServiceClientOnScreenAvailabilityUpdatedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
    
    
        }
    
        public PresentationServiceClientOnScreenAvailabilityUpdatedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnScreenAvailabilityUpdatedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnScreenAvailabilityUpdatedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnScreenAvailabilityUpdatedParams result = new PresentationServiceClientOnScreenAvailabilityUpdatedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.url = decoder0.readString(8, false);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.available = decoder0.readBoolean(16, 0);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(url, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(available, 16, 0);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            PresentationServiceClientOnScreenAvailabilityUpdatedParams other = (PresentationServiceClientOnScreenAvailabilityUpdatedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.url, other.url))
                return false;
    
            if (this.available != other.available)
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(url);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(available);
    
            return result;
        }
    }



    
    static final class PresentationServiceClientOnConnectionStateChangedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 24;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(24, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public PresentationSessionInfo connection;
    
        public int newState;
    
    
    
        private PresentationServiceClientOnConnectionStateChangedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
    
    
        }
    
        public PresentationServiceClientOnConnectionStateChangedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnConnectionStateChangedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnConnectionStateChangedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnConnectionStateChangedParams result = new PresentationServiceClientOnConnectionStateChangedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(8, false);
                
                result.connection = PresentationSessionInfo.decode(decoder1);
                
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.newState = decoder0.readInt(16);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(connection, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(newState, 16);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            PresentationServiceClientOnConnectionStateChangedParams other = (PresentationServiceClientOnConnectionStateChangedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.connection, other.connection))
                return false;
    
            if (this.newState != other.newState)
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(connection);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(newState);
    
            return result;
        }
    }



    
    static final class PresentationServiceClientOnConnectionClosedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 32;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(32, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public PresentationSessionInfo connection;
    
        public int reason;
    
        public String message;
    
    
    
        private PresentationServiceClientOnConnectionClosedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
    
    
    
    
        }
    
        public PresentationServiceClientOnConnectionClosedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnConnectionClosedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnConnectionClosedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnConnectionClosedParams result = new PresentationServiceClientOnConnectionClosedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(8, false);
                
                result.connection = PresentationSessionInfo.decode(decoder1);
                
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.reason = decoder0.readInt(16);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.message = decoder0.readString(24, false);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(connection, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(reason, 16);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            
            
            encoder0.encode(message, 24, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            PresentationServiceClientOnConnectionClosedParams other = (PresentationServiceClientOnConnectionClosedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.connection, other.connection))
                return false;
    
            if (this.reason != other.reason)
                return false;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.message, other.message))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(connection);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(reason);
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(message);
    
            return result;
        }
    }



    
    static final class PresentationServiceClientOnSessionMessagesReceivedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 24;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(24, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public PresentationSessionInfo sessionInfo;
    
        public SessionMessage[] messages;
    
    
    
        private PresentationServiceClientOnSessionMessagesReceivedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
    
    
        }
    
        public PresentationServiceClientOnSessionMessagesReceivedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnSessionMessagesReceivedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnSessionMessagesReceivedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnSessionMessagesReceivedParams result = new PresentationServiceClientOnSessionMessagesReceivedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(8, false);
                
                result.sessionInfo = PresentationSessionInfo.decode(decoder1);
                
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(16, false);
                
                
                {
                
                
                    org.chromium.mojo.bindings.DataHeader si1 = decoder1.readDataHeaderForPointerArray(org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
                    result.messages = new SessionMessage[si1.elementsOrVersion];
                    for (int i1 = 0; i1 < si1.elementsOrVersion; ++i1) {
                        
                        
                        org.chromium.mojo.bindings.Decoder decoder2 = decoder1.readPointer(org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i1, false);
                        
                        result.messages[i1] = SessionMessage.decode(decoder2);
                        
                        
                    }
                
                }
                
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(sessionInfo, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            
            
            
            
            if (messages == null) {
                encoder0.encodeNullPointer(16, false);
            } else {
            
            
            
            
                org.chromium.mojo.bindings.Encoder encoder1 = encoder0.encodePointerArray(messages.length, 16, org.chromium.mojo.bindings.BindingsHelper.UNSPECIFIED_ARRAY_LENGTH);
                for (int i0 = 0; i0 < messages.length; ++i0) {
                    
                    
                    encoder1.encode(messages[i0], org.chromium.mojo.bindings.DataHeader.HEADER_SIZE + org.chromium.mojo.bindings.BindingsHelper.POINTER_SIZE * i0, false);
                    
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
    
            PresentationServiceClientOnSessionMessagesReceivedParams other = (PresentationServiceClientOnSessionMessagesReceivedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.sessionInfo, other.sessionInfo))
                return false;
    
            if (!java.util.Arrays.deepEquals(this.messages, other.messages))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(sessionInfo);
    
            result = prime * result + java.util.Arrays.deepHashCode(messages);
    
            return result;
        }
    }



    
    static final class PresentationServiceClientOnDefaultSessionStartedParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public PresentationSessionInfo sessionInfo;
    
    
    
        private PresentationServiceClientOnDefaultSessionStartedParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public PresentationServiceClientOnDefaultSessionStartedParams() {
            this(0);
        }
    
        public static PresentationServiceClientOnDefaultSessionStartedParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static PresentationServiceClientOnDefaultSessionStartedParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            PresentationServiceClientOnDefaultSessionStartedParams result = new PresentationServiceClientOnDefaultSessionStartedParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                org.chromium.mojo.bindings.Decoder decoder1 = decoder0.readPointer(8, false);
                
                result.sessionInfo = PresentationSessionInfo.decode(decoder1);
                
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(sessionInfo, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            PresentationServiceClientOnDefaultSessionStartedParams other = (PresentationServiceClientOnDefaultSessionStartedParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.sessionInfo, other.sessionInfo))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(sessionInfo);
    
            return result;
        }
    }



}
