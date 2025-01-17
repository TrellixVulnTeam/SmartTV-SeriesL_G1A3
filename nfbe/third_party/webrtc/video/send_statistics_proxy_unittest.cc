/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file includes unit tests for SendStatisticsProxy.
#include "webrtc/video/send_statistics_proxy.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"
#include "webrtc/system_wrappers/include/metrics.h"
#include "webrtc/test/histogram.h"

namespace webrtc {

static const uint32_t kFirstSsrc = 17;
static const uint32_t kSecondSsrc = 42;
static const uint32_t kFirstRtxSsrc = 18;
static const uint32_t kSecondRtxSsrc = 43;

class SendStatisticsProxyTest : public ::testing::Test {
 public:
  SendStatisticsProxyTest()
      : fake_clock_(1234), config_(GetTestConfig()), avg_delay_ms_(0),
        max_delay_ms_(0) {}
  virtual ~SendStatisticsProxyTest() {}

 protected:
  virtual void SetUp() {
    statistics_proxy_.reset(new SendStatisticsProxy(
        &fake_clock_, GetTestConfig(),
        VideoEncoderConfig::ContentType::kRealtimeVideo));
    expected_ = VideoSendStream::Stats();
  }

  VideoSendStream::Config GetTestConfig() {
    VideoSendStream::Config config(nullptr);
    config.rtp.ssrcs.push_back(kFirstSsrc);
    config.rtp.ssrcs.push_back(kSecondSsrc);
    config.rtp.rtx.ssrcs.push_back(kFirstRtxSsrc);
    config.rtp.rtx.ssrcs.push_back(kSecondRtxSsrc);
    config.rtp.fec.red_payload_type = 17;
    return config;
  }

  void ExpectEqual(VideoSendStream::Stats one, VideoSendStream::Stats other) {
    EXPECT_EQ(one.input_frame_rate, other.input_frame_rate);
    EXPECT_EQ(one.encode_frame_rate, other.encode_frame_rate);
    EXPECT_EQ(one.media_bitrate_bps, other.media_bitrate_bps);
    EXPECT_EQ(one.suspended, other.suspended);

    EXPECT_EQ(one.substreams.size(), other.substreams.size());
    for (std::map<uint32_t, VideoSendStream::StreamStats>::const_iterator it =
             one.substreams.begin();
         it != one.substreams.end(); ++it) {
      std::map<uint32_t, VideoSendStream::StreamStats>::const_iterator
          corresponding_it = other.substreams.find(it->first);
      ASSERT_TRUE(corresponding_it != other.substreams.end());
      const VideoSendStream::StreamStats& a = it->second;
      const VideoSendStream::StreamStats& b = corresponding_it->second;

      EXPECT_EQ(a.frame_counts.key_frames, b.frame_counts.key_frames);
      EXPECT_EQ(a.frame_counts.delta_frames, b.frame_counts.delta_frames);
      EXPECT_EQ(a.total_bitrate_bps, b.total_bitrate_bps);
      EXPECT_EQ(a.avg_delay_ms, b.avg_delay_ms);
      EXPECT_EQ(a.max_delay_ms, b.max_delay_ms);

      EXPECT_EQ(a.rtp_stats.transmitted.payload_bytes,
                b.rtp_stats.transmitted.payload_bytes);
      EXPECT_EQ(a.rtp_stats.transmitted.header_bytes,
                b.rtp_stats.transmitted.header_bytes);
      EXPECT_EQ(a.rtp_stats.transmitted.padding_bytes,
                b.rtp_stats.transmitted.padding_bytes);
      EXPECT_EQ(a.rtp_stats.transmitted.packets,
                b.rtp_stats.transmitted.packets);
      EXPECT_EQ(a.rtp_stats.retransmitted.packets,
                b.rtp_stats.retransmitted.packets);
      EXPECT_EQ(a.rtp_stats.fec.packets, b.rtp_stats.fec.packets);

      EXPECT_EQ(a.rtcp_stats.fraction_lost, b.rtcp_stats.fraction_lost);
      EXPECT_EQ(a.rtcp_stats.cumulative_lost, b.rtcp_stats.cumulative_lost);
      EXPECT_EQ(a.rtcp_stats.extended_max_sequence_number,
                b.rtcp_stats.extended_max_sequence_number);
      EXPECT_EQ(a.rtcp_stats.jitter, b.rtcp_stats.jitter);
    }
  }

