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
    variant<empty<MotionProfileTypeValue>, missing<MotionProfileTypeValue>,
            invalid<MotionProfileTypeValue>, MotionProfileTypeValue>;

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
    if (!node["Position"]["Max Position Limit"]) {
      mfe = missing<int32_t>{"Max Position Limit"};
      return true;
    }
    if (!node["Position"]["Min Position Limit"]) {
      mfe = missing<int32_t>{"Min Position Limit"};
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
    mfe = withinBounds<uint32_t>(n, "Max Acceleration", 0, 4294967295)
              .match(
                  passAlong<invalid<uint32_t>, Acceleration>{},
                  [&n](uint32_t mac) -> Acceleration {
                    return withinBounds<uint32_t>(n, "Profile Acceleration", 1,
                                                  mac)
                        .match(
                            passAlong<invalid<uint32_t>, Acceleration>(),
                            [&n, mac](uint32_t pac) -> Acceleration {
                              return withinBounds<uint32_t>(
                                         n, "Profile Deceleration", 1, mac)
                                  .match(
                                      passAlong<invalid<uint32_t>,
                                                Acceleration>{},
                                      [&n, mac,
                                       pac](uint32_t pdc) -> Acceleration {
                                        return withinBounds<uint32_t>(
                                                   n, "Quickstop Deceleration",
                                                   1, mac)
                                            .match(passAlong<invalid<uint32_t>,
                                                             Acceleration>{},
                                                   [mac, pac, pdc](uint32_t qd)
                                                       -> Acceleration {
                                                     return {MaxonParameterList{
                                                         {"Max "
                                                          "Acceleration",
                                                          mac},
                                                         {"Profile "
                                                          "Acceleration",
                                                          pac},
                                                         {"Profile "
                                                          "Deceleration",
                                                          pdc},
                                                         {"Quickstop "
                                                          "Deceleration ",
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
    if (!node["Motion Profile Type"]) {
      mfe = missing<MotionProfileTypeValue>();
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
}; // end of namespace YAML
