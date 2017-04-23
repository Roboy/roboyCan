#include "roboy_can/motorsParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(parseMotors, motorNames) {
  auto const node = YAML::LoadFile("roboy.yaml");
  MotorNamesVariant mnv = node["Maxon"].as<MotorNamesVariant>();
  mnv.match([](empty<MotorNames>) -> void { FAIL() << "empty<MotorNames>"; },
            [](invalid<MotorNames> in) -> void { FAIL() << in.reason; },
            [](MotorNames mn) -> void { EXPECT_EQ(mn.size(), 11); });
}
TEST(parseMotors, motorNamesValues) {
  auto const node = YAML::LoadFile("roboy.yaml");
  node["Maxon"].as<MotorNamesVariant>().match(
      [](empty<MotorNames>) -> void { FAIL() << "empty<MotorNames>"; },
      [](invalid<MotorNames> in) -> void { FAIL() << in.reason; },
      [](MotorNames mn) -> void {
        EXPECT_EQ(mn.at("BriskBugs").canId, 1);
        EXPECT_EQ(mn.at("BriskBugs").network, std::string("Left"));
        EXPECT_EQ(mn.at("ChosenPlants").canId, 34);
        EXPECT_EQ(mn.at("ChosenPlants").network, std::string("Left"));
        EXPECT_EQ(mn.at("GoodLemurs").canId, 41);
        EXPECT_EQ(mn.at("GoodLemurs").network, std::string("Right"));
      });
}

TEST(parseMotors, duplicateMotorName) {
  auto const node = YAML::LoadFile("motornames_duplicate_motor.yaml");
  node["Maxon"].as<MotorNamesVariant>().match(
      [](empty<MotorNames>) -> void { FAIL() << "empty<MotorNames>"; },
      [](invalid<MotorNames> in) -> void {
        EXPECT_EQ(in.reason,
                  std::string("Motors: Duplicate CAN ID: Left: 34."));
      },
      [](MotorNames) -> void { FAIL() << "MotorNames"; });
}

TEST(parseEverything, success) {
  auto const node = YAML::LoadFile("roboy.yaml");
  node["Maxon"].as<MotorConfigVariant>().match(
      [](empty<MotorConfigs>) -> void { FAIL() << "empty<MotorConfig>"; },
      [](invalid<MotorConfigs> in) -> void { FAIL() << in.reason; },
      [](MotorConfigs mn) -> void { SUCCEED(); });
}

TEST(parseEverything, baudrate) {
  auto const node = YAML::LoadFile("roboy.yaml");
  node["Maxon"].as<MotorConfigVariant>().match(
      [](empty<MotorConfigs>) -> void { FAIL() << "empty<MotorConfig>"; },
      [](invalid<MotorConfigs> in) -> void { FAIL() << in.reason; },
      [](MotorConfigs mn) -> void {
        KaCanOpenBaudrate temp = mn.at("BriskBugs").network.getBaudrate();
        EXPECT_EQ(temp, KaCanOpenBaudrate::Baud1M);
      });
}

TEST(parseEverything, duplicateSerial) {
  auto const node = YAML::LoadFile("roboy_duplicate_serial.yaml");
  node["Maxon"].as<MotorConfigVariant>().match(
      [](empty<MotorConfigs>) -> void { FAIL() << "empty<MotorConfig>"; },
      [](invalid<MotorConfigs> in) -> void {
        EXPECT_EQ(
            in.reason,
            std::string("Network: Right: USB Serial: Serial is duplicated."));
      },
      [](MotorConfigs) -> void { FAIL() << "MotorConfigs"; });
}
