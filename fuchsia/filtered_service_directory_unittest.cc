// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/fuchsia/filtered_service_directory.h"

#include <lib/zx/channel.h>
#include <utility>

#include "base/fuchsia/service_directory_test_base.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace fuchsia {

class FilteredServiceDirectoryTest : public ServiceDirectoryTestBase {
 public:
  FilteredServiceDirectoryTest() {
    filtered_service_dir_ =
        std::make_unique<FilteredServiceDirectory>(client_context_.get());
    filtered_client_context_ = std::make_unique<ComponentContext>(
        filtered_service_dir_->ConnectClient());
  }

 protected:
  std::unique_ptr<FilteredServiceDirectory> filtered_service_dir_;
  std::unique_ptr<ComponentContext> filtered_client_context_;
};

// Verify that we can connect to a whitelisted service.
TEST_F(FilteredServiceDirectoryTest, Connect) {
  filtered_service_dir_->AddService(test_fidl::TestInterface::Name_);

  auto stub =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  VerifyTestInterface(&stub, false);
}

// Verify that multiple connections to the same service work properly.
TEST_F(FilteredServiceDirectoryTest, ConnectMultiple) {
  filtered_service_dir_->AddService(test_fidl::TestInterface::Name_);

  auto stub1 =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  auto stub2 =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  VerifyTestInterface(&stub1, false);
  VerifyTestInterface(&stub2, false);
}

// Verify that non-whitelisted services are blocked.
TEST_F(FilteredServiceDirectoryTest, ServiceBlocked) {
  auto stub =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  VerifyTestInterface(&stub, true);
}

// Verify that FilteredServiceDirectory handles the case when the target service
// is not available in the underlying service directory.
TEST_F(FilteredServiceDirectoryTest, NoService) {
  filtered_service_dir_->AddService(test_fidl::TestInterface::Name_);

  service_binding_.reset();

  auto stub =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  VerifyTestInterface(&stub, true);
}

// Verify that FilteredServiceDirectory handles the case when the underlying
// service directory is destroyed.
TEST_F(FilteredServiceDirectoryTest, NoServiceDir) {
  filtered_service_dir_->AddService(test_fidl::TestInterface::Name_);

  service_binding_.reset();
  service_directory_.reset();

  auto stub =
      filtered_client_context_->ConnectToService<test_fidl::TestInterface>();
  VerifyTestInterface(&stub, true);
}

}  // namespace fuchsia
}  // namespace base
