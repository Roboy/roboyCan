#include "roboy_can/sensorParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(ParseSensor, sensor_yaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  Sensor se = node["Standard Motor Configuration"].as<Sensor>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<Sensor>) -> unsigned int { return 0; },
      [](missing<Sensor>) -> unsigned int { return 1; },
      [](Sensor sen) -> unsigned int { return 2; },
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

TEST(SensorValues, sensor_yaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  Sensor se = node["Standard Motor Configuration"].as<Sensor>();
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

TEST(MissingPNIE, sensor_yaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  Sensor se = node["Standard Motor Configuration Missing PNIE"].as<Sensor>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<Sensor>) -> unsigned int { return 0; },
      [](missing<Sensor>) -> unsigned int { return 1; },
      [](Sensor sen) -> unsigned int { return 2; },
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

TEST(InvalidPNIE, sensor_yaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  Sensor se = node["Standard Motor Configuration Invalid PNIE"].as<Sensor>();

  unsigned int isSensor = 12;

  isSensor = se.match(
      [](empty<Sensor>) -> unsigned int { return 0; },
      [](missing<Sensor>) -> unsigned int { return 1; },
      [](Sensor sen) -> unsigned int { return 2; },
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
TEST(InvalidPNIE_Value, sensor_yaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  Sensor se = node["Standard Motor Configuration Invalid PNIE"].as<Sensor>();

  std::string invEPNIE;

  invEPNIE = se.match(
      [](empty<Sensor>) -> std::string { return ""; },
      [](missing<Sensor>) -> std::string { return ""; },
      [](Sensor sen) -> std::string { return ""; },
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
