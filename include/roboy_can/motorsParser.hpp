#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/controllerParser.hpp"
#include "roboy_can/errorHandling.hpp"
#include "roboy_can/networkParser.hpp"
#include "roboy_can/sensorParser.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <numeric>

struct CanIdNetworktuple {
  unsigned int canId;
  std::string network;
};

using MotorNames = std::map<std::string, CanIdNetworktuple>;
using MotorNamesVariant =
    variant<empty<MotorNames>, invalid<MotorNames>, MotorNames>;
using MotorConfigVariant =
    variant<empty<MotorConfigs>, invalid<MotorConfigs>, MotorConfigs>;

// todo: Add MotorConfigvariant options

inline auto growMotorNames(MotorNames previous,
                           YAML::const_iterator::value_type subnet)
    -> MotorNamesVariant {
  std::set<unsigned int> unique;

  for (auto &mname : subnet.second) {
    CanIdNetworktuple tuple;
    try {
      tuple = {mname.first.as<unsigned int>(), subnet.first.as<std::string>()};
    } catch (const YAML::BadConversion &e) {
      return invalid<MotorNames>{
          "Motors: Values are missing or are not of type unsigned int or "
          "std::string respectively."};
    }

    if (unique.emplace(tuple.canId).second == false) {
      return invalid<MotorNames>{
          std::string("Duplicate CAN ID: " + tuple.network + ": " +
                      std::to_string(tuple.canId) + ".")};
    }
    try {
      previous[mname.second["id"].as<std::string>()] = tuple;
    } catch (const YAML::BadConversion &e) {
      return invalid<MotorNames>{
          "Motors: ID Values are missing or are not of type unsigned int"};
    }
  }
  return {previous};
};

namespace YAML {
template <> struct convert<MotorNamesVariant> {
  static bool decode(Node const &node, MotorNamesVariant &names) {
    if (!node["Motors"]) {
      names = invalid<MotorNames>{"Missing Motor names."};
      return true;
    }
    Node motorNode = node["Motors"];
    names =
        std::accumulate(
            motorNode.begin(), motorNode.end(), MotorNamesVariant{},
            [](MotorNamesVariant nms, YAML::const_iterator::value_type subnet) {
              return nms.match(
                  [&subnet](empty<MotorNames>) -> MotorNamesVariant {
                    return growMotorNames(MotorNames{}, subnet);
                  },
                  [&subnet](invalid<MotorNames> in) -> MotorNamesVariant {
                    return in;
                  },
                  [&subnet](MotorNames previous) -> MotorNamesVariant {
                    return growMotorNames(previous, subnet);
                  });
            })
            .match([](empty<MotorNames> in) -> MotorNamesVariant { return in; },
                   [](invalid<MotorNames> in) -> MotorNamesVariant {
                     return invalid<MotorNames>{
                         std::string("Motors: " + in.reason)};
                   },
                   [](MotorNames in) -> MotorNamesVariant { return in; });
    return true;
  };
};

using MCV = MotorConfigVariant;
template <> struct convert<MotorConfigVariant> {
  static bool decode(Node const &node, MCV &motorConfig) {
    motorConfig = node.as<NetworkVariant>().match(
        [](empty<Networks>) -> MCV {
          return invalid<MotorConfigs>{"Parsing failed for Networks"};
        },
        [](invalid<Networks> in) -> MCV {
          return invalid<MotorConfigs>{in.reason};
        },
        [&node](Networks nn) -> MCV { // next level
          return node.as<SensorVariant>().match(
              [](empty<SensorConfig>) -> MCV {
                return invalid<MotorConfigs>{"Parsing failed for Sensors"};
              },
              [](invalid<SensorConfig> in) -> MCV {
                return invalid<MotorConfigs>{in.reason};
              },
              [&node, &nn](SensorConfig sc) -> MCV { // next level
                GearConfig gears;
                if (!node["Standard Motor Configuration"]
                         ["Gear Configuration"]) {
                  return invalid<MotorConfigs>{"Missing: Gear Configuration."};
                } else {
                  gears =
                      node["Standard Motor Configuration"]["Gear Configuration"]
                          .as<uint16_t>();
                }
                return node.as<ControllersVariant>().match(
                    [](empty<MaxonControllers>) -> MCV {
                      return invalid<MotorConfigs>{
                          "Parsing failed for Controllers"};
                    },
                    [](invalid<MaxonControllers> in) -> MCV {
                      return invalid<MotorConfigs>{in.reason};
                    },
                    [&node, &nn, &sc, &gears](MaxonControllers mcs) -> MCV {
                      return node.as<MotorNamesVariant>().match(
                          [](empty<MotorNames>) -> MCV {
                            return invalid<MotorConfigs>{
                                "Parsing failed for Motors"};
                          },
                          [](invalid<MotorNames> in) -> MCV {
                            return invalid<MotorConfigs>{
                                std::string("Motors: " + in.reason)};
                          },
                          [&nn, &sc, &mcs, &gears](MotorNames mn) -> MCV {
                            MotorConfigs motors;
                            for (auto &motor : mn) {
                              if (nn.find(motor.second.network) == nn.end()) {
                                return invalid<MotorConfigs>{
                                    motor.first}; // Should mean: invalid
                                // network configuration on
                                // motorname motor.first
                              }
                              motors.emplace(
                                  motor.first,
                                  MotorConfig{motor.first, motor.second.canId,
                                              nn.at(motor.second.network), sc,
                                              mcs, gears});
                            }
                            return motors;
                          });
                    });
              });
        });
    return true;
  };
};
}; // end of namespace
