#include "roboy_can/sensorParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(sensor_yaml, ParseSensor) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se = node.as<SensorVariant>();

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
TEST(broken_roboy_yaml, ParseSensor) {
  auto node = YAML::LoadFile("broken_roboy.yaml");
  SensorVariant se = node.as<SensorVariant>();

  se.match([](empty<SensorConfig>) -> void { FAIL() << "empty<SensorConfig>"; },
           [](missing<SensorConfig>) -> void { SUCCEED(); },
           [](SensorConfig sen) -> void { FAIL() << "Sensorconfig"; },
           [](invalid<EposPulseNumberIncrementalEncoders>) -> void {
             FAIL() << "invalid<EposPulseNumberIncrementalEncoders>";
           },
           [](missing<EposPulseNumberIncrementalEncoders>) -> void {
             FAIL() << "missing<EposPulseNumberIncrementalEncoders>";
           },
           [](invalid<EposPositionSensorType>) -> void {
             FAIL() << "invalid<EposPositionSensorType>";
           },
           [](missing<EposPositionSensorType>) -> void {
             FAIL() << "missing<EposPositionSensorType>";
           });
}

TEST(sensor_yaml, SensorValues) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se = node.as<SensorVariant>();
  se.match(
      [](empty<SensorConfig>) -> void { FAIL() << "empty<SensorConfig>"; },
      [](missing<SensorConfig>) -> void { FAIL() << "missing<SensorConfig>"; },
      [](SensorConfig sc) -> void {
        sc.getParameter("Pulse Number Incremental Encoder 1")
            .match([](int16_t val) -> void { FAIL() << "int16_t"; },
                   [](uint16_t val) -> void { FAIL() << "uint16_t"; },
                   [](int32_t val) -> void { FAIL() << "int32_t"; },
                   [](uint32_t val) -> void { EXPECT_EQ(val, 512); });
        EXPECT_EQ(sc.getParameter("Position Sensor Type"),
                  static_cast<uint16_t>(
                      EposPositionSensorType::INC_ENCODER_1_WO_INDEX_2CH));
        EXPECT_NE(sc.getParameter("Position Sensor Type"),
                  static_cast<uint16_t>(
                      EposPositionSensorType::INC_ENCODER_1_W_INDEX_3CH));
      },
      [](invalid<EposPulseNumberIncrementalEncoders>) -> void {
        FAIL() << "invalid<EposPulseNumberIncrementalEncoders>";
      },
      [](missing<EposPulseNumberIncrementalEncoders>) -> void {
        FAIL() << "missing<EposPulseNumberIncrementalEncoders>";
      },
      [](invalid<EposPositionSensorType>) -> void {
        FAIL() << "invalid<EposPositionSensorType>";
      },
      [](missing<EposPositionSensorType>) -> void {
        FAIL() << "missing<EposPositionSensorType>";
      });
}

TEST(sensor_yaml, MissingPNIE) {
  auto node = YAML::LoadFile("sensor_missing_pni.yaml");
  SensorVariant se = node.as<SensorVariant>();

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
  auto node = YAML::LoadFile("sensor_invalid_pni.yaml");
  SensorVariant se = node.as<SensorVariant>();

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
  auto node = YAML::LoadFile("sensor_invalid_pni.yaml");
  SensorVariant se = node.as<SensorVariant>();

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
