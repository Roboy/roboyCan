#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <iostream>
#include <numeric>

inline auto growController(MaxonControllers previous,
                           YAML::const_iterator::value_type subnet)
    -> ControllersVariant {
  if (!subnet.second["Max Following Error"]) {
    return invalid<MaxonControllers>{"Missing Max Following Error."};
  };
  if (!subnet.second["Position"]) {
    return invalid<MaxonControllers>{"Missing Position."};
  };
  if (!subnet.second["Velocity"]) {
    return invalid<MaxonControllers>{"Missing Velocity."};
  };
  if (!subnet.second["Acceleration"]) {
    return invalid<MaxonControllers>{"Missing Acceleration."};
  };

  return subnet.second.as<MaxFollowingErrorConfigVariant>().match(
      [](empty<MaxFollowingErrorConfig>) -> ControllersVariant {
        return invalid<MaxonControllers>{
            std::string("Parsing failed for Max Following Error.")};
      },
      [](invalid<MaxFollowingErrorConfig> in) -> ControllersVariant {
        return invalid<MaxonControllers>{in.reason};
      },
      [&subnet, &previous](MaxFollowingErrorConfig mfev) -> ControllersVariant {
        return subnet.second.as<PositionConfigVariant>().match(
            [](empty<PositionConfig>) -> ControllersVariant {
              return invalid<MaxonControllers>{
                  std::string("Parsing failed for Position.")};
            },
            [](invalid<PositionConfig> in) -> ControllersVariant {
              return invalid<MaxonControllers>{in.reason};
            },
            [&subnet, &previous,
             &mfev](PositionConfig plv) -> ControllersVariant {
              return subnet.second.as<VelocityConfigVariant>().match(
                  [](empty<VelocityConfig>) -> ControllersVariant {
                    return invalid<MaxonControllers>{
                        std::string("Parsing failed for Velocity.")};
                  },
                  [](invalid<VelocityConfig> in) -> ControllersVariant {
                    return invalid<MaxonControllers>{in.reason};
                  },
                  [&subnet, &previous, &mfev,
                   &plv](VelocityConfig vel) -> ControllersVariant {
                    return subnet.second.as<AccelerationConfigVariant>().match(
                        [](empty<AccelerationConfig>) -> ControllersVariant {
                          return invalid<MaxonControllers>{
                              std::string("Parsing failed for Acceleration.")};
                        },
                        [](invalid<AccelerationConfig> in)
                            -> ControllersVariant {
                          return invalid<MaxonControllers>{in.reason};
                        },
                        [&subnet, &previous, &mfev, &plv,
                         &vel](AccelerationConfig acc) -> ControllersVariant {
                          return subnet.second.as<MotionProfileTypeVariant>()
                              .match(
                                  [](empty<MotionProfileType>)
                                      -> ControllersVariant {
                                    return invalid<MaxonControllers>{
                                        std::string("Parsing failed for Motion "
                                                    "Profile Type.")};
                                  },
                                  [](invalid<MotionProfileType> in)
                                      -> ControllersVariant {
                                    return invalid<MaxonControllers>{in.reason};
                                  },
                                  [&subnet, &previous, &mfev, &plv, &vel,
                                   &acc](MotionProfileType mpt)
                                      -> ControllersVariant {
                                    auto key = subnet.first.as<std::string>();
                                    if (previous
                                            .emplace(key,
                                                     ProfilePositionModeConfig(
                                                         std::move(mfev),
                                                         std::move(plv),
                                                         std::move(vel),
                                                         std::move(acc),
                                                         std::move(mpt)))
                                            .second == false) {
                                      return invalid<MaxonControllers>{
                                          key + " : " + "is duplicated."};
                                    }
                                    return {previous};
                                  });
                        });
                  });
            });
      });
};

