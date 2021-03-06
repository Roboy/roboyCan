#include "roboy_can/Motors.hpp"
using namespace Maxon;

auto RoboyMotor::connect(kaco::Master &master, MotorConfig &&config)
    -> variant<RoboyMotor, std::pair<RoboyMotorCommandStatus, MotorConfig>> {
  // todo: Check integrety of config - or do it when constructing the config
  return RoboyMotor::setupMotor(master, std::move(config))
      .match(
          [](std::pair<kaco::Master &, MotorConfig> &setupMotor) -> RoboyMotor {
            return RoboyMotor(setupMotor.first, std::move(setupMotor.second));
          },
          [](std::pair<RoboyMotorCommandStatus, MotorConfig> &setupMotor)
              -> std::pair<RoboyMotorCommandStatus, MotorConfig> {
            return std::move(setupMotor);
          });
}

RoboyMotor::RoboyMotor(kaco::Master &master, MotorConfig &&config)
    : motor{master->get_Device(config.getCanId())} {
  config_ = std::move(config);
}

RoboyMotor::~RoboyMotor(void) {
  std::bitset<16> cw = motor_.get_entry("controlword");
  cw.reset(1); // Disable Voltage
  motor_.set_entry("controlword", static_cast<uint16_t>(cw));
}

auto RoboyMotor::setupMotor(kaco::Master &master, MotorConfig &&config)
    -> variant<std::pair<kaco::Master &, MotorConfig>,
               std::pair<RoboyMotorCommandStatus, MotorConfig>> {
  try {
    kaco::Device motor = master->get_device(config.getCanId());
    motor.load_dictionary_from_eds(config.getEdsAddress());
  } catch (const kaco::dictionary_error &e) {
    std::cout << "RoboyCan Error - Dictionary Error on EDS loading: "
              << e.what() << std::endl;
    return std::make_pair<RoboyMotorCommandStatus::EDS_ERROR,
                          std::move(config)>;
  } catch (const kaco::sdo_error &e) {
    std::cout << "RoboyCan Error - SDO Error on EDS loading: " << e.what()
              << std::endl;
    return std::make_pair<RoboyMotorCommandStatus::SDO_ERROR,
                          std::move(config)>;
  }
  return std::make_pair<master, std::move(config)>;
}

RoboyMotorCommandStatus RoboyMotor::moveMotor(Epos2ControlMode &controlMode,
                                              double &&setpoint) {
  switch (controlMode) {
  case nullptr:
    return RoboyMotorCommandStatus::CONTROL_MODE_NOT_CONFIGURED;
  case Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY:
    return writeMotorModeProfilePositionAbsoluteImmediately(std::move(setpoint),
                                                            CachedWrite::NO);
    break;
  default:
    return RoboyMotorCommandStatus::CONTROL_MODE_UNKNOWN;
  }
}

