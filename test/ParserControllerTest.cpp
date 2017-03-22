#include "roboy_can/controllerParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(ParseMFEV, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  MaxFollowingError ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MaxFollowingError>();

  unsigned int isMFE = 12;
  isMFE = ctrl.match([](missing<uint32_t>) -> unsigned int { return 0; },
                     [](invalid<uint32_t>) -> unsigned int { return 1; },
                     [](uint32_t) -> unsigned int { return 2; });

  EXPECT_EQ(isMFE, 2);
}

TEST(MFEVvalue, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  MaxFollowingError ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MaxFollowingError>();

  ctrl.match([](missing<uint32_t>) -> void { FAIL() << "missing<uint32_t>"; },
             [](invalid<uint32_t>) -> void { "invalid<uint32_t>"; },
             [](uint32_t mm) -> void { EXPECT_EQ(mm, 5000); });
}

TEST(ParsePL, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  PositionLimit ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<PositionLimit>();

  unsigned int isMFE = 12;
  isMFE = ctrl.match(
      [](missing<MaxonParameterList> mm) -> unsigned int { return 0; },
      [](MaxonParameterList) -> unsigned int { return 1; });

  EXPECT_EQ(isMFE, 1);
}

TEST(ParsePLValues, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  PositionLimit ctrl =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<PositionLimit>();

  MaxonParameterList isMFE;
  isMFE = ctrl.match(
      [](missing<MaxonParameterList> mm) -> MaxonParameterList { return {}; },
      [](MaxonParameterList mm) -> MaxonParameterList { return mm; });

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

  isAcceleration.at("Quickstop Deceleration")
      .match(
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
  MaxonParameterList mxl = {{"my", uint32_t(10000)}};

  mxl["my"].match(
      [](int16_t) -> void { FAIL() << "We shouldn't get here: int16_t"; },
      [](uint16_t) -> void { FAIL() << "We shouldn't get here: uint16_t"; },
      [](int32_t) -> void { FAIL() << "We shouldn't get here: int32_t"; },
      [](uint32_t value) -> void { EXPECT_EQ(value, 10000); });
}

TEST(MissingMaxAcceleration, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Broken Control Mode Configuration"]
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

  Acceleration acc = node["Broken Control Mode Configuration"]
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

  isAcceleration =
      mfe.match([](missing<uint32_t> mm) -> unsigned int { return 0; },
                [](invalid<uint32_t> mm) -> unsigned int { return 1; },
                [](uint32_t) -> unsigned int { return 2; });

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

TEST(MotionProfileTypeValue, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  MotionProfileType mfe =
      node["Control Mode Configuration"]["Profile Position Mode"]
          .as<MotionProfileType>();

  MotionProfileTypeValue value;

  value = mfe.match(
      [](empty<MotionProfileTypeValue>) -> MotionProfileTypeValue {
        return {};
      },
      [](missing<MotionProfileTypeValue>) -> MotionProfileTypeValue {
        return {};
      },
      [](invalid<MotionProfileTypeValue>) -> MotionProfileTypeValue {
        return {};
      },
      [](MotionProfileTypeValue mm) -> MotionProfileTypeValue { return mm; });

  EXPECT_EQ(value, MotionProfileTypeValue::SIN2_RAMP_SINUSOIDAL_PROFILE);
}

TEST(Controllers, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  Controllers ctrls = node["Control Mode Configuration"].as<Controllers>();
  ctrls.match(
      [](empty<MaxonControllers>) -> void {
        FAIL() << "empty<MaxonControllers>";
      },
      [](MaxonControllers mcs) -> void { EXPECT_EQ(mcs.size(), 1); },
      [](empty<MaxonParameterList>) -> void {
        FAIL() << "empty<MaxonParameterList>";
      },
      [](missing<MaxonParameterList>) -> void {
        FAIL() << "missing<MaxonParameterList>";
      },
      [](missing<uint32_t>) -> void { FAIL() << "missing<uint32_t>"; },
      [](invalid<uint32_t>) -> void { FAIL() << "invalid<uint32_t>"; },
      [](missing<int32_t>) -> void { FAIL() << "missing<int32_t>"; },
      [](invalid<int32_t>) -> void { FAIL() << "invalid<int32_t>"; },
      [](empty<MotionProfileTypeValue>) -> void {
        FAIL() << "missing<MotionProfileTypeValue>";
      },
      [](missing<MotionProfileTypeValue>) -> void {
        FAIL() << "missing<MotionProfileTypeValue>";
      },
      [](invalid<MotionProfileTypeValue>) -> void {
        FAIL() << "invalid<MotionProfileTypeValue>";
      },
      [](duplicate<MaxonControllerConfig, std::string>) -> void {
        FAIL() << "duplicate<MaxonControllerConfig, std::string>)";
      });
}

TEST(ControllersValues, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");
  Controllers ctrls = node["Control Mode Configuration"].as<Controllers>();
  ctrls.match(
      [](empty<MaxonControllers>) -> void {
        FAIL() << "empty<MaxonControllers>";
      },
      [](MaxonControllers mcs) -> void {
        mcs.at("Profile Position Mode")
            .match([](ProfilePositionModeConfig config) -> void {
              EXPECT_EQ(config.getParameter("Max Following Error"),
                        static_cast<uint32_t>(5000));
              EXPECT_EQ(config.getParameter("Min Position Limit"),
                        static_cast<int32_t>(-2147483648));
              EXPECT_EQ(config.getParameter("Max Position Limit"),
                        static_cast<int32_t>(2147483647));
              EXPECT_EQ(config.getParameter("Max Profile Velocity"),
                        static_cast<uint32_t>(8000));
              EXPECT_EQ(config.getParameter("Profile Velocity"),
                        static_cast<uint32_t>(8000));
              EXPECT_EQ(config.getParameter("Max Acceleration"),
                        static_cast<uint32_t>(40001));
              EXPECT_EQ(config.getParameter("Profile Acceleration"),
                        static_cast<uint32_t>(40000));
              EXPECT_EQ(config.getParameter("Profile Deceleration"),
                        static_cast<uint32_t>(40000));
              EXPECT_EQ(config.getParameter("Quickstop Deceleration"),
                        static_cast<uint32_t>(10000));
              EXPECT_EQ(config.getParameter("Motion Profile Type"),
                        static_cast<int16_t>(1));
            });

      },
      [](empty<MaxonParameterList>) -> void {
        FAIL() << "empty<MaxonParameterList>";
      },
      [](missing<MaxonParameterList>) -> void {
        FAIL() << "missing<MaxonParameterList>";
      },
      [](missing<uint32_t>) -> void { FAIL() << "missing<uint32_t>"; },
      [](invalid<uint32_t>) -> void { FAIL() << "invalid<uint32_t>"; },
      [](missing<int32_t>) -> void { FAIL() << "missing<int32_t>"; },
      [](invalid<int32_t>) -> void { FAIL() << "invalid<int32_t>"; },
      [](empty<MotionProfileTypeValue>) -> void {
        FAIL() << "missing<MotionProfileTypeValue>";
      },
      [](missing<MotionProfileTypeValue>) -> void {
        FAIL() << "missing<MotionProfileTypeValue>";
      },
      [](invalid<MotionProfileTypeValue>) -> void {
        FAIL() << "invalid<MotionProfileTypeValue>";
      },
      [](duplicate<MaxonControllerConfig, std::string>) -> void {
        FAIL() << "duplicate<MaxonControllerConfig, std::string>)";
      });
}
