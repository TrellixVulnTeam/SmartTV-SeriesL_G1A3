/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/modules/audio_coding/acm2/audio_coding_module_impl.h"

#include <assert.h>
#include <stdlib.h>
#include <vector>

#include "webrtc/base/checks.h"
#include "webrtc/base/safe_conversions.h"
#include "webrtc/engine_configurations.h"
#include "webrtc/modules/audio_coding/include/audio_coding_module_typedefs.h"
#include "webrtc/modules/audio_coding/acm2/acm_common_defs.h"
#include "webrtc/modules/audio_coding/acm2/acm_resampler.h"
#include "webrtc/modules/audio_coding/acm2/call_statistics.h"
#include "webrtc/system_wrappers/include/logging.h"
#include "webrtc/system_wrappers/include/metrics.h"
#include "webrtc/system_wrappers/include/rw_lock_wrapper.h"
#include "webrtc/system_wrappers/include/trace.h"
#include "webrtc/typedefs.h"

namespace webrtc {

namespace acm2 {

struct EncoderFactory {
  AudioEncoder* external_speech_encoder = nullptr;
  CodecManager codec_manager;
  RentACodec rent_a_codec;
};

namespace {

// TODO(turajs): the same functionality is used in NetEq. If both classes
// need them, make it a static function in ACMCodecDB.
bool IsCodecRED(const CodecInst& codec) {
  return (STR_CASE_CMP(codec.plname, "RED") == 0);
}

bool IsCodecCN(const CodecInst& codec) {
  return (STR_CASE_CMP(codec.plname, "CN") == 0);
}

// Stereo-to-mono can be used as in-place.
int DownMix(const AudioFrame& frame,
            size_t length_out_buff,
            int16_t* out_buff) {
  if (length_out_buff < frame.samples_per_channel_) {
    return -1;
  }
  for (size_t n = 0; n < frame.samples_per_channel_; ++n)
    out_buff[n] = (frame.data_[2 * n] + frame.data_[2 * n + 1]) >> 1;
  return 0;
}

// Mono-to-stereo can be used as in-place.
int UpMix(const AudioFrame& frame, size_t length_out_buff, int16_t* out_buff) {
  if (length_out_buff < frame.samples_per_channel_) {
    return -1;
  }
  for (size_t n = frame.samples_per_channel_; n != 0; --n) {
    size_t i = n - 1;
    int16_t sample = frame.data_[i];
    out_buff[2 * i + 1] = sample;
    out_buff[2 * i] = sample;
  }
  return 0;
}

void ConvertEncodedInfoToFragmentationHeader(
    const AudioEncoder::EncodedInfo& info,
    RTPFragmentationHeader* frag) {
  if (info.redundant.empty()) {
    frag->fragmentationVectorSize = 0;
    return;
  }

  frag->VerifyAndAllocateFragmentationHeader(
      static_cast<uint16_t>(info.redundant.size()));
  frag->fragmentationVectorSize = static_cast<uint16_t>(info.redundant.size());
  size_t offset = 0;
  for (size_t i = 0; i < info.redundant.size(); ++i) {
    frag->fragmentationOffset[i] = offset;
    offset += info.redundant[i].encoded_bytes;
    frag->fragmentationLength[i] = info.redundant[i].encoded_bytes;
    frag->fragmentationTimeDiff[i] = rtc::checked_cast<uint16_t>(
        info.encoded_timestamp - info.redundant[i].encoded_timestamp);
    frag->fragmentationPlType[i] = info.redundant[i].payload_type;
  }
}

// Wraps a raw AudioEncoder pointer. The idea is that you can put one of these
// in a unique_ptr, to protect the contained raw pointer from being deleted
// when the unique_ptr expires. (This is of course a bad idea in general, but
// backwards compatibility.)
class RawAudioEncoderWrapper final : public AudioEncoder {
 public:
  RawAudioEncoderWrapper(AudioEncoder* enc) : enc_(enc) {}
  size_t MaxEncodedBytes() const override { return enc_->MaxEncodedBytes(); }
  int SampleRateHz() const override { return enc_->SampleRateHz(); }
  size_t NumChannels() const override { return enc_->NumChannels(); }
  int RtpTimestampRateHz() const override { return enc_->RtpTimestampRateHz(); }
  size_t Num10MsFramesInNextPacket() const override {
    return enc_->Num10MsFramesInNextPacket();
  }
  size_t Max10MsFramesInAPacket() const override {
    return enc_->Max10MsFramesInAPacket();
  }
  int GetTargetBitrate() const override { return enc_->GetTargetBitrate(); }
  EncodedInfo EncodeImpl(uint32_t rtp_timestamp,
                         rtc::ArrayView<const int16_t> audio,
                         rtc::Buffer* encoded) override {
    return enc_->Encode(rtp_timestamp, audio, encoded);
  }
  EncodedInfo EncodeInternal(uint32_t rtp_timestamp,
                             rtc::ArrayView<const int16_t> audio,
                             size_t max_encoded_bytes,
                             uint8_t* encoded) override {
    return enc_->EncodeInternal(rtp_timestamp, audio, max_encoded_bytes,
                                encoded);
  }
  void Reset() override { return enc_->Reset(); }
  bool SetFec(bool enable) override { return enc_->SetFec(enable); }
  bool SetDtx(bool enable) override { return enc_->SetDtx(enable); }
  bool SetApplication(Application application) override {
    return enc_->SetApplication(application);
  }
  void SetMaxPlaybackRate(int frequency_hz) override {
    return enc_->SetMaxPlaybackRate(frequency_hz);
  }
  void SetProjectedPacketLossRate(double fraction) override {
    return enc_->SetProjectedPacketLossRate(fraction);
  }
  void SetTargetBitrate(int target_bps) override {
    return enc_->SetTargetBitrate(target_bps);
  }

