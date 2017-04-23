#include "roboy_can/Motors.hpp"
#include "dictionary_error.h"
#include "sdo_error.h"
#include <iostream>

auto RoboyMotor::connect(std::reference_wrapper<kaco::Master> master,
                         MotorConfig config)
    -> variant<RoboyMotor, unintMotor> {
  return RoboyMotor::setupMotor(master, config)
      .match(
          [config](std::reference_wrapper<kaco::Device> setupMotor)
              -> variant<RoboyMotor, unintMotor> {
            RoboyMotor rm{setupMotor, config};
            return std::move(rm);
          },
          [](unintMotor setupMotor) -> variant<RoboyMotor, unintMotor> {
            return {setupMotor};
          });
}

RoboyMotor::RoboyMotor(std::reference_wrapper<kaco::Device> device,
                       MotorConfig config)
    : motor_{device}, config_{std::move(config)} {}

RoboyMotor::~RoboyMotor(void) {
  std::bitset<16> cw =
      static_cast<uint16_t>(motor_.get().get_entry("controlword"));
  cw.reset(1); // Disable Voltage
  motor_.get().set_entry("controlword", static_cast<uint16_t>(cw.to_ulong()));
}

auto RoboyMotor::setupMotor(std::reference_wrapper<kaco::Master> master,
                            MotorConfig config) -> CanMotorConfig {
  try {
    std::reference_wrapper<kaco::Device> motor =
        master.get().get_device(config.canId);

    motor.get().start();
    motor.get().set_entry("CAN Bitrate", (uint16_t)0);
    std::cout << "RoboyCAN - : Set bitrate to 0" << std::endl;

    // TODO: load_dictionary_from_library() relies on a eds_file.json as
    // defined
    // here:
    // https://kitmedical.github.io/kacanopen/master_2src_2eds__library_8cpp_source.html#l00229
    // the file might need adapting to work properly
    motor.get().load_dictionary_from_library();
    // TODO: check for correct device type...

    std::cout << "RoboyCAN: Found CiA device profile: "
              << motor.get().get_device_profile_number() << std::endl;

    // motor.get()
    //     .setControlMode(Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY)
    //     .match();

    return motor;
  } catch (const kaco::dictionary_error &dict_error) {
    std::cout << "RoboyCan Error - Dictionary Error on EDS loading: "
              << dict_error.what() << std::endl;
    return std::make_pair(RoboyMotorCommandStatus::EDS_ERROR,
                          std::move(config));
  } catch (const kaco::sdo_error &sdo_err) {
    std::cout << "RoboyCan Error - SDO Error on EDS loading: " << sdo_err.what()
              << std::endl;
    return std::make_pair(RoboyMotorCommandStatus::SDO_ERROR, config);
  }
}

RoboyMotorCommandStatus RoboyMotor::moveMotor(Epos2ControlMode &controlMode,
                                              double &&setpoint) {
  switch (controlMode) {
  case Epos2ControlMode::NOINIT:
    return RoboyMotorCommandStatus::CONTROL_MODE_NOT_CONFIGURED;
  case Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY:
    return writeMotorModeProfilePositionAbsoluteImmediately(
        std::move(setpoint), std::move(CachedWrite::NO));
    break;
  default:
    return RoboyMotorCommandStatus::CONTROL_MODE_UNKNOWN;
  }
}