namespace YAML {
template <> struct convert<MaxFollowingErrorConfigVariant> {
  static bool decode(Node const &node, MaxFollowingErrorConfigVariant &mfe) {

    if (!node["Max Following Error"]) {
      mfe = invalid<MaxFollowingErrorConfig>{"Missing Max Following Error."};
      return true;
    }

    mfe = withinBounds<uint32_t>(node, "Max Following Error", 0, 4294967295)
              .match(
                  [](invalid<uint32_t> in) -> MaxFollowingErrorConfigVariant {
                    return invalid<MaxFollowingErrorConfig>{in.reason};
                  },
                  [](uint32_t in) -> MaxFollowingErrorConfigVariant {
                    return MaxFollowingErrorConfig{in};
                  });
    return true;
  };
};

template <> struct convert<PositionConfigVariant> {
  static bool decode(Node const &node, PositionConfigVariant &mfe) {

    if (!node["Position"]["Max Position Limit"]) {
      mfe = invalid<PositionConfig>{"Missing Max Position Limit"};
      return true;
    }
    if (!node["Position"]["Min Position Limit"]) {
      mfe = invalid<PositionConfig>{"Missing Min Position Limit"};
      return true;
    }
    PositionConfig pc;
    pc.maxPositionLimit = node["Position"]["Max Position Limit"].as<int32_t>();
    pc.minPositionLimit = node["Position"]["Min Position Limit"].as<int32_t>();
    mfe = std::move(pc);
    return true;
  };
};

template <> struct convert<VelocityConfigVariant> {
  static bool decode(Node const &node, VelocityConfigVariant &mfe) {
    if (!node["Velocity"]) {
      mfe = invalid<VelocityConfig>{"Missing Velocity"};
      return true;
    };
    if (!node["Velocity"]["Max Profile Velocity"]) {
      mfe = invalid<VelocityConfig>{"Velocity: Missing Max Profile Velocity"};
      return true;
    };
    if (!node["Velocity"]["Profile Velocity"]) {
      mfe = invalid<VelocityConfig>{"Velocity: Missing Profile Velocity"};
      return true;
    };
    Node n = node["Velocity"];
    uint32_t mpv;
    try {
      mpv = n["Max Profile Velocity"].as<uint32_t>();
    } catch (const YAML::BadConversion &e) {
      mfe = invalid<VelocityConfig>{"Velocity: Max Profile Velocity: Value "
                                    "missing or not of type uint32_t."};
      return true;
    }
    // Limit depends on Maximal Gear Speed and Maximal Motor Speed. See
    // EPOS2
    // Firmware 8.2.106 Maximal Profile VelocityConfigVariant and Table
    // 8-119

    mfe = withinBounds<uint32_t>(n, "Profile Velocity", 1, mpv)
              .match(
                  [](invalid<uint32_t> in) -> VelocityConfigVariant {
                    return invalid<VelocityConfig>{
                        std::string("Velocity: " + in.reason)};
                  },
                  [&mpv](uint32_t value) -> VelocityConfigVariant {
                    VelocityConfig vc;
                    vc.maxProfileVelocity = mpv;
                    vc.profileVelocity = value;
                    return vc;
                  });
    return true;
  };
};

template <> struct convert<AccelerationConfigVariant> {
  static bool decode(Node const &node, AccelerationConfigVariant &mfe) {
    if (!node["Acceleration"]) {
      mfe = invalid<AccelerationConfig>{"Missing Acceleration."};
      return true;
    }
    Node n = node["Acceleration"];

    if (!n["Max Acceleration"]) {
      mfe = invalid<AccelerationConfig>{
          "Acceleration: Missing Max Acceleration."};
      return true;
    }

    if (!n["Profile Acceleration"]) {
      mfe = invalid<AccelerationConfig>{
          "Acceleration: Missing Profile Acceleration."};
      return true;
    }

    if (!n["Profile Deceleration"]) {
      mfe = invalid<AccelerationConfig>{
          "Acceleration: Missing Profile Deceleration."};
      return true;
    }

    if (!n["Quickstop Deceleration"]) {
      mfe = invalid<AccelerationConfig>{
          "Acceleration: Missing Quickstop Deceleration."};
      return true;
    }

    mfe =
        withinBounds<uint32_t>(n, "Max Acceleration", 0, 4294967295)
            .match(
                [](invalid<uint32_t> in) -> AccelerationConfigVariant {
                  return invalid<AccelerationConfig>{
                      std::string("Acceleration: " + in.reason)};
                },
                [&n](uint32_t mac) -> AccelerationConfigVariant {
                  return withinBounds<uint32_t>(n, "Profile Acceleration", 1,
                                                mac)
                      .match(
                          [](invalid<uint32_t> in)
                              -> AccelerationConfigVariant {
                            return invalid<AccelerationConfig>{
                                std::string("Acceleration: " + in.reason)};
                          },
                          [&n,
                           &mac](uint32_t pac) -> AccelerationConfigVariant {
                            return withinBounds<uint32_t>(
                                       n, "Profile Deceleration", 1, mac)
                                .match(
                                    [](invalid<uint32_t> in)
                                        -> AccelerationConfigVariant {
                                      return invalid<AccelerationConfig>{
                                          std::string("Acceleration: " +
                                                      in.reason)};
                                    },
                                    [&n, &mac, &pac](uint32_t pdc)
                                        -> AccelerationConfigVariant {
                                      return withinBounds<uint32_t>(
                                                 n, "Quickstop Deceleration", 1,
                                                 mac)
                                          .match(
                                              [](invalid<uint32_t> in)
                                                  -> AccelerationConfigVariant {
                                                return invalid<
                                                    AccelerationConfig>{
                                                    std::string(
                                                        "Acceleration: " +
                                                        in.reason)};
                                              },
                                              [&mac, &pac, &pdc](uint32_t qd)
                                                  -> AccelerationConfigVariant {
                                                AccelerationConfig ac;
                                                ac.maxAcceleration =
                                                    std::move(mac);
                                                ac.profileAcceleration =
                                                    std::move(pac);
                                                ac.profileDeceleration =
                                                    std::move(pdc);
                                                ac.quickstopDeceleration =
                                                    std::move(qd);
                                                return ac;
                                              });
                                    });
                          });
                });
    return true;
  };
};

template <> struct convert<MotionProfileTypeVariant> {
  static bool decode(Node const &node, MotionProfileTypeVariant &mfe) {
    if (!node["Motion Profile Type"]) {
      mfe = invalid<MotionProfileType>{"Missing Motion Profile Type."};
      return true;
    }
    switch (node["Motion Profile Type"].as<int16_t>()) {
    case 0:
      mfe = MotionProfileType::LINEAR_RAMP_TRAPEZOIDAL_PROFILE;
      break;
    case 1:
      mfe = MotionProfileType::SIN2_RAMP_SINUSOIDAL_PROFILE;
      break;
    default:
      mfe =
          invalid<MotionProfileType>{"Motion Profile Type: Not a valid value."};
    };
    return true;
  };
};

template <> struct convert<ControllersVariant> {
  static bool decode(Node const &node, ControllersVariant &controllers) {
    if (!node["Control Mode Configuration"]) {
      controllers =
          invalid<MaxonControllers>{"Missing Control Mode Configuration."};
      return true;
    }
    Node newnode = node["Control Mode Configuration"];
    ControllersVariant ctrl;
    std::string controllerName;
    ctrl = std::accumulate(
        newnode.begin(), newnode.end(), ControllersVariant{},
        [&controllerName](ControllersVariant ctrl,
                          YAML::const_iterator::value_type subnet) {
          return ctrl.match(
              [&subnet,
               &controllerName](empty<MaxonControllers>) -> ControllersVariant {
                controllerName = subnet.first.as<std::string>();
                return growController(MaxonControllers{}, subnet);
              },
              [&subnet](invalid<MaxonControllers> in) -> ControllersVariant {

                return invalid<MaxonControllers>{std::string(
                    "Control Mode Configuration: " +
                    subnet.first.as<std::string>() + " : " + in.reason)};
              },
              [&subnet, &controllerName](
                  MaxonControllers previous) -> ControllersVariant {
                controllerName = subnet.first.as<std::string>();

                return growController(previous, subnet);
              });
        });
    controllers = ctrl.match(
        [](empty<MaxonControllers>) -> ControllersVariant {
          return invalid<MaxonControllers>{
              "Parsing failed for Control Mode Configuration."};
        },
        [&controllerName](invalid<MaxonControllers> in) -> ControllersVariant {
          return invalid<MaxonControllers>{
              std::string("Control Mode Configuration: " + controllerName +
                          ": " + in.reason)};
        },
        [](MaxonControllers in) -> ControllersVariant { return in; });
    return true;
  };
};
}; // end of namespace YAML
