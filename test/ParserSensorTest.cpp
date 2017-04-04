#include "roboy_can/sensorParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(parseSensor, sensorYaml) {
  auto node = YAML::LoadFile("sensor.yaml");
  SensorVariant se = node.as<SensorVariant>();

  se.match(
      [](empty<SensorConfig> in) -> void { FAIL() << "empty sensorConfig"; },
      [](invalid<SensorConfig> in) -> void { FAIL() << in.reason; },
      [](SensorConfig sen) -> void { SUCCEED(); });
}
TEST(parseSensor, brokenRoboyYaml) {
  auto node = YAML::LoadFile("broken_roboy.yaml");
  node.as<SensorVariant>().match(
      [](empty<SensorConfig> in) -> void { FAIL() << "empty sensorConfig"; },
      [](invalid<SensorConfig> in) -> void {
        EXPECT_EQ(in.reason, "Missing Sensor Configuration.");
      },
      [](SensorConfig sen) -> void { FAIL() << "sensorConfig"; });
}

TEST(parseSensor, SensorValues) {
  auto node = YAML::LoadFile("sensor.yaml");
  node.as<SensorVariant>().match(
      [](empty<SensorConfig> in) -> void { FAIL() << "empty sensorConfig"; },
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
      [](invalid<SensorConfig> in) -> void { FAIL() << in.reason; });
}

TEST(parseSensor, missingPnie) {
  auto node = YAML::LoadFile("sensor_missing_pni.yaml");
  node.as<SensorVariant>().match(
      [](empty<SensorConfig> in) -> void { FAIL() << "empty sensorConfig"; },
      [](invalid<SensorConfig> in) -> void {
        EXPECT_EQ(in.reason, "Standard Motor Configuration: Sensor "
                             "Configuration: Missing Pulse Number Incremental "
                             "Encoder 1.");
      },
      [](SensorConfig sen) -> void { FAIL() << "sensorConfig"; });
}

TEST(parseSensor, invalidPnie) {
  auto node = YAML::LoadFile("sensor_invalid_pni.yaml");
  node.as<SensorVariant>().match(
      [](empty<SensorConfig> in) -> void { FAIL() << "empty sensorConfig"; },
      [](invalid<SensorConfig> in) -> void {
        EXPECT_EQ(in.reason, "Standard Motor Configuration: "
                             "Sensor Configuration: Pulse Number Incremental "
                             "Encoder 1 : Value out of bounds.");
      },
      [](SensorConfig sen) -> void { FAIL() << "sensorConfig"; });
}
