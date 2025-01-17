/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/video/overuse_frame_detector.h"

#include <assert.h>
#include <math.h>

#include <algorithm>
#include <list>
#include <map>

#include "webrtc/base/checks.h"
#include "webrtc/base/exp_filter.h"
#include "webrtc/base/logging.h"
#include "webrtc/frame_callback.h"
#include "webrtc/system_wrappers/include/clock.h"
#include "webrtc/video_frame.h"

#if defined(WEBRTC_MAC)
#include <mach/mach.h>
#endif

namespace webrtc {

namespace {
const int64_t kProcessIntervalMs = 5000;

// Delay between consecutive rampups. (Used for quick recovery.)
const int kQuickRampUpDelayMs = 10 * 1000;
// Delay between rampup attempts. Initially uses standard, scales up to max.
const int kStandardRampUpDelayMs = 40 * 1000;
const int kMaxRampUpDelayMs = 240 * 1000;
// Expontential back-off factor, to prevent annoying up-down behaviour.
const double kRampUpBackoffFactor = 2.0;

// Max number of overuses detected before always applying the rampup delay.
const int kMaxOverusesBeforeApplyRampupDelay = 4;

// The maximum exponent to use in VCMExpFilter.
const float kSampleDiffMs = 33.0f;
const float kMaxExp = 7.0f;

}  // namespace

CpuOveruseOptions::CpuOveruseOptions()
    : high_encode_usage_threshold_percent(85),
      frame_timeout_interval_ms(1500),
      min_frame_samples(120),
      min_process_count(3),
      high_threshold_consecutive_count(2) {
#if defined(WEBRTC_MAC)
  // This is proof-of-concept code for letting the physical core count affect
  // the interval into which we attempt to scale. For now, the code is Mac OS
  // specific, since that's the platform were we saw most problems.
  // TODO(torbjorng): Enhance SystemInfo to return this metric.

  mach_port_t mach_host = mach_host_self();
  host_basic_info hbi = {};
  mach_msg_type_number_t info_count = HOST_BASIC_INFO_COUNT;
  kern_return_t kr =
      host_info(mach_host, HOST_BASIC_INFO, reinterpret_cast<host_info_t>(&hbi),
                &info_count);
  mach_port_deallocate(mach_task_self(), mach_host);

  int n_physical_cores;
  if (kr != KERN_SUCCESS) {
    // If we couldn't get # of physical CPUs, don't panic. Assume we have 1.
    n_physical_cores = 1;
    LOG(LS_ERROR) << "Failed to determine number of physical cores, assuming 1";
  } else {
    n_physical_cores = hbi.physical_cpu;
    LOG(LS_INFO) << "Number of physical cores:" << n_physical_cores;
  }

  // Change init list default for few core systems. The assumption here is that
  // encoding, which we measure here, takes about 1/4 of the processing of a
  // two-way call. This is roughly true for x86 using both vp8 and vp9 without
  // hardware encoding. Since we don't affect the incoming stream here, we only
  // control about 1/2 of the total processing needs, but this is not taken into
  // account.
  if (n_physical_cores == 1)
    high_encode_usage_threshold_percent = 20;  // Roughly 1/4 of 100%.
  else if (n_physical_cores == 2)
    high_encode_usage_threshold_percent = 40;  // Roughly 1/4 of 200%.

#endif  // WEBRTC_MAC
  // Note that we make the interval 2x+epsilon wide, since libyuv scaling steps
  // are close to that (when squared). This wide interval makes sure that
  // scaling up or down does not jump all the way across the interval.
  low_encode_usage_threshold_percent =
      (high_encode_usage_threshold_percent - 1) / 2;
}

// Class for calculating the processing usage on the send-side (the average
// processing time of a frame divided by the average time difference between
// captured frames).
class OveruseFrameDetector::SendProcessingUsage {
 public:
  explicit SendProcessingUsage(const CpuOveruseOptions& options)
      : kWeightFactorFrameDiff(0.998f),
        kWeightFactorProcessing(0.995f),
        kInitialSampleDiffMs(40.0f),
        kMaxSampleDiffMs(45.0f),
        count_(0),
        options_(options),
        filtered_processing_ms_(new rtc::ExpFilter(kWeightFactorProcessing)),
        filtered_frame_diff_ms_(new rtc::ExpFilter(kWeightFactorFrameDiff)) {
    Reset();
  }
  ~SendProcessingUsage() {}

  void Reset() {
    count_ = 0;
    filtered_frame_diff_ms_->Reset(kWeightFactorFrameDiff);
    filtered_frame_diff_ms_->Apply(1.0f, kInitialSampleDiffMs);
    filtered_processing_ms_->Reset(kWeightFactorProcessing);
    filtered_processing_ms_->Apply(1.0f, InitialProcessingMs());
  }

  void AddCaptureSample(float sample_ms) {
    float exp = sample_ms / kSampleDiffMs;
    exp = std::min(exp, kMaxExp);
    filtered_frame_diff_ms_->Apply(exp, sample_ms);
  }

