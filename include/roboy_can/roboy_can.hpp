#pragma once
#include <chrono>
#include <cstdint>
#include <thread>

#include "canopen_error.h"
#include "logger.h"
#include "master.h"
#include "roboy_can/setEPOSControllers.h"
#include "ros/ros.h"
#include <map>
#include <rosparam_shortcuts/rosparam_shortcuts.h>
#include <vector>

class canRoboy {
public:
  canRoboy();
  void initialise(std::string busname = "slcan0", std::string baudrate = "1M");

  void configureNodes(void);
  void enable_current_mode(std::string);
  void enable_profile_position_mode(std::string);

  void moveMotor(std::string jointName, int setpoint);

  void stopMotors(std::map<unsigned int, int> motorMap);

  void moveMultipleMotors(std::map<std::string, int> motorMap);

  std::vector<unsigned int> getMotorIDs(void);
  std::vector<double> readPosition(void);
  bool set_controllers(roboy_can::setEPOSControllers::Request &req,
                       roboy_can::setEPOSControllers::Response &res);

private:
  std::vector<std::string> getJointNames(void);

  unsigned int getCanAddress(std::string jointName);

  unsigned int getCanAddress_cached(std::string jointName);

  void findNodes(std::vector<std::string> jointNames);

  void setupMotor(std::string JointName);
  void PDO_setup(std::string JointName);

  kaco::Master master_;
  std::string busname_;
  std::string baudrate_;
  std::map<unsigned int, kaco::Device &> deviceVector_;
  std::map<std::string, unsigned int> jointCanMap_;
  std::map<unsigned int, int> controlMode_;
  std::vector<unsigned int> motor_ids_;
  ros::NodeHandle roboyCanNH_;
  ros::ServiceServer setEposController_;
};
