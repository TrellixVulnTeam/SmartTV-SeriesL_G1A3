/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ImageFrameGenerator_h
#define ImageFrameGenerator_h

#include "platform/PlatformExport.h"
#include "platform/graphics/ThreadSafeDataTransport.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkTypes.h"
#include "third_party/skia/include/core/SkYUVSizeInfo.h"
#include "wtf/Allocator.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/Vector.h"

class SkData;

namespace blink {

class ImageDecoder;
class SharedBuffer;

class PLATFORM_EXPORT ImageDecoderFactory {
    USING_FAST_MALLOC(ImageDecoderFactory);
    WTF_MAKE_NONCOPYABLE(ImageDecoderFactory);
public:
    ImageDecoderFactory() {}
    virtual ~ImageDecoderFactory() { }
    virtual PassOwnPtr<ImageDecoder> create() = 0;
};

class PLATFORM_EXPORT ImageFrameGenerator final : public ThreadSafeRefCounted<ImageFrameGenerator> {
    WTF_MAKE_NONCOPYABLE(ImageFrameGenerator);
public:
    static PassRefPtr<ImageFrameGenerator> create(const SkISize& fullSize, PassRefPtr<SharedBuffer> data, bool allDataReceived, bool isMultiFrame = false)
    {
        return adoptRef(new ImageFrameGenerator(fullSize, data, allDataReceived, isMultiFrame));
    }

    ~ImageFrameGenerator();

    void setData(PassRefPtr<SharedBuffer>, bool allDataReceived);

    // Return our encoded image data. Caller takes ownership and must unref the data
    // according to the contract SkImageGenerator::refEncodedData. Returns null if
    // the data is has not been fully received.
    SkData* refEncodedData();

    // Decodes and scales the specified frame at |index|. The dimensions and output
    // format are given in SkImageInfo. Decoded pixels are written into |pixels| with
    // a stride of |rowBytes|. Returns true if decoding was successful.
    bool decodeAndScale(size_t index, const SkImageInfo&, void* pixels, size_t rowBytes);

    // Decodes YUV components directly into the provided memory planes.
    bool decodeToYUV(size_t index, const SkISize componentSizes[3], void* planes[3], const size_t rowBytes[3]);

    const SkISize& getFullSize() const { return m_fullSize; }

    bool isMultiFrame() const { return m_isMultiFrame; }
    bool decodeFailed() const { return m_decodeFailed; }

    bool hasAlpha(size_t index);

    bool getYUVComponentSizes(SkYUVSizeInfo*);

private:
    ImageFrameGenerator(const SkISize& fullSize, PassRefPtr<SharedBuffer>, bool allDataReceived, bool isMultiFrame);

    friend class ImageFrameGeneratorTest;
    friend class DeferredImageDecoderTest;
    // For testing. |factory| will overwrite the default ImageDecoder creation logic if |factory->create()| returns non-zero.
    void setImageDecoderFactory(PassOwnPtr<ImageDecoderFactory> factory) { m_imageDecoderFactory = std::move(factory); }

    void setHasAlpha(size_t index, bool hasAlpha);

    // These methods are called while m_decodeMutex is locked.
    SkBitmap tryToResumeDecode(size_t index, const SkISize& scaledSize);
    bool decode(size_t index, ImageDecoder**, SkBitmap*);

    SkISize m_fullSize;

    // ThreadSafeDataTransport is referenced by this class and m_encodedData.
    // In case that ImageFrameGenerator get's deleted before m_encodedData,
    // m_encodedData would hold the reference to it (and underlying data).
    RefPtr<ThreadSafeDataTransport> m_data;

    bool m_isMultiFrame;
    bool m_decodeFailed;
    bool m_yuvDecodingFailed;
    size_t m_frameCount;
    Vector<bool> m_hasAlpha;

    class ExternalMemoryAllocator;
    OwnPtr<ExternalMemoryAllocator> m_externalAllocator;

    OwnPtr<ImageDecoderFactory> m_imageDecoderFactory;

    // Prevents multiple decode operations on the same data.
    Mutex m_decodeMutex;

    // Protect concurrent access to m_hasAlpha.
    Mutex m_alphaMutex;

    // Our encoded image data returned in refEncodedData.
    SkData* m_encodedData;

#if COMPILER(MSVC)
    friend struct ::WTF::OwnedPtrDeleter<ExternalMemoryAllocator>;
#endif
};

} // namespace blink

#endif
