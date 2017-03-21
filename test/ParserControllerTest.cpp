#include "roboy_can/controllerParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(ParseMFEV, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  MaxFollowingError ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MaxFollowingError>();

  unsigned int isMFE = 12;
  isMFE = ctrl.match(
      [](missing<MaxFollowingErrorValue>) -> unsigned int { return 0; },
      [](invalid<MaxFollowingErrorValue>) -> unsigned int { return 1; },
      [](MaxFollowingErrorValue) -> unsigned int { return 2; });

  EXPECT_EQ(isMFE, 2);
}
TEST(ParsePL, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  PositionLimit ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<PositionLimit>();

  unsigned int isMFE = 12;
  isMFE = ctrl.match([](missing<int32_t> mm) -> unsigned int { return 0; },
                     [](PositionLimitValue) -> unsigned int { return 1; });

  EXPECT_EQ(isMFE, 1);
}

TEST(ParsePLValues, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  PositionLimit ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<PositionLimit>();

  MaxonParameterList isMFE;
  isMFE = ctrl.match(
      [](missing<int32_t> mm) -> MaxonParameterList { return {}; },
      [](PositionLimitValue mm) -> MaxonParameterList { return mm; });

  EXPECT_EQ(isMFE["Min Position Limit"], static_cast<int32_t>(-2147483648));
  EXPECT_EQ(isMFE["Max Position Limit"], static_cast<int32_t>(2147483647));
}

TEST(ParseVelocity, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Velocity vel = node["Control Mode Configuration"]["Profile Position Mode"]
                     .as<Velocity>();

  unsigned int isVelocity = 12;

  isVelocity =
      vel.match([](missing<MaxProfileVelocity>) -> unsigned int { return 0; },
                [](missing<ProfileVelocity>) -> unsigned int { return 1; },
                [](invalid<ProfileVelocity>) -> unsigned int { return 2; },
                [](MaxonParameterList) -> unsigned int { return 3; });

  EXPECT_EQ(isVelocity, 3);
}
TEST(ParseVelocityValues, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Velocity vel = node["Control Mode Configuration"]["Profile Position Mode"]
                     .as<Velocity>();

  MaxonParameterList isVelocity;

  isVelocity = vel.match(
      [](missing<MaxProfileVelocity>) -> MaxonParameterList { return {}; },
      [](missing<ProfileVelocity>) -> MaxonParameterList { return {}; },
      [](invalid<ProfileVelocity>) -> MaxonParameterList { return {}; },
      [](MaxonParameterList mm) -> MaxonParameterList { return mm; });

  EXPECT_EQ(isVelocity["Max Profile Velocity"], static_cast<uint32_t>(8000));
  EXPECT_EQ(isVelocity["Profile Velocity"], static_cast<uint32_t>(8000));
}

TEST(ParseAcceleration, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Control Mode Configuration"]["Profile Position Mode"]
                         .as<Acceleration>();

  unsigned int isAcceleration = 11;

  isAcceleration =
      acc.match([](empty<MaxonParameterList>) -> unsigned int { return 0; },
                [](missing<uint32_t> mm) -> unsigned int { return 1; },
                [](invalid<uint32_t> mm) -> unsigned int { return 2; },
                [](MaxonParameterList) -> unsigned int { return 3; });

  EXPECT_EQ(isAcceleration, 3);
}

TEST(ParseAccelerationValue, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Control Mode Configuration"]["Profile Position Mode"]
                         .as<Acceleration>();

  MaxonParameterList isAcceleration;

  isAcceleration = acc.match(
      [](empty<MaxonParameterList>) -> MaxonParameterList { return {}; },
      [](missing<uint32_t> mm) -> MaxonParameterList { return {}; },
      [](invalid<uint32_t> mm) -> MaxonParameterList { return {}; },
      [](MaxonParameterList mm) -> MaxonParameterList { return mm; });

  EXPECT_EQ(isAcceleration["Max Acceleration"],
            MaxonParameter(static_cast<uint32_t>(40001)));
  EXPECT_EQ(isAcceleration["Profile Acceleration"],
            MaxonParameter(static_cast<uint32_t>(40000)));
  EXPECT_EQ(isAcceleration["Profile Deceleration"],
            MaxonParameter(static_cast<uint32_t>(40000)));
  isAcceleration["Quickstop Deceleration"].match(
      [](int16_t) -> void { FAIL() << "We shouldn't get here: int16_t"; },
      [](uint16_t) -> void { FAIL() << "We shouldn't get here: uint16_t"; },
      [](int32_t) -> void { FAIL() << "We shouldn't get here: int32_t"; },
      [](uint32_t value) -> void { EXPECT_EQ(value, 10000); });
}