 private:
  AudioEncoder* enc_;
};

// Return false on error.
bool CreateSpeechEncoderIfNecessary(EncoderFactory* ef) {
  auto* sp = ef->codec_manager.GetStackParams();
  if (sp->speech_encoder) {
    // Do nothing; we already have a speech encoder.
  } else if (ef->codec_manager.GetCodecInst()) {
    RTC_DCHECK(!ef->external_speech_encoder);
    // We have no speech encoder, but we have a specification for making one.
    std::unique_ptr<AudioEncoder> enc =
        ef->rent_a_codec.RentEncoder(*ef->codec_manager.GetCodecInst());
    if (!enc)
      return false;  // Encoder spec was bad.
    sp->speech_encoder = std::move(enc);
  } else if (ef->external_speech_encoder) {
    RTC_DCHECK(!ef->codec_manager.GetCodecInst());
    // We have an external speech encoder.
    sp->speech_encoder = std::unique_ptr<AudioEncoder>(
        new RawAudioEncoderWrapper(ef->external_speech_encoder));
  }
  return true;
}

}  // namespace

void AudioCodingModuleImpl::ChangeLogger::MaybeLog(int value) {
  if (value != last_value_ || first_time_) {
    first_time_ = false;
    last_value_ = value;
    RTC_HISTOGRAM_COUNTS_SPARSE_100(histogram_name_, value);
  }
}

AudioCodingModuleImpl::AudioCodingModuleImpl(
    const AudioCodingModule::Config& config)
    : id_(config.id),
      expected_codec_ts_(0xD87F3F9F),
      expected_in_ts_(0xD87F3F9F),
      receiver_(config),
      bitrate_logger_("WebRTC.Audio.TargetBitrateInKbps"),
      encoder_factory_(new EncoderFactory),
      encoder_stack_(nullptr),
      previous_pltype_(255),
      receiver_initialized_(false),
      first_10ms_data_(false),
      first_frame_(true),
      packetization_callback_(NULL),
      vad_callback_(NULL) {
  if (InitializeReceiverSafe() < 0) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Cannot initialize receiver");
  }
  WEBRTC_TRACE(webrtc::kTraceMemory, webrtc::kTraceAudioCoding, id_, "Created");
}

AudioCodingModuleImpl::~AudioCodingModuleImpl() = default;

int32_t AudioCodingModuleImpl::Encode(const InputData& input_data) {
  AudioEncoder::EncodedInfo encoded_info;
  uint8_t previous_pltype;

  // Check if there is an encoder before.
  if (!HaveValidEncoder("Process"))
    return -1;

  // Scale the timestamp to the codec's RTP timestamp rate.
  uint32_t rtp_timestamp =
      first_frame_ ? input_data.input_timestamp
                   : last_rtp_timestamp_ +
                         rtc::CheckedDivExact(
                             input_data.input_timestamp - last_timestamp_,
                             static_cast<uint32_t>(rtc::CheckedDivExact(
                                 encoder_stack_->SampleRateHz(),
                                 encoder_stack_->RtpTimestampRateHz())));
  last_timestamp_ = input_data.input_timestamp;
  last_rtp_timestamp_ = rtp_timestamp;
  first_frame_ = false;

  // Clear the buffer before reuse - encoded data will get appended.
  encode_buffer_.Clear();
  encoded_info = encoder_stack_->Encode(
      rtp_timestamp, rtc::ArrayView<const int16_t>(
                         input_data.audio, input_data.audio_channel *
                                               input_data.length_per_channel),
      &encode_buffer_);

  bitrate_logger_.MaybeLog(encoder_stack_->GetTargetBitrate() / 1000);
  if (encode_buffer_.size() == 0 && !encoded_info.send_even_if_empty) {
    // Not enough data.
    return 0;
  }
  previous_pltype = previous_pltype_;  // Read it while we have the critsect.

  RTPFragmentationHeader my_fragmentation;
  ConvertEncodedInfoToFragmentationHeader(encoded_info, &my_fragmentation);
  FrameType frame_type;
  if (encode_buffer_.size() == 0 && encoded_info.send_even_if_empty) {
    frame_type = kEmptyFrame;
    encoded_info.payload_type = previous_pltype;
  } else {
    RTC_DCHECK_GT(encode_buffer_.size(), 0u);
    frame_type = encoded_info.speech ? kAudioFrameSpeech : kAudioFrameCN;
  }

  {
    rtc::CritScope lock(&callback_crit_sect_);
    if (packetization_callback_) {
      packetization_callback_->SendData(
          frame_type, encoded_info.payload_type, encoded_info.encoded_timestamp,
          encode_buffer_.data(), encode_buffer_.size(),
          my_fragmentation.fragmentationVectorSize > 0 ? &my_fragmentation
                                                       : nullptr);
    }

    if (vad_callback_) {
      // Callback with VAD decision.
      vad_callback_->InFrameType(frame_type);
    }
  }
  previous_pltype_ = encoded_info.payload_type;
  return static_cast<int32_t>(encode_buffer_.size());
}

/////////////////////////////////////////
//   Sender
//

// Can be called multiple times for Codec, CNG, RED.
int AudioCodingModuleImpl::RegisterSendCodec(const CodecInst& send_codec) {
  rtc::CritScope lock(&acm_crit_sect_);
  if (!encoder_factory_->codec_manager.RegisterEncoder(send_codec)) {
    return -1;
  }
  if (encoder_factory_->codec_manager.GetCodecInst()) {
    encoder_factory_->external_speech_encoder = nullptr;
  }
  if (!CreateSpeechEncoderIfNecessary(encoder_factory_.get())) {
    return -1;
  }
  auto* sp = encoder_factory_->codec_manager.GetStackParams();
  if (sp->speech_encoder)
    encoder_stack_ = encoder_factory_->rent_a_codec.RentEncoderStack(sp);
  return 0;
}

void AudioCodingModuleImpl::RegisterExternalSendCodec(
    AudioEncoder* external_speech_encoder) {
  rtc::CritScope lock(&acm_crit_sect_);
  encoder_factory_->codec_manager.UnsetCodecInst();
  encoder_factory_->external_speech_encoder = external_speech_encoder;
  RTC_CHECK(CreateSpeechEncoderIfNecessary(encoder_factory_.get()));
  auto* sp = encoder_factory_->codec_manager.GetStackParams();
  RTC_CHECK(sp->speech_encoder);
  encoder_stack_ = encoder_factory_->rent_a_codec.RentEncoderStack(sp);
}

void AudioCodingModuleImpl::ModifyEncoder(
    FunctionView<void(std::unique_ptr<AudioEncoder>*)> modifier) {
  rtc::CritScope lock(&acm_crit_sect_);

  // Wipe the encoder factory, so that everything that relies on it will fail.
  // We don't want the complexity of supporting swapping back and forth.
  if (encoder_factory_) {
    encoder_factory_.reset();
    RTC_CHECK(!encoder_stack_);  // Ensure we hadn't started using the factory.
  }

  modifier(&encoder_stack_);
}

// Get current send codec.
rtc::Optional<CodecInst> AudioCodingModuleImpl::SendCodec() const {
  rtc::CritScope lock(&acm_crit_sect_);
  if (encoder_factory_) {
    auto* ci = encoder_factory_->codec_manager.GetCodecInst();
    if (ci) {
      return rtc::Optional<CodecInst>(*ci);
    }
    CreateSpeechEncoderIfNecessary(encoder_factory_.get());
    const std::unique_ptr<AudioEncoder>& enc =
        encoder_factory_->codec_manager.GetStackParams()->speech_encoder;
    if (enc) {
      return rtc::Optional<CodecInst>(CodecManager::ForgeCodecInst(enc.get()));
    }
    return rtc::Optional<CodecInst>();
  } else {
    return encoder_stack_
               ? rtc::Optional<CodecInst>(
                     CodecManager::ForgeCodecInst(encoder_stack_.get()))
               : rtc::Optional<CodecInst>();
  }
}

// Get current send frequency.
int AudioCodingModuleImpl::SendFrequency() const {
  WEBRTC_TRACE(webrtc::kTraceStream, webrtc::kTraceAudioCoding, id_,
               "SendFrequency()");
  rtc::CritScope lock(&acm_crit_sect_);

  if (!encoder_stack_) {
    WEBRTC_TRACE(webrtc::kTraceStream, webrtc::kTraceAudioCoding, id_,
                 "SendFrequency Failed, no codec is registered");
    return -1;
  }

  return encoder_stack_->SampleRateHz();
}

void AudioCodingModuleImpl::SetBitRate(int bitrate_bps) {
  rtc::CritScope lock(&acm_crit_sect_);
  if (encoder_stack_) {
    encoder_stack_->SetTargetBitrate(bitrate_bps);
  }
}

// Register a transport callback which will be called to deliver
// the encoded buffers.
int AudioCodingModuleImpl::RegisterTransportCallback(
    AudioPacketizationCallback* transport) {
  rtc::CritScope lock(&callback_crit_sect_);
  packetization_callback_ = transport;
  return 0;
}

// Add 10MS of raw (PCM) audio data to the encoder.
int AudioCodingModuleImpl::Add10MsData(const AudioFrame& audio_frame) {
  InputData input_data;
  rtc::CritScope lock(&acm_crit_sect_);
  int r = Add10MsDataInternal(audio_frame, &input_data);
  return r < 0 ? r : Encode(input_data);
}

int AudioCodingModuleImpl::Add10MsDataInternal(const AudioFrame& audio_frame,
                                               InputData* input_data) {
  if (audio_frame.samples_per_channel_ == 0) {
    assert(false);
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Cannot Add 10 ms audio, payload length is zero");
    return -1;
  }

  if (audio_frame.sample_rate_hz_ > 48000) {
    assert(false);
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Cannot Add 10 ms audio, input frequency not valid");
    return -1;
  }

  // If the length and frequency matches. We currently just support raw PCM.
  if (static_cast<size_t>(audio_frame.sample_rate_hz_ / 100) !=
      audio_frame.samples_per_channel_) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Cannot Add 10 ms audio, input frequency and length doesn't"
                 " match");
    return -1;
  }

  if (audio_frame.num_channels_ != 1 && audio_frame.num_channels_ != 2) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Cannot Add 10 ms audio, invalid number of channels.");
    return -1;
  }

  // Do we have a codec registered?
  if (!HaveValidEncoder("Add10MsData")) {
    return -1;
  }

  const AudioFrame* ptr_frame;
  // Perform a resampling, also down-mix if it is required and can be
  // performed before resampling (a down mix prior to resampling will take
  // place if both primary and secondary encoders are mono and input is in
  // stereo).
  if (PreprocessToAddData(audio_frame, &ptr_frame) < 0) {
    return -1;
  }

  // Check whether we need an up-mix or down-mix?
  const size_t current_num_channels = encoder_stack_->NumChannels();
  const bool same_num_channels =
      ptr_frame->num_channels_ == current_num_channels;

  if (!same_num_channels) {
    if (ptr_frame->num_channels_ == 1) {
      if (UpMix(*ptr_frame, WEBRTC_10MS_PCM_AUDIO, input_data->buffer) < 0)
        return -1;
    } else {
      if (DownMix(*ptr_frame, WEBRTC_10MS_PCM_AUDIO, input_data->buffer) < 0)
        return -1;
    }
  }

  // When adding data to encoders this pointer is pointing to an audio buffer
  // with correct number of channels.
  const int16_t* ptr_audio = ptr_frame->data_;

  // For pushing data to primary, point the |ptr_audio| to correct buffer.
  if (!same_num_channels)
    ptr_audio = input_data->buffer;

  input_data->input_timestamp = ptr_frame->timestamp_;
  input_data->audio = ptr_audio;
  input_data->length_per_channel = ptr_frame->samples_per_channel_;
  input_data->audio_channel = current_num_channels;

  return 0;
}

