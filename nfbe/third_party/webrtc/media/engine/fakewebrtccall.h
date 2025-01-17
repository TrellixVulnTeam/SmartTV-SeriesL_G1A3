/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains fake implementations, for use in unit tests, of the
// following classes:
//
//   webrtc::Call
//   webrtc::AudioSendStream
//   webrtc::AudioReceiveStream
//   webrtc::VideoSendStream
//   webrtc::VideoReceiveStream

#ifndef WEBRTC_MEDIA_ENGINE_FAKEWEBRTCCALL_H_
#define WEBRTC_MEDIA_ENGINE_FAKEWEBRTCCALL_H_

#include <memory>
#include <vector>

#include "webrtc/audio_receive_stream.h"
#include "webrtc/audio_send_stream.h"
#include "webrtc/call.h"
#include "webrtc/video_frame.h"
#include "webrtc/video_receive_stream.h"
#include "webrtc/video_send_stream.h"

namespace cricket {
class FakeAudioSendStream final : public webrtc::AudioSendStream {
 public:
  struct TelephoneEvent {
    int payload_type = -1;
    int event_code = 0;
    int duration_ms = 0;
  };

  explicit FakeAudioSendStream(const webrtc::AudioSendStream::Config& config);

  const webrtc::AudioSendStream::Config& GetConfig() const;
  void SetStats(const webrtc::AudioSendStream::Stats& stats);
  TelephoneEvent GetLatestTelephoneEvent() const;
  bool IsSending() const { return sending_; }

 private:
  // webrtc::SendStream implementation.
  void Start() override { sending_ = true; }
  void Stop() override { sending_ = false; }
  void SignalNetworkState(webrtc::NetworkState state) override {}
  bool DeliverRtcp(const uint8_t* packet, size_t length) override {
    return true;
  }

  // webrtc::AudioSendStream implementation.
  bool SendTelephoneEvent(int payload_type, int event,
                          int duration_ms) override;
  webrtc::AudioSendStream::Stats GetStats() const override;

  TelephoneEvent latest_telephone_event_;
  webrtc::AudioSendStream::Config config_;
  webrtc::AudioSendStream::Stats stats_;
  bool sending_ = false;
};

class FakeAudioReceiveStream final : public webrtc::AudioReceiveStream {
 public:
  explicit FakeAudioReceiveStream(
      const webrtc::AudioReceiveStream::Config& config);

  const webrtc::AudioReceiveStream::Config& GetConfig() const;
  void SetStats(const webrtc::AudioReceiveStream::Stats& stats);
  int received_packets() const { return received_packets_; }
  void IncrementReceivedPackets();
  const webrtc::AudioSinkInterface* sink() const { return sink_.get(); }

 private:
  // webrtc::ReceiveStream implementation.
  void Start() override {}
  void Stop() override {}
  void SignalNetworkState(webrtc::NetworkState state) override {}
  bool DeliverRtcp(const uint8_t* packet, size_t length) override {
    return true;
  }
  bool DeliverRtp(const uint8_t* packet,
                  size_t length,
                  const webrtc::PacketTime& packet_time) override {
    return true;
  }

  // webrtc::AudioReceiveStream implementation.
  webrtc::AudioReceiveStream::Stats GetStats() const override;
  void SetSink(std::unique_ptr<webrtc::AudioSinkInterface> sink) override;

  webrtc::AudioReceiveStream::Config config_;
  webrtc::AudioReceiveStream::Stats stats_;
  int received_packets_;
  std::unique_ptr<webrtc::AudioSinkInterface> sink_;
};

class FakeVideoSendStream final : public webrtc::VideoSendStream,
                                  public webrtc::VideoCaptureInput {
 public:
  FakeVideoSendStream(const webrtc::VideoSendStream::Config& config,
                      const webrtc::VideoEncoderConfig& encoder_config);
  webrtc::VideoSendStream::Config GetConfig() const;
  webrtc::VideoEncoderConfig GetEncoderConfig() const;
  std::vector<webrtc::VideoStream> GetVideoStreams();

  bool IsSending() const;
  bool GetVp8Settings(webrtc::VideoCodecVP8* settings) const;
  bool GetVp9Settings(webrtc::VideoCodecVP9* settings) const;

  int GetNumberOfSwappedFrames() const;
  int GetLastWidth() const;
  int GetLastHeight() const;
  int64_t GetLastTimestamp() const;
  void SetStats(const webrtc::VideoSendStream::Stats& stats);
  int num_encoder_reconfigurations() const {
    return num_encoder_reconfigurations_;
  }

 private:
  void IncomingCapturedFrame(const webrtc::VideoFrame& frame) override;

  // webrtc::SendStream implementation.
  void Start() override;
  void Stop() override;
  void SignalNetworkState(webrtc::NetworkState state) override {}
  bool DeliverRtcp(const uint8_t* packet, size_t length) override {
    return true;
  }

  // webrtc::VideoSendStream implementation.
  webrtc::VideoSendStream::Stats GetStats() override;
  void ReconfigureVideoEncoder(
      const webrtc::VideoEncoderConfig& config) override;
  webrtc::VideoCaptureInput* Input() override;

  bool sending_;
  webrtc::VideoSendStream::Config config_;
  webrtc::VideoEncoderConfig encoder_config_;
  bool codec_settings_set_;
  union VpxSettings {
    webrtc::VideoCodecVP8 vp8;
    webrtc::VideoCodecVP9 vp9;
  } vpx_settings_;
  int num_swapped_frames_;
  webrtc::VideoFrame last_frame_;
  webrtc::VideoSendStream::Stats stats_;
  int num_encoder_reconfigurations_ = 0;
};

class FakeVideoReceiveStream final : public webrtc::VideoReceiveStream {
 public:
  explicit FakeVideoReceiveStream(
      const webrtc::VideoReceiveStream::Config& config);