RoboyMotorCommandStatus
RoboyMotor::writeMotorModeProfilePositionAbsoluteImmediately(
    double &&setpoint, CachedWrite &&useCache)
    : useCache(CachedWrite::NO) {
  // todo: turn setpoint into encoder ticks

  if (activeController_ !=
      Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY) {

    if (setParameters(
            Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY) !=
        RoboyMotorCommandStatus::OK)
      return RoboyMotorCommandStatus::MOTOR_CONFIGURATION_FAILED;

    if (enableDevice() != RoboyMotorCommandStatus::OK)
      return RoboyMotorCommandStatus::MOTOR_CONFIGURATION_FAILED;

    activeController_ = controlMode;
  }
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> sw = motor_.get_entry("statusword");
      if (sw.check(3) == true)
        return RoboyMotorCommandStatus::DEVICE_FAULT;
      motor_.set_entry("Target position", (int32_t)setpoint);
      std::bitset<16> cw = motor_.get_entry("controlword");
      cw.set(4);    // Declare new setpoint
      cw.set(5);    // Change set immediately
      cw.reset(6);  // Absolute value
      cw.reset(8);  // Don't halt
      cw.reset(15); // Normal operation, no endless motion
      motor_.set_entry("controlword", static_cast<uint16_t>(cw));
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> sw =
          motor_.get_entry("statusword", kaco::ReadAccessMethod::cache);
      if (sw.check(3) == true)
        return RoboyMotorCommandStatus::DEVICE_FAULT;
      motor_.set_entry("Target position", (int32_t)setpoint,
                       kaco::WriteAccessMethod::cache);
      std::bitset<16> cw =
          motor_.get_entry("controlword", kaco::ReadAccessMethod::cache);
      cw.set(4);    // Declare new setpoint
      cw.set(5);    // Change set immediately
      cw.reset(6);  // Absolute value
      cw.reset(8);  // Don't halt
      cw.reset(15); // Normal operation, no endless motion
      motor_.set_entry("controlword", static_cast<uint16_t>(cw),
                       kaco::WriteAccessMethod::cache);
    }
  } catch (const kaco::dictionary_error &e) {
    std::cout << "RoboyCan Error - Dictionary Error on enabling Device: "
              << e.what() << std::endl;
    return < RoboyMotorCommandStatus::EDS_ERROR;
  } catch (const kaco::sdo_error &e) {
    std::cout << "RoboyCan Error - SDO Error on enabling Devicey: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

RoboyMotorCommandStatus RoboyMotor::enableDevice(CachedWrite &&useCache)
    : useCache(CachedWrite::NO) {
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> cw = motor_.get_entry("controlword");
      // Shutdown the device
      cw.reset(0);
      cw.set(1);
      cw.set(2);
      motor_.set_entry("controlword", static_cast<uint16_t>(cw));
      // Switch the device on
      cw = motor_.get_entry("controlword");
      cw.set(0);
      cw.set(1);
      cw.set(2);
      cw.set(3);
      motor_.set_entry("controlword", static_cast<uint16_t>(cw));
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> cw =
          motor_.get_entry("controlword", kaco::ReadAccessMethod::cache);
      // Shutdown the device
      cw.reset(0);
      cw.set(1);
      cw.set(2);
      motor_.set_entry("controlword", static_cast<uint16_t>(cw),
                       kaco::WriteAccessMethod::cache);
      // Switch the device on
      cw = motor_.get_entry("controlword", kaco::ReadAccessMethod::cache);

      cw.set(0);
      cw.set(1);
      cw.set(2);
      cw.set(3);
      motor_.set_entry("controlword", static_cast<uint16_t>(cw),
                       kaco::WriteAccessMethod::cache);
    }
  } catch (const kaco::dictionary_error &e) {

    std::cout << "RoboyCan Error - Dictionary Error on enabling Device: "
              << e.what() << std::endl;
    return < RoboyMotorCommandStatus::EDS_ERROR;

  } catch (const kaco::sdo_error &e) {

    std::cout << "RoboyCan Error - SDO Error on enabling Devicey: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

RoboyMotorCommandStatus RoboyMotor::resetFault(CachedWrite &&useCache)
    : useCache(CachedWrite::NO) {
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> cw = motor_.get_entry("controlword");
          motor_.set_entry("controlword", static_cast<uint16_t>(cw.set(7));
          enableDevice(CachedWrite::NO);
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> cw =
          motor_.get_entry("controlword", kaco::ReadAccessMethod::cache);
      motor_.set_entry("controlword", static_cast<uint16_t>(cw.set(7)),
                       kaco::WriteAccessMethod::cache);
      enableDevice(CachedWrite::YES);
    }
  } catch (const kaco::dictionary_error &e) {

    std::cout << "RoboyCan Error - Dictionary Error on resetFault: " << e.what()
              << std::endl;
    return < RoboyMotorCommandStatus::EDS_ERROR;

  } catch (const kaco::sdo_error &e) {

    std::cout << "RoboyCan Error - SDO Error on resetFaulty: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

auto RoboyMotor::getStatus(void)
    -> variant<std::set<Epos2Status &&>,
               std::pair<RoboyMotorCommandStatus, std::set<Epos2Status> &&>> {

  std::bitset<16> motorStatus = nullptr;
  std::set<Epos2Status> motorState;
  try {
    motorStatus = motor_.get_entry("Statusword");
  } catch (const kaco::dictionary_error &e) {
    std::cout << "RoboyCan Error - Dictionary Error on getStatus: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::EDS_ERROR;
  } catch (const kaco::sdo_error &e) {
    std::cout << "RoboyCan Error - SDO Error on getStatus: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  for (std::size_t i = 0; i < motorStatus.size(), ++i) {
    if (motorstatus.test(i)) {
      motorState.insert(Epos2Status[i]);
    }
  }
  if (motorState.find(Epos2Status::FAULT)) {
    return std::make_pair<RoboyMotorCommandStatus::FAULT,
                          std::move(motorState)>;
  } else {
    return std::move(motorState);
  }
}

RoboyMotorCommandStatus setParameters(Epos2ControlMode &&controlMode,
                                      &&MotorConfig config) {
  switch (controlMode) {
  case Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY:
    MaxonParameters parameters = config.getConfigurationProfilePositionMode();
  default:
    return RoboyMotorCommandStatus::CONTROL_MODE_UNKNOWN;
  }
  for (auto &param : parameters) {
    try {
      motor_.set_entry(param.first, param.second);

    } catch (const kaco::dictionary_error &e) {
      std::cout << "RoboyCan Error - Dictionary Error on setParameters: "
                << e.what() << std::endl;
      return RoboyMotorCommandStatus::EDS_ERROR;
    } catch (const kaco::sdo_error &e) {
      std::cout << "RoboyCan Error - SDO Error on setParameters: " << e.what()
                << std::endl;
      return RoboyMotorCommandStatus::SDO_ERROR;
    }
  }
  return RoboyMotorCommandStatus::OK;
}
