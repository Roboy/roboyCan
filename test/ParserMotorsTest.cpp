#include "roboy_can/motorsParser.hpp"
#include <gtest/gtest.h>
#include <iostream>
TEST(roboy_yaml, parseMotorNames) {
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

TEST(roboy_yaml, MotorNamesValue) {
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

TEST(roboy_yaml, DuplicateMotorName) {
  auto const node = YAML::LoadFile("roboy_duplicate_motor.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void { FAIL() << "motorNames mn"; },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; },
      [](duplicate<CanIdNetworktuple, std::string> dd) -> void {
        EXPECT_EQ(dd.key, std::string("Left: 34"));
      });
}
