#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <numeric>

using MaxFollowingErrorValue = uint32_t;
using MaxFollowingError =
    variant<missing<MaxFollowingErrorValue>, invalid<MaxFollowingErrorValue>,
            MaxFollowingErrorValue>;

using PositionLimitValue = MaxonParameterList;
using PositionLimit = variant<missing<int32_t>, PositionLimitValue>;

using MaxProfileVelocity = uint32_t;
using ProfileVelocity = uint32_t;
using Velocity = variant<missing<MaxProfileVelocity>, missing<ProfileVelocity>,
                         invalid<ProfileVelocity>, MaxonParameterList>;

using Acceleration = variant<empty<MaxonParameterList>, missing<uint32_t>,
                             invalid<uint32_t>, MaxonParameterList>;

enum class MotionProfileTypeValue : int16_t {
  LINEAR_RAMP_TRAPEZOIDAL_PROFILE,
  SIN2_RAMP_SINUSOIDAL_PROFILE
};
using MotionProfileType =
    variant<invalid<MotionProfileTypeValue>, MotionProfileTypeValue>;

using Controllers =
    variant<empty<MaxonControllers>, missing<MaxonControllers>,
            MaxonControllers, missing<ProfilePositionModeConfig>,
            missing<MaxFollowingErrorValue>, invalid<MaxFollowingErrorValue>,
            missing<PositionLimitValue>, invalid<PositionLimitValue>,
            missing<MaxProfileVelocity>, invalid<MaxProfileVelocity>,
            missing<ProfileVelocity>, invalid<ProfileVelocity>,
            missing<uint32_t>, invalid<uint32_t>, missing<uint32_t>,
            invalid<uint32_t>, missing<uint32_t>, invalid<uint32_t>,
            missing<MotionProfileTypeValue>, invalid<MotionProfileTypeValue>>;

namespace YAML {
template <> struct convert<MaxFollowingError> {
  static bool decode(Node const &node, MaxFollowingError &mfe) {
    if (!node["Max Following Error"]) {
      mfe = missing<MaxFollowingErrorValue>{std::string("MaxFollowingError")};
      return true;
    }
    mfe =
        withinBounds<MaxFollowingErrorValue>(node, "Max Following Error", 0,
                                             4294967295)
            .match(
                passAlong<invalid<MaxFollowingErrorValue>, MaxFollowingError>{},
                passAlong<MaxFollowingErrorValue, MaxFollowingError>{});
    return true;
  };
};

template <> struct convert<PositionLimit> {
  static bool decode(Node const &node, PositionLimit &mfe) {
    if (!node["Max Position Limit"]) {
      mfe = missing<int32_t>{"Max Position Limit"};
      return true;
    }
    if (!node["Min Position Limit"]) {
      mfe = missing<int32_t>{"Min Position Limit"};
      return true;
    }

    mfe = MaxonParameterList{
        {"Max Position Limit", node["Max Position Limit"].as<int32_t>()},
        {"Min Position Limit", node["Min Position Limit"].as<int32_t>()}};
    std::cout << "ok" << std::endl;

    return true;
  };
};

template <> struct convert<Velocity> {
  static bool decode(Node const &node, Velocity &mfe) {
    if (!node["Max Profile Velocity"]) {
      mfe = missing<MaxProfileVelocity>{};
      return true;
    };
    if (!node["Profile Velocity"]) {
      mfe = missing<ProfileVelocity>{};
      return true;
    };
    MaxProfileVelocity mpv =
        node["Max Profile Velocity"].as<MaxProfileVelocity>();
    // Limit depends on Maximal Gear Speed and Maximal Motor Speed. See EPOS2
    // Firmware 8.2.106 Maximal Profile Velocity and Table 8-119

    mfe = withinBounds<ProfileVelocity>(node, "Profile Velocity", 1, mpv)
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
    if (!node["Max Acceleration"]) {
      mfe = missing<uint32_t>{std::string("Max Acceleration")};
      return true;
    }

    if (!node["Profile Acceleration"]) {
      mfe = missing<uint32_t>{std::string("Profile Acceleration")};
      return true;
    }

    if (!node["Profile Deceleration"]) {
      mfe = missing<uint32_t>{std::string("Profile Deceleration")};
      return true;
    }
    if (!node["Quickstop Deceleration"]) {
      mfe = missing<uint32_t>{std::string("Quickstop Deceleration")};
      return true;
    }
    // std::vector<std::string> accelerationParameters = {
    //     "Max Acceleration", "Profile Acceleration", "Profile Deceleration",
    //     "Quickstop Deceleration"};
    mfe =
        withinBounds<uint32_t>(node, "Max Acceleration", 0, 4294967295)
            .match(
                passAlong<invalid<uint32_t>, Acceleration>{},
                [&node](uint32_t mac) -> Acceleration {
                  return withinBounds<uint32_t>(node, "Profile Acceleration", 1,
                                                mac)
                      .match(
                          passAlong<invalid<uint32_t>, Acceleration>(),
                          [&node, mac](uint32_t pac) -> Acceleration {
                            return withinBounds<uint32_t>(
                                       node, "Profile Deceleration", 1, mac)
                                .match(
                                    passAlong<invalid<uint32_t>,
                                              Acceleration>{},
                                    [&node, mac,
                                     pac](uint32_t pdc) -> Acceleration {
                                      return withinBounds<uint32_t>(
                                                 node, "Quickstop Deceleration",
                                                 1, mac)
                                          .match(
                                              passAlong<invalid<uint32_t>,
                                                        Acceleration>{},
                                              [mac, pac, pdc](
                                                  uint32_t qd) -> Acceleration {
                                                return {MaxonParameterList{
                                                    {"Max Acceleration", mac},
                                                    {"Profile Acceleration",
                                                     pac},
                                                    {"Profile Deceleration",
                                                     pdc},
                                                    {"Quickstop Deceleration ",
                                                     qd}}};
                                              });
                                    });
                          });
                });

    return true;
  };
};

template <> struct convert<MotionProfileType> {
  static bool decode(Node const &node, MotionProfileType &mfe) {
    int16_t value = node.as<int16_t>();
    switch (value) {
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
}; // end of namespace YAML
