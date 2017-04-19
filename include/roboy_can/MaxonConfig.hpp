#pragma once

#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include <map>
#include <vector>
struct PositionConfig {
  int32_t minPositionLimit;
  int32_t maxPositionLimit;
};
using PositionConfigVariant =
    variant<empty<PositionConfig>, invalid<PositionConfig>, PositionConfig>;

struct VelocityConfig {
  uint32_t maxProfileVelocity;
  uint32_t profileVelocity;
};
using VelocityConfigVariant =
    variant<empty<VelocityConfig>, invalid<VelocityConfig>, VelocityConfig>;

struct AccelerationConfig {
  uint32_t maxAcceleration;
  uint32_t profileAcceleration;
  uint32_t profileDeceleration;
  uint32_t quickstopDeceleration;
};
using AccelerationConfigVariant =
    variant<empty<AccelerationConfig>, invalid<AccelerationConfig>,
            AccelerationConfig>;

struct MaxFollowingErrorConfig {
  uint32_t maxFollowingError;
};
using MaxFollowingErrorConfigVariant =
    variant<empty<MaxFollowingErrorConfig>, invalid<MaxFollowingErrorConfig>,
            MaxFollowingErrorConfig>;

using EposPulseNumberIncrementalEncoders = uint32_t;
enum class EposPositionSensorType : uint16_t {
  UNKNOWN,
  INC_ENCODER_1_W_INDEX_3CH,
  INC_ENCODER_1_WO_INDEX_2CH,
  HALL_SENSORS,
  ABS_ENCODER_SSI, // only available on EPOS2 70/10, 50/5
  RESERVED,
  INC_ENCODER_2_W_INDEX_3CH, // only available on EPOS2 70/10, 50/5
  INC_ENCODER_2_W_INDEX_2CH, // only available on EPOS2 70/10, 50/5 and EPOS2
                             // Module
  SINUS_INC_ENCODER_2
};

using MaxonParameterVariant = variant<int16_t, uint16_t, int32_t, uint32_t>;

using MaxonParameterList = std::map<std::string, MaxonParameterVariant>;

enum class KaCanOpenUsbOptions { USBTIN, PEAK };

enum class KaCanOpenBaudrate : unsigned int {
  Baud10k,
  Baud20k,
  Baud50k,
  Baud100k,
  Baud125k,
  Baud250k,
  Baud500k,
  Baud800k,
  Baud1M
};

using PositionSensorType =
    variant<empty<EposPositionSensorType>, invalid<EposPositionSensorType>,
            EposPositionSensorType>;

enum class MotionProfileType : int16_t {
  LINEAR_RAMP_TRAPEZOIDAL_PROFILE,
  SIN2_RAMP_SINUSOIDAL_PROFILE
};
using MotionProfileTypeVariant =
    variant<empty<MotionProfileType>, invalid<MotionProfileType>,
            MotionProfileType>;

class SensorConfig {
public:
  SensorConfig() = default;

  inline SensorConfig(EposPulseNumberIncrementalEncoders pnie1,
                      EposPositionSensorType pst) {
    parameters_["Pulse Number Incremental Encoder 1"] = {pnie1};
    parameters_["Position Sensor Type"] = static_cast<uint16_t>(pst);
  };

  inline MaxonParameterList getParameterList(void) { return parameters_; };

  inline MaxonParameterVariant getParameter(std::string name) {
    return parameters_.at(name);
  };

private:
  MaxonParameterList parameters_;
};

class ProfilePositionModeConfig {
public:
  inline ProfilePositionModeConfig(MaxFollowingErrorConfig mfeIn,
                                   PositionConfig plvIn, VelocityConfig velIn,
                                   AccelerationConfig accIn,
                                   MotionProfileType mptIn)
      : mfe{std::move(mfeIn)}, pcfg{std::move(plvIn)}, vcfg{std::move(velIn)},
        acfg{std::move(accIn)}, mpt{std::move(mptIn)} {};

  const std::string type = "Profile Position Mode";
  inline auto getParameterList(void) -> MaxonParameterList {
    return {{"Max Following Error", uint32_t(mfe.maxFollowingError)},
            {"Min Position Limit", int32_t(pcfg.minPositionLimit)},
            {"Max Position Limit", int32_t(pcfg.maxPositionLimit)},
            {"Max Profile Velocity", uint32_t(vcfg.maxProfileVelocity)},
            {"Profile Velocity", uint32_t(vcfg.profileVelocity)},
            {"Max Acceleration", uint32_t(acfg.maxAcceleration)},
            {"Profile Acceleration", uint32_t(acfg.profileAcceleration)},
            {"Profile Deceleration", uint32_t(acfg.profileDeceleration)},
            {"Quickstop Deceleration", uint32_t(acfg.quickstopDeceleration)},
            {"Motion Profile Type", int16_t(mpt)}};
  };

private:
  MaxFollowingErrorConfig mfe;
  PositionConfig pcfg;
  VelocityConfig vcfg;
  AccelerationConfig acfg;
  MotionProfileType mpt;
};

using MaxonControllerConfig = variant<ProfilePositionModeConfig>;
using MaxonControllers = std::map<std::string, MaxonControllerConfig>;
using ControllersVariant = variant<empty<MaxonControllers>,
                                   invalid<MaxonControllers>, MaxonControllers>;
class NetworkConfig {
public:
  NetworkConfig() = default;
  inline NetworkConfig(std::string tag, KaCanOpenUsbOptions d,
                       KaCanOpenBaudrate br, std::string sn) {
    yamlTag_ = tag;
    driver_ = d;
    baudrate_ = br;
    usbSerialNumber_ = sn;
  };
  inline KaCanOpenUsbOptions getDriver(void) { return driver_; };
  inline KaCanOpenBaudrate getBaudrate(void) { return baudrate_; };
  inline std::string getUsbSerial(void) { return usbSerialNumber_; };
  inline std::string getNetworkName(void) { return yamlTag_; };

private:
  KaCanOpenUsbOptions driver_;
  KaCanOpenBaudrate baudrate_;
  std::string usbSerialNumber_;
  std::string yamlTag_;
};
using Networks = std::map<std::string, NetworkConfig>;

struct MotorConfig {
  std::string name;
  unsigned int canId;
  NetworkConfig network;
  SensorConfig sensor;
  MaxonControllers controllers;
};

using MotorConfigs = std::map<std::string, MotorConfig>;

class RoboyConfig {
public:
  RoboyConfig() = default;
  MotorConfigs configs;
  inline auto getNetworks(void) -> std::set<std::string> {
    std::set<std::string> networkSet;
    for (auto &motor : configs) {
      networkSet.emplace(motor.second.network.getNetworkName());
    }
    return networkSet;
  };
};
