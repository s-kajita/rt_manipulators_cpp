// Copyright 2021 RT Corporation
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

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>
#include "rt_manipulators_cpp/hardware.hpp"

void SleepWait(int sec){
  std::cout << sec << "秒間スリープして動作完了を待ちます." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(sec));
}

int main() {
  std::cout << "CRANE-X7のサーボモータ目標角度を書き込むサンプルです." << std::endl;

  std::string port_name = "/dev/ttyUSB0";
  int baudrate = 3000000;  // 3Mbps
  std::string config_file = "../config/crane-x7.yaml";

  rt_manipulators_cpp::Hardware hardware(port_name);
  if (!hardware.connect(baudrate)) {
    std::cerr << "ロボットとの接続に失敗しました." << std::endl;
    return -1;
  }

  if (!hardware.load_config_file(config_file)) {
    std::cerr << "コンフィグファイルの読み込みに失敗しました." << std::endl;
    return -1;
  }

  std::cout << "armグループのサーボ最大加速度を0.5pi rad/s^2、最大速度を0.5pi rad/sに設定します."
            << std::endl;
  if (!hardware.write_max_acceleration_to_group("arm", 0.5 * M_PI)) {
    std::cerr << "armグループの最大加速度を設定できませんでした." << std::endl;
    return -1;
  }

  if (!hardware.write_max_velocity_to_group("arm", 0.5 * M_PI)) {
    std::cerr << "armグループの最大速度を設定できませんでした." << std::endl;
    return -1;
  }

  std::cout << "handグループのサーボ最大加速度を0.5pi rad/s^2、最大速度を0.5pi rad/sに設定します."
            << std::endl;
  if (!hardware.write_max_acceleration_to_group("hand", 0.5 * M_PI)) {
    std::cerr << "handグループの最大加速度を設定できませんでした." << std::endl;
    return -1;
  }

  if (!hardware.write_max_velocity_to_group("hand", 0.5 * M_PI)) {
    std::cerr << "handグループの最大速度を設定できませんでした." << std::endl;
    return -1;
  }

  std::cout << "arm、handグループのサーボ位置制御PIDゲインに(800, 0, 0)を書き込みます."
            << std::endl;
  if (!hardware.write_position_pid_gain_to_group("arm", 800, 0, 0)) {
    std::cerr << "armグループにPIDゲインを書き込めませんでした." << std::endl;
    return -1;
  }
  if (!hardware.write_position_pid_gain_to_group("hand", 800, 0, 0)) {
    std::cerr << "handグループにPIDゲインを書き込めませんでした." << std::endl;
    return -1;
  }
  // PIDゲインは指定したサーボモータにも設定できます.
  if (!hardware.write_position_pid_gain(9, 800, 0, 0)) {
    std::cerr << "ID:9ジョイントにPIDゲインを書き込めませんでした." << std::endl;
    return -1;
  }
  if (!hardware.write_position_pid_gain("joint_hand", 800, 0, 0)) {
    std::cerr << "joint_handジョイントにPIDゲインを書き込めませんでした." << std::endl;
    return -1;
  }

  if (!hardware.torque_on("arm")) {
    std::cerr << "armグループのトルクをONできませんでした." << std::endl;
    return -1;
  }

  if (!hardware.torque_on("hand")) {
    std::cerr << "handグループのトルクをONできませんでした." << std::endl;
    return -1;
  }

  std::vector<double> target_positions(7, 0.0);
  target_positions[1] =  -20 * M_PI / 180.0;
  target_positions[3] =  -120.0 * M_PI / 180.0;  // radian
  std::cout << "5秒後にX7がホーム姿勢へ移行するため、X7の周りに物や人を近づけないで下さい."
            << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  hardware.set_positions("arm", target_positions);
  if (!hardware.sync_write("arm")) {
    std::cerr << "armグループのsync writeに失敗しました." << std::endl;
  }
  SleepWait(5);

  std::string joint_name = "joint_hand";
  double target_position = 30.0 * M_PI / 180.0;  // radian
  std::cout << joint_name << "ジョイントのサーボ目標角度に" << std::to_string(target_position)
            << " radを書き込みます." << std::endl;
  hardware.set_position(joint_name, target_position);
  if (!hardware.sync_write("hand")) {
    std::cerr << "handグループのsync writeに失敗しました." << std::endl;
  }
  SleepWait(1);

  target_position = 0.0;  // radian
  std::cout << joint_name << "ジョイントのサーボ目標角度に" << std::to_string(target_position)
            << " radを書き込みます." << std::endl;
  hardware.set_position(joint_name, target_position);
  if (!hardware.sync_write("hand")) {
    std::cerr << "handグループのsync writeに失敗しました." << std::endl;
  }
  SleepWait(1);

  std::cout << "arm、handグループのサーボ位置制御PIDゲインに(5, 0, 0)を書き込み、脱力させます."
            << std::endl;
  if (!hardware.write_position_pid_gain_to_group("arm", 5, 0, 0)) {
    std::cerr << "armグループにPIDゲインを書き込めませんでした." << std::endl;
  }
  if (!hardware.write_position_pid_gain_to_group("hand", 5, 0, 0)) {
    std::cerr << "handグループにPIDゲインを書き込めませんでした." << std::endl;
  }
  SleepWait(5);


  
  if (!hardware.torque_off("arm")) {
    std::cerr << "armグループのトルクをOFFできませんでした." << std::endl;
  }

  if (!hardware.torque_off("hand")) {
    std::cerr << "handグループのトルクをOFFできませんでした." << std::endl;
  }

  if (!hardware.write_position_pid_gain_to_group("arm", 800, 0, 0)) {
    std::cerr << "armグループにPIDゲインを書き込めませんでした." << std::endl;
  }
  if (!hardware.write_position_pid_gain_to_group("hand", 800, 0, 0)) {
    std::cerr << "handグループにPIDゲインを書き込めませんでした." << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "CRANE-X7との接続を解除します." << std::endl;
  hardware.disconnect();
  return 0;
}
