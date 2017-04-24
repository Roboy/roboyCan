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
using masterMap = std::map<std::string, std::reference_wrapper<kaco::Master>>;
using motorMap = std::map<std::string, RoboyMotor>;

enum class RoboyCanStatus {
  OK,
  CONNECTION_FAILED,
  DEVICE_NOT_FOUND,
  WRONG_DRIVER,
  OTHER_ERROR
};
using failedCanRoboy = std::pair<RoboyConfig, RoboyCanStatus>;

class canRoboy {
public:
  canRoboy(canRoboy &&) = default;
  canRoboy &operator=(canRoboy &&) = default;

  static auto connect(masterMap masters, RoboyConfig &&roboyConfigs)
      -> variant<canRoboy, failedCanRoboy>;
  auto initialiseMotors(void) -> RoboyMotorCommandStatus;
  inline auto getMotorNames(void) -> std::vector<std::string> {
    std::vector<std::string> names;
    for (auto &name : motorsCan_) {
      names.emplace_back(name.first);
    }
    return std::move(names);
  };
  inline auto getPositions(void) -> std::map<std::string, double> {
    std::map<std::string, double> posMap;
    for (auto &motor : motorsCan_) {
      posMap.emplace(motor.first, motor.second.getPosition());
    }
    return std::move(posMap);
  };

private:
  canRoboy(masterMap canMasters, motorMap &&motors);

  masterMap master_;
  motorMap motorsCan_;
};