  SimulatedClock fake_clock_;
  std::unique_ptr<SendStatisticsProxy> statistics_proxy_;
  VideoSendStream::Config config_;
  int avg_delay_ms_;
  int max_delay_ms_;
  VideoSendStream::Stats expected_;
  typedef std::map<uint32_t, VideoSendStream::StreamStats>::const_iterator
      StreamIterator;
};

TEST_F(SendStatisticsProxyTest, RtcpStatistics) {
  RtcpStatisticsCallback* callback = statistics_proxy_.get();
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.ssrcs.begin();
       it != config_.rtp.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    VideoSendStream::StreamStats& ssrc_stats = expected_.substreams[ssrc];

    // Add statistics with some arbitrary, but unique, numbers.
    uint32_t offset = ssrc * sizeof(RtcpStatistics);
    ssrc_stats.rtcp_stats.cumulative_lost = offset;
    ssrc_stats.rtcp_stats.extended_max_sequence_number = offset + 1;
    ssrc_stats.rtcp_stats.fraction_lost = offset + 2;
    ssrc_stats.rtcp_stats.jitter = offset + 3;
    callback->StatisticsUpdated(ssrc_stats.rtcp_stats, ssrc);
  }
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.rtx.ssrcs.begin();
       it != config_.rtp.rtx.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    VideoSendStream::StreamStats& ssrc_stats = expected_.substreams[ssrc];

    // Add statistics with some arbitrary, but unique, numbers.
    uint32_t offset = ssrc * sizeof(RtcpStatistics);
    ssrc_stats.rtcp_stats.cumulative_lost = offset;
    ssrc_stats.rtcp_stats.extended_max_sequence_number = offset + 1;
    ssrc_stats.rtcp_stats.fraction_lost = offset + 2;
    ssrc_stats.rtcp_stats.jitter = offset + 3;
    callback->StatisticsUpdated(ssrc_stats.rtcp_stats, ssrc);
  }
  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  ExpectEqual(expected_, stats);
}

TEST_F(SendStatisticsProxyTest, EncodedBitrateAndFramerate) {
  int media_bitrate_bps = 500;
  int encode_fps = 29;

  statistics_proxy_->OnOutgoingRate(encode_fps, media_bitrate_bps);

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_EQ(media_bitrate_bps, stats.media_bitrate_bps);
  EXPECT_EQ(encode_fps, stats.encode_frame_rate);
}

TEST_F(SendStatisticsProxyTest, Suspended) {
  // Verify that the value is false by default.
  EXPECT_FALSE(statistics_proxy_->GetStats().suspended);

  // Verify that we can set it to true.
  statistics_proxy_->OnSuspendChange(true);
  EXPECT_TRUE(statistics_proxy_->GetStats().suspended);

  // Verify that we can set it back to false again.
  statistics_proxy_->OnSuspendChange(false);
  EXPECT_FALSE(statistics_proxy_->GetStats().suspended);
}