// Perform a resampling and down-mix if required. We down-mix only if
// encoder is mono and input is stereo. In case of dual-streaming, both
// encoders has to be mono for down-mix to take place.
// |*ptr_out| will point to the pre-processed audio-frame. If no pre-processing
// is required, |*ptr_out| points to |in_frame|.
int AudioCodingModuleImpl::PreprocessToAddData(const AudioFrame& in_frame,
                                               const AudioFrame** ptr_out) {
  const bool resample =
      in_frame.sample_rate_hz_ != encoder_stack_->SampleRateHz();

  // This variable is true if primary codec and secondary codec (if exists)
  // are both mono and input is stereo.
  // TODO(henrik.lundin): This condition should probably be
  //   in_frame.num_channels_ > encoder_stack_->NumChannels()
  const bool down_mix =
      in_frame.num_channels_ == 2 && encoder_stack_->NumChannels() == 1;

  if (!first_10ms_data_) {
    expected_in_ts_ = in_frame.timestamp_;
    expected_codec_ts_ = in_frame.timestamp_;
    first_10ms_data_ = true;
  } else if (in_frame.timestamp_ != expected_in_ts_) {
    // TODO(turajs): Do we need a warning here.
    expected_codec_ts_ +=
        (in_frame.timestamp_ - expected_in_ts_) *
        static_cast<uint32_t>(
            static_cast<double>(encoder_stack_->SampleRateHz()) /
            static_cast<double>(in_frame.sample_rate_hz_));
    expected_in_ts_ = in_frame.timestamp_;
  }


  if (!down_mix && !resample) {
    // No pre-processing is required.
    expected_in_ts_ += static_cast<uint32_t>(in_frame.samples_per_channel_);
    expected_codec_ts_ += static_cast<uint32_t>(in_frame.samples_per_channel_);
    *ptr_out = &in_frame;
    return 0;
  }

  *ptr_out = &preprocess_frame_;
  preprocess_frame_.num_channels_ = in_frame.num_channels_;
  int16_t audio[WEBRTC_10MS_PCM_AUDIO];
  const int16_t* src_ptr_audio = in_frame.data_;
  int16_t* dest_ptr_audio = preprocess_frame_.data_;
  if (down_mix) {
    // If a resampling is required the output of a down-mix is written into a
    // local buffer, otherwise, it will be written to the output frame.
    if (resample)
      dest_ptr_audio = audio;
    if (DownMix(in_frame, WEBRTC_10MS_PCM_AUDIO, dest_ptr_audio) < 0)
      return -1;
    preprocess_frame_.num_channels_ = 1;
    // Set the input of the resampler is the down-mixed signal.
    src_ptr_audio = audio;
  }

  preprocess_frame_.timestamp_ = expected_codec_ts_;
  preprocess_frame_.samples_per_channel_ = in_frame.samples_per_channel_;
  preprocess_frame_.sample_rate_hz_ = in_frame.sample_rate_hz_;
  // If it is required, we have to do a resampling.
  if (resample) {
    // The result of the resampler is written to output frame.
    dest_ptr_audio = preprocess_frame_.data_;

    int samples_per_channel = resampler_.Resample10Msec(
        src_ptr_audio, in_frame.sample_rate_hz_, encoder_stack_->SampleRateHz(),
        preprocess_frame_.num_channels_, AudioFrame::kMaxDataSizeSamples,
        dest_ptr_audio);

    if (samples_per_channel < 0) {
      WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                   "Cannot add 10 ms audio, resampling failed");
      return -1;
    }
    preprocess_frame_.samples_per_channel_ =
        static_cast<size_t>(samples_per_channel);
    preprocess_frame_.sample_rate_hz_ = encoder_stack_->SampleRateHz();
  }

  expected_codec_ts_ +=
      static_cast<uint32_t>(preprocess_frame_.samples_per_channel_);
  expected_in_ts_ += static_cast<uint32_t>(in_frame.samples_per_channel_);

  return 0;
}

