
#pragma once

#include "yaml-cpp/yaml.h"

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"

namespace YAML {
using namespace Maxon;

class RoboyParserErrorNotConfigured {};
class RoboyParserErrorOtherError {};
class RoboyParserErrorMotorUsesUndeclaredNetwork {};
class RoboyParserErrorMissingEntry {};
class InvalidBaudrate {};
class InvalidUsbDriver {};

template <typename T>
using ConversionTypes =
    variant<RoboyParserErrorNotConfigured, RoboyParserErrorOtherError,
            RoboyParserErrorMotorUsesUndeclaredNetwork,
            RoboyParserErrorMissingEntry, T>;

template <> struct convert<SensorVariant> {
  static bool decode(Node const &node, SensorVariant &sensor) {
    std::vector<Node> nodes = {
        foo<uint32_t>{node["Pulse Number Incremental Encoder 1"], 16, 2500000},
        foo<uint16_t>{node["Position Sensor Type"], 0, 8}};

    sensor = std::accumulate(nodes.begin(), nodes.end(), Sensors{}, []() {});
    MaxonParameterList parameters_;
    if (node["Pulse Number Incremental Encoder 1"]) {
      return missing<SensorConfig>{"Pulse Number Incremental Encoder 1"};
    }
    if (node["Position Sensor Type"]) {
      return missing<SensorConfig>{"Position Sensor Type"};
    }

#define LOWER_BOUND = 16
#define UPPER_BOUND = 2500000
    uint32_t pnie = node["Pulse Number Incremental Encoder 1"].as<uint32_t>());
    if (pnie < LOWER_BOUND || pnie > UPPER_BOUND) {
      return invalid<SensorConfig>{"Pulse Number Incremental Encoder 1"};
    } else
      parameters_.emplace("Pulse Number Incremental Encoder 1", pnie);
#undef LOWER_BOUND
#undef UPPER_BOUND

#define LOWER_BOUND = 0
#define UPPER_BOUND = 8
    uint16_t pst = node["Position Sensor Type"].as<uint16_t>();
    if (pst < LOWER_BOUND || pst > UPPER_BOUND) {
      return invalid<SensorConfig>{"Position Sensor Type"};
    } else
      parameters_.emplace("Position Sensor Type", pst);
#undef LOWER_BOUND
#undef UPPER_BOUND
    sensor = SensorConfig(std::move(parameters_));

    return true;
  }
};

using ControllersVariant =
    variant<empty<MaxonControllers>, missing<MaxonParameterVariant>,
            invalid<MaxonParameterVariant>, RoboyParserErrorMissingEntry,
            MaxonControllers>;

template <> struct convert<ControllersVariant> {
  static bool decode(Node const &node, ControllersVariant &controllers) {
    if (node["Profile Position Mode"]) {
      MaxonParameterList parameters;

      if (!node["Max Following Error"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Max Following Error",
                         node["Max Following Error"].as<uint32_t>());

      if (node["Min Position Limit"]) { // optional parameter
        parameters.emplace("Min Position Limit",
                           node["Min Position Limit"].as<int32_t>());
      }

      if (node["Max Position Limit"]) { // optional parameter
        parameters.emplace("Max Position Limit",
                           node["Max Position Limit"].as<int32_t>());
      }

      if (!node["Max Profile Velocity"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Max Profile Velocity",
                         node["Max Profile Velocity"].as<uint32_t>());

      if (!node["Profile Velocity"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Profile Velocity",
                         node["Profile Velocity"].as<uint32_t>());

      if (!node["Profile Acceleration"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Profile Acceleration",
                         node["Profile Acceleration"].as<uint32_t>());

      if (!node["Profile Deceleration"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Profile Deceleration",
                         node["Profile Deceleration"].as<uint32_t>());

      if (node["Quickstop Deceleration"]) { // optional parameter
        parameters.emplace("Quickstop Deceleration",
                           node["Quickstop Deceleration"].as<uint32_t>());
      }

      if (!node["Motion Profile Type"]) {
        controllers = RoboyParserErrorMissingEntry();
        return true;
      }
      parameters.emplace("Motion Profile Type",
                         node["Motion Profile Type"].as<int16_t>());

      controllers = MaxonControllers{{"Profile Position Mode", parameters}};
      return true;
    }

    controllers = RoboyParserErrorMissingEntry();
    return true;
  }
};

using Maxons = ConversionTypes<MaxonConfig>;

template <> struct convert<Maxons> {
  static bool decode(Node const &node, Maxons &rhs) {

    Networks networks;
    if (node["Network"])
      networks = node["Network"].as<Networks>().match(
          [](Networks nw) -> Networks { return nw; }, MATCHERROROPTIONS_MACRO);
    else {
      rhs = RoboyParserErrorMissingEntry();
      return true;
    }

    Sensors sensor;
    if (node["Standard Motor Configuration"]["Sensor Configuration"])
      sensor = node["Standard Motor Configuration"]["Sensor Configuration"]
                   .as<SensorVariant>()
                   .match([](SensorConfig sc) -> SensorConfig { return sc; },
                          MATCHERROROPTIONS_MACRO);
    else {
      rhs = RoboyParserErrorMissingEntry();
      return true;
    }

    ControllersVariant controllers;
    if (node["Control Mode Configuration"])
      controllers =
          node["Control Mode Configuration:"].as<ControllersVariant>().match(
              [](MaxonControllers mc) -> MaxonControllers { return mc; },
              MATCHERROROPTIONS_MACRO);
    else {
      rhs = RoboyParserErrorMissingEntry();
      return true;
    }

    Node motors = node["Motors"];
    for (YAML::const_iterator itSubnet = motors.begin();
         itSubnet != motors.end(); ++itSubnet) {
      if (networks.find(itSubnet->first.as<std::string>()) == networks.end()) {
        rhs = RoboyParserErrorMotorUsesUndeclaredNetwork();
        return true;
      }
      for (YAML::const_iterator itMotors = itSubnet->second.begin();
           itMotors != itSubnet->second.end(); ++itMotors) {

        if (itMotors->second["Sensor Configuration"]) { // Look for overriding
                                                        // sensor configs
          sensor = itMotors->second["Sensor Configuration"].as<Sensors>().match(
              [](SensorConfig sc) -> SensorConfig { return sc; },
              MATCHERROROPTIONS_MACRO);
        }
        if (itMotors->second["Control Mode Configuration"]) { // Look for
                                                              // overriding
                                                              // motor configs
          controllers = itMotors->second["Control Mode Configuration"]
                            .as<ControllersVariant>()
                            .match(
                                [](MaxonControllers mc) -> MaxonControllers {
                                  return mc;
                                },
                                MATCHERROROPTIONS_MACRO);
        }
        rhs = MaxonConfig();
        rhs.motors_.addMotor(MotorConfig(
            itMotors->second["id"].as<std::string>(),
            itMotors->first.as<uint8_t>(),
            networks[itSubnet->first.as<std::string>()], sensor, controllers));
      }
    }
    return true;
  }
};

using Roboy = ConversionTypes<RoboyConfig>;
template <> struct convert<Roboy> {

  static bool decode(Node const &node, Roboy &rhs) {
    Roboy rhs;
    rhs.configs.clear();
    if (node["Maxon"]) {
      rhs = node["Maxon"].as<Maxons>().match(
          [](MaxonConfig conf) -> Maxons {
            RoboyConfig rb;
            rb.configs[MotorControlArchitecture::MAXON] = conf;
            return rb;
          },
          MATCHERROROPTIONS_MACRO);
      return true;
    }
    rhs = RoboyParserErrorMissingEntry();
    return true;
  };
} // end of YAML namespace
