#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <numeric>

using MaxFollowingError =
    variant<missing<uint32_t>, invalid<uint32_t>, uint32_t>;

using PositionLimit = variant<missing<MaxonParameterList>, MaxonParameterList>;

using MaxProfileVelocity = uint32_t;
using ProfileVelocity = uint32_t;
using Velocity =
    variant<missing<uint32_t>, invalid<uint32_t>, MaxonParameterList>;

using Acceleration = variant<empty<MaxonParameterList>, missing<uint32_t>,
                             invalid<uint32_t>, MaxonParameterList>;

using MotionProfileType =
    variant<empty<MotionProfileTypeValue>, missing<MotionProfileTypeValue>,
            invalid<MotionProfileTypeValue>, MotionProfileTypeValue>;

using Controllers =
    variant<empty<MaxonControllers>, MaxonControllers,
            empty<MaxonParameterList>, missing<MaxonParameterList>,
            missing<uint32_t>, invalid<uint32_t>, missing<int32_t>,
            invalid<int32_t>, empty<MotionProfileTypeValue>,
            missing<MotionProfileTypeValue>, invalid<MotionProfileTypeValue>,
            duplicate<MaxonControllerConfig, std::string>>;

inline auto growController(MaxonControllers previous,
                           YAML::const_iterator::value_type subnet)
    -> Controllers {
  if (!subnet.second["Max Following Error"]) {
    return missing<uint32_t>{"Max Following Error"};
  };
  if (!subnet.second["Position"]) {
    return missing<MaxonParameterList>{"Position"};
  };
  if (!subnet.second["Velocity"]) {
    return missing<MaxonParameterList>{"Velocity"};
  };
  if (!subnet.second["Acceleration"]) {
    return missing<MaxonParameterList>{"Acceleration"};
  };

  return subnet.second.as<MaxFollowingError>().match(
      passAlong<missing<uint32_t>, Controllers>{},
      passAlong<invalid<uint32_t>, Controllers>{},
      [&subnet, &previous](uint32_t mfev) -> Controllers {

        return subnet.second.as<PositionLimit>().match(
            passAlong<missing<MaxonParameterList>, Controllers>{},
            [&subnet, &previous, &mfev](MaxonParameterList plv) -> Controllers {

              return subnet.second.as<Velocity>().match(
                  passAlong<missing<uint32_t>, Controllers>{},
                  passAlong<invalid<uint32_t>, Controllers>{},
                  [&subnet, &previous, &mfev,
                   &plv](MaxonParameterList vel) -> Controllers {

                    return subnet.second.as<Acceleration>().match(
                        passAlong<empty<MaxonParameterList>, Controllers>(),
                        passAlong<missing<uint32_t>, Controllers>{},
                        passAlong<invalid<uint32_t>, Controllers>{},
                        [&subnet, &previous, &mfev, &plv,
                         &vel](MaxonParameterList acc) -> Controllers {

                          return subnet.second.as<MotionProfileType>().match(
                              passAlong<empty<MotionProfileTypeValue>,
                                        Controllers>(),
                              passAlong<missing<MotionProfileTypeValue>,
                                        Controllers>(),
                              passAlong<invalid<MotionProfileTypeValue>,
                                        Controllers>(),
                              [&subnet, &previous, &mfev, &plv, &vel, &acc](
                                  MotionProfileTypeValue mpt) -> Controllers {

                                auto key = subnet.first.as<std::string>();
                                if (previous
                                        .emplace(key, ProfilePositionModeConfig(
                                                          std::move(mfev),
                                                          std::move(plv),
                                                          std::move(vel),
                                                          std::move(acc),
                                                          std::move(mpt)))
                                        .second == false) {
                                  return duplicate<MaxonControllerConfig,
                                                   std::string>{key};
                                }
                                return {previous};
                              });
                        });
                  });
            });
      });
};