  webrtc::VideoReceiveStream::Config GetConfig();

  bool IsReceiving() const;

  void InjectFrame(const webrtc::VideoFrame& frame);

  void SetStats(const webrtc::VideoReceiveStream::Stats& stats);

 private:
  // webrtc::ReceiveStream implementation.
  void Start() override;
  void Stop() override;
  void SignalNetworkState(webrtc::NetworkState state) override {}
  bool DeliverRtcp(const uint8_t* packet, size_t length) override {
    return true;
  }
  bool DeliverRtp(const uint8_t* packet,
                  size_t length,
                  const webrtc::PacketTime& packet_time) override {
    return true;
  }

  // webrtc::VideoReceiveStream implementation.
  webrtc::VideoReceiveStream::Stats GetStats() const override;

  webrtc::VideoReceiveStream::Config config_;
  bool receiving_;
  webrtc::VideoReceiveStream::Stats stats_;
};

class FakeCall final : public webrtc::Call, public webrtc::PacketReceiver {
 public:
  explicit FakeCall(const webrtc::Call::Config& config);
  ~FakeCall() override;

  webrtc::Call::Config GetConfig() const;
  const std::vector<FakeVideoSendStream*>& GetVideoSendStreams();
  const std::vector<FakeVideoReceiveStream*>& GetVideoReceiveStreams();

  const std::vector<FakeAudioSendStream*>& GetAudioSendStreams();
  const FakeAudioSendStream* GetAudioSendStream(uint32_t ssrc);
  const std::vector<FakeAudioReceiveStream*>& GetAudioReceiveStreams();
  const FakeAudioReceiveStream* GetAudioReceiveStream(uint32_t ssrc);

  rtc::SentPacket last_sent_packet() const { return last_sent_packet_; }
  webrtc::NetworkState GetNetworkState(webrtc::MediaType media) const;
  int GetNumCreatedSendStreams() const;
  int GetNumCreatedReceiveStreams() const;
  void SetStats(const webrtc::Call::Stats& stats);

 private:
  webrtc::AudioSendStream* CreateAudioSendStream(
      const webrtc::AudioSendStream::Config& config) override;
  void DestroyAudioSendStream(webrtc::AudioSendStream* send_stream) override;

  webrtc::AudioReceiveStream* CreateAudioReceiveStream(
      const webrtc::AudioReceiveStream::Config& config) override;
  void DestroyAudioReceiveStream(
      webrtc::AudioReceiveStream* receive_stream) override;

  webrtc::VideoSendStream* CreateVideoSendStream(
      const webrtc::VideoSendStream::Config& config,
      const webrtc::VideoEncoderConfig& encoder_config) override;
  void DestroyVideoSendStream(webrtc::VideoSendStream* send_stream) override;

  webrtc::VideoReceiveStream* CreateVideoReceiveStream(
      const webrtc::VideoReceiveStream::Config& config) override;
  void DestroyVideoReceiveStream(
      webrtc::VideoReceiveStream* receive_stream) override;
  webrtc::PacketReceiver* Receiver() override;

  DeliveryStatus DeliverPacket(webrtc::MediaType media_type,
                               const uint8_t* packet,
                               size_t length,
                               const webrtc::PacketTime& packet_time) override;

  webrtc::Call::Stats GetStats() const override;

  void SetBitrateConfig(
      const webrtc::Call::Config::BitrateConfig& bitrate_config) override;
  void SignalChannelNetworkState(webrtc::MediaType media,
                                 webrtc::NetworkState state) override;
  void OnSentPacket(const rtc::SentPacket& sent_packet) override;

  webrtc::Call::Config config_;
  webrtc::NetworkState audio_network_state_;
  webrtc::NetworkState video_network_state_;
  rtc::SentPacket last_sent_packet_;
  webrtc::Call::Stats stats_;
  std::vector<FakeVideoSendStream*> video_send_streams_;
  std::vector<FakeAudioSendStream*> audio_send_streams_;
  std::vector<FakeVideoReceiveStream*> video_receive_streams_;
  std::vector<FakeAudioReceiveStream*> audio_receive_streams_;

  int num_created_send_streams_;
  int num_created_receive_streams_;
};

}  // namespace cricket
#endif  // TALK_MEDIA_WEBRTC_WEBRTCVIDEOENGINE2_UNITTEST_H_
