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