/////////////////////////////////////////
//   (RED) Redundant Coding
//

bool AudioCodingModuleImpl::REDStatus() const {
  rtc::CritScope lock(&acm_crit_sect_);
  return encoder_factory_->codec_manager.GetStackParams()->use_red;
}

// Configure RED status i.e on/off.
int AudioCodingModuleImpl::SetREDStatus(bool enable_red) {
#ifdef WEBRTC_CODEC_RED
  rtc::CritScope lock(&acm_crit_sect_);
  CreateSpeechEncoderIfNecessary(encoder_factory_.get());
  if (!encoder_factory_->codec_manager.SetCopyRed(enable_red)) {
    return -1;
  }
  auto* sp = encoder_factory_->codec_manager.GetStackParams();
  if (sp->speech_encoder)
    encoder_stack_ = encoder_factory_->rent_a_codec.RentEncoderStack(sp);
  return 0;
#else
  WEBRTC_TRACE(webrtc::kTraceWarning, webrtc::kTraceAudioCoding, id_,
               "  WEBRTC_CODEC_RED is undefined");
  return -1;
#endif
}

/////////////////////////////////////////
//   (FEC) Forward Error Correction (codec internal)
//

bool AudioCodingModuleImpl::CodecFEC() const {
  rtc::CritScope lock(&acm_crit_sect_);
  return encoder_factory_->codec_manager.GetStackParams()->use_codec_fec;
}