RoboyMotorCommandStatus
RoboyMotor::writeMotorModeProfilePositionAbsoluteImmediately(
    double &&setpoint, CachedWrite &&useCache) {
  // TODO: turn setpoint into encoder ticks

  if (activeController_ !=
      Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY) {

    if (setControlMode(std::move(
            Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY)) !=
        RoboyMotorCommandStatus::OK)
      return RoboyMotorCommandStatus::MOTOR_CONFIGURATION_FAILED;

    if (enableDevice() != RoboyMotorCommandStatus::OK)
      return RoboyMotorCommandStatus::MOTOR_CONFIGURATION_FAILED;

    activeController_ = Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY;
  }
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> sw =
          static_cast<uint16_t>(motor_.get().get_entry("statusword"));
      if (sw.test(3) == true)
        return RoboyMotorCommandStatus::DEVICE_FAULT;
      motor_.get().set_entry("Target position",
                             radToEncoderTics(std::move(setpoint)));
      std::bitset<16> cw =
          static_cast<uint16_t>(motor_.get().get_entry("controlword"));
      cw.set(4);    // Declare new setpoint
      cw.set(5);    // Change set immediately
      cw.reset(6);  // Absolute value
      cw.reset(8);  // Don't halt
      cw.reset(15); // Normal operation, no endless motion
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()));
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> sw = static_cast<uint16_t>(
          motor_.get().get_entry("statusword", kaco::ReadAccessMethod::cache));
      if (sw.test(3) == true)
        return RoboyMotorCommandStatus::DEVICE_FAULT;
      motor_.get().set_entry("Target position",
                             radToEncoderTics(std::move(setpoint)),
                             kaco::WriteAccessMethod::cache);
      std::bitset<16> cw = static_cast<uint16_t>(
          motor_.get().get_entry("controlword", kaco::ReadAccessMethod::cache));
      cw.set(4);    // Declare new setpoint
      cw.set(5);    // Change set immediately
      cw.reset(6);  // Absolute value
      cw.reset(8);  // Don't halt
      cw.reset(15); // Normal operation, no endless motion
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()),
                             kaco::WriteAccessMethod::cache);
    }
  } catch (const kaco::dictionary_error &e) {
    std::cout << "RoboyCan Error - Dictionary Error on enabling Device: "
              << e.what() << std::endl;
    return RoboyMotorCommandStatus::EDS_ERROR;
  } catch (const kaco::sdo_error &e) {
    std::cout << "RoboyCan Error - SDO Error on enabling Devicey: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

RoboyMotorCommandStatus RoboyMotor::enableDevice(CachedWrite &&useCache) {
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> cw =
          static_cast<uint16_t>(motor_.get().get_entry("controlword"));
      // Shutdown the device
      cw.reset(0);
      cw.set(1);
      cw.set(2);
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()));
      // Switch the device on
      cw = static_cast<uint16_t>(motor_.get().get_entry("controlword"));
      cw.set(0);
      cw.set(1);
      cw.set(2);
      cw.set(3);
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()));
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> cw = static_cast<uint16_t>(
          motor_.get().get_entry("controlword", kaco::ReadAccessMethod::cache));
      // Shutdown the device
      cw.reset(0);
      cw.set(1);
      cw.set(2);
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()),
                             kaco::WriteAccessMethod::cache);
      // Switch the device on
      cw = static_cast<uint16_t>(
          motor_.get().get_entry("controlword", kaco::ReadAccessMethod::cache));

      cw.set(0);
      cw.set(1);
      cw.set(2);
      cw.set(3);
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.to_ulong()),
                             kaco::WriteAccessMethod::cache);
    }
  } catch (const kaco::dictionary_error &e) {

    std::cout << "RoboyCan Error - Dictionary Error on enabling Device: "
              << e.what() << std::endl;
    return RoboyMotorCommandStatus::EDS_ERROR;

  } catch (const kaco::sdo_error &e) {

    std::cout << "RoboyCan Error - SDO Error on enabling Devicey: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

RoboyMotorCommandStatus
RoboyMotor::resetFault(CachedWrite &&useCache = CachedWrite::NO) {
  try {
    if (useCache == CachedWrite::NO) {
      std::bitset<16> cw =
          static_cast<uint16_t>(motor_.get().get_entry("controlword"));
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.set(7).to_ulong()));
      enableDevice(CachedWrite::NO);
    } else if (useCache == CachedWrite::YES) {
      std::bitset<16> cw = static_cast<uint16_t>(
          motor_.get().get_entry("controlword", kaco::ReadAccessMethod::cache));
      motor_.get().set_entry("controlword",
                             static_cast<uint16_t>(cw.set(7).to_ulong()),
                             kaco::WriteAccessMethod::cache);
      enableDevice(CachedWrite::YES);
    }
  } catch (const kaco::dictionary_error &e) {

    std::cout << "RoboyCan Error - Dictionary Error on resetFault: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::EDS_ERROR;

  } catch (const kaco::sdo_error &e) {

    std::cout << "RoboyCan Error - SDO Error on resetFaulty: " << e.what()
              << std::endl;
    return RoboyMotorCommandStatus::SDO_ERROR;
  }
  return RoboyMotorCommandStatus::OK;
}