TEST_F(SendStatisticsProxyTest, FrameCounts) {
  FrameCountObserver* observer = statistics_proxy_.get();
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.ssrcs.begin();
       it != config_.rtp.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    // Add statistics with some arbitrary, but unique, numbers.
    VideoSendStream::StreamStats& stats = expected_.substreams[ssrc];
    uint32_t offset = ssrc * sizeof(VideoSendStream::StreamStats);
    FrameCounts frame_counts;
    frame_counts.key_frames = offset;
    frame_counts.delta_frames = offset + 1;
    stats.frame_counts = frame_counts;
    observer->FrameCountUpdated(frame_counts, ssrc);
  }
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.rtx.ssrcs.begin();
       it != config_.rtp.rtx.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    // Add statistics with some arbitrary, but unique, numbers.
    VideoSendStream::StreamStats& stats = expected_.substreams[ssrc];
    uint32_t offset = ssrc * sizeof(VideoSendStream::StreamStats);
    FrameCounts frame_counts;
    frame_counts.key_frames = offset;
    frame_counts.delta_frames = offset + 1;
    stats.frame_counts = frame_counts;
    observer->FrameCountUpdated(frame_counts, ssrc);
  }

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  ExpectEqual(expected_, stats);
}

TEST_F(SendStatisticsProxyTest, DataCounters) {
  StreamDataCountersCallback* callback = statistics_proxy_.get();
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.ssrcs.begin();
       it != config_.rtp.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    StreamDataCounters& counters = expected_.substreams[ssrc].rtp_stats;
    // Add statistics with some arbitrary, but unique, numbers.
    size_t offset = ssrc * sizeof(StreamDataCounters);
    uint32_t offset_uint32 = static_cast<uint32_t>(offset);
    counters.transmitted.payload_bytes = offset;
    counters.transmitted.header_bytes = offset + 1;
    counters.fec.packets = offset_uint32 + 2;
    counters.transmitted.padding_bytes = offset + 3;
    counters.retransmitted.packets = offset_uint32 + 4;
    counters.transmitted.packets = offset_uint32 + 5;
    callback->DataCountersUpdated(counters, ssrc);
  }
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.rtx.ssrcs.begin();
       it != config_.rtp.rtx.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    StreamDataCounters& counters = expected_.substreams[ssrc].rtp_stats;
    // Add statistics with some arbitrary, but unique, numbers.
    size_t offset = ssrc * sizeof(StreamDataCounters);
    uint32_t offset_uint32 = static_cast<uint32_t>(offset);
    counters.transmitted.payload_bytes = offset;
    counters.transmitted.header_bytes = offset + 1;
    counters.fec.packets = offset_uint32 + 2;
    counters.transmitted.padding_bytes = offset + 3;
    counters.retransmitted.packets = offset_uint32 + 4;
    counters.transmitted.packets = offset_uint32 + 5;
    callback->DataCountersUpdated(counters, ssrc);
  }

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  ExpectEqual(expected_, stats);
}

TEST_F(SendStatisticsProxyTest, Bitrate) {
  BitrateStatisticsObserver* observer = statistics_proxy_.get();
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.ssrcs.begin();
       it != config_.rtp.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    BitrateStatistics total;
    BitrateStatistics retransmit;
    // Use ssrc as bitrate_bps to get a unique value for each stream.
    total.bitrate_bps = ssrc;
    retransmit.bitrate_bps = ssrc + 1;
    observer->Notify(total, retransmit, ssrc);
    expected_.substreams[ssrc].total_bitrate_bps = total.bitrate_bps;
    expected_.substreams[ssrc].retransmit_bitrate_bps = retransmit.bitrate_bps;
  }
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.rtx.ssrcs.begin();
       it != config_.rtp.rtx.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    BitrateStatistics total;
    BitrateStatistics retransmit;
    // Use ssrc as bitrate_bps to get a unique value for each stream.
    total.bitrate_bps = ssrc;
    retransmit.bitrate_bps = ssrc + 1;
    observer->Notify(total, retransmit, ssrc);
    expected_.substreams[ssrc].total_bitrate_bps = total.bitrate_bps;
    expected_.substreams[ssrc].retransmit_bitrate_bps = retransmit.bitrate_bps;
  }

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  ExpectEqual(expected_, stats);
}