int AudioCodingModuleImpl::SetCodecFEC(bool enable_codec_fec) {
  rtc::CritScope lock(&acm_crit_sect_);
  CreateSpeechEncoderIfNecessary(encoder_factory_.get());
  if (!encoder_factory_->codec_manager.SetCodecFEC(enable_codec_fec)) {
    return -1;
  }
  auto* sp = encoder_factory_->codec_manager.GetStackParams();
  if (sp->speech_encoder)
    encoder_stack_ = encoder_factory_->rent_a_codec.RentEncoderStack(sp);
  if (enable_codec_fec) {
    return sp->use_codec_fec ? 0 : -1;
  } else {
    RTC_DCHECK(!sp->use_codec_fec);
    return 0;
  }
}

int AudioCodingModuleImpl::SetPacketLossRate(int loss_rate) {
  rtc::CritScope lock(&acm_crit_sect_);
  if (HaveValidEncoder("SetPacketLossRate")) {
    encoder_stack_->SetProjectedPacketLossRate(loss_rate / 100.0);
  }
  return 0;
}

/////////////////////////////////////////
//   (VAD) Voice Activity Detection
//
int AudioCodingModuleImpl::SetVAD(bool enable_dtx,
                                  bool enable_vad,
                                  ACMVADMode mode) {
  // Note: |enable_vad| is not used; VAD is enabled based on the DTX setting.
  RTC_DCHECK_EQ(enable_dtx, enable_vad);
  rtc::CritScope lock(&acm_crit_sect_);
  CreateSpeechEncoderIfNecessary(encoder_factory_.get());
  if (!encoder_factory_->codec_manager.SetVAD(enable_dtx, mode)) {
    return -1;
  }
  auto* sp = encoder_factory_->codec_manager.GetStackParams();
  if (sp->speech_encoder)
    encoder_stack_ = encoder_factory_->rent_a_codec.RentEncoderStack(sp);
  return 0;
}