TEST(MaxonParameterTest, controller_yaml) {
  MaxonParameter mx = uint32_t(10000);
  mx.match(
      [](int16_t) -> void { FAIL() << "We shouldn't get here: int16_t"; },
      [](uint16_t) -> void { FAIL() << "We shouldn't get here: uint16_t"; },
      [](int32_t) -> void { FAIL() << "We shouldn't get here: int32_t"; },
      [](uint32_t value) -> void { EXPECT_EQ(value, 10000); });
}

TEST(MaxonParameterListTest, controller_yaml) {
  MaxonParameter mx = uint32_t(10000);
  MaxonParameterList mxl = {{"my", uint32_t(10000)}};

  mxl["my"].match(
      [](int16_t) -> void { FAIL() << "We shouldn't get here: int16_t"; },
      [](uint16_t) -> void { FAIL() << "We shouldn't get here: uint16_t"; },
      [](int32_t) -> void { FAIL() << "We shouldn't get here: int32_t"; },
      [](uint32_t value) -> void { EXPECT_EQ(value, 10000); });
}

TEST(MissingMaxAcceleration, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Control Mode Configuration"]
                         ["Profile Position Mode Missing Max Acc"]
                             .as<Acceleration>();

  std::string isAcceleration = "";

  isAcceleration = acc.match(
      [](empty<MaxonParameterList>) -> std::string { return ""; },
      [](missing<uint32_t> mm) -> std::string { return mm.paramName; },
      [](invalid<uint32_t> mm) -> std::string { return mm.paramName; },
      [](MaxonParameterList) -> std::string { return ""; });

  EXPECT_EQ(isAcceleration, std::string("Max Acceleration"));
}

TEST(MissingProfileAcceleration, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Control Mode Configuration"]
                         ["Profile Position Mode Missing Profile Acc"]
                             .as<Acceleration>();

  std::string isAcceleration = "";

  isAcceleration = acc.match(
      [](empty<MaxonParameterList>) -> std::string { return ""; },
      [](missing<uint32_t> mm) -> std::string { return mm.paramName; },
      [](invalid<uint32_t> mm) -> std::string { return mm.paramName; },
      [](MaxonParameterList) -> std::string { return ""; });

  EXPECT_EQ(isAcceleration, std::string("Profile Acceleration"));
}

TEST(MaxFollowingError, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  MaxFollowingError mfe =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MaxFollowingError>();

  unsigned int isAcceleration = 12;

  isAcceleration = mfe.match(
      [](missing<MaxFollowingErrorValue> mm) -> unsigned int { return 0; },
      [](invalid<MaxFollowingErrorValue> mm) -> unsigned int { return 1; },
      [](MaxFollowingErrorValue) -> unsigned int { return 2; });

  EXPECT_EQ(isAcceleration, 2);
}

TEST(MotionProfileType, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  MotionProfileType mfe =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MotionProfileType>();

  unsigned int isAcceleration = 12;

  isAcceleration = mfe.match(
      [](empty<MotionProfileTypeValue> mm) -> unsigned int { return 0; },
      [](missing<MotionProfileTypeValue> mm) -> unsigned int { return 1; },
      [](invalid<MotionProfileTypeValue> mm) -> unsigned int { return 2; },
      [](MotionProfileTypeValue) -> unsigned int { return 3; });

  EXPECT_EQ(isAcceleration, 3);
}