TEST_F(SendStatisticsProxyTest, SendSideDelay) {
  SendSideDelayObserver* observer = statistics_proxy_.get();
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.ssrcs.begin();
       it != config_.rtp.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    // Use ssrc as avg_delay_ms and max_delay_ms to get a unique value for each
    // stream.
    int avg_delay_ms = ssrc;
    int max_delay_ms = ssrc + 1;
    observer->SendSideDelayUpdated(avg_delay_ms, max_delay_ms, ssrc);
    expected_.substreams[ssrc].avg_delay_ms = avg_delay_ms;
    expected_.substreams[ssrc].max_delay_ms = max_delay_ms;
  }
  for (std::vector<uint32_t>::const_iterator it = config_.rtp.rtx.ssrcs.begin();
       it != config_.rtp.rtx.ssrcs.end();
       ++it) {
    const uint32_t ssrc = *it;
    // Use ssrc as avg_delay_ms and max_delay_ms to get a unique value for each
    // stream.
    int avg_delay_ms = ssrc;
    int max_delay_ms = ssrc + 1;
    observer->SendSideDelayUpdated(avg_delay_ms, max_delay_ms, ssrc);
    expected_.substreams[ssrc].avg_delay_ms = avg_delay_ms;
    expected_.substreams[ssrc].max_delay_ms = max_delay_ms;
  }
  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  ExpectEqual(expected_, stats);
}

TEST_F(SendStatisticsProxyTest, OnEncodedFrameTimeMeasured) {
  const int kEncodeTimeMs = 11;
  CpuOveruseMetrics metrics;
  metrics.encode_usage_percent = 80;
  statistics_proxy_->OnEncodedFrameTimeMeasured(kEncodeTimeMs, metrics);

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_EQ(kEncodeTimeMs, stats.avg_encode_time_ms);
  EXPECT_EQ(metrics.encode_usage_percent, stats.encode_usage_percent);
}

TEST_F(SendStatisticsProxyTest, SwitchContentTypeUpdatesHistograms) {
  test::ClearHistograms();
  const int kMinRequiredSamples = 200;
  const int kWidth = 640;
  const int kHeight = 480;

  for (int i = 0; i < kMinRequiredSamples; ++i)
    statistics_proxy_->OnIncomingFrame(kWidth, kHeight);

  // No switch, stats not should be updated.
  statistics_proxy_->SetContentType(
      VideoEncoderConfig::ContentType::kRealtimeVideo);
  EXPECT_EQ(0, test::NumHistogramSamples("WebRTC.Video.InputWidthInPixels"));

  // Switch to screenshare, real-time stats should be updated.
  statistics_proxy_->SetContentType(VideoEncoderConfig::ContentType::kScreen);
  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.InputWidthInPixels"));
}

TEST_F(SendStatisticsProxyTest, VerifyQpHistogramStats_Vp8) {
  test::ClearHistograms();
  const int kMinRequiredSamples = 200;
  const int kQpIdx0 = 21;
  const int kQpIdx1 = 39;
  EncodedImage encoded_image;

  RTPVideoHeader rtp_video_header;
  rtp_video_header.codec = kRtpVideoVp8;

  for (int i = 0; i < kMinRequiredSamples; ++i) {
    rtp_video_header.simulcastIdx = 0;
    encoded_image.qp_ = kQpIdx0;
    statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);
    rtp_video_header.simulcastIdx = 1;
    encoded_image.qp_ = kQpIdx1;
    statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);
  }
  statistics_proxy_.reset();
  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.Encoded.Qp.Vp8.S0"));
  EXPECT_EQ(kQpIdx0,
            test::LastHistogramSample("WebRTC.Video.Encoded.Qp.Vp8.S0"));
  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.Encoded.Qp.Vp8.S1"));
  EXPECT_EQ(kQpIdx1,
            test::LastHistogramSample("WebRTC.Video.Encoded.Qp.Vp8.S1"));
}

