// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/renderer/media_stream_video_sink.h"

#include "content/renderer/media/media_stream_video_track.h"

namespace content {

MediaStreamVideoSink::MediaStreamVideoSink() : MediaStreamSink() {}

MediaStreamVideoSink::~MediaStreamVideoSink() {
  // Ensure this sink has disconnected from the track.
  DisconnectFromTrack();
}

void MediaStreamVideoSink::ConnectToTrack(
    const blink::WebMediaStreamTrack& track,
    const VideoCaptureDeliverFrameCB& callback) {
  DCHECK(connected_track_.isNull());
  connected_track_ = track;
  MediaStreamVideoTrack* const video_track =
      MediaStreamVideoTrack::GetVideoTrack(connected_track_);
  DCHECK(video_track);
  video_track->AddSink(this, callback);
}

void MediaStreamVideoSink::DisconnectFromTrack() {
  MediaStreamVideoTrack* const video_track =
      MediaStreamVideoTrack::GetVideoTrack(connected_track_);
  if (video_track)
    video_track->RemoveSink(this);
  connected_track_.reset();
}

}  // namespace content