// Get VAD/DTX settings.
int AudioCodingModuleImpl::VAD(bool* dtx_enabled, bool* vad_enabled,
                               ACMVADMode* mode) const {
  rtc::CritScope lock(&acm_crit_sect_);
  const auto* sp = encoder_factory_->codec_manager.GetStackParams();
  *dtx_enabled = *vad_enabled = sp->use_cng;
  *mode = sp->vad_mode;
  return 0;
}

/////////////////////////////////////////
//   Receiver
//

int AudioCodingModuleImpl::InitializeReceiver() {
  rtc::CritScope lock(&acm_crit_sect_);
  return InitializeReceiverSafe();
}

// Initialize receiver, resets codec database etc.
int AudioCodingModuleImpl::InitializeReceiverSafe() {
  // If the receiver is already initialized then we want to destroy any
  // existing decoders. After a call to this function, we should have a clean
  // start-up.
  if (receiver_initialized_) {
    if (receiver_.RemoveAllCodecs() < 0)
      return -1;
  }
  receiver_.ResetInitialDelay();
  receiver_.SetMinimumDelay(0);
  receiver_.SetMaximumDelay(0);
  receiver_.FlushBuffers();

  // Register RED and CN.
  auto db = RentACodec::Database();
  for (size_t i = 0; i < db.size(); i++) {
    if (IsCodecRED(db[i]) || IsCodecCN(db[i])) {
      if (receiver_.AddCodec(static_cast<int>(i),
                             static_cast<uint8_t>(db[i].pltype), 1,
                             db[i].plfreq, nullptr, db[i].plname) < 0) {
        WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                     "Cannot register master codec.");
        return -1;
      }
    }
  }
  receiver_initialized_ = true;
  return 0;
}

// Get current receive frequency.
int AudioCodingModuleImpl::ReceiveFrequency() const {
  const auto last_packet_sample_rate = receiver_.last_packet_sample_rate_hz();
  return last_packet_sample_rate ? *last_packet_sample_rate
                                 : receiver_.last_output_sample_rate_hz();
}

// Get current playout frequency.
int AudioCodingModuleImpl::PlayoutFrequency() const {
  WEBRTC_TRACE(webrtc::kTraceStream, webrtc::kTraceAudioCoding, id_,
               "PlayoutFrequency()");
  return receiver_.last_output_sample_rate_hz();
}

int AudioCodingModuleImpl::RegisterReceiveCodec(const CodecInst& codec) {
  rtc::CritScope lock(&acm_crit_sect_);
  auto* ef = encoder_factory_.get();
  return RegisterReceiveCodecUnlocked(
      codec, [ef] { return ef->rent_a_codec.RentIsacDecoder(); });
}

int AudioCodingModuleImpl::RegisterReceiveCodec(
    const CodecInst& codec,
    FunctionView<std::unique_ptr<AudioDecoder>()> isac_factory) {
  rtc::CritScope lock(&acm_crit_sect_);
  return RegisterReceiveCodecUnlocked(codec, isac_factory);
}

