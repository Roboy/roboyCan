#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"

#include <numeric>

using SensorVariant =
    variant<empty<SensorConfig>, SensorConfig, invalid<SensorConfig>>;

namespace YAML {
template <> struct convert<PositionSensorType> {
  static bool decode(Node const &input_node, PositionSensorType &sensor) {
    switch (input_node.as<uint16_t>()) {
    case 0:
      sensor = {EposPositionSensorType::UNKNOWN};
      break;
    case 1:
      sensor = {EposPositionSensorType::INC_ENCODER_1_W_INDEX_3CH};
      break;
    case 2:
      sensor = {EposPositionSensorType::INC_ENCODER_1_WO_INDEX_2CH};
      break;
    case 3:
      sensor = {EposPositionSensorType::HALL_SENSORS};
      break;
    case 4:
      sensor = {EposPositionSensorType::ABS_ENCODER_SSI};
      break;
    case 5:
      sensor = {EposPositionSensorType::RESERVED};
      break;
    case 6:
      sensor = {EposPositionSensorType::INC_ENCODER_2_W_INDEX_3CH};
      break;
    case 7:
      sensor = {EposPositionSensorType::INC_ENCODER_2_W_INDEX_2CH};
      break;
    case 8:
      sensor = {EposPositionSensorType::SINUS_INC_ENCODER_2};
      break;
    default:
      sensor = invalid<EposPositionSensorType>{
          "Position Sensor Type: Not a valid value."};
    }
    return true;
  };
};

template <> struct convert<SensorVariant> {
  static bool decode(Node const &input_node, SensorVariant &sensor) {
    Node node;
    if (!input_node["Standard Motor Configuration"] ||
        !input_node["Standard Motor Configuration"]["Sensor Configuration"]) {
      sensor = invalid<SensorConfig>{"Missing Sensor Configuration."};
      return true;
    } else
      node = input_node["Standard Motor Configuration"]["Sensor Configuration"];

    if (!node["Pulse Number Incremental Encoder 1"]) {
      sensor = invalid<SensorConfig>{
          "Standard Motor Configuration: "
          "Sensor Configuration: Missing Pulse Number Incremental Encoder 1."};
      return true;
    }
    if (!node["Position Sensor Type"]) {
      sensor = invalid<SensorConfig>{
          "Standard Motor Configuration: "
          "Sensor Configuration: Missing Position Sensor Type."};
      return true;
    };

    sensor =
        withinBounds<EposPulseNumberIncrementalEncoders>(
            node, "Pulse Number Incremental Encoder 1", 16, 2500000)
            .match(
                [](invalid<EposPulseNumberIncrementalEncoders> in)
                    -> SensorVariant {
                  return invalid<SensorConfig>{
                      std::string("Standard Motor Configuration: "
                                  "Sensor Configuration: " +
                                  in.reason)};
                },
                [&node](EposPulseNumberIncrementalEncoders pulses)
                    -> SensorVariant {
                  return node["Position Sensor Type"]
                      .as<PositionSensorType>()
                      .match(
                          [](empty<EposPositionSensorType>) -> SensorVariant {
                            return invalid<SensorConfig>{std::string(
                                "Parsing failed for Position Sensor Type.")};
                          },
                          [](invalid<EposPositionSensorType> in)
                              -> SensorVariant {
                            return invalid<SensorConfig>{
                                std::string("Standard Motor Configuration: "
                                            "Sensor Configuration: " +
                                            in.reason)};
                          },
                          [&pulses](
                              EposPositionSensorType pst) -> SensorVariant {
                            return SensorConfig(pulses, pst);
                          });
                });
    return true;
  };
};
}; // end of namespace
