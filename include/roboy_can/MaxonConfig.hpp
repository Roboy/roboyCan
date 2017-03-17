#pragma once

#include "roboy_can/Types.hpp"
#include <map>
#include <vector>

enum class MotorControlArchitecture { MAXON, FLEXRAY, SPI };

using MaxonParameter = variant<int16_t, uint32_t, int32_t>;

using MaxonParameterList =
    std::map<std::string, variant<int16_t, uint32_t, int32_t>>;

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

class SensorConfig {
public:
  SensorConfig() = default;
  inline SensorConfig(MaxonParameterList &&pms) {
    parameters_ = std::move(pms);
  };
  inline MaxonParameterList getParameterList(void) { return parameters_; };
  inline MaxonParameter getParameter(std::string name) {
    return parameters_[name];
  };
  inline void setParameter(std::string name, MaxonParameter mp) {
    parameters_[name] = mp;
  };

private:
  MaxonParameterList parameters_ = {{"Pulse Number Incremental Encoder 1", 512},
                                    {"Position Sensor Type", 2}};
};

class ProfilePositionModeConfig {
public:
  ProfilePositionModeConfig() = default;
  inline ProfilePositionModeConfig(MaxonParameterList &&pms) {
    parameters_ = std::move(pms);
  };

  const std::string type = "Profile Position Mode";
  inline MaxonParameterList getParameterList(void) { return parameters_; };
  inline MaxonParameter getParameter(std::string name) {
    return (parameters_.at(name));
  };
  inline void setParameter(std::string name, MaxonParameter mp) {
    parameters_[name] = mp;
  };

private:
  MaxonParameterList parameters_ = {
      {"Max Following Error", uint32_t(2000)},
      {"Min Position Limit", int32_t(-2147483648)},
      {"Max Position Limit", int32_t(2147483648)},
      {"Max Profile Velocity", uint32_t(25000)},
      {"Profile Velocity", uint32_t(1000)},
      {"Profile Acceleration", uint32_t(10000)},
      {"Profile Deceleration", uint32_t(10000)},
      {"Quickstop Deceleration", uint32_t(10000)},
      {"Motion Profile Type", int16_t(25000)}};
};

using MaxonControllerConfig = variant<ProfilePositionModeConfig>;
using MaxonControllers = std::map<std::string, MaxonControllerConfig>;

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

private:
  KaCanOpenUsbOptions driver_;
  KaCanOpenBaudrate baudrate_;
  std::string usbSerialNumber_;
  std::string yamlTag_;
};
using Networks = std::map<std::string, NetworkConfig>;

class MotorConfig {
public:
  MotorConfig() = default;
  inline MotorConfig(std::string n, unsigned int cid, NetworkConfig nw,
                     SensorConfig sc, MaxonControllers ctrls) {
    name = n;
    canId = cid;
    network = nw;
    sensor = sc;
    controllers = ctrls;
  };
  std::string name;
  unsigned int canId;
  NetworkConfig network;
  SensorConfig sensor;
  MaxonControllers controllers;
};

class MaxonConfig {
public:
  MaxonConfig() = default;
  inline MaxonConfig(std::vector<MotorConfig> mcs) {
    for (auto &&mc : mcs) {
      addMotor(std::move(mc));
    }
  };
  inline void addMotor(MotorConfig &&mc) { motors_[mc.name] = std::move(mc); };

  std::map<std::string, MotorConfig> motors_;
};

using Maxon = std::map<MotorControlArchitecture, variant<MaxonConfig>>;

class RoboyConfig {
public:
  RoboyConfig() = default;
  Maxon configs;
};
