/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "webrtc/video/video_capture_input.h"

#include <memory>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"
#include "webrtc/base/event.h"
#include "webrtc/base/refcount.h"
#include "webrtc/test/fake_texture_frame.h"
#include "webrtc/video/send_statistics_proxy.h"

// If an output frame does not arrive in 500ms, the test will fail.
#define FRAME_TIMEOUT_MS 500

namespace webrtc {

bool EqualFrames(const VideoFrame& frame1, const VideoFrame& frame2);
bool EqualTextureFrames(const VideoFrame& frame1, const VideoFrame& frame2);
bool EqualBufferFrames(const VideoFrame& frame1, const VideoFrame& frame2);
bool EqualFramesVector(const std::vector<std::unique_ptr<VideoFrame>>& frames1,
                       const std::vector<std::unique_ptr<VideoFrame>>& frames2);
std::unique_ptr<VideoFrame> CreateVideoFrame(uint8_t length);

class VideoCaptureInputTest : public ::testing::Test {
 protected:
  VideoCaptureInputTest()
      : stats_proxy_(Clock::GetRealTimeClock(),
                     webrtc::VideoSendStream::Config(nullptr),
                     webrtc::VideoEncoderConfig::ContentType::kRealtimeVideo),
        capture_event_(false, false) {}

  virtual void SetUp() {
    overuse_detector_.reset(
        new OveruseFrameDetector(Clock::GetRealTimeClock(), CpuOveruseOptions(),
                                 nullptr, nullptr, &stats_proxy_));
    input_.reset(new internal::VideoCaptureInput(
        &capture_event_, nullptr, &stats_proxy_, overuse_detector_.get()));
  }

  void AddInputFrame(VideoFrame* frame) {
    input_->IncomingCapturedFrame(*frame);
  }

  void WaitOutputFrame() {
    EXPECT_TRUE(capture_event_.Wait(FRAME_TIMEOUT_MS));
    VideoFrame frame;
    EXPECT_TRUE(input_->GetVideoFrame(&frame));
    if (!frame.native_handle()) {
      output_frame_ybuffers_.push_back(
          static_cast<const VideoFrame*>(&frame)->buffer(kYPlane));
    }
    output_frames_.push_back(
        std::unique_ptr<VideoFrame>(new VideoFrame(frame)));
  }

  SendStatisticsProxy stats_proxy_;

  rtc::Event capture_event_;

  std::unique_ptr<OveruseFrameDetector> overuse_detector_;

  // Used to send input capture frames to VideoCaptureInput.
  std::unique_ptr<internal::VideoCaptureInput> input_;

  // Input capture frames of VideoCaptureInput.
  std::vector<std::unique_ptr<VideoFrame>> input_frames_;

  // Output delivered frames of VideoCaptureInput.
  std::vector<std::unique_ptr<VideoFrame>> output_frames_;

