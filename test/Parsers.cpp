#include "roboy_can/Parser.hpp"
#include <gtest/gtest.h>
using namespace Maxon;
TEST(ParseRoboy, root) {
  auto node = YAML::LoadFile("roboy.yaml");
  RoboyConfig boy = node.as<RoboyConfig>();
  EXPECT_EQ(boy.configs.size(), 1);
  EXPECT_EQ(boy.configs.count(MotorControlArchitecture::MAXON), 1);
  uint8_t ret_val = boy.configs.at(MotorControlArchitecture::MAXON)
                        .match([](MaxonConfig config) -> uint8_t {
                          return config.motors_.size();
                        });
  EXPECT_EQ(ret_val, 11);
  ret_val = boy.configs.at(MotorControlArchitecture::MAXON)
                .match([](MaxonConfig config) -> uint8_t {
                  return config.motors_["SoberHunters"].canId;
                });
  EXPECT_EQ(ret_val, 31);
}
