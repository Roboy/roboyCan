#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"

#include <numeric>

using Sensor =
    variant<empty<SensorConfig>, missing<SensorConfig>, SensorConfig,
            invalid<EposPulseNumberIncrementalEncoders>,
            missing<EposPulseNumberIncrementalEncoders>,
            invalid<EposPositionSensorType>, missing<EposPositionSensorType>>;

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
      sensor = invalid<EposPositionSensorType>{};
    }
    return true;
  }
};
template <> struct convert<Sensor> {
  static bool decode(Node const &input_node, Sensor &sensor) {
    Node node;
    if (!input_node["Sensor Configuration"]) {
      sensor = missing<SensorConfig>{};
      return true;
    } else
      node = input_node["Sensor Configuration"];

    if (!node["Pulse Number Incremental Encoder 1"]) {
      sensor = missing<EposPulseNumberIncrementalEncoders>{};
      return true;
    }
    if (!node["Position Sensor Type"]) {
      sensor = missing<EposPositionSensorType>{};
      return true;
    };

    sensor =
        withinBounds<EposPulseNumberIncrementalEncoders>(
            node["Pulse Number Incremental Encoder 1"].as<uint32_t>(), 16,
            2500000)
            .match(
                passAlong<invalid<EposPulseNumberIncrementalEncoders>,
                          Sensor>{},
                [&node](EposPulseNumberIncrementalEncoders pulses) -> Sensor {
                  return node["Position Sensor Type"]
                      .as<PositionSensorType>()
                      .match(
                          passAlong<invalid<EposPositionSensorType>, Sensor>{},
                          passAlong<missing<EposPositionSensorType>, Sensor>{},
                          [&pulses](EposPositionSensorType pst) -> Sensor {
                            return {SensorConfig(pulses, pst)};
                          });
                });
    return true;
  };
};
}; // end of namespace
