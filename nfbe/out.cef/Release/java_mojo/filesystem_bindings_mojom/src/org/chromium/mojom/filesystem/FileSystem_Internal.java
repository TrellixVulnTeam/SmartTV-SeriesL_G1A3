
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is autogenerated by:
//     mojo/public/tools/bindings/mojom_bindings_generator.py
// For:
//     components/filesystem/public/interfaces/file_system.mojom
//

package org.chromium.mojom.filesystem;

import org.chromium.base.annotations.SuppressFBWarnings;


class FileSystem_Internal {

    public static final org.chromium.mojo.bindings.Interface.Manager<FileSystem, FileSystem.Proxy> MANAGER =
            new org.chromium.mojo.bindings.Interface.Manager<FileSystem, FileSystem.Proxy>() {
    
        public String getName() {
            return "filesystem::FileSystem";
        }
    
        public int getVersion() {
          return 0;
        }
    
        public Proxy buildProxy(org.chromium.mojo.system.Core core,
                                org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            return new Proxy(core, messageReceiver);
        }
    
        public Stub buildStub(org.chromium.mojo.system.Core core, FileSystem impl) {
            return new Stub(core, impl);
        }
    
        public FileSystem[] buildArray(int size) {
          return new FileSystem[size];
        }
    };


    private static final int OPEN_TEMP_DIRECTORY_ORDINAL = 0;

    private static final int OPEN_PERSISTENT_FILE_SYSTEM_ORDINAL = 1;


    static final class Proxy extends org.chromium.mojo.bindings.Interface.AbstractProxy implements FileSystem.Proxy {

        Proxy(org.chromium.mojo.system.Core core,
              org.chromium.mojo.bindings.MessageReceiverWithResponder messageReceiver) {
            super(core, messageReceiver);
        }


        @Override
        public void openTempDirectory(
org.chromium.mojo.bindings.InterfaceRequest<Directory> directory, 
OpenTempDirectoryResponse callback) {

            FileSystemOpenTempDirectoryParams _message = new FileSystemOpenTempDirectoryParams();

            _message.directory = directory;


            getProxyHandler().getMessageReceiver().acceptWithResponder(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(
                                    OPEN_TEMP_DIRECTORY_ORDINAL,
                                    org.chromium.mojo.bindings.MessageHeader.MESSAGE_EXPECTS_RESPONSE_FLAG,
                                    0)),
                    new FileSystemOpenTempDirectoryResponseParamsForwardToCallback(callback));

        }