int AudioCodingModuleImpl::RegisterReceiveCodecUnlocked(
    const CodecInst& codec,
    FunctionView<std::unique_ptr<AudioDecoder>()> isac_factory) {
  RTC_DCHECK(receiver_initialized_);
  if (codec.channels > 2) {
    LOG_F(LS_ERROR) << "Unsupported number of channels: " << codec.channels;
    return -1;
  }

  auto codec_id =
      RentACodec::CodecIdByParams(codec.plname, codec.plfreq, codec.channels);
  if (!codec_id) {
    LOG_F(LS_ERROR) << "Wrong codec params to be registered as receive codec";
    return -1;
  }
  auto codec_index = RentACodec::CodecIndexFromId(*codec_id);
  RTC_CHECK(codec_index) << "Invalid codec ID: " << static_cast<int>(*codec_id);

  // Check if the payload-type is valid.
  if (!RentACodec::IsPayloadTypeValid(codec.pltype)) {
    LOG_F(LS_ERROR) << "Invalid payload type " << codec.pltype << " for "
                    << codec.plname;
    return -1;
  }

  AudioDecoder* isac_decoder = nullptr;
  if (STR_CASE_CMP(codec.plname, "isac") == 0) {
    if (!isac_decoder_) {
      isac_decoder_ = isac_factory();
    }
    isac_decoder = isac_decoder_.get();
  }
  return receiver_.AddCodec(*codec_index, codec.pltype, codec.channels,
                            codec.plfreq, isac_decoder, codec.plname);
}

int AudioCodingModuleImpl::RegisterExternalReceiveCodec(
    int rtp_payload_type,
    AudioDecoder* external_decoder,
    int sample_rate_hz,
    int num_channels,
    const std::string& name) {
  rtc::CritScope lock(&acm_crit_sect_);
  RTC_DCHECK(receiver_initialized_);
  if (num_channels > 2 || num_channels < 0) {
    LOG_F(LS_ERROR) << "Unsupported number of channels: " << num_channels;
    return -1;
  }

  // Check if the payload-type is valid.
  if (!RentACodec::IsPayloadTypeValid(rtp_payload_type)) {
    LOG_F(LS_ERROR) << "Invalid payload-type " << rtp_payload_type
                    << " for external decoder.";
    return -1;
  }

  return receiver_.AddCodec(-1 /* external */, rtp_payload_type, num_channels,
                            sample_rate_hz, external_decoder, name);
}

// Get current received codec.
int AudioCodingModuleImpl::ReceiveCodec(CodecInst* current_codec) const {
  rtc::CritScope lock(&acm_crit_sect_);
  return receiver_.LastAudioCodec(current_codec);
}

// Incoming packet from network parsed and ready for decode.
int AudioCodingModuleImpl::IncomingPacket(const uint8_t* incoming_payload,
                                          const size_t payload_length,
                                          const WebRtcRTPHeader& rtp_header) {
  return receiver_.InsertPacket(
      rtp_header,
      rtc::ArrayView<const uint8_t>(incoming_payload, payload_length));
}

// Minimum playout delay (Used for lip-sync).
int AudioCodingModuleImpl::SetMinimumPlayoutDelay(int time_ms) {
  if ((time_ms < 0) || (time_ms > 10000)) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Delay must be in the range of 0-1000 milliseconds.");
    return -1;
  }
  return receiver_.SetMinimumDelay(time_ms);
}

int AudioCodingModuleImpl::SetMaximumPlayoutDelay(int time_ms) {
  if ((time_ms < 0) || (time_ms > 10000)) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "Delay must be in the range of 0-1000 milliseconds.");
    return -1;
  }
  return receiver_.SetMaximumDelay(time_ms);
}

// Get 10 milliseconds of raw audio data to play out.
// Automatic resample to the requested frequency.
int AudioCodingModuleImpl::PlayoutData10Ms(int desired_freq_hz,
                                           AudioFrame* audio_frame) {
  // GetAudio always returns 10 ms, at the requested sample rate.
  if (receiver_.GetAudio(desired_freq_hz, audio_frame) != 0) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "PlayoutData failed, RecOut Failed");
    return -1;
  }
  audio_frame->id_ = id_;
  return 0;
}

/////////////////////////////////////////
//   Statistics
//

// TODO(turajs) change the return value to void. Also change the corresponding
// NetEq function.
int AudioCodingModuleImpl::GetNetworkStatistics(NetworkStatistics* statistics) {
  receiver_.GetNetworkStatistics(statistics);
  return 0;
}

int AudioCodingModuleImpl::RegisterVADCallback(ACMVADCallback* vad_callback) {
  WEBRTC_TRACE(webrtc::kTraceDebug, webrtc::kTraceAudioCoding, id_,
               "RegisterVADCallback()");
  rtc::CritScope lock(&callback_crit_sect_);
  vad_callback_ = vad_callback;
  return 0;
}

