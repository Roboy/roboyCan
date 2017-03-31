#include "roboy_can/networkParser.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(parseNetwork, networks) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { SUCCEED(); },
           [](invalid<Networks> in) -> void { FAIL() << in.reason; });
}
TEST(parseNetwork, brokenRoboy) {
  auto node = YAML::LoadFile("broken_roboy.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Missing Network Section.");
           });
}

TEST(parseNetwork, networksSize) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks nn) -> void { EXPECT_EQ(nn.size(), 2); },
           [](invalid<Networks> in) -> void { FAIL() << in.reason; });
}

TEST(parseNetwork, networkEFT) {
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
      [](invalid<Networks> in) -> void { FAIL() << in.reason; });
}

TEST(parseNetwork, missingBaudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_baudrate"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Missing Baudrate.");
           });
}

TEST(parseNetwork, wrongBaudrate) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_wrong_baudrate"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Baudrate: Not a valid value.");
           });
}

TEST(parseNetwork, missingUsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_UsbOption"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Missing Driver.");
           });
}

TEST(parseNetwork, wrongUsbOption) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_wrong_UsbOption"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Driver: Not a valid value.");
           });
}
TEST(parseNetwork, missingUsbSerial) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_missing_serial"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Missing USB Serial.");
           });
}

TEST(parseNetwork, emptyUsbSerial) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_empty_serial"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason,
                       "USB Serial: Value missing or not of type std::string.");
           });
}

TEST(parseNetwork, duplicateNetwork) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_duplicate_network"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "Network label is duplicated.");
           });
}

TEST(parseNetwork, duplicateUsbSerial) {
  auto node = YAML::LoadFile("networks.yaml");
  NetworkVariant nw = node["Network_duplicate_serial"].as<NetworkVariant>();

  nw.match([](empty<Networks>) -> void { FAIL() << "empty<Networks>"; },
           [](Networks) -> void { FAIL() << "Networks"; },
           [](invalid<Networks> in) -> void {
             EXPECT_EQ(in.reason, "USB Serial: Serial is duplicated.");
           });
}
