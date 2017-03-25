#include "roboy_can/motorsParser.hpp"
#include <gtest/gtest.h>
#include <iostream>
TEST(motorParser, parseMotorNames) {
  auto const node = YAML::LoadFile("roboy.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void { EXPECT_EQ(mn.size(), 11); },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; },
      [](duplicate<CanIdNetworktuple, std::string>) -> void {
        FAIL() << "duplicate<CanIdNetworktuple, std::string>";
      });
}

TEST(motorParser, MotorNamesValue) {
  auto const node = YAML::LoadFile("roboy.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void {
        EXPECT_EQ(mn.at("BriskBugs").canId, 1);
        EXPECT_EQ(mn.at("BriskBugs").network, std::string("Left"));
        EXPECT_EQ(mn.at("ChosenPlants").canId, 34);
        EXPECT_EQ(mn.at("ChosenPlants").network, std::string("Left"));
        EXPECT_EQ(mn.at("GoodLemurs").canId, 41);
        EXPECT_EQ(mn.at("GoodLemurs").network, std::string("Right"));
      },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; },
      [](duplicate<CanIdNetworktuple, std::string>) -> void {
        FAIL() << "duplicate<CanIdNetworktuple, std::string>";
      });
}

TEST(motorParser, DuplicateMotorName) {
  auto const node = YAML::LoadFile("motornames_duplicate_motor.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void { FAIL() << "motorNames mn"; },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; },
      [](duplicate<CanIdNetworktuple, std::string> dd) -> void {
        EXPECT_EQ(dd.key, std::string("Left: 34"));
      });
}

TEST(completeParse, ParseAll) {
  auto const node = YAML::LoadFile("roboy.yaml");
  MotorConfigVariant mcv = node["Maxon"].as<MotorConfigVariant>();
  mcv.match(
      [](empty<MotorConfigs>) -> void { FAIL() << "empty<MotorConfig>"; },
      [](MotorConfigs mn) -> void { SUCCEED(); },
      [](invalid<MotorConfig>) -> void { FAIL() << "invalid<MotorConfig>"; },
      [](NetworkVariant) -> void { FAIL() << "NetworkVariant"; },
      [](SensorVariant) -> void { FAIL() << "SensorVariant"; },
      [](motorNamesVariant) -> void { FAIL() << "motorNamesVariant"; });
}

TEST(completeParse, ParseAll_DUPLICATESerial) {
  auto const node = YAML::LoadFile("roboy_duplicate_serial.yaml");
  MotorConfigVariant mcv = node["Maxon"].as<MotorConfigVariant>();
  mcv.match(
      [](empty<MotorConfigs>) -> void { FAIL() << "empty<MotorConfig>"; },
      [](MotorConfigs mn) -> void { FAIL() << "MotorConfigs mn"; },
      [](invalid<MotorConfig>) -> void { FAIL() << "invalid<MotorConfig>"; },
      [](NetworkVariant nv) -> void {
        nv.match(
            [](empty<Networks>) -> void {
              FAIL() << "empty<Networks>" << std::endl;
            },
            [](Networks) -> void { FAIL() << "Networks" << std::endl; },
            [](missing<Networks>) -> void {
              FAIL() << "missing<Networks>" << std::endl;
            },
            [](missing<KaCanOpenBaudrate>) -> void {
              FAIL() << "missing<KaCanOpenBaudrate>" << std::endl;
            },
            [](invalid<KaCanOpenBaudrate>) -> void {
              FAIL() << "invalid<KaCanOpenBaudrate>" << std::endl;
            },
            [](missing<KaCanOpenUsbOptions>) -> void {
              FAIL() << "missing<KaCanOpenUsbOptions>" << std::endl;
            },
            [](invalid<KaCanOpenUsbOptions>) -> void {
              FAIL() << "invalid<KaCanOpenUsbOptions>" << std::endl;
            },
            [](missing<UsbSerial>) -> void {
              FAIL() << "missing<UsbSerial>" << std::endl;
            },
            [](duplicate<NetworkConfig, std::string>) -> void { SUCCEED(); });
      },
      [](SensorVariant) -> void { FAIL() << "SensorVariant"; },
      [](motorNamesVariant mnv) -> void {
        mnv.match(
            [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
            [](motorNames) -> void { FAIL() << "motorNames"; },
            [](missing<motorNames>) -> void {
              FAIL() << "missing<motorNames>";
            },
            [](duplicate<CanIdNetworktuple, std::string>) -> void {
              FAIL() << "duplicate<CanIdNetworktuple, std::string>";
            });
      });
}