TEST_F(SendStatisticsProxyTest, VerifyQpHistogramStats_Vp8OneSsrc) {
  VideoSendStream::Config config(nullptr);
  config.rtp.ssrcs.push_back(kFirstSsrc);
  statistics_proxy_.reset(new SendStatisticsProxy(
      &fake_clock_, config, VideoEncoderConfig::ContentType::kRealtimeVideo));

  test::ClearHistograms();
  const int kMinRequiredSamples = 200;
  const int kQpIdx0 = 21;
  EncodedImage encoded_image;

  RTPVideoHeader rtp_video_header;
  rtp_video_header.codec = kRtpVideoVp8;

  for (int i = 0; i < kMinRequiredSamples; ++i) {
    rtp_video_header.simulcastIdx = 0;
    encoded_image.qp_ = kQpIdx0;
    statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);
  }
  statistics_proxy_.reset();
  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.Encoded.Qp.Vp8"));
  EXPECT_EQ(kQpIdx0, test::LastHistogramSample("WebRTC.Video.Encoded.Qp.Vp8"));
}

TEST_F(SendStatisticsProxyTest, NoSubstreams) {
  uint32_t excluded_ssrc =
      std::max(
          *std::max_element(config_.rtp.ssrcs.begin(), config_.rtp.ssrcs.end()),
          *std::max_element(config_.rtp.rtx.ssrcs.begin(),
                            config_.rtp.rtx.ssrcs.end())) +
      1;
  // From RtcpStatisticsCallback.
  RtcpStatistics rtcp_stats;
  RtcpStatisticsCallback* rtcp_callback = statistics_proxy_.get();
  rtcp_callback->StatisticsUpdated(rtcp_stats, excluded_ssrc);

  // From BitrateStatisticsObserver.
  BitrateStatistics total;
  BitrateStatistics retransmit;
  BitrateStatisticsObserver* bitrate_observer = statistics_proxy_.get();
  bitrate_observer->Notify(total, retransmit, excluded_ssrc);

  // From FrameCountObserver.
  FrameCountObserver* fps_observer = statistics_proxy_.get();
  FrameCounts frame_counts;
  frame_counts.key_frames = 1;
  fps_observer->FrameCountUpdated(frame_counts, excluded_ssrc);

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_TRUE(stats.substreams.empty());
}

TEST_F(SendStatisticsProxyTest, EncodedResolutionTimesOut) {
  static const int kEncodedWidth = 123;
  static const int kEncodedHeight = 81;
  EncodedImage encoded_image;
  encoded_image._encodedWidth = kEncodedWidth;
  encoded_image._encodedHeight = kEncodedHeight;

  RTPVideoHeader rtp_video_header;

  rtp_video_header.simulcastIdx = 0;
  statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);
  rtp_video_header.simulcastIdx = 1;
  statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_EQ(kEncodedWidth, stats.substreams[config_.rtp.ssrcs[0]].width);
  EXPECT_EQ(kEncodedHeight, stats.substreams[config_.rtp.ssrcs[0]].height);
  EXPECT_EQ(kEncodedWidth, stats.substreams[config_.rtp.ssrcs[1]].width);
  EXPECT_EQ(kEncodedHeight, stats.substreams[config_.rtp.ssrcs[1]].height);

  // Forward almost to timeout, this should not have removed stats.
  fake_clock_.AdvanceTimeMilliseconds(SendStatisticsProxy::kStatsTimeoutMs - 1);
  stats = statistics_proxy_->GetStats();
  EXPECT_EQ(kEncodedWidth, stats.substreams[config_.rtp.ssrcs[0]].width);
  EXPECT_EQ(kEncodedHeight, stats.substreams[config_.rtp.ssrcs[0]].height);

  // Update the first SSRC with bogus RTCP stats to make sure that encoded
  // resolution still times out (no global timeout for all stats).
  RtcpStatistics rtcp_statistics;
  RtcpStatisticsCallback* rtcp_stats = statistics_proxy_.get();
  rtcp_stats->StatisticsUpdated(rtcp_statistics, config_.rtp.ssrcs[0]);

  // Report stats for second SSRC to make sure it's not outdated along with the
  // first SSRC.
  rtp_video_header.simulcastIdx = 1;
  statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);

  // Forward 1 ms, reach timeout, substream 0 should have no resolution
  // reported, but substream 1 should.
  fake_clock_.AdvanceTimeMilliseconds(1);
  stats = statistics_proxy_->GetStats();
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[0]].width);
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[0]].height);
  EXPECT_EQ(kEncodedWidth, stats.substreams[config_.rtp.ssrcs[1]].width);
  EXPECT_EQ(kEncodedHeight, stats.substreams[config_.rtp.ssrcs[1]].height);
}

