#include "roboy_can/networkParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(ParseNetwork, networks_yaml) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network"].as<Network>();

  Networks networks;

  unsigned int isNetwork = 12;

  isNetwork = nw.match(
      [](empty<Networks>) -> unsigned int { return 0; },
      [](Networks nn) -> unsigned int { return 1; },
      [](missing<KaCanOpenBaudrate>) -> unsigned int { return 2; },
      [](invalid<KaCanOpenBaudrate>) -> unsigned int { return 3; },
      [](missing<KaCanOpenUsbOptions>) -> unsigned int { return 4; },
      [](invalid<KaCanOpenUsbOptions>) -> unsigned int { return 5; },
      [](missing<std::string>) -> unsigned int { return 6; },
      [](duplicate<NetworkConfig, std::string>) -> unsigned int { return 7; });

  EXPECT_EQ(isNetwork, 1);
}

TEST(NetworkSize, networks_yaml) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network"].as<Network>();

  Networks networks;

  unsigned int nwsize = 7;

  nwsize = nw.match(
      [](empty<Networks>) -> unsigned int { return 0; },
      [](Networks nn) -> unsigned int { return nn.size(); },
      [](missing<KaCanOpenBaudrate>) -> unsigned int { return 0; },
      [](invalid<KaCanOpenBaudrate>) -> unsigned int { return 0; },
      [](missing<KaCanOpenUsbOptions>) -> unsigned int { return 0; },
      [](invalid<KaCanOpenUsbOptions>) -> unsigned int { return 0; },
      [](missing<std::string>) -> unsigned int { return 0; },
      [](duplicate<NetworkConfig, std::string>) -> unsigned int { return 0; });
  EXPECT_EQ(nwsize, 2);
}
TEST(Network_eft, networks_yaml) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network"].as<Network>();

  unsigned int nwsize = 7;

  Networks network = nw.match(
      [](empty<Networks>) -> Networks { return Networks{}; },
      [](Networks nn) -> Networks { return nn; },
      [](missing<KaCanOpenBaudrate>) -> Networks { return Networks{}; },
      [](invalid<KaCanOpenBaudrate>) -> Networks { return Networks{}; },
      [](missing<KaCanOpenUsbOptions>) -> Networks { return Networks{}; },
      [](invalid<KaCanOpenUsbOptions>) -> Networks { return Networks{}; },
      [](missing<std::string>) -> Networks { return Networks{}; },
      [](duplicate<NetworkConfig, std::string>) -> Networks {
        return Networks{};
      });
  EXPECT_EQ(network["Left"].getDriver(), KaCanOpenUsbOptions::USBTIN);
}
//
// TEST(valid, baudrate) {
//   auto node = YAML::LoadFile("baudrate.yaml");
//   Network nw = node["Network"].as<Baudrate>();
//
//   Networks networks;
//
//   unsigned int nwsize = 7;
//
//   nwsize = nw.match(
//       [](empty<Networks>) -> unsigned int { return 0; },
//       [](Networks nn) -> unsigned int { return nn.size(); },
//       [](missing<KaCanOpenBaudrate>) -> unsigned int { return 0; },
//       [](invalid<KaCanOpenBaudrate>) -> unsigned int { return 0; },
//       [](missing<KaCanOpenUsbOptions>) -> unsigned int { return 0; },
//       [](invalid<KaCanOpenUsbOptions>) -> unsigned int { return 0; },
//       [](missing<std::string>) -> unsigned int { return 0; },
//       [](duplicate<NetworkConfig, std::string>) -> unsigned int { return 0;
//       });
//   EXPECT_EQ(nwsize, 2);
// }

// TEST(ParseRoboy, root) {
//
//   auto node = YAML::LoadFile("roboy.yaml");
//   RoboyConfig boy = node.as<RoboyConfig>();
//   EXPECT_EQ(boy.configs.size(), 1);
//   EXPECT_EQ(boy.configs.count(MotorControlArchitecture::MAXON), 1);
//   uint8_t ret_val = boy.configs.at(MotorControlArchitecture::MAXON)
//                         .match([](MaxonConfig config) -> uint8_t {
//                           return config.motors_.size();
//                         });
//   EXPECT_EQ(ret_val, 11);
//   ret_val = boy.configs.at(MotorControlArchitecture::MAXON)
//                 .match([](MaxonConfig config) -> uint8_t {
//                   return config.motors_["SoberHunters"].canId;
//                 });
//   EXPECT_EQ(ret_val, 31);
// }
