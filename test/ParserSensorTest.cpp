#include "roboy_can/sensorParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(sensor_yaml, ParseSensor) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se = node["Standard Motor Configuration"].as<SensorVariant>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<SensorConfig>) -> unsigned int { return 0; },
      [](missing<SensorConfig>) -> unsigned int { return 1; },
      [](SensorConfig sen) -> unsigned int { return 2; },
      [](invalid<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 3;
      },
      [](missing<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 4;
      },
      [](invalid<EposPositionSensorType>) -> unsigned int { return 5; },
      [](missing<EposPositionSensorType>) -> unsigned int { return 6; });

  EXPECT_EQ(isSensor, 2);
}

TEST(sensor_yaml, SensorValues) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se = node["Standard Motor Configuration"].as<SensorVariant>();
  SensorConfig seco = se.match(
      [](empty<SensorConfig>) -> SensorConfig { return SensorConfig{}; },
      [](missing<SensorConfig>) -> SensorConfig { return SensorConfig{}; },
      [](SensorConfig sc) -> SensorConfig { return sc; },
      [](invalid<EposPulseNumberIncrementalEncoders>) -> SensorConfig {
        return SensorConfig{};
      },
      [](missing<EposPulseNumberIncrementalEncoders>) -> SensorConfig {
        return SensorConfig{};
      },
      [](invalid<EposPositionSensorType>) -> SensorConfig {
        return SensorConfig{};
      },
      [](missing<EposPositionSensorType>) -> SensorConfig {
        return SensorConfig{};
      },
      [](invalid<KaCanOpenUsbOptions>) -> SensorConfig {
        return SensorConfig{};
      });

  uint16_t pnie_val = seco.getParameter("Pulse Number Incremental Encoder 1")
                          .match([](int16_t val) -> uint16_t { return val; },
                                 [](uint16_t val) -> uint16_t { return val; },
                                 [](int32_t val) -> uint16_t { return val; },
                                 [](uint32_t val) -> uint16_t { return val; });
  EXPECT_EQ(pnie_val, 512);

  EXPECT_EQ(seco.getParameter("Position Sensor Type"),
            static_cast<uint16_t>(
                EposPositionSensorType::INC_ENCODER_1_WO_INDEX_2CH));
  EXPECT_NE(
      seco.getParameter("Position Sensor Type"),
      static_cast<uint16_t>(EposPositionSensorType::INC_ENCODER_1_W_INDEX_3CH));
}

TEST(sensor_yaml, MissingPNIE) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se =
      node["Standard Motor Configuration Missing PNIE"].as<SensorVariant>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<SensorConfig>) -> unsigned int { return 0; },
      [](missing<SensorConfig>) -> unsigned int { return 1; },
      [](SensorConfig sen) -> unsigned int { return 2; },
      [](invalid<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 3;
      },
      [](missing<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 4;
      },
      [](invalid<EposPositionSensorType>) -> unsigned int { return 5; },
      [](missing<EposPositionSensorType>) -> unsigned int { return 6; });

  EXPECT_EQ(isSensor, 4);
}

TEST(sensor_yaml, InvalidPNIE) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se =
      node["Standard Motor Configuration Invalid PNIE"].as<SensorVariant>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<SensorConfig>) -> unsigned int { return 0; },
      [](missing<SensorConfig>) -> unsigned int { return 1; },
      [](SensorConfig sen) -> unsigned int { return 2; },
      [](invalid<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 3;
      },
      [](missing<EposPulseNumberIncrementalEncoders>) -> unsigned int {
        return 4;
      },
      [](invalid<EposPositionSensorType>) -> unsigned int { return 5; },
      [](missing<EposPositionSensorType>) -> unsigned int { return 6; });

  EXPECT_EQ(isSensor, 3);
}
TEST(sensor_yaml, InvalidPNIE_Value) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se =
      node["Standard Motor Configuration Invalid PNIE"].as<SensorVariant>();

  std::string invEPNIE;

  invEPNIE = se.match(
      [](empty<SensorConfig>) -> std::string { return ""; },
      [](missing<SensorConfig>) -> std::string { return ""; },
      [](SensorConfig sen) -> std::string { return ""; },
      [](invalid<EposPulseNumberIncrementalEncoders> inv) -> std::string {
        return inv.paramName;
      },
      [](missing<EposPulseNumberIncrementalEncoders> inv) -> std::string {
        return inv.paramName;
      },
      [](invalid<EposPositionSensorType>) -> std::string { return ""; },
      [](missing<EposPositionSensorType>) -> std::string { return ""; });

  EXPECT_EQ(invEPNIE, "Pulse Number Incremental Encoder 1");
}