TEST_F(SendStatisticsProxyTest, ClearsResolutionFromInactiveSsrcs) {
  static const int kEncodedWidth = 123;
  static const int kEncodedHeight = 81;
  EncodedImage encoded_image;
  encoded_image._encodedWidth = kEncodedWidth;
  encoded_image._encodedHeight = kEncodedHeight;

  RTPVideoHeader rtp_video_header;

  rtp_video_header.simulcastIdx = 0;
  statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);
  rtp_video_header.simulcastIdx = 1;
  statistics_proxy_->OnSendEncodedImage(encoded_image, &rtp_video_header);

  statistics_proxy_->OnInactiveSsrc(config_.rtp.ssrcs[1]);
  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_EQ(kEncodedWidth, stats.substreams[config_.rtp.ssrcs[0]].width);
  EXPECT_EQ(kEncodedHeight, stats.substreams[config_.rtp.ssrcs[0]].height);
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[1]].width);
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[1]].height);
}

TEST_F(SendStatisticsProxyTest, ClearsBitratesFromInactiveSsrcs) {
  BitrateStatistics bitrate;
  bitrate.bitrate_bps = 42;
  BitrateStatisticsObserver* observer = statistics_proxy_.get();
  observer->Notify(bitrate, bitrate, config_.rtp.ssrcs[0]);
  observer->Notify(bitrate, bitrate, config_.rtp.ssrcs[1]);

  statistics_proxy_->OnInactiveSsrc(config_.rtp.ssrcs[1]);

  VideoSendStream::Stats stats = statistics_proxy_->GetStats();
  EXPECT_EQ(static_cast<int>(bitrate.bitrate_bps),
            stats.substreams[config_.rtp.ssrcs[0]].total_bitrate_bps);
  EXPECT_EQ(static_cast<int>(bitrate.bitrate_bps),
            stats.substreams[config_.rtp.ssrcs[0]].retransmit_bitrate_bps);
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[1]].total_bitrate_bps);
  EXPECT_EQ(0, stats.substreams[config_.rtp.ssrcs[1]].retransmit_bitrate_bps);
}

