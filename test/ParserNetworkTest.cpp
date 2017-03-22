#include "roboy_can/networkParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(networks_yaml, ParseNetwork) {
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

TEST(networks_yaml, NetworkSize) {
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
TEST(networks_yaml, Network_eft) {
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
  EXPECT_EQ(network["Left"].getBaudrate(), KaCanOpenBaudrate::Baud1M);
  EXPECT_EQ(network["Left"].getUsbSerial(), std::string("A1B2C3"));

  EXPECT_EQ(network["Right"].getDriver(), KaCanOpenUsbOptions::PEAK);
  EXPECT_EQ(network["Right"].getBaudrate(), KaCanOpenBaudrate::Baud500k);
  EXPECT_EQ(network["Right"].getUsbSerial(), std::string("D4E5F6"));
}

TEST(networks_yaml, missing_Baudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_missing_baudrate"].as<Network>();

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

  EXPECT_EQ(isNetwork, 2);
}

TEST(networks_yaml, wrong_Baudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_wrong_baudrate"].as<Network>();

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

  EXPECT_EQ(isNetwork, 3);
}

TEST(networks_yaml, missing_UsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_missing_UsbOption"].as<Network>();

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

  EXPECT_EQ(isNetwork, 4);
}

TEST(networks_yaml, wrong_UsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_wrong_UsbOption"].as<Network>();

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

  EXPECT_EQ(isNetwork, 5);
}
TEST(networks_yaml, missing_USBSerial_node) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_missing_serial"].as<Network>();

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

  EXPECT_EQ(isNetwork, 6);
}

TEST(networks_yaml, empty_USBSerial_node) {
  auto node = YAML::LoadFile("networks.yaml");
  EXPECT_THROW(
      try {
        Network nw = node["Network_empty_serial"].as<Network>();
      } catch (const YAML::BadConversion &e) {
        EXPECT_STREQ(std::string("bad conversion").c_str(),
                     std::string(e.msg).c_str());
        throw;
      },
      YAML::BadConversion);
}

TEST(networks_yaml, duplicateNetwork) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_duplicate_network"].as<Network>();

  Networks networks;

  std::pair<unsigned int, std::string> isNetwork;

  isNetwork = nw.match(
      [](empty<Networks>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(0, "");
      },
      [](Networks nn) -> std::pair<unsigned int, std::string> {
        return std::make_pair(1, "");
      },
      [](missing<KaCanOpenBaudrate>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(2, "");
      },
      [](invalid<KaCanOpenBaudrate>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(3, "");
      },
      [](missing<KaCanOpenUsbOptions>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(4, "");
      },
      [](invalid<KaCanOpenUsbOptions>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(5, "");
      },
      [](missing<std::string>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(6, "");
      },
      [](duplicate<NetworkConfig, std::string> dn)
          -> std::pair<unsigned int, std::string> {
        return std::make_pair(7, dn.key);
      });

  EXPECT_EQ(isNetwork.first, 7);
  EXPECT_EQ(isNetwork.second, "Left");
}

TEST(networks_yaml, duplicateDriver) {
  auto node = YAML::LoadFile("networks.yaml");
  Network nw = node["Network_duplicate_driver"].as<Network>();

  Networks networks;

  std::pair<unsigned int, std::string> isNetwork;

  isNetwork = nw.match(
      [](empty<Networks>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(0, "");
      },
      [](Networks nn) -> std::pair<unsigned int, std::string> {
        return std::make_pair(1, "");
      },
      [](missing<KaCanOpenBaudrate>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(2, "");
      },
      [](invalid<KaCanOpenBaudrate>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(3, "");
      },
      [](missing<KaCanOpenUsbOptions>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(4, "");
      },
      [](invalid<KaCanOpenUsbOptions>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(5, "");
      },
      [](missing<std::string>) -> std::pair<unsigned int, std::string> {
        return std::make_pair(6, "");
      },
      [](duplicate<NetworkConfig, std::string>)
          -> std::pair<unsigned int, std::string> {
        return std::make_pair(7, "Left");
      });

  EXPECT_EQ(isNetwork.first, 7);
  EXPECT_EQ(isNetwork.second, "Driver");
}
