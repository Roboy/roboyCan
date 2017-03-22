#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
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
}; // end of namespace