namespace YAML {
template <> struct convert<MaxFollowingError> {
  static bool decode(Node const &node, MaxFollowingError &mfe) {

    if (!node["Max Following Error"]) {

      mfe = missing<uint32_t>{std::string("MaxFollowingError")};
      return true;
    }

    mfe = withinBounds<uint32_t>(node, "Max Following Error", 0, 4294967295)
              .match(passAlong<invalid<uint32_t>, MaxFollowingError>{},
                     passAlong<uint32_t, MaxFollowingError>{});
    return true;
  };
};

template <> struct convert<PositionLimit> {
  static bool decode(Node const &node, PositionLimit &mfe) {

    if (!node["Position"]["Max Position Limit"]) {
      mfe = missing<MaxonParameterList>{"Max Position Limit"};
      return true;
    }
    if (!node["Position"]["Min Position Limit"]) {
      mfe = missing<MaxonParameterList>{"Min Position Limit"};
      return true;
    }

    mfe = MaxonParameterList{
        {"Max Position Limit",
         node["Position"]["Max Position Limit"].as<int32_t>()},
        {"Min Position Limit",
         node["Position"]["Min Position Limit"].as<int32_t>()}};

    return true;
  };
};

template <> struct convert<Velocity> {
  static bool decode(Node const &node, Velocity &mfe) {
    if (!node["Velocity"]["Max Profile Velocity"]) {
      mfe = missing<MaxProfileVelocity>{};
      return true;
    };
    if (!node["Velocity"]["Profile Velocity"]) {
      mfe = missing<ProfileVelocity>{};
      return true;
    };
    Node n = node["Velocity"];
    MaxProfileVelocity mpv = n["Max Profile Velocity"].as<MaxProfileVelocity>();
    // Limit depends on Maximal Gear Speed and Maximal Motor Speed. See
    // EPOS2
    // Firmware 8.2.106 Maximal Profile Velocity and Table 8-119

    mfe = withinBounds<ProfileVelocity>(n, "Profile Velocity", 1, mpv)
              .match(passAlong<invalid<ProfileVelocity>, Velocity>{},
                     [&mpv](ProfileVelocity value) -> Velocity {
                       return MaxonParameterList{{"Max Profile Velocity", mpv},
                                                 {"Profile Velocity", value}};
                     });
    return true;
  };
};

template <> struct convert<Acceleration> {
  static bool decode(Node const &node, Acceleration &mfe) {
    if (!node["Acceleration"]) {
      return true;
    }
    Node n = node["Acceleration"];
    if (!n["Max Acceleration"]) {
      mfe = missing<uint32_t>{std::string("Max Acceleration")};
      return true;
    }

    if (!n["Profile Acceleration"]) {
      mfe = missing<uint32_t>{std::string("Profile Acceleration")};
      return true;
    }

    if (!n["Profile Deceleration"]) {
      mfe = missing<uint32_t>{std::string("Profile Deceleration")};
      return true;
    }
    if (!n["Quickstop Deceleration"]) {
      mfe = missing<uint32_t>{std::string("Quickstop Deceleration")};
      return true;
    }
    // std::vector<std::string> accelerationParameters = {
    //     "Max Acceleration", "Profile Acceleration", "Profile
    //     Deceleration",
    //     "Quickstop Deceleration"};
    mfe =
        withinBounds<uint32_t>(n, "Max Acceleration", 0, 4294967295)
            .match(passAlong<invalid<uint32_t>, Acceleration>{},
                   [&n](uint32_t mac) -> Acceleration {
                     return withinBounds<uint32_t>(n, "Profile Acceleration", 1,
                                                   mac)
                         .match(
                             passAlong<invalid<uint32_t>, Acceleration>(),
                             [&n, &mac](uint32_t pac) -> Acceleration {
                               return withinBounds<uint32_t>(
                                          n, "Profile Deceleration", 1, mac)
                                   .match(
                                       passAlong<invalid<uint32_t>,
                                                 Acceleration>{},
                                       [&n, &mac,
                                        &pac](uint32_t pdc) -> Acceleration {
                                         return withinBounds<uint32_t>(
                                                    n, "Quickstop Deceleration",
                                                    1, mac)
                                             .match(
                                                 passAlong<invalid<uint32_t>,
                                                           Acceleration>{},
                                                 [&mac, &pac, &pdc](uint32_t qd)
                                                     -> Acceleration {
                                                   return MaxonParameterList{
                                                       {"Max "
                                                        "Acceleration",
                                                        std::move(mac)},
                                                       {"Profile "
                                                        "Acceleration",
                                                        std::move(pac)},
                                                       {"Profile "
                                                        "Deceleration",
                                                        std::move(pdc)},
                                                       {"Quickstop "
                                                        "Deceleration",
                                                        MaxonParameter{qd}}};
                                                 });
                                       });
                             });
                   });
    return true;
  };
};

template <> struct convert<MotionProfileType> {
  static bool decode(Node const &node, MotionProfileType &mfe) {
    if (!node["Motion Profile Type"]) {
      mfe = missing<MotionProfileTypeValue>{};
      return true;
    }
    switch (node["Motion Profile Type"].as<int16_t>()) {
    case 0:
      mfe = MotionProfileTypeValue::LINEAR_RAMP_TRAPEZOIDAL_PROFILE;
      break;
    case 1:
      mfe = MotionProfileTypeValue::SIN2_RAMP_SINUSOIDAL_PROFILE;
      break;
    default:
      mfe = invalid<MotionProfileTypeValue>();
    };
    return true;
  };
};

template <> struct convert<Controllers> {
  static bool decode(Node const &node, Controllers &controllers) {
    controllers = std::accumulate(
        node.begin(), node.end(), Controllers{},
        [](Controllers ctrl, YAML::const_iterator::value_type subnet) {

          return ctrl.match(
              [&subnet](empty<MaxonControllers>) -> Controllers {
                return growController(MaxonControllers{}, subnet);

              },
              [&subnet](MaxonControllers previous) -> Controllers {

                return growController(previous, subnet);
              },
              passAlong<empty<MaxonParameterList>, Controllers>{},
              passAlong<missing<MaxonParameterList>, Controllers>{},
              passAlong<missing<uint32_t>, Controllers>{},
              passAlong<invalid<uint32_t>, Controllers>{},
              passAlong<missing<int32_t>, Controllers>{},
              passAlong<invalid<int32_t>, Controllers>{},
              passAlong<empty<MotionProfileTypeValue>, Controllers>{},
              passAlong<missing<MotionProfileTypeValue>, Controllers>{},
              passAlong<invalid<MotionProfileTypeValue>, Controllers>{},
              passAlong<duplicate<MaxonControllerConfig, std::string>,
                        Controllers>{});
        });
    return true;
  };
};
}; // end of namespace YAML