TEST_F(SendStatisticsProxyTest, ResetsRtcpCountersOnContentChange) {
  RtcpPacketTypeCounterObserver* proxy =
      static_cast<RtcpPacketTypeCounterObserver*>(statistics_proxy_.get());
  RtcpPacketTypeCounter counters;
  counters.first_packet_time_ms = fake_clock_.TimeInMilliseconds();
  proxy->RtcpPacketTypesCounterUpdated(kFirstSsrc, counters);
  proxy->RtcpPacketTypesCounterUpdated(kSecondSsrc, counters);

  fake_clock_.AdvanceTimeMilliseconds(1000 * metrics::kMinRunTimeInSeconds);

  counters.nack_packets += 1 * metrics::kMinRunTimeInSeconds;
  counters.fir_packets += 2 * metrics::kMinRunTimeInSeconds;
  counters.pli_packets += 3 * metrics::kMinRunTimeInSeconds;
  counters.unique_nack_requests += 4 * metrics::kMinRunTimeInSeconds;
  counters.nack_requests += 5 * metrics::kMinRunTimeInSeconds;

  proxy->RtcpPacketTypesCounterUpdated(kFirstSsrc, counters);
  proxy->RtcpPacketTypesCounterUpdated(kSecondSsrc, counters);

  // Changing content type causes histograms to be reported.
  statistics_proxy_->SetContentType(VideoEncoderConfig::ContentType::kScreen);

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.NackPacketsReceivedPerMinute"));
  EXPECT_EQ(
      1, test::NumHistogramSamples("WebRTC.Video.FirPacketsReceivedPerMinute"));
  EXPECT_EQ(
      1, test::NumHistogramSamples("WebRTC.Video.PliPacketsReceivedPerMinute"));
  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.UniqueNackRequestsReceivedInPercent"));

  const int kRate = 60 * 2;  // Packets per minute with two streams.

  EXPECT_EQ(1 * kRate, test::LastHistogramSample(
                           "WebRTC.Video.NackPacketsReceivedPerMinute"));
  EXPECT_EQ(2 * kRate, test::LastHistogramSample(
                           "WebRTC.Video.FirPacketsReceivedPerMinute"));
  EXPECT_EQ(3 * kRate, test::LastHistogramSample(
                           "WebRTC.Video.PliPacketsReceivedPerMinute"));
  EXPECT_EQ(4 * 100 / 5,
            test::LastHistogramSample(
                "WebRTC.Video.UniqueNackRequestsReceivedInPercent"));

  // New start time but same counter values.
  proxy->RtcpPacketTypesCounterUpdated(kFirstSsrc, counters);
  proxy->RtcpPacketTypesCounterUpdated(kSecondSsrc, counters);

  fake_clock_.AdvanceTimeMilliseconds(1000 * metrics::kMinRunTimeInSeconds);

  counters.nack_packets += 1 * metrics::kMinRunTimeInSeconds;
  counters.fir_packets += 2 * metrics::kMinRunTimeInSeconds;
  counters.pli_packets += 3 * metrics::kMinRunTimeInSeconds;
  counters.unique_nack_requests += 4 * metrics::kMinRunTimeInSeconds;
  counters.nack_requests += 5 * metrics::kMinRunTimeInSeconds;

  proxy->RtcpPacketTypesCounterUpdated(kFirstSsrc, counters);
  proxy->RtcpPacketTypesCounterUpdated(kSecondSsrc, counters);

  SetUp();  // Reset stats proxy also causes histograms to be reported.

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.NackPacketsReceivedPerMinute"));
  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.FirPacketsReceivedPerMinute"));
  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.PliPacketsReceivedPerMinute"));
  EXPECT_EQ(
      1, test::NumHistogramSamples(
             "WebRTC.Video.Screenshare.UniqueNackRequestsReceivedInPercent"));

  EXPECT_EQ(1 * kRate,
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.NackPacketsReceivedPerMinute"));
  EXPECT_EQ(2 * kRate,
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.FirPacketsReceivedPerMinute"));
  EXPECT_EQ(3 * kRate,
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.PliPacketsReceivedPerMinute"));
  EXPECT_EQ(
      4 * 100 / 5,
      test::LastHistogramSample(
          "WebRTC.Video.Screenshare.UniqueNackRequestsReceivedInPercent"));
}

TEST_F(SendStatisticsProxyTest, ResetsRtpCountersOnContentChange) {
  StreamDataCountersCallback* proxy =
      static_cast<StreamDataCountersCallback*>(statistics_proxy_.get());
  StreamDataCounters counters;
  StreamDataCounters rtx_counters;
  counters.first_packet_time_ms = fake_clock_.TimeInMilliseconds();
  proxy->DataCountersUpdated(counters, kFirstSsrc);
  proxy->DataCountersUpdated(counters, kSecondSsrc);
  proxy->DataCountersUpdated(rtx_counters, kFirstRtxSsrc);
  proxy->DataCountersUpdated(rtx_counters, kSecondRtxSsrc);

  counters.transmitted.header_bytes = 400;
  counters.transmitted.packets = 20;
  counters.transmitted.padding_bytes = 1000;
  counters.transmitted.payload_bytes = 2000;

  counters.retransmitted.header_bytes = 40;
  counters.retransmitted.packets = 2;
  counters.retransmitted.padding_bytes = 100;
  counters.retransmitted.payload_bytes = 200;

  counters.fec = counters.retransmitted;

  rtx_counters.transmitted = counters.transmitted;

  fake_clock_.AdvanceTimeMilliseconds(1000 * metrics::kMinRunTimeInSeconds);
  proxy->DataCountersUpdated(counters, kFirstSsrc);
  proxy->DataCountersUpdated(counters, kSecondSsrc);
  proxy->DataCountersUpdated(rtx_counters, kFirstRtxSsrc);
  proxy->DataCountersUpdated(rtx_counters, kSecondRtxSsrc);

  // Changing content type causes histograms to be reported.
  statistics_proxy_->SetContentType(VideoEncoderConfig::ContentType::kScreen);

  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.BitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.transmitted.TotalBytes() * 4 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample("WebRTC.Video.BitrateSentInKbps"));

  EXPECT_EQ(1,
            test::NumHistogramSamples("WebRTC.Video.MediaBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.MediaPayloadBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample("WebRTC.Video.MediaBitrateSentInKbps"));

  EXPECT_EQ(1,
            test::NumHistogramSamples("WebRTC.Video.PaddingBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.transmitted.padding_bytes * 4 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample("WebRTC.Video.PaddingBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.RetransmittedBitrateSentInKbps"));
  EXPECT_EQ(
      static_cast<int>((counters.retransmitted.TotalBytes() * 2 * 8) /
                       metrics::kMinRunTimeInSeconds / 1000),
      test::LastHistogramSample("WebRTC.Video.RetransmittedBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.RtxBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((rtx_counters.transmitted.TotalBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample("WebRTC.Video.RtxBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples("WebRTC.Video.FecBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((rtx_counters.fec.TotalBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample("WebRTC.Video.FecBitrateSentInKbps"));

  // New start time but same counter values.
  proxy->DataCountersUpdated(counters, kFirstSsrc);
  proxy->DataCountersUpdated(counters, kSecondSsrc);
  proxy->DataCountersUpdated(rtx_counters, kFirstRtxSsrc);
  proxy->DataCountersUpdated(rtx_counters, kSecondRtxSsrc);

  // Double counter values, this should result in the same counts as before but
  // with new histogram names.
  StreamDataCounters new_counters = counters;
  new_counters.Add(counters);
  StreamDataCounters new_rtx_counters = rtx_counters;
  new_rtx_counters.Add(rtx_counters);

  fake_clock_.AdvanceTimeMilliseconds(1000 * metrics::kMinRunTimeInSeconds);
  proxy->DataCountersUpdated(new_counters, kFirstSsrc);
  proxy->DataCountersUpdated(new_counters, kSecondSsrc);
  proxy->DataCountersUpdated(new_rtx_counters, kFirstRtxSsrc);
  proxy->DataCountersUpdated(new_rtx_counters, kSecondRtxSsrc);

  SetUp();  // Reset stats proxy also causes histograms to be reported.

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.BitrateSentInKbps"));
  EXPECT_EQ(
      static_cast<int>((counters.transmitted.TotalBytes() * 4 * 8) /
                       metrics::kMinRunTimeInSeconds / 1000),
      test::LastHistogramSample("WebRTC.Video.Screenshare.BitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.MediaBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.MediaPayloadBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.MediaBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.PaddingBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.transmitted.padding_bytes * 4 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.PaddingBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.RetransmittedBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((counters.retransmitted.TotalBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.RetransmittedBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.RtxBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((rtx_counters.transmitted.TotalBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.RtxBitrateSentInKbps"));

  EXPECT_EQ(1, test::NumHistogramSamples(
                   "WebRTC.Video.Screenshare.FecBitrateSentInKbps"));
  EXPECT_EQ(static_cast<int>((rtx_counters.fec.TotalBytes() * 2 * 8) /
                             metrics::kMinRunTimeInSeconds / 1000),
            test::LastHistogramSample(
                "WebRTC.Video.Screenshare.FecBitrateSentInKbps"));
}

}  // namespace webrtc
