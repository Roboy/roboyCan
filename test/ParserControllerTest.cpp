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
TEST(MissingMaxAcceleration, controller_yaml) {
  auto const node = YAML::LoadFile("controller.yaml");

  Acceleration acc = node["Control Mode Configuration"]
                         ["Profile Position Mode Missing Max Acc"]
                             .as<Acceleration>();

  std::string isAcceleration = "";

  isAcceleration =
      acc.match([](empty<MaxonParameterList>) -> unsigned int { return 0; },
                [](missing<MaxonParameter> mm) -> unsigned int {
                  std::cout << mm.paramName << std::endl;
                  return 1;
                },
                [](invalid<MaxonParameter> mm) -> unsigned int {
                  std::cout << mm.paramName << std::endl;
                  return 2;
                },
                [](MaxonParameterList) -> unsigned int { return 3; });

  EXPECT_EQ(isAcceleration, 0);
}
// TEST(MissingProfileAcceleration, controller_yaml) {
//   auto const node = YAML::LoadFile("controller.yaml");
//
//   Acceleration acc = node["Control Mode Configuration"]
//                          ["Profile Position Mode Missing Profile Acc"]
//                              .as<Acceleration>();
//
//   std::string isAcceleration = "";
//
//   isAcceleration =
//       acc.match([](empty<MaxonParameterList>) -> unsigned int { return 0;
//       },
//                 [](missing<MaxonParameter> mm) -> unsigned int {
//                   std::cout << mm.paramName << std::endl;
//                   return 1;
//                 },
//                 [](invalid<MaxonParameter> mm) -> unsigned int {
//                   std::cout << mm.paramName << std::endl;
//                   return 2;
//                 },
//                 [](MaxonParameterList) -> unsigned int { return 3; });
//
//   EXPECT_EQ(isAcceleration, 2);
// }