// TODO(kwiberg): Remove this method, and have callers call IncomingPacket
// instead. The translation logic and state belong with them, not with
// AudioCodingModuleImpl.
int AudioCodingModuleImpl::IncomingPayload(const uint8_t* incoming_payload,
                                           size_t payload_length,
                                           uint8_t payload_type,
                                           uint32_t timestamp) {
  // We are not acquiring any lock when interacting with |aux_rtp_header_| no
  // other method uses this member variable.
  if (!aux_rtp_header_) {
    // This is the first time that we are using |dummy_rtp_header_|
    // so we have to create it.
    aux_rtp_header_.reset(new WebRtcRTPHeader);
    aux_rtp_header_->header.payloadType = payload_type;
    // Don't matter in this case.
    aux_rtp_header_->header.ssrc = 0;
    aux_rtp_header_->header.markerBit = false;
    // Start with random numbers.
    aux_rtp_header_->header.sequenceNumber = 0x1234;  // Arbitrary.
    aux_rtp_header_->type.Audio.channel = 1;
  }

  aux_rtp_header_->header.timestamp = timestamp;
  IncomingPacket(incoming_payload, payload_length, *aux_rtp_header_);
  // Get ready for the next payload.
  aux_rtp_header_->header.sequenceNumber++;
  return 0;
}

int AudioCodingModuleImpl::SetOpusApplication(OpusApplicationMode application) {
  rtc::CritScope lock(&acm_crit_sect_);
  if (!HaveValidEncoder("SetOpusApplication")) {
    return -1;
  }
  AudioEncoder::Application app;
  switch (application) {
    case kVoip:
      app = AudioEncoder::Application::kSpeech;
      break;
    case kAudio:
      app = AudioEncoder::Application::kAudio;
      break;
    default:
      FATAL();
      return 0;
  }
  return encoder_stack_->SetApplication(app) ? 0 : -1;
}

// Informs Opus encoder of the maximum playback rate the receiver will render.
int AudioCodingModuleImpl::SetOpusMaxPlaybackRate(int frequency_hz) {
  rtc::CritScope lock(&acm_crit_sect_);
  if (!HaveValidEncoder("SetOpusMaxPlaybackRate")) {
    return -1;
  }
  encoder_stack_->SetMaxPlaybackRate(frequency_hz);
  return 0;
}

int AudioCodingModuleImpl::EnableOpusDtx() {
  rtc::CritScope lock(&acm_crit_sect_);
  if (!HaveValidEncoder("EnableOpusDtx")) {
    return -1;
  }
  return encoder_stack_->SetDtx(true) ? 0 : -1;
}

int AudioCodingModuleImpl::DisableOpusDtx() {
  rtc::CritScope lock(&acm_crit_sect_);
  if (!HaveValidEncoder("DisableOpusDtx")) {
    return -1;
  }
  return encoder_stack_->SetDtx(false) ? 0 : -1;
}

int32_t AudioCodingModuleImpl::PlayoutTimestamp(uint32_t* timestamp) {
  rtc::Optional<uint32_t> ts = PlayoutTimestamp();
  if (!ts)
    return -1;
  *timestamp = *ts;
  return 0;
}

rtc::Optional<uint32_t> AudioCodingModuleImpl::PlayoutTimestamp() {
  return receiver_.GetPlayoutTimestamp();
}

bool AudioCodingModuleImpl::HaveValidEncoder(const char* caller_name) const {
  if (!encoder_stack_) {
    WEBRTC_TRACE(webrtc::kTraceError, webrtc::kTraceAudioCoding, id_,
                 "%s failed: No send codec is registered.", caller_name);
    return false;
  }
  return true;
}

int AudioCodingModuleImpl::UnregisterReceiveCodec(uint8_t payload_type) {
  return receiver_.RemoveCodec(payload_type);
}

int AudioCodingModuleImpl::EnableNack(size_t max_nack_list_size) {
  return receiver_.EnableNack(max_nack_list_size);
}

void AudioCodingModuleImpl::DisableNack() {
  receiver_.DisableNack();
}

std::vector<uint16_t> AudioCodingModuleImpl::GetNackList(
    int64_t round_trip_time_ms) const {
  return receiver_.GetNackList(round_trip_time_ms);
}

int AudioCodingModuleImpl::LeastRequiredDelayMs() const {
  return receiver_.LeastRequiredDelayMs();
}

void AudioCodingModuleImpl::GetDecodingCallStatistics(
      AudioDecodingCallStats* call_stats) const {
  receiver_.GetDecodingCallStatistics(call_stats);
}

}  // namespace acm2
}  // namespace webrtc
