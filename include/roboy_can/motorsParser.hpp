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
using motorNames = std::map<std::string, CanIdNetworktuple>;
using motorNamesVariant =
    variant<empty<motorNames>, motorNames, missing<motorNames>,
            duplicate<CanIdNetworktuple, std::string>>;
using MotorConfigVariant =
    variant<empty<MotorConfigs>, MotorConfigs, invalid<MotorConfig>,
            NetworkVariant, SensorVariant, motorNamesVariant>;
// todo: Add MotorConfigvariant options

inline auto growMotorNames(motorNames previous,
                           YAML::const_iterator::value_type subnet)
    -> motorNamesVariant {
  std::set<unsigned int> unique;

  for (auto &mname : subnet.second) {
    CanIdNetworktuple tuple = {mname.first.as<unsigned int>(),
                               subnet.first.as<std::string>()};

    if (unique.emplace(tuple.canId).second == false) {
      std::stringstream temp;
      temp << tuple.network << ": " << tuple.canId;
      return duplicate<CanIdNetworktuple, std::string>{temp.str()};
    }
    previous[mname.second["id"].as<std::string>()] = tuple;
  }
  return {previous};
};

namespace YAML {
template <> struct convert<motorNamesVariant> {
  static bool decode(Node const &node, motorNamesVariant &names) {
    if (!node["Motors"]) {
      names = missing<motorNames>();
      return true;
    }
    Node motorNode = node["Motors"];
    names = std::accumulate(
        motorNode.begin(), motorNode.end(), motorNamesVariant{},
        [](motorNamesVariant nms, YAML::const_iterator::value_type subnet) {
          return nms.match(
              [&subnet](empty<motorNames>) -> motorNamesVariant {
                return growMotorNames(motorNames{}, subnet);
              },
              [&subnet](motorNames previous) -> motorNamesVariant {
                return growMotorNames(previous, subnet);
              },
              passAlong<missing<motorNames>, motorNamesVariant>{},
              passAlong<duplicate<CanIdNetworktuple, std::string>,
                        motorNamesVariant>{});
        });
    return true;
  };
};

using MCV = MotorConfigVariant;
template <> struct convert<MotorConfigVariant> {
  static bool decode(Node const &node, MCV &motorConfig) {
    motorConfig = node.as<NetworkVariant>().match(
        [](empty<Networks> n) -> MCV { passAlong<empty<Networks>, MCV>{}; },
        [&node](Networks nn) -> MCV { // next level
          return node.as<SensorVariant>().match(
              passAlong<empty<SensorConfig>, MCV>{},
              passAlong<missing<SensorConfig>, MCV>{},
              [&node, &nn](SensorConfig sc) -> MCV { // next level
                return node.as<ControllersVariant>().match(
                    [](empty<MaxonControllers>) -> MCV {
                      passAlong<empty<MaxonControllers>, MCV>{};
                    },
                    [&node, &nn, &sc](MaxonControllers mcs) -> MCV {
                      return node.as<motorNamesVariant>().match(
                          [](empty<motorNames>) -> MCV {
                            passAlong<empty<motorNames>, MCV>{};
                          },
                          [&nn, &sc, &mcs](motorNames mn) -> MCV {
                            MotorConfigs motors;
                            for (auto &motor : mn) {
                              if (nn.find(motor.second.network) == nn.end()) {
                                return invalid<MotorConfig>{
                                    motor.first}; // Should mean: invalid
                                // network configuration on
                                // motorname motor.first
                              }
                              motors.emplace(
                                  motor.first,
                                  MotorConfig{motor.first, motor.second.canId,
                                              nn.at(motor.second.network), sc,
                                              mcs});
                            }
                            return motors;
                          },
                          [](missing<motorNames>) -> MCV {
                            passAlong<missing<motorNames>, MCV>{};
                          },
                          [](duplicate<CanIdNetworktuple, std::string>) -> MCV {
                            passAlong<duplicate<CanIdNetworktuple, std::string>,
                                      MCV>{};
                          });
                    },
                    [](missing<MaxonControllers>) -> MCV {
                      passAlong<missing<MaxonControllers>, MCV>{};
                    },
                    [](empty<MaxonParameterList>) -> MCV {
                      passAlong<empty<MaxonParameterList>, MCV>{};
                    },
                    [](missing<MaxonParameterList>) -> MCV {
                      passAlong<missing<MaxonParameterList>, MCV>{};
                    },
                    [](missing<uint32_t>) -> MCV {
                      passAlong<missing<uint32_t>, MCV>{};
                    },
                    [](invalid<uint32_t>) -> MCV {
                      passAlong<invalid<uint32_t>, MCV>{};
                    },
                    [](missing<int32_t>) -> MCV {
                      passAlong<missing<int32_t>, MCV>{};
                    },
                    [](invalid<int32_t>) -> MCV {
                      passAlong<invalid<int32_t>, MCV>{};
                    },
                    [](empty<MotionProfileTypeValue>) -> MCV {
                      passAlong<empty<MotionProfileTypeValue>, MCV>{};
                    },
                    [](missing<MotionProfileTypeValue>) -> MCV {
                      passAlong<missing<MotionProfileTypeValue>, MCV>{};
                    },
                    [](invalid<MotionProfileTypeValue>) -> MCV {
                      passAlong<invalid<MotionProfileTypeValue>, MCV>{};
                    },
                    [](duplicate<MaxonControllerConfig, std::string>) -> MCV {
                      passAlong<duplicate<MaxonControllerConfig, std::string>,
                                MCV>{};
                    });
              },
              [](invalid<EposPulseNumberIncrementalEncoders>) -> MCV {
                passAlong<invalid<EposPulseNumberIncrementalEncoders>, MCV>{};
              },
              [](missing<EposPulseNumberIncrementalEncoders>) -> MCV {
                passAlong<missing<EposPulseNumberIncrementalEncoders>, MCV>{};
              },
              [](invalid<EposPositionSensorType>) -> MCV {
                passAlong<invalid<EposPositionSensorType>, MCV>{};
              },
              [](missing<EposPositionSensorType>) -> MCV {
                passAlong<missing<EposPositionSensorType>, MCV>{};
              },
              [](invalid<KaCanOpenUsbOptions>) -> MCV {
                passAlong<invalid<KaCanOpenUsbOptions>, MCV>{};
              });
        },
        passAlong<missing<Networks>, MCV>{},
        [](missing<KaCanOpenBaudrate>) -> MCV {
          passAlong<missing<KaCanOpenBaudrate>, MCV>{};
        },
        [](invalid<KaCanOpenBaudrate>) -> MCV {
          passAlong<invalid<KaCanOpenBaudrate>, MCV>{};
        },
        [](missing<KaCanOpenUsbOptions>) -> MCV {
          passAlong<missing<KaCanOpenUsbOptions>, MCV>{};
        },
        [](invalid<KaCanOpenUsbOptions>) -> MCV {
          passAlong<invalid<KaCanOpenUsbOptions>, MCV>{};
        },
        [](missing<std::string>) -> MCV {
          passAlong<missing<std::string>, MCV>{};
        },
        [](duplicate<NetworkConfig, std::string>) -> MCV {
          passAlong<duplicate<NetworkConfig, std::string>, MCV>{};
        });
    return true;
  };
};
}; // end of namespace
