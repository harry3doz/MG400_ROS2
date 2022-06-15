// Copyright 2022 HarvestX Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gmock/gmock.h>
#include <mg400_interface/commander/dashboard_commander.hpp>

using ::testing::_;
using ::testing::StrEq;


class MockTcpInterface : public mg400_interface::TcpInterfaceBase
{
public:
  MockTcpInterface()
  : mg400_interface::TcpInterfaceBase() {}

  MOCK_METHOD(void, sendCommand, (const std::string &), (override));
};

class TestDashboardCommander : public ::testing::Test
{
protected:
  std::unique_ptr<mg400_interface::DashboardCommander> commander;
  MockTcpInterface mock;
  virtual void SetUp()
  {
    this->commander =
      std::make_unique<mg400_interface::DashboardCommander>(&this->mock);
  }

  virtual void TearDown() {}
};

TEST_F(TestDashboardCommander, EnableRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("EnableRobot()"))).Times(1);
  commander->enableRobot();
}

TEST_F(TestDashboardCommander, DisableRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("DisableRobot()"))).Times(1);
  commander->disableRobot();
}

TEST_F(TestDashboardCommander, ClearError) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ClearError()"))).Times(1);
  commander->clearError();
}

TEST_F(TestDashboardCommander, ResetRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ResetRobot()"))).Times(1);
  commander->resetRobot();
}

TEST_F(TestDashboardCommander, SpeedFactor) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SpeedFactor(78)"))).Times(1);
  commander->speedFactor(78);
}

TEST_F(TestDashboardCommander, GetErrorID) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetErrorID()"))).Times(1);
  commander->getErrorId();
}