// /** [readPosition reads the position of all motors ]
//  *
//  */
// auto RoboyMotor::getPosition(void) -> variant<double,
// RoboyMotorCommandStatus> {
//   double pos;
//   try {
//     pos = motor_.get_entry("Position Actual Value");
//   } catch (const kaco::dictionary_error &e) {
//     std::cout << "RoboyCan Error - Dictionary Error on getPosition: "
//               << e.what() << std::endl;
//     return RoboyMotorCommandStatus::EDS_ERROR;
//   } catch (const kaco::sdo_error &e) {
//     std::cout << "RoboyCan Error - SDO Error on getPosition: " << e.what()
//               << std::endl;
//     return RoboyMotorCommandStatus::SDO_ERROR;
//   }
//   return std::move(pos);
// }

auto RoboyMotor::getStatus(void)
    -> variant<std::set<Epos2Status>,
               std::pair<RoboyMotorCommandStatus, std::set<Epos2Status>>> {
  std::bitset<16> motorStatus{0};
  std::set<Epos2Status> motorState;
  try {
    motorStatus = static_cast<uint16_t>(motor_.get().get_entry("Statusword"));
  } catch (const kaco::dictionary_error &e) {
    std::cout << "RoboyCan Error - Dictionary Error on getStatus: " << e.what()
              << std::endl;
    return std::make_pair(RoboyMotorCommandStatus::EDS_ERROR,
                          std::move(motorState));
  } catch (const kaco::sdo_error &e) {
    std::cout << "RoboyCan Error - SDO Error on getStatus: " << e.what()
              << std::endl;
    return std::make_pair(RoboyMotorCommandStatus::SDO_ERROR,
                          std::move(motorState));
  }
  for (std::size_t i = 0; i < motorStatus.size(); ++i) {
    if (motorStatus.test(i)) {
      motorState.insert(static_cast<Epos2Status>(i));
    }
  }
  if (motorState.find(Epos2Status::FAULT) != motorState.end()) {
    return std::make_pair(RoboyMotorCommandStatus::DEVICE_FAULT,
                          std::move(motorState));
  } else {
    return std::move(motorState);
  }
}

RoboyMotorCommandStatus
RoboyMotor::setControlMode(Epos2ControlMode &&controlMode) {
  switch (controlMode) {
  case Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY:
    return config_.controllers.at("Profile Position Mode")
        .match(
            [this](ProfilePositionModeConfig ppmc) -> RoboyMotorCommandStatus {
              RoboyMotorCommandStatus status =
                  writeParameterList(motor_, ppmc.getParameterList());
              if (status == RoboyMotorCommandStatus::OK) {
                activeController_ =
                    Epos2ControlMode::PROFILE_POSITION_ABSOLUTE_IMMEDIATELY;
                return status;
              } else {
                return status;
              }
            });
  default:
    return RoboyMotorCommandStatus::CONTROL_MODE_UNKNOWN;
  }
}

RoboyMotorCommandStatus
RoboyMotor::writeParameterList(std::reference_wrapper<kaco::Device> device,
                               MaxonParameterList params) {
  for (auto &param : params) {
    try {
      param.second.match(
          [&param, &device](int16_t in) -> void {
            device.get().set_entry(param.first, static_cast<int16_t>(in));
          },
          [&param, &device](uint16_t in) -> void {
            device.get().set_entry(param.first, static_cast<uint16_t>(in));
          },
          [&param, &device](int32_t in) -> void {
            device.get().set_entry(param.first, static_cast<int32_t>(in));
          },
          [&param, &device](uint32_t in) -> void {
            device.get().set_entry(param.first, static_cast<uint32_t>(in));
          });
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