        @Override
        public void openPersistentFileSystem(
org.chromium.mojo.bindings.InterfaceRequest<Directory> directory, 
OpenPersistentFileSystemResponse callback) {

            FileSystemOpenPersistentFileSystemParams _message = new FileSystemOpenPersistentFileSystemParams();

            _message.directory = directory;


            getProxyHandler().getMessageReceiver().acceptWithResponder(
                    _message.serializeWithHeader(
                            getProxyHandler().getCore(),
                            new org.chromium.mojo.bindings.MessageHeader(
                                    OPEN_PERSISTENT_FILE_SYSTEM_ORDINAL,
                                    org.chromium.mojo.bindings.MessageHeader.MESSAGE_EXPECTS_RESPONSE_FLAG,
                                    0)),
                    new FileSystemOpenPersistentFileSystemResponseParamsForwardToCallback(callback));

        }


    }

    static final class Stub extends org.chromium.mojo.bindings.Interface.Stub<FileSystem> {

        Stub(org.chromium.mojo.system.Core core, FileSystem impl) {
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
                                FileSystem_Internal.MANAGER, messageWithHeader);
            
            
            
            
            
            
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
                                getCore(), FileSystem_Internal.MANAGER, messageWithHeader, receiver);
            
            
            
            
            
            
            
                    case OPEN_TEMP_DIRECTORY_ORDINAL: {
            
                        FileSystemOpenTempDirectoryParams data =
                                FileSystemOpenTempDirectoryParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().openTempDirectory(data.directory, new FileSystemOpenTempDirectoryResponseParamsProxyToResponder(getCore(), receiver, header.getRequestId()));
                        return true;
                    }
            
            
            
            
            
            
            
                    case OPEN_PERSISTENT_FILE_SYSTEM_ORDINAL: {
            
                        FileSystemOpenPersistentFileSystemParams data =
                                FileSystemOpenPersistentFileSystemParams.deserialize(messageWithHeader.getPayload());
            
                        getImpl().openPersistentFileSystem(data.directory, new FileSystemOpenPersistentFileSystemResponseParamsProxyToResponder(getCore(), receiver, header.getRequestId()));
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
    }


    
    static final class FileSystemOpenTempDirectoryParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public org.chromium.mojo.bindings.InterfaceRequest<Directory> directory;
    
    
    
        private FileSystemOpenTempDirectoryParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public FileSystemOpenTempDirectoryParams() {
            this(0);
        }
    
        public static FileSystemOpenTempDirectoryParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static FileSystemOpenTempDirectoryParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            FileSystemOpenTempDirectoryParams result = new FileSystemOpenTempDirectoryParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.directory = decoder0.readInterfaceRequest(8, false);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(directory, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            FileSystemOpenTempDirectoryParams other = (FileSystemOpenTempDirectoryParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.directory, other.directory))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(directory);
    
            return result;
        }
    }



    
    static final class FileSystemOpenTempDirectoryResponseParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public int error;
    
    
    
        private FileSystemOpenTempDirectoryResponseParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public FileSystemOpenTempDirectoryResponseParams() {
            this(0);
        }
    
        public static FileSystemOpenTempDirectoryResponseParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static FileSystemOpenTempDirectoryResponseParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            FileSystemOpenTempDirectoryResponseParams result = new FileSystemOpenTempDirectoryResponseParams(mainDataHeader.elementsOrVersion);
    
    
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
    
            FileSystemOpenTempDirectoryResponseParams other = (FileSystemOpenTempDirectoryResponseParams) object;
    
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

    static class FileSystemOpenTempDirectoryResponseParamsForwardToCallback extends org.chromium.mojo.bindings.SideEffectFreeCloseable
            implements org.chromium.mojo.bindings.MessageReceiver {
        private final FileSystem.OpenTempDirectoryResponse mCallback;

        FileSystemOpenTempDirectoryResponseParamsForwardToCallback(FileSystem.OpenTempDirectoryResponse callback) {
            this.mCallback = callback;
        }

        @Override
        public boolean accept(org.chromium.mojo.bindings.Message message) {
            try {
                org.chromium.mojo.bindings.ServiceMessage messageWithHeader =
                        message.asServiceMessage();
                org.chromium.mojo.bindings.MessageHeader header = messageWithHeader.getHeader();
                if (!header.validateHeader(OPEN_TEMP_DIRECTORY_ORDINAL,
                                           org.chromium.mojo.bindings.MessageHeader.MESSAGE_IS_RESPONSE_FLAG)) {
                    return false;
                }

                FileSystemOpenTempDirectoryResponseParams response = FileSystemOpenTempDirectoryResponseParams.deserialize(messageWithHeader.getPayload());

                mCallback.call(response.error);
                return true;
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                return false;
            }
        }
    }

    static class FileSystemOpenTempDirectoryResponseParamsProxyToResponder implements FileSystem.OpenTempDirectoryResponse {

        private final org.chromium.mojo.system.Core mCore;
        private final org.chromium.mojo.bindings.MessageReceiver mMessageReceiver;
        private final long mRequestId;

        FileSystemOpenTempDirectoryResponseParamsProxyToResponder(
                org.chromium.mojo.system.Core core,
                org.chromium.mojo.bindings.MessageReceiver messageReceiver,
                long requestId) {
            mCore = core;
            mMessageReceiver = messageReceiver;
            mRequestId = requestId;
        }

        @Override
        public void call(Integer error) {
            FileSystemOpenTempDirectoryResponseParams _response = new FileSystemOpenTempDirectoryResponseParams();

            _response.error = error;

            org.chromium.mojo.bindings.ServiceMessage _message =
                    _response.serializeWithHeader(
                            mCore,
                            new org.chromium.mojo.bindings.MessageHeader(
                                    OPEN_TEMP_DIRECTORY_ORDINAL,
                                    org.chromium.mojo.bindings.MessageHeader.MESSAGE_IS_RESPONSE_FLAG,
                                    mRequestId));
            mMessageReceiver.accept(_message);
        }
    }



    
    static final class FileSystemOpenPersistentFileSystemParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public org.chromium.mojo.bindings.InterfaceRequest<Directory> directory;
    
    
    
        private FileSystemOpenPersistentFileSystemParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public FileSystemOpenPersistentFileSystemParams() {
            this(0);
        }
    
        public static FileSystemOpenPersistentFileSystemParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static FileSystemOpenPersistentFileSystemParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            FileSystemOpenPersistentFileSystemParams result = new FileSystemOpenPersistentFileSystemParams(mainDataHeader.elementsOrVersion);
    
    
            if (mainDataHeader.elementsOrVersion >= 0) {
                
                
                result.directory = decoder0.readInterfaceRequest(8, false);
                
            }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
            return result;
        }
    
        @SuppressWarnings("unchecked")
        @Override
        protected final void encode(org.chromium.mojo.bindings.Encoder encoder) {
    
            org.chromium.mojo.bindings.Encoder encoder0 = encoder.getEncoderAtDataOffset(DEFAULT_STRUCT_INFO);
    
    
    
            
            
            encoder0.encode(directory, 8, false);
            
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
    
            FileSystemOpenPersistentFileSystemParams other = (FileSystemOpenPersistentFileSystemParams) object;
    
            if (!org.chromium.mojo.bindings.BindingsHelper.equals(this.directory, other.directory))
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
    
            result = prime * result + org.chromium.mojo.bindings.BindingsHelper.hashCode(directory);
    
            return result;
        }
    }



    
    static final class FileSystemOpenPersistentFileSystemResponseParams extends org.chromium.mojo.bindings.Struct {
    
        private static final int STRUCT_SIZE = 16;
        private static final org.chromium.mojo.bindings.DataHeader[] VERSION_ARRAY = new org.chromium.mojo.bindings.DataHeader[] {new org.chromium.mojo.bindings.DataHeader(16, 0)};
        private static final org.chromium.mojo.bindings.DataHeader DEFAULT_STRUCT_INFO = VERSION_ARRAY[0];
    
    
    
    
    
        public int error;
    
    
    
        private FileSystemOpenPersistentFileSystemResponseParams(int version) {
            super(STRUCT_SIZE, version);
    
    
    
        }
    
        public FileSystemOpenPersistentFileSystemResponseParams() {
            this(0);
        }
    
        public static FileSystemOpenPersistentFileSystemResponseParams deserialize(org.chromium.mojo.bindings.Message message) {
            return decode(new org.chromium.mojo.bindings.Decoder(message));
        }
    
        @SuppressWarnings("unchecked")
        public static FileSystemOpenPersistentFileSystemResponseParams decode(org.chromium.mojo.bindings.Decoder decoder0) {
            if (decoder0 == null) {
                return null;
            }
            org.chromium.mojo.bindings.DataHeader mainDataHeader = decoder0.readAndValidateDataHeader(VERSION_ARRAY);
            FileSystemOpenPersistentFileSystemResponseParams result = new FileSystemOpenPersistentFileSystemResponseParams(mainDataHeader.elementsOrVersion);
    
    
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
    
            FileSystemOpenPersistentFileSystemResponseParams other = (FileSystemOpenPersistentFileSystemResponseParams) object;
    
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

    static class FileSystemOpenPersistentFileSystemResponseParamsForwardToCallback extends org.chromium.mojo.bindings.SideEffectFreeCloseable
            implements org.chromium.mojo.bindings.MessageReceiver {
        private final FileSystem.OpenPersistentFileSystemResponse mCallback;

        FileSystemOpenPersistentFileSystemResponseParamsForwardToCallback(FileSystem.OpenPersistentFileSystemResponse callback) {
            this.mCallback = callback;
        }

        @Override
        public boolean accept(org.chromium.mojo.bindings.Message message) {
            try {
                org.chromium.mojo.bindings.ServiceMessage messageWithHeader =
                        message.asServiceMessage();
                org.chromium.mojo.bindings.MessageHeader header = messageWithHeader.getHeader();
                if (!header.validateHeader(OPEN_PERSISTENT_FILE_SYSTEM_ORDINAL,
                                           org.chromium.mojo.bindings.MessageHeader.MESSAGE_IS_RESPONSE_FLAG)) {
                    return false;
                }

                FileSystemOpenPersistentFileSystemResponseParams response = FileSystemOpenPersistentFileSystemResponseParams.deserialize(messageWithHeader.getPayload());

                mCallback.call(response.error);
                return true;
            } catch (org.chromium.mojo.bindings.DeserializationException e) {
                return false;
            }
        }
    }

    static class FileSystemOpenPersistentFileSystemResponseParamsProxyToResponder implements FileSystem.OpenPersistentFileSystemResponse {

        private final org.chromium.mojo.system.Core mCore;
        private final org.chromium.mojo.bindings.MessageReceiver mMessageReceiver;
        private final long mRequestId;

        FileSystemOpenPersistentFileSystemResponseParamsProxyToResponder(
                org.chromium.mojo.system.Core core,
                org.chromium.mojo.bindings.MessageReceiver messageReceiver,
                long requestId) {
            mCore = core;
            mMessageReceiver = messageReceiver;
            mRequestId = requestId;
        }

        @Override
        public void call(Integer error) {
            FileSystemOpenPersistentFileSystemResponseParams _response = new FileSystemOpenPersistentFileSystemResponseParams();

            _response.error = error;

            org.chromium.mojo.bindings.ServiceMessage _message =
                    _response.serializeWithHeader(
                            mCore,
                            new org.chromium.mojo.bindings.MessageHeader(
                                    OPEN_PERSISTENT_FILE_SYSTEM_ORDINAL,
                                    org.chromium.mojo.bindings.MessageHeader.MESSAGE_IS_RESPONSE_FLAG,
                                    mRequestId));
            mMessageReceiver.accept(_message);
        }
    }



}
