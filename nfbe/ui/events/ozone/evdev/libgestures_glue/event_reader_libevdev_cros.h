// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_LIBGESTURES_GLUE_EVENT_READER_LIBEVDEV_CROS_H_
#define UI_EVENTS_OZONE_EVDEV_LIBGESTURES_GLUE_EVENT_READER_LIBEVDEV_CROS_H_

#include <libevdev/libevdev.h>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "ui/events/ozone/evdev/event_converter_evdev.h"
#include "ui/events/ozone/evdev/event_device_info.h"

namespace ui {

// Basic wrapper for libevdev-cros.
//
// This drives libevdev-cros from a file descriptor and calls delegate
// with the updated event state from libevdev-cros.
//
// The library doesn't support all devices currently. In particular there
// is no support for keyboard events.
class EventReaderLibevdevCros : public EventConverterEvdev {
 public:
  class Delegate {
   public:
    virtual ~Delegate();

    // Notifier for open. This is called with the initial event state.
    virtual void OnLibEvdevCrosOpen(Evdev* evdev, EventStateRec* evstate) = 0;

    // Notifier for event. This is called with the updated event state.
    virtual void OnLibEvdevCrosEvent(Evdev* evdev,
                                     EventStateRec* state,
                                     const timeval& time) = 0;

    // Notifier for stop. This is called with the final event state.
    virtual void OnLibEvdevCrosStopped(Evdev* evdev, EventStateRec* state) = 0;
  };

  EventReaderLibevdevCros(int fd,
                          const base::FilePath& path,
                          int id,
                          const EventDeviceInfo& devinfo,
                          scoped_ptr<Delegate> delegate);
  ~EventReaderLibevdevCros() override;

  // EventConverterEvdev:
  void OnFileCanReadWithoutBlocking(int fd) override;
  bool HasKeyboard() const override;
  bool HasMouse() const override;
  bool HasTouchpad() const override;
  bool HasCapsLockLed() const override;
  void OnDisabled() override;

 private:
  static void OnSynReport(void* data,
                          EventStateRec* evstate,
                          struct timeval* tv);
  static void OnLogMessage(void*, int level, const char*, ...);

  // Input modalities for this device.
  bool has_keyboard_;
  bool has_mouse_;
  bool has_touchpad_;

  // LEDs for this device.
  bool has_caps_lock_led_;

  // Libevdev state.
  Evdev evdev_;

  // Event state.
  EventStateRec evstate_;

  // Path to input device.
  base::FilePath path_;

  // Delegate for event processing.
  scoped_ptr<Delegate> delegate_;

  DISALLOW_COPY_AND_ASSIGN(EventReaderLibevdevCros);
};

}  // namspace ui

#endif  // UI_EVENTS_OZONE_EVDEV_LIBGESTURES_GLUE_EVENT_READER_LIBEVDEV_CROS_H_
