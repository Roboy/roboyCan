#include "yaml-cpp/yaml.h"

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"

namespace YAML {
using namespace Maxon;

#define MATCHERROROPTIONS_MACRO                                                \
  [](RoboyParserErrorNotConfigured) -> RoboyParserErrorNotConfigured {         \
    return RoboyParserErrorNotConfigured();                                    \
  },                                                                           \
      [](RoboyParserErrorOtherError) -> RoboyParserErrorOtherError {           \
        return RoboyParserErrorOtherError();                                   \
      },                                                                       \
      [](RoboyParserErrorMotorUsesUndeclaredNetwork)                           \
          -> RoboyParserErrorMotorUsesUndeclaredNetwork {                      \
        return RoboyParserErrorMotorUsesUndeclaredNetwork();                   \
      },                                                                       \
      [](RoboyParserErrorMissingEntry) -> RoboyParserErrorMissingEntry {       \
        return RoboyParserErrorMissingEntry();                                 \
      }

class RoboyParserErrorNotConfigured {};
class RoboyParserErrorOtherError {};
class RoboyParserErrorMotorUsesUndeclaredNetwork {};
class RoboyParserErrorMissingEntry {};
class InvalidBaudrate {};
class InvalidUsbDriver {};

template <typename T> struct empty {};

template <typename T> struct invalid {};

template <typename T> struct missing {};
template <typename T, typename Key> struct duplicate { Key key; };
template <typename Case, typename Variant> auto passAlong(Case c) {
  return Variant{c};
};

template <typename T>
using ConversionTypes =
    variant<RoboyParserErrorNotConfigured, RoboyParserErrorOtherError,
            RoboyParserErrorMotorUsesUndeclaredNetwork,
            RoboyParserErrorMissingEntry, T>;

using Sensors = variant<RoboyParserErrorNotConfigured,
                        RoboyParserErrorMissingEntry, SensorConfig>;

template <> struct convert<Sensors> {
  static bool decode(Node const &node, Sensors &sensor) {
    MaxonParameterList parameters_;
    if (node["Pulse Number Incremental Encoder 1"] &&
        node["Position Sensor Type"]) {
      parameters_.emplace(
          "Pulse Number Incremental Encoder 1",
          node["Pulse Number Incremental Encoder 1"].as<uint32_t>());
      parameters_.emplace("Position Sensor Type",
                          node["Position Sensor Type"].as<uint16_t>());
      sensor = SensorConfig(std::move(parameters_));
    } else
      sensor = RoboyParserErrorMissingEntry();
    return true;
  }
};

using Controllers = variant<RoboyParserErrorNotConfigured,
                            RoboyParserErrorMissingEntry, MaxonControllers>;

template <> struct convert<Controllers> {
  static bool decode(Node const &node, Controllers &controllers) {
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

using Baudrate = variant<invalid<KaCanOpenBaudrate>, KaCanOpenBaudrate>;

template <> struct convert<Baudrate> {
  static bool decode(Node const &node, Baudrate &baud) {
    std::string baudString = node.as<std::string>();

    if (baudString == "10k")
      baud = KaCanOpenBaudrate::Baud10k;
    else if (baudString == "20k")
      baud = KaCanOpenBaudrate::Baud20k;
    else if (baudString == "50k")
      baud = KaCanOpenBaudrate::Baud50k;
    else if (baudString == "100k")
      baud = KaCanOpenBaudrate::Baud100k;
    else if (baudString == "125k")
      baud = KaCanOpenBaudrate::Baud125k;
    else if (baudString == "250k")
      baud = KaCanOpenBaudrate::Baud250k;
    else if (baudString == "500k")
      baud = KaCanOpenBaudrate::Baud500k;
    else if (baudString == "800k")
      baud = KaCanOpenBaudrate::Baud800k;
    else if (baudString == "1M")
      baud = KaCanOpenBaudrate::Baud1M;
    else
      baud = invalid<KaCanOpenBaudrate>{};
    return true;
  }
};

using UsbOptions = variant<invalid<KaCanOpenUsbOptions>, KaCanOpenUsbOptions>;

template <> struct convert<UsbOptions> {
  static bool decode(Node const &node, UsbOptions &usbopt) {
    std::string usboptString = node.as<std::string>();

    if (usboptString == "usbtin")
      usbopt = KaCanOpenUsbOptions::USBTIN;
    else if (usboptString == "peak_linux")
      usbopt = KaCanOpenUsbOptions::PEAK;
    else
      usbopt = invalid<KaCanOpenUsbOptions>{};
    return true;
  }
};

auto growNetwork(Networks previous, YAML::const_iterator subnet) -> Network {
  if (!subnet.second["Baudrate"]) {
    return missing<Baudrate>{};
  }
  if (!subnet.second["Driver"]) {
    return missing<Driver>{};
  }
  if (!subnet.second["USB Serial"]) {
    return missing<std::string>{};
  }

  return subnet.second["Baudrate"].as<Baudrate>().match(
      [](InvalidBaudrate) -> Network { return {invalid<Baudrate>{}}; },
      [](KaCanOpenBaudrate baudr) -> Network {

        return subnet.second["Driver"].as<Baudrate>().match(
            [](invalid<KaCanOpenUsbOptions>) {
              return Network{invalid<KaCanOpenUsbOptions>{}};
            },
            [](KaCanOpenUsbOptions usbopt) -> Network {
              auto serial = subnet.second["USB Serial"].as<std::string>();
              auto key = subnet.first.as<std::string>();
              if (previous
                      .emplace(key,
                               NetworkConfig(subnet, usbopt, baudr, serial))
                      .second) {
                return duplicate<NetworkConfig, std::string>{key};
              }
              return {previous};
            });
      });
}

using Network =
    variant<empty<Network>, Networks, missing<Baudrate>, invalid<Baudrate>,
            missing<KaCanOpenUsbOptions>, invalid<KaCanOpenUsbOptions>,
            duplicate<NetworkConfig, std::string>>;

template <> struct convert<Network> {
  static bool decode(Node const &node, Network &network) {

    network = std::accumulate(
        node.begin(), node.end(), Network{},
        [](Network nw, YAML::const_iterator subnet) {
          return nw.match(
              [](empty<Network>) -> Network {
                return growNetwork(Networks{}, subnet);
              },
              [](Networks previous) -> Network {
                return growNetwork(previous, subnet);
              },
              passAlong<missing<Baudrate>, Network>,
              passAlong<invalid<Baudrate>, Network>,
              passAlong<missing<KaCanOpenUsbOptions>, Network>,
              passAlong<invalid<KaCanOpenUsbOptions>, Network>,
              passAlong<duplicate<NetworkConfig, std::string>, Network>);
        });
    return true;
  }
};

using Maxons = ConversionTypes<MaxonConfig>;

template <> struct convert<Maxons> {
  // todo: check if I can return something other than bool...
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
                   .as<Sensors>()
                   .match([](SensorConfig sc) -> SensorConfig { return sc; },
                          MATCHERROROPTIONS_MACRO);
    else {
      rhs = RoboyParserErrorMissingEntry();
      return true;
    }

    Controllers controllers;
    if (node["Control Mode Configuration"])
      controllers = node["Control Mode Configuration:"].as<Controllers>().match(
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
                            .as<Controllers>()
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
