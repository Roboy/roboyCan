#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <numeric>

using motorNames = std::map<std::string, std::pair<unsigned int, std::string>>;
using motorNamesVariant =
    variant<empty<motorNames>, motorNames, missing<motorNames>>;

auto growMotorNames(motorNames previous,
                    YAML::const_iterator::value_type subnet)
    -> motorNamesVariant {
  std::string networkName = subnet.first.as<std::string>();
  motorNames names;
  for (auto &mname : subnet.second) {
    names[mname.second["id"].as<std::string>()] =
        std::make_pair(mname.first.as<unsigned int>(), networkName);
  }
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
              passAlong<missing<motorNames>, motorNamesVariant>{});
        });
    return true;
  };
};
}; // end of namespace
