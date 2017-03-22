#include "roboy_can/motorsParser.hpp"
#include <gtest/gtest.h>
#include <iostream>
TEST(roboy_yaml, parseMotorNames) {
  auto const node = YAML::LoadFile("roboy.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void { EXPECT_EQ(mn.size(), 11); },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; });
}

TEST(roboy_yaml, MotorNamesValue) {
  auto const node = YAML::LoadFile("roboy.yaml");
  motorNamesVariant mnv = node["Maxon"].as<motorNamesVariant>();
  mnv.match(
      [](empty<motorNames>) -> void { FAIL() << "empty<motorNames>"; },
      [](motorNames mn) -> void {
        EXPECT_EQ(mn.at("BriskBugs").id, 1);
        EXPECT_EQ(mn.at("BriskBugs").network, std::string("Left"));
        EXPECT_EQ(mn.at("ChosenPlants").id, 34);
        EXPECT_EQ(mn.at("ChosenPlants").network, std::string("Left"));
        EXPECT_EQ(mn.at("GoodLemurs").id, 41);
        EXPECT_EQ(mn.at("GoodLemurs").network, std::string("Right"));
      },
      [](missing<motorNames>) -> void { FAIL() << "missing<motorNames>"; });
}
