#include "roboy_can/controllerParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(parseController, maxFollowingError) {
  auto const node = YAML::LoadFile("controller.yaml");
  node["Control Mode Configuration"]["Profile Position Mode"]
      .as<MaxFollowingErrorConfigVariant>()
      .match(
          [](empty<MaxFollowingErrorConfig>) -> void {
            FAIL() << "empty MaxFollowingErrorConfig";
          },
          [](invalid<MaxFollowingErrorConfig> in) -> void {
            FAIL() << in.reason;
          },
          [](MaxFollowingErrorConfig mm) -> void {
            EXPECT_EQ(mm.maxFollowingError, 5000);
          });
}

TEST(parseController, position) {
  auto const node = YAML::LoadFile("controller.yaml");

  node["Control Mode Configuration"]["Profile Position Mode"]
      .as<PositionConfigVariant>()
      .match(
          [](empty<PositionConfig>) -> void {
            FAIL() << "empty PositionConfig";
          },
          [](invalid<PositionConfig> in) -> void { FAIL() << in.reason; },
          [](PositionConfig mm) -> void {
            EXPECT_EQ(mm.minPositionLimit, static_cast<int32_t>(-2147483648));
            EXPECT_EQ(mm.maxPositionLimit, static_cast<int32_t>(2147483647));
          });
}

TEST(parseController, velocity) {
  auto const node = YAML::LoadFile("controller.yaml");

  node["Control Mode Configuration"]["Profile Position Mode"]
      .as<VelocityConfigVariant>()
      .match(
          [](empty<VelocityConfig>) -> void {
            FAIL() << "empty VelocityConfig";
          },
          [](invalid<VelocityConfig> in) -> void { FAIL() << in.reason; },
          [](VelocityConfig mm) -> void {
            EXPECT_EQ(mm.maxProfileVelocity, static_cast<uint32_t>(8000));
            EXPECT_EQ(mm.profileVelocity, static_cast<uint32_t>(8000));
          });
}

TEST(parseController, acceleration) {
  auto const node = YAML::LoadFile("controller.yaml");

  node["Control Mode Configuration"]["Profile Position Mode"]
      .as<AccelerationConfigVariant>()
      .match(
          [](empty<AccelerationConfig>) -> void {
            FAIL() << "empty AccelerationConfig";
          },
          [](invalid<AccelerationConfig> in) -> void { FAIL() << in.reason; },
          [](AccelerationConfig mm) -> void {
            EXPECT_EQ(mm.maxAcceleration, static_cast<uint32_t>(40001));
            EXPECT_EQ(mm.profileAcceleration, static_cast<uint32_t>(40000));
            EXPECT_EQ(mm.profileDeceleration, static_cast<uint32_t>(40000));
            EXPECT_EQ(mm.quickstopDeceleration, static_cast<uint32_t>(10000));
          });
}

TEST(parseController, missingMaxAcceleration) {
  auto const node = YAML::LoadFile("controller.yaml");

  node["Broken Control Mode Configuration"]
      ["Profile Position Mode Missing Max Acc"]
          .as<AccelerationConfigVariant>()
          .match(
              [](empty<AccelerationConfig>) -> void {
                FAIL() << "empty AccelerationConfig";
              },
              [](invalid<AccelerationConfig> in) -> void {
                EXPECT_EQ(in.reason, "Acceleration: Missing Max Acceleration.");
              },
              [](AccelerationConfig) -> void {
                FAIL() << "AccelerationConfig";
              });
}

TEST(parseController, missingProfileAcceleration) {
  auto const node = YAML::LoadFile("controller.yaml");
  node["Broken Control Mode Configuration"]
      ["Profile Position Mode Missing Profile Acc"]
          .as<AccelerationConfigVariant>()
          .match(
              [](empty<AccelerationConfig>) -> void {
                FAIL() << "empty AccelerationConfig";
              },
              [](invalid<AccelerationConfig> in) -> void {
                EXPECT_EQ(in.reason,
                          "Acceleration: Missing Profile Acceleration.");
              },
              [](AccelerationConfig) -> void {
                FAIL() << "AccelerationConfig";
              });
}

TEST(parseController, motionProfileType) {
  auto const node = YAML::LoadFile("controller.yaml");

  node["Control Mode Configuration"]["Profile Position Mode"]
      .as<MotionProfileTypeVariant>()
      .match(
          [](empty<MotionProfileType>) -> void {
            FAIL() << "empty MotionProfileType";
          },
          [](invalid<MotionProfileType> in) -> void { FAIL() << in.reason; },
          [](MotionProfileType in) -> void {
            EXPECT_EQ(in, MotionProfileType::SIN2_RAMP_SINUSOIDAL_PROFILE);
          });
}

TEST(parseController, controllersValues) {
  auto const node = YAML::LoadFile("controller.yaml");
  node.as<ControllersVariant>().match(
      [](empty<MaxonControllers>) -> void {
        FAIL() << "empty MaxonControllers";
      },
      [](invalid<MaxonControllers> in) -> void { FAIL() << in.reason; },
      [](MaxonControllers in) -> void {
        in.at("Profile Position Mode")
            .match([](ProfilePositionModeConfig ppmc) -> void {
              MaxonParameterList config = ppmc.getParameterList();
              EXPECT_EQ(config.at("Max Following Error"),
                        static_cast<uint32_t>(5000));
              EXPECT_EQ(config.at("Min Position Limit"),
                        static_cast<int32_t>(-2147483648));
              EXPECT_EQ(config.at("Max Position Limit"),
                        static_cast<int32_t>(2147483647));
              EXPECT_EQ(config.at("Max Profile Velocity"),
                        static_cast<uint32_t>(8000));
              EXPECT_EQ(config.at("Profile Velocity"),
                        static_cast<uint32_t>(8000));
              EXPECT_EQ(config.at("Max Acceleration"),
                        static_cast<uint32_t>(40001));
              EXPECT_EQ(config.at("Profile Acceleration"),
                        static_cast<uint32_t>(40000));
              EXPECT_EQ(config.at("Profile Deceleration"),
                        static_cast<uint32_t>(40000));
              EXPECT_EQ(config.at("Quickstop Deceleration"),
                        static_cast<uint32_t>(10000));
              EXPECT_EQ(config.at("Motion Profile Type"),
                        static_cast<int16_t>(1));
            });
      });
}

TEST(parseController, controllersMissingMaxAcc) {
  auto const node = YAML::LoadFile("broken_contro_mode.yaml");
  node.as<ControllersVariant>().match(
      [](empty<MaxonControllers>) -> void {
        FAIL() << "empty MaxonControllers";
      },
      [](invalid<MaxonControllers> in) -> void {
        EXPECT_EQ(in.reason, "Control Mode Configuration: Profile Position "
                             "Mode Missing Max Acc: Acceleration: Missing Max "
                             "Acceleration.");
      },
      [](MaxonControllers) -> void { FAIL() << "MaxonControllers"; });
}
TEST(parseController, controllersVariant) {
  auto const node = YAML::LoadFile("roboy.yaml");
  node["Maxon"].as<ControllersVariant>().match(
      [](empty<MaxonControllers>) -> void {
        FAIL() << "empty MaxonControllers";
      },
      [](invalid<MaxonControllers> in) -> void { FAIL() << in.reason; },
      [](MaxonControllers mcs) -> void { EXPECT_EQ(mcs.size(), 1); });
}
