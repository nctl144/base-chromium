// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_SYS_UTILS_H_
#define BASE_ANDROID_SYS_UTILS_H_

#include "base/android/jni_android.h"

namespace base {
namespace android {

class BASE_EXPORT SysUtils {
 public:
  static bool Register(JNIEnv* env);

  // Returns true iff this is a low-end device.
  static bool IsLowEndDevice();

  // Return the device's RAM size in kilo-bytes.
  static size_t AmountOfPhysicalMemoryKB();

 private:
  SysUtils();

  static bool IsLowEndDeviceFromJni();
  static size_t AmountOfPhysicalMemoryKBFromJni();
};

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_SYS_UTILS_H_
