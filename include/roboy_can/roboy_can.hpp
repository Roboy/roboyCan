#pragma once
#include <chrono>
#include <cstdint>
#include <thread>

#include "canopen_error.h"
#include "logger.h"
#include "master.h"

#include "Motors.hpp"
#include <map>
#include <vector>

enum class RoboyCanStatus {
  OK,
  CONNECTION_FAILED,
  DEVICE_NOT_FOUND,
  WRONG_DRIVER,
  OTHER_ERROR
};

class canRoboy {
public:
  RoboyMotorCommandStatus moveMotor(EPOSCommand);

  RoboyMotorCommandStatus stopMotors(std::string);

  RoboyMotorCommandStatus moveMultipleMotors(std::vector<EPOSCommand>);

  canRoboy(canRoboy &&) = default;
  canRoboy &operator=(canRoboy &&) = default;
  static auto connect(kaco::Master &master, MotorConfigs &&roboyConfigs)
      -> variant<canRoboy, std::pair<MotorConfigs, RoboyCanStatus>>;

private:
  canRoboy(kaco::Master &master, MotorConfigs &&roboyConfigs);
  std::vector<std::string> getJointNames(void);
  void configureNodes(void);
  // void initialise(std::string busname = "slcan0", std::string baudrate =
  // "1M");

  unsigned int getCanAddress(std::string jointName);

  unsigned int getCanAddress_cached(std::string jointName);

  void findNodes(std::vector<std::string> jointNames);

  void setupMotor(std::string JointName);
  void PDO_setup(std::string JointName);

  kaco::Master *master_;

  std::map<unsigned int, kaco::Device &> deviceVector_;
  std::map<std::string, unsigned int> jointCanMap_;
  std::map<unsigned int, int> controlMode_;
  std::vector<unsigned int> motor_ids_;
};