  // The pointers of Y plane buffers of output frames. This is used to verify
  // the frame are swapped and not copied.
  std::vector<const uint8_t*> output_frame_ybuffers_;
};

TEST_F(VideoCaptureInputTest, DoesNotRetainHandleNorCopyBuffer) {
  // Indicate an output frame has arrived.
  rtc::Event frame_destroyed_event(false, false);
  class TestBuffer : public webrtc::I420Buffer {
   public:
    explicit TestBuffer(rtc::Event* event) : I420Buffer(5, 5), event_(event) {}

   private:
    friend class rtc::RefCountedObject<TestBuffer>;
    ~TestBuffer() override { event_->Set(); }
    rtc::Event* const event_;
  };

  VideoFrame frame(
      new rtc::RefCountedObject<TestBuffer>(&frame_destroyed_event), 1, 1,
      kVideoRotation_0);

  AddInputFrame(&frame);
  WaitOutputFrame();

  EXPECT_EQ(output_frames_[0]->video_frame_buffer().get(),
            frame.video_frame_buffer().get());
  output_frames_.clear();
  frame.Reset();
  EXPECT_TRUE(frame_destroyed_event.Wait(FRAME_TIMEOUT_MS));
}

TEST_F(VideoCaptureInputTest, TestNtpTimeStampSetIfRenderTimeSet) {
  input_frames_.push_back(CreateVideoFrame(0));
  input_frames_[0]->set_render_time_ms(5);
  input_frames_[0]->set_ntp_time_ms(0);

  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();
  EXPECT_GT(output_frames_[0]->ntp_time_ms(),
            input_frames_[0]->render_time_ms());
}

TEST_F(VideoCaptureInputTest, TestRtpTimeStampSet) {
  input_frames_.push_back(CreateVideoFrame(0));
  input_frames_[0]->set_render_time_ms(0);
  input_frames_[0]->set_ntp_time_ms(1);
  input_frames_[0]->set_timestamp(0);

  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();
  EXPECT_EQ(output_frames_[0]->timestamp(),
            input_frames_[0]->ntp_time_ms() * 90);
}

TEST_F(VideoCaptureInputTest, DropsFramesWithSameOrOldNtpTimestamp) {
  input_frames_.push_back(CreateVideoFrame(0));

  input_frames_[0]->set_ntp_time_ms(17);
  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();
  EXPECT_EQ(output_frames_[0]->timestamp(),
            input_frames_[0]->ntp_time_ms() * 90);

  // Repeat frame with the same NTP timestamp should drop.
  AddInputFrame(input_frames_[0].get());
  EXPECT_FALSE(capture_event_.Wait(FRAME_TIMEOUT_MS));

  // As should frames with a decreased NTP timestamp.
  input_frames_[0]->set_ntp_time_ms(input_frames_[0]->ntp_time_ms() - 1);
  AddInputFrame(input_frames_[0].get());
  EXPECT_FALSE(capture_event_.Wait(FRAME_TIMEOUT_MS));

  // But delivering with an increased NTP timestamp should succeed.
  input_frames_[0]->set_ntp_time_ms(4711);
  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();
  EXPECT_EQ(output_frames_[1]->timestamp(),
            input_frames_[0]->ntp_time_ms() * 90);
}

TEST_F(VideoCaptureInputTest, TestTextureFrames) {
  const int kNumFrame = 3;
  for (int i = 0 ; i < kNumFrame; ++i) {
    test::FakeNativeHandle* dummy_handle = new test::FakeNativeHandle();
    // Add one to |i| so that width/height > 0.
    input_frames_.push_back(std::unique_ptr<VideoFrame>(new VideoFrame(
        test::FakeNativeHandle::CreateFrame(dummy_handle, i + 1, i + 1, i + 1,
                                            i + 1, webrtc::kVideoRotation_0))));
    AddInputFrame(input_frames_[i].get());
    WaitOutputFrame();
    EXPECT_EQ(dummy_handle, output_frames_[i]->native_handle());
  }

  EXPECT_TRUE(EqualFramesVector(input_frames_, output_frames_));
}

TEST_F(VideoCaptureInputTest, TestI420Frames) {
  const int kNumFrame = 4;
  std::vector<const uint8_t*> ybuffer_pointers;
  for (int i = 0; i < kNumFrame; ++i) {
    input_frames_.push_back(CreateVideoFrame(static_cast<uint8_t>(i + 1)));
    const VideoFrame* const_input_frame = input_frames_[i].get();
    ybuffer_pointers.push_back(const_input_frame->buffer(kYPlane));
    AddInputFrame(input_frames_[i].get());
    WaitOutputFrame();
  }

  EXPECT_TRUE(EqualFramesVector(input_frames_, output_frames_));
  // Make sure the buffer is not copied.
  for (int i = 0; i < kNumFrame; ++i)
    EXPECT_EQ(ybuffer_pointers[i], output_frame_ybuffers_[i]);
}

TEST_F(VideoCaptureInputTest, TestI420FrameAfterTextureFrame) {
  test::FakeNativeHandle* dummy_handle = new test::FakeNativeHandle();
  input_frames_.push_back(std::unique_ptr<VideoFrame>(
      new VideoFrame(test::FakeNativeHandle::CreateFrame(
          dummy_handle, 1, 1, 1, 1, webrtc::kVideoRotation_0))));
  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();
  EXPECT_EQ(dummy_handle, output_frames_[0]->native_handle());

  input_frames_.push_back(CreateVideoFrame(2));
  AddInputFrame(input_frames_[1].get());
  WaitOutputFrame();

  EXPECT_TRUE(EqualFramesVector(input_frames_, output_frames_));
}

TEST_F(VideoCaptureInputTest, TestTextureFrameAfterI420Frame) {
  input_frames_.push_back(CreateVideoFrame(1));
  AddInputFrame(input_frames_[0].get());
  WaitOutputFrame();

  test::FakeNativeHandle* dummy_handle = new test::FakeNativeHandle();
  input_frames_.push_back(std::unique_ptr<VideoFrame>(
      new VideoFrame(test::FakeNativeHandle::CreateFrame(
          dummy_handle, 1, 1, 2, 2, webrtc::kVideoRotation_0))));
  AddInputFrame(input_frames_[1].get());
  WaitOutputFrame();

  EXPECT_TRUE(EqualFramesVector(input_frames_, output_frames_));
}

bool EqualFrames(const VideoFrame& frame1, const VideoFrame& frame2) {
  if (frame1.native_handle() || frame2.native_handle())
    return EqualTextureFrames(frame1, frame2);
  return EqualBufferFrames(frame1, frame2);
}

bool EqualTextureFrames(const VideoFrame& frame1, const VideoFrame& frame2) {
  return ((frame1.native_handle() == frame2.native_handle()) &&
          (frame1.width() == frame2.width()) &&
          (frame1.height() == frame2.height()));
}

bool EqualBufferFrames(const VideoFrame& frame1, const VideoFrame& frame2) {
  return ((frame1.width() == frame2.width()) &&
          (frame1.height() == frame2.height()) &&
          (frame1.stride(kYPlane) == frame2.stride(kYPlane)) &&
          (frame1.stride(kUPlane) == frame2.stride(kUPlane)) &&
          (frame1.stride(kVPlane) == frame2.stride(kVPlane)) &&
          (frame1.allocated_size(kYPlane) == frame2.allocated_size(kYPlane)) &&
          (frame1.allocated_size(kUPlane) == frame2.allocated_size(kUPlane)) &&
          (frame1.allocated_size(kVPlane) == frame2.allocated_size(kVPlane)) &&
          (memcmp(frame1.buffer(kYPlane), frame2.buffer(kYPlane),
                  frame1.allocated_size(kYPlane)) == 0) &&
          (memcmp(frame1.buffer(kUPlane), frame2.buffer(kUPlane),
                  frame1.allocated_size(kUPlane)) == 0) &&
          (memcmp(frame1.buffer(kVPlane), frame2.buffer(kVPlane),
                  frame1.allocated_size(kVPlane)) == 0));
}

bool EqualFramesVector(
    const std::vector<std::unique_ptr<VideoFrame>>& frames1,
    const std::vector<std::unique_ptr<VideoFrame>>& frames2) {
  if (frames1.size() != frames2.size())
    return false;
  for (size_t i = 0; i < frames1.size(); ++i) {
    if (!EqualFrames(*frames1[i], *frames2[i]))
      return false;
  }
  return true;
}

std::unique_ptr<VideoFrame> CreateVideoFrame(uint8_t data) {
  std::unique_ptr<VideoFrame> frame(new VideoFrame());
  const int width = 36;
  const int height = 24;
  const int kSizeY = width * height * 2;
  uint8_t buffer[kSizeY];
  memset(buffer, data, kSizeY);
  frame->CreateFrame(buffer, buffer, buffer, width, height, width, width / 2,
                     width / 2, kVideoRotation_0);
  frame->set_render_time_ms(data);
  return frame;
}

}  // namespace webrtc
