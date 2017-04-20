#pragma once
#include "master.h"
#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include <cstdint>
#include <set>

enum class ControlMode : int { Raw, Torque, Velocity, Position, Force };

enum class CommandResult : int {
  Ok,
  MuscleDoesNotExist,
  ControllerNotInitialized
};

enum class Epos2ControlMode {
  PROFILE_POSITION_ABSOLUTE_IMMEDIATELY,
  PROFILE_POSITION_RELATIVE_IMMEDIATELY,
  VELOCITY,
  CURRENT,
  FORCE
};

enum class RoboyMotorCommandStatus {
  OK,
  DEVICE_FAULT,
  CONTROL_MODE_NOT_CONFIGURED,
  CONTROL_MODE_UNKNOWN,
  MOTOR_CONFIGURATION_FAILED,
  EDS_ERROR,
  SDO_ERROR,
  OTHER_ERROR
};

enum class CachedWrite { YES, NO };

enum class Epos2Status {
  READY_TO_SWITCH_ON,
  SWITCHED_ON,
  OPERATION_ENABLED,
  FAULT,
  VOLTAGE_ENABLED,
  QUICKSTOP,
  SWITCH_ON_ENABLED,
  WARNING,
  OFFSET_CURRENT_MEASURED,
  REMOTE,
  OPERATING_MODE_SPECIFIC_TARGET_REACHED,
  INTERNAL_LIMIT_ACTIVE,
  OPERATING_MODE_SPECIFIC_BIT_12,
  OPERATING_MODE_SPECIFIC_BIT_13,
  REFRESH_CYCLE_OF_POWER_STAGE,
  POSITION_REFERENCED_TO_HOME_POSITION
};
using unintMotor = std::pair<RoboyMotorCommandStatus, MotorConfig>;

using CanMotorConfig =
    variant<std::reference_wrapper<kaco::Device>, unintMotor>;

class EPOSCommand {
public:
  EPOSCommand(std::string jn, Epos2ControlMode cm, signed int sp)
      : jointName(jn), controlMode(cm), setpoint(sp){};

  std::string getJointName() { return jointName; };
  Epos2ControlMode getControlMode() { return controlMode; };
  signed int getSetpoint() { return setpoint; };

private:
  std::string jointName;
  Epos2ControlMode controlMode;
  signed int setpoint;
};

class RoboyMotor {
public:
  inline std::string getName() { return config_.name; };
  RoboyMotorCommandStatus moveMotor(Epos2ControlMode &&controlMode,
                                    double &&setpoint);
  inline double getPosition(void) {
    return motor_.get().get_entry("Position Actual Value");
  }
  inline double getCurrent(void) {
    return motor_.get().get_entry("Current Demand Value");
  }
  inline auto getStatus(void)
      -> variant<std::set<Epos2Status &&>,
                 std::pair<RoboyMotorCommandStatus, std::set<Epos2Status> &&>>;
  RoboyMotorCommandStatus resetFault(CachedWrite &&useCache);

  RoboyMotor(RoboyMotor &&) = default;
  RoboyMotor &operator=(RoboyMotor &&) = default;
  virtual ~RoboyMotor();
  static auto connect(std::reference_wrapper<kaco::Master> master,
                      MotorConfig motorConfig)
      -> variant<RoboyMotor, unintMotor>;

private:
  RoboyMotor(std::reference_wrapper<kaco::Device> device, MotorConfig config);
  static auto setupMotor(std::reference_wrapper<kaco::Master> master,
                         MotorConfig config) -> CanMotorConfig;

  RoboyMotorCommandStatus
  writeMotorModeProfilePositionAbsoluteImmediately(double &&setpoint,
                                                   CachedWrite &&useCache);
  RoboyMotorCommandStatus enableDevice(CachedWrite &&useCache);
  RoboyMotorCommandStatus setParameters(Epos2ControlMode &&controlMode);

  Epos2ControlMode activeController_;
  std::reference_wrapper<kaco::Device> motor_;
  MotorConfig config_;
};