  void AddSample(float processing_ms, int64_t diff_last_sample_ms) {
    ++count_;
    float exp = diff_last_sample_ms / kSampleDiffMs;
    exp = std::min(exp, kMaxExp);
    filtered_processing_ms_->Apply(exp, processing_ms);
  }

  int Value() const {
    if (count_ < static_cast<uint32_t>(options_.min_frame_samples)) {
      return static_cast<int>(InitialUsageInPercent() + 0.5f);
    }
    float frame_diff_ms = std::max(filtered_frame_diff_ms_->filtered(), 1.0f);
    frame_diff_ms = std::min(frame_diff_ms, kMaxSampleDiffMs);
    float encode_usage_percent =
        100.0f * filtered_processing_ms_->filtered() / frame_diff_ms;
    return static_cast<int>(encode_usage_percent + 0.5);
  }

 private:
  float InitialUsageInPercent() const {
    // Start in between the underuse and overuse threshold.
    return (options_.low_encode_usage_threshold_percent +
            options_.high_encode_usage_threshold_percent) / 2.0f;
  }

  float InitialProcessingMs() const {
    return InitialUsageInPercent() * kInitialSampleDiffMs / 100;
  }

  const float kWeightFactorFrameDiff;
  const float kWeightFactorProcessing;
  const float kInitialSampleDiffMs;
  const float kMaxSampleDiffMs;
  uint64_t count_;
  const CpuOveruseOptions options_;
  std::unique_ptr<rtc::ExpFilter> filtered_processing_ms_;
  std::unique_ptr<rtc::ExpFilter> filtered_frame_diff_ms_;
};

OveruseFrameDetector::OveruseFrameDetector(
    Clock* clock,
    const CpuOveruseOptions& options,
    CpuOveruseObserver* observer,
    EncodedFrameObserver* encoder_timing,
    CpuOveruseMetricsObserver* metrics_observer)
    : options_(options),
      observer_(observer),
      encoder_timing_(encoder_timing),
      metrics_observer_(metrics_observer),
      clock_(clock),
      num_process_times_(0),
      last_capture_time_ms_(-1),
      last_processed_capture_time_ms_(-1),
      num_pixels_(0),
      next_process_time_ms_(clock_->TimeInMilliseconds()),
      last_overuse_time_ms_(-1),
      checks_above_threshold_(0),
      num_overuse_detections_(0),
      last_rampup_time_ms_(-1),
      in_quick_rampup_(false),
      current_rampup_delay_ms_(kStandardRampUpDelayMs),
      usage_(new SendProcessingUsage(options)) {
  RTC_DCHECK(metrics_observer);
  processing_thread_.DetachFromThread();
}

OveruseFrameDetector::~OveruseFrameDetector() {
}

void OveruseFrameDetector::EncodedFrameTimeMeasured(int encode_duration_ms) {
  if (!metrics_)
    metrics_ = rtc::Optional<CpuOveruseMetrics>(CpuOveruseMetrics());
  metrics_->encode_usage_percent = usage_->Value();

  metrics_observer_->OnEncodedFrameTimeMeasured(encode_duration_ms, *metrics_);
}

int64_t OveruseFrameDetector::TimeUntilNextProcess() {
  RTC_DCHECK(processing_thread_.CalledOnValidThread());
  return next_process_time_ms_ - clock_->TimeInMilliseconds();
}

bool OveruseFrameDetector::FrameSizeChanged(int num_pixels) const {
  if (num_pixels != num_pixels_) {
    return true;
  }
  return false;
}

bool OveruseFrameDetector::FrameTimeoutDetected(int64_t now) const {
  if (last_capture_time_ms_ == -1)
    return false;
  return (now - last_capture_time_ms_) > options_.frame_timeout_interval_ms;
}

void OveruseFrameDetector::ResetAll(int num_pixels) {
  num_pixels_ = num_pixels;
  usage_->Reset();
  frame_timing_.clear();
  last_capture_time_ms_ = -1;
  last_processed_capture_time_ms_ = -1;
  num_process_times_ = 0;
  metrics_ = rtc::Optional<CpuOveruseMetrics>();
}

void OveruseFrameDetector::FrameCaptured(const VideoFrame& frame) {
  rtc::CritScope cs(&crit_);

  int64_t now = clock_->TimeInMilliseconds();
  if (FrameSizeChanged(frame.width() * frame.height()) ||
      FrameTimeoutDetected(now)) {
    ResetAll(frame.width() * frame.height());
  }

  if (last_capture_time_ms_ != -1)
    usage_->AddCaptureSample(now - last_capture_time_ms_);

  last_capture_time_ms_ = now;

  frame_timing_.push_back(
      FrameTiming(frame.ntp_time_ms(), frame.timestamp(), now));
}

void OveruseFrameDetector::FrameSent(uint32_t timestamp) {
  rtc::CritScope cs(&crit_);
  // Delay before reporting actual encoding time, used to have the ability to
  // detect total encoding time when encoding more than one layer. Encoding is
  // here assumed to finish within a second (or that we get enough long-time
  // samples before one second to trigger an overuse even when this is not the
  // case).
  static const int64_t kEncodingTimeMeasureWindowMs = 1000;
  int64_t now = clock_->TimeInMilliseconds();
  for (auto& it : frame_timing_) {
    if (it.timestamp == timestamp) {
      it.last_send_ms = now;
      break;
    }
  }
  // TODO(pbos): Handle the case/log errors when not finding the corresponding
  // frame (either very slow encoding or incorrect wrong timestamps returned
  // from the encoder).
  // This is currently the case for all frames on ChromeOS, so logging them
  // would be spammy, and triggering overuse would be wrong.
  // https://crbug.com/350106
  while (!frame_timing_.empty()) {
    FrameTiming timing = frame_timing_.front();
    if (now - timing.capture_ms < kEncodingTimeMeasureWindowMs)
      break;
    if (timing.last_send_ms != -1) {
      int encode_duration_ms =
          static_cast<int>(timing.last_send_ms - timing.capture_ms);
      if (encoder_timing_) {
        encoder_timing_->OnEncodeTiming(timing.capture_ntp_ms,
                                        encode_duration_ms);
      }
      if (last_processed_capture_time_ms_ != -1) {
        int64_t diff_ms = timing.capture_ms - last_processed_capture_time_ms_;
        usage_->AddSample(encode_duration_ms, diff_ms);
      }
      last_processed_capture_time_ms_ = timing.capture_ms;
      EncodedFrameTimeMeasured(encode_duration_ms);
    }
    frame_timing_.pop_front();
  }
}

void OveruseFrameDetector::Process() {
  RTC_DCHECK(processing_thread_.CalledOnValidThread());

  int64_t now = clock_->TimeInMilliseconds();

  // Used to protect against Process() being called too often.
  if (now < next_process_time_ms_)
    return;

  next_process_time_ms_ = now + kProcessIntervalMs;

  CpuOveruseMetrics current_metrics;
  {
    rtc::CritScope cs(&crit_);
    ++num_process_times_;
    if (num_process_times_ <= options_.min_process_count || !metrics_)
      return;

    current_metrics = *metrics_;
  }

  if (IsOverusing(current_metrics)) {
    // If the last thing we did was going up, and now have to back down, we need
    // to check if this peak was short. If so we should back off to avoid going
    // back and forth between this load, the system doesn't seem to handle it.
    bool check_for_backoff = last_rampup_time_ms_ > last_overuse_time_ms_;
    if (check_for_backoff) {
      if (now - last_rampup_time_ms_ < kStandardRampUpDelayMs ||
          num_overuse_detections_ > kMaxOverusesBeforeApplyRampupDelay) {
        // Going up was not ok for very long, back off.
        current_rampup_delay_ms_ *= kRampUpBackoffFactor;
        if (current_rampup_delay_ms_ > kMaxRampUpDelayMs)
          current_rampup_delay_ms_ = kMaxRampUpDelayMs;
      } else {
        // Not currently backing off, reset rampup delay.
        current_rampup_delay_ms_ = kStandardRampUpDelayMs;
      }
    }

    last_overuse_time_ms_ = now;
    in_quick_rampup_ = false;
    checks_above_threshold_ = 0;
    ++num_overuse_detections_;

    if (observer_)
      observer_->OveruseDetected();
  } else if (IsUnderusing(current_metrics, now)) {
    last_rampup_time_ms_ = now;
    in_quick_rampup_ = true;

    if (observer_)
      observer_->NormalUsage();
  }

  int rampup_delay =
      in_quick_rampup_ ? kQuickRampUpDelayMs : current_rampup_delay_ms_;

  LOG(LS_VERBOSE) << " Frame stats: "
                  << " encode usage " << current_metrics.encode_usage_percent
                  << " overuse detections " << num_overuse_detections_
                  << " rampup delay " << rampup_delay;
}

bool OveruseFrameDetector::IsOverusing(const CpuOveruseMetrics& metrics) {
  if (metrics.encode_usage_percent >=
      options_.high_encode_usage_threshold_percent) {
    ++checks_above_threshold_;
  } else {
    checks_above_threshold_ = 0;
  }
  return checks_above_threshold_ >= options_.high_threshold_consecutive_count;
}

bool OveruseFrameDetector::IsUnderusing(const CpuOveruseMetrics& metrics,
                                        int64_t time_now) {
  int delay = in_quick_rampup_ ? kQuickRampUpDelayMs : current_rampup_delay_ms_;
  if (time_now < last_rampup_time_ms_ + delay)
    return false;

  return metrics.encode_usage_percent <
         options_.low_encode_usage_threshold_percent;
}
}  // namespace webrtc
