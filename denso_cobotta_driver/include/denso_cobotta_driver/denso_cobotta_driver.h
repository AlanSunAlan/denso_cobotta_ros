/*
 * Copyright (C) 2018-2019  DENSO WAVE INCORPORATED
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _DENSO_COBOTTA_DRIVER_H_
#define _DENSO_COBOTTA_DRIVER_H_

// C++ standard
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <array>
#include <mutex>

// ROS
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/UInt16.h>
#include <realtime_tools/realtime_publisher.h>
#include "denso_cobotta_driver/GetBrakeState.h"
#include "denso_cobotta_driver/GetMotorState.h"
#include "denso_cobotta_driver/SetBrakeState.h"
#include "denso_cobotta_driver/SetMotorState.h"
#include "denso_cobotta_driver/SetLEDState.h"
#include "denso_cobotta_driver/ClearError.h"
#include "denso_cobotta_driver/ClearRobotError.h"
#include "denso_cobotta_driver/ClearSafeState.h"
#include "denso_cobotta_driver/RobotState.h"
#include "denso_cobotta_driver/SafeState.h"
#include "denso_cobotta_driver/ArmPosition.h"

// COBOTTA device driver
#include <fcntl.h>
#include <sys/ioctl.h>
#include "denso_cobotta_lib/cobotta_ioctl.h"
#include "denso_cobotta_lib/cobotta_common.h"
#include "denso_cobotta_lib/cobotta.h"
#include "denso_cobotta_lib/publish_info.h"

namespace denso_cobotta_driver
{
using namespace cobotta_common;
using namespace denso_cobotta_lib;

/**
 * ROS node: DensoCobottaDriver
 */
class DensoCobottaDriver
{
public:
  DensoCobottaDriver();
  virtual ~DensoCobottaDriver() = default;

  bool initialize(ros::NodeHandle& nh);
  void start();
  void stop();
  void terminate();
  void update();
  void publish(const bool sync, const denso_cobotta_lib::cobotta::PublishInfo pi);

  // Service callback functions.
  bool setMotorStateSv(SetMotorState::Request& req, SetMotorState::Response& res);
  bool getMotorStateSv(GetMotorState::Request& req, GetMotorState::Response& res);
  bool setBrakeStateSv(SetBrakeState::Request& req, SetBrakeState::Response& res);
  bool getBrakeStateSv(GetBrakeState::Request& req, GetBrakeState::Response& res);
  bool moveArmSv(ArmPosition::Request& req, ArmPosition::Response& res);
  bool execCalsetSv(ClearError::Request& req, ClearError::Response& res);
  bool clearErrorSv(ClearError::Request& req, ClearError::Response& res);
  bool clearRobotErrorSv(ClearError::Request& req, ClearError::Response& res);
  bool clearSafeStateSv(ClearError::Request& req, ClearError::Response& res);
  bool setLedStateSv(SetLEDState::Request& req, SetLEDState::Response& res);

  // Subscriber callback functions.
  void miniIoOutputCallback(const std_msgs::UInt16::ConstPtr& msg);

private:
  void dequeueDriver(long arm_no, int count, bool sync = false);
  void dequeueSafetyMcu(int count, bool sync = false);
  void putRosLog(const char* tag, uint32_t main_code, uint32_t sub_code);
  const std::shared_ptr<cobotta::Cobotta>& getCobotta() const;

  bool isForceClearFlag() const;
  void setForceClearFlag(bool);

  // Service server
  ros::ServiceServer sv_set_motor_;
  ros::ServiceServer sv_get_motor_;
  ros::ServiceServer sv_clear_error_;
  ros::ServiceServer sv_clear_robot_error_;
  ros::ServiceServer sv_clear_safe_state_;
  ros::ServiceServer sv_set_brake_;
  ros::ServiceServer sv_get_brake_;
  ros::ServiceServer sv_move_arm_;
  ros::ServiceServer sv_exec_calset_;
  ros::ServiceServer sv_set_led_;

  // Publisher
  realtime_tools::RealtimePublisher<std_msgs::Bool> pub_function_button_;
  realtime_tools::RealtimePublisher<std_msgs::Bool> pub_plus_button_;
  realtime_tools::RealtimePublisher<std_msgs::Bool> pub_minus_button_;
  realtime_tools::RealtimePublisher<std_msgs::UInt16> pub_mini_io_input_;
  realtime_tools::RealtimePublisher<denso_cobotta_driver::RobotState> pub_robot_state_;
  realtime_tools::RealtimePublisher<denso_cobotta_driver::SafeState> pub_safe_state_;

  // Subscriber
  ros::Subscriber sub_mini_io_output_;

  // cobotta
  std::shared_ptr<cobotta::Cobotta> cobotta_;

  std_msgs::Bool function_button_state_;
  std_msgs::Bool plus_button_state_;
  std_msgs::Bool minus_button_state_;
  std_msgs::UInt16 mini_io_state_;

  bool force_clear_flag_ = false;

};
}  // namespace denso_cobotta_driver

#endif  // _DENSO_COBOTTA_DRIVER_H_
