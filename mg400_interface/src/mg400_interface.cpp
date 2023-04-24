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

#include "mg400_interface/mg400_interface.hpp"

namespace mg400_interface
{

MG400Interface::MG400Interface(const std::string & ip_address)
: IP(ip_address)
{
}

bool MG400Interface::configure(const std::string & frame_id_prefix)
{
  this->dashboard_tcp_if_ = std::make_unique<DashboardTcpInterface>(this->IP);
  this->motion_tcp_if_ = std::make_unique<MotionTcpInterface>(this->IP);
  this->realtime_tcp_interface = std::make_shared<RealtimeFeedbackTcpInterface>(
    this->IP, frame_id_prefix);

  this->error_msg_generator =
    std::make_unique<ErrorMsgGenerator>("alarm_controller.json");

  return this->error_msg_generator->loadJsonFile();
}


bool MG400Interface::activate()
{
  using namespace std::chrono_literals;  // NOLINT
  this->dashboard_tcp_if_->init();
  this->realtime_tcp_interface->init();
  this->motion_tcp_if_->init();

  auto clock = rclcpp::Clock();
  const auto start = clock.now();

  const auto is_connected = [&]() -> bool {
      return this->dashboard_tcp_if_->isConnected() &&
             this->realtime_tcp_interface->isConnected() &&
             this->motion_tcp_if_->isConnected();
    };

  while (!is_connected()) {
    if (clock.now() - start > rclcpp::Duration(3s)) {
      RCLCPP_ERROR(this->getLogger(), "Could not connect DOBOT MG400.");
      std::thread disconnect1([this](){ this->dashboard_tcp_if_->disConnect(); });
      std::thread disconnect2([this](){ this->realtime_tcp_interface->disConnect(); });
      std::thread disconnect3([this](){ this->motion_tcp_if_->disConnect(); });
      disconnect1.join();
      disconnect2.join();
      disconnect3.join();
      return false;
    }

    RCLCPP_WARN(this->getLogger(), "Waiting for the connection...");
    rclcpp::sleep_for(1s);
  }

  this->dashboard_commander = std::make_shared<DashboardCommander>(this->dashboard_tcp_if_.get());
  this->motion_commander = std::make_shared<MotionCommander>(this->motion_tcp_if_.get());
  return true;
}

bool MG400Interface::deactivate()
{
  this->dashboard_commander.reset();
  this->motion_commander.reset();
  this->dashboard_tcp_if_->disConnect();
  this->motion_tcp_if_->disConnect();
  this->realtime_tcp_interface->disConnect();
  return true;
}

const rclcpp::Logger MG400Interface::getLogger() noexcept
{
  return rclcpp::get_logger("MG400Interface");
}
}  // namespace mg400_interface
