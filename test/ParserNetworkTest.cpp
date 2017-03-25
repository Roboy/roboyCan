#include "roboy_can/networkParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(networks_yaml, ParseNetwork) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { SUCCEED(); },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "missing<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(broken_roboy, ParseNetwork) {
  auto node = YAML::LoadFile("broken_roboy.yaml");
  NetworkVariant nw = node.as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn>"; },
           [](missing<Networks>) -> void { SUCCEED(); },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "missing<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, NetworkSize) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  Networks networks;

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { EXPECT_EQ(nn.size(), 2); },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "missing<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}
TEST(networks_yaml, Network_eft) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  nw.match(
      [](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
      [](Networks network) -> void {
        EXPECT_EQ(network["Left"].getDriver(), KaCanOpenUsbOptions::USBTIN);
        EXPECT_EQ(network["Left"].getBaudrate(), KaCanOpenBaudrate::Baud1M);
        EXPECT_EQ(network["Left"].getUsbSerial(), std::string("A1B2C3"));

        EXPECT_EQ(network["Right"].getDriver(), KaCanOpenUsbOptions::PEAK);
        EXPECT_EQ(network["Right"].getBaudrate(), KaCanOpenBaudrate::Baud500k);
        EXPECT_EQ(network["Right"].getUsbSerial(), std::string("D4E5F6"));
      },
      [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
      [](missing<KaCanOpenBaudrate>) -> void {
        FAIL() << "missing<KaCanOpenBaudrate>";
      },
      [](invalid<KaCanOpenBaudrate>) -> void {
        FAIL() << "invalid<KaCanOpenBaudrate>";
      },
      [](missing<KaCanOpenUsbOptions>) -> void {
        FAIL() << "missing<KaCanOpenUsbOptions>";
      },
      [](invalid<KaCanOpenUsbOptions>) -> void {
        FAIL() << "invalid<KaCanOpenUsbOptions>";
      },
      [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
      [](duplicate<NetworkConfig, std::string>) -> void {
        FAIL() << "duplicate<NetworkConfig, std::string>";
      });
}

TEST(networks_yaml, missing_Baudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_baudrate"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void { SUCCEED(); },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, wrong_Baudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_wrong_baudrate"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "missing<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void { SUCCEED(); },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, missing_UsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_UsbOption"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void { SUCCEED(); },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, wrong_UsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_wrong_UsbOption"].as<NetworkVariant>();
  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void { SUCCEED(); },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}
TEST(networks_yaml, missing_USBSerial_node) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_serial"].as<NetworkVariant>();
  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { SUCCEED(); },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, empty_USBSerial_node) {
  auto node = YAML::LoadFile("networks.yaml");

  NetworkVariant nw = node["Network_empty_serial"].as<NetworkVariant>();
  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { SUCCEED(); },
           [](duplicate<NetworkConfig, std::string>) -> void {
             FAIL() << "duplicate<NetworkConfig, std::string>";
           });
}

TEST(networks_yaml, duplicateNetwork) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_duplicate_network"].as<NetworkVariant>();
  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { SUCCEED(); },
           [](duplicate<NetworkConfig, std::string> dn) -> void {
             EXPECT_EQ(dn.key, "Left");
           });
}

TEST(networks_yaml, duplicateUsbSerial) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_duplicate_serial"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { FAIL() << "Networks nn"; },
           [](missing<Networks>) -> void { FAIL() << "missing<Networks>"; },
           [](missing<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](invalid<KaCanOpenBaudrate>) -> void {
             FAIL() << "invalid<KaCanOpenBaudrate>";
           },
           [](missing<KaCanOpenUsbOptions>) -> void {
             FAIL() << "missing<KaCanOpenUsbOptions>";
           },
           [](invalid<KaCanOpenUsbOptions>) -> void {
             FAIL() << "invalid<KaCanOpenUsbOptions>";
           },
           [](missing<UsbSerial>) -> void { FAIL() << "missing<UsbSerial>"; },
           [](duplicate<NetworkConfig, std::string> dn) -> void {
             EXPECT_EQ(dn.key, "Usb Serial");
           });
}
