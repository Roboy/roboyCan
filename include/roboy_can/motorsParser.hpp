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
using MotorConfigVariant = variant<empty<MotorConfig>, MotorConfig>;
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
using MaxonConfigVariant = variant<empty<MaxonConfig>, MaxonConfig>;

// using MCV = MaxonConfigVariant;
// template <> struct convert<MaxonConfigVariant> {
//   static bool decode(Node const &node, MCV &motors) {
//     NetworkVariant nw = node["Network"].as<NetworkVariant>().match(
//         [](empty<Networks> n) -> MCV { passAlong<, MCV>{}; },
//         [&node](Networks nn) -> MCV { // next level
//           return node["Standard Motor Configuration"].as<Sensor>().match(
//               [](empty<SensorConfig>) -> MCV { passAlong<, MCV>{}; },
//               [](missing<MCV>) -> MCV { passAlong<, MCV>{}; },
//               [&node, &nn](MCV sc) -> MCV { // next level
//                 return node["Control Mode Configuration"]
//                     .as<Controllers>()
//                     .match(
//                         [](empty<MaxonControllers>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [&node, &nn, &sc](MaxonControllers mcs) -> MCV {
//                           return node["Maxon"].as<motorNamesVariant>().match(
//                               [](empty<motorNames>) -> MCV {
//                                 passAlong<, MCV>{};
//                               },
//                               [&node, &nn, &sc, &mcs](motorNames mn) -> MCV {
//                                 MAGIC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                               },
//                               [](missing<motorNames>) -> MCV {
//                                 passAlong<, MCV>{};
//                               },
//                               [](duplicate<CanIdNetworktuple, std::string>)
//                                   -> MCV { passAlong<, MCV>{}; });
//                         },
//                         [](empty<MaxonParameterList>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [](missing<MaxonParameterList>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [](missing<uint32_t>) -> MCV { passAlong<, MCV>{}; },
//                         [](invalid<uint32_t>) -> MCV { passAlong<, MCV>{}; },
//                         [](missing<int32_t>) -> MCV { passAlong<, MCV>{}; },
//                         [](invalid<int32_t>) -> MCV { passAlong<, MCV>{}; },
//                         [](empty<MotionProfileTypeValue>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [](missing<MotionProfileTypeValue>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [](invalid<MotionProfileTypeValue>) -> MCV {
//                           passAlong<, MCV>{};
//                         },
//                         [](duplicate<MaxonControllerConfig, std::string>)
//                             -> MCV { passAlong<, MCV>{}; });
//               },
//               [](invalid<EposPulseNumberIncrementalEncoders>) -> MCV {
//                 passAlong<, MCV>{};
//               },
//               [](missing<EposPulseNumberIncrementalEncoders>) -> MCV {
//                 passAlong<, MCV>{};
//               },
//               [](invalid<EposPositionSensorType>) -> MCV {
//                 passAlong<, MCV>{};
//               },
//               [](missing<EposPositionSensorType>) -> MCV {
//                 passAlong<, MCV>{};
//               },
//               [](invalid<KaCanOpenUsbOptions>) -> MCV { passAlong<, MCV>{};
//               });
//         },
//         [](missing<KaCanOpenBaudrate>) -> MCV { passAlong<, MCV>{}; },
//         [](invalid<KaCanOpenBaudrate>) -> MCV { passAlong<, MCV>{}; },
//         [](missing<KaCanOpenUsbOptions>) -> MCV { passAlong<, MCV>{}; },
//         [](invalid<KaCanOpenUsbOptions>) -> MCV { passAlong<, MCV>{}; },
//         [](missing<std::string>) -> MCV { passAlong<, MCV>{}; },
//         [](duplicate<NetworkConfig, std::string>) -> MCV {
//           passAlong<, MCV>{};
//         });
//
//     motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
//   };
// };
}; // end of namespace
