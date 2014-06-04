// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_path_watcher.h"
#include "base/files/file_path_watcher_kqueue.h"

#if !defined(OS_IOS)
#include "base/files/file_path_watcher_fsevents.h"
#include "base/mac/mac_util.h"
#endif

namespace base {

namespace {

class FilePathWatcherImpl : public FilePathWatcher::PlatformDelegate {
 public:
  virtual bool Watch(const FilePath& path,
                     bool recursive,
                     const FilePathWatcher::Callback& callback) OVERRIDE {
    // Use kqueue for non-recursive watches and FSEvents for recursive ones.
    DCHECK(!impl_.get());
    if (recursive) {
      // FSEvents isn't available on iOS and is broken on OSX 10.6 and earlier.
      // See http://crbug.com/54822#c31
#if !defined(OS_IOS)
      if (mac::IsOSLionOrLater())
        impl_ = new FilePathWatcherFSEvents();
#endif  // OS_IOS

      if (!impl_)
        return false;
    } else {
      impl_ = new FilePathWatcherKQueue();
    }
    return impl_->Watch(path, recursive, callback);
  }

  virtual void Cancel() OVERRIDE {
    if (impl_)
      impl_->Cancel();
    set_cancelled();
  }

  virtual void CancelOnMessageLoopThread() OVERRIDE {
    if (impl_)
      impl_->Cancel();
    set_cancelled();
  }

 protected:
  virtual ~FilePathWatcherImpl() {}

  scoped_refptr<PlatformDelegate> impl_;
};

}  // namespace

FilePathWatcher::FilePathWatcher() {
  impl_ = new FilePathWatcherImpl();
}

}  // namespace base
