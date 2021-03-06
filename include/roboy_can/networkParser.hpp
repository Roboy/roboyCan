#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

using Baudrate = variant<invalid<KaCanOpenBaudrate>, KaCanOpenBaudrate>;
using UsbOptions = variant<invalid<KaCanOpenUsbOptions>, KaCanOpenUsbOptions>;
struct UsbSerial {
  std::string serial;
};
using NetworkVariant =
    variant<empty<Networks>, Networks, missing<Networks>,
            missing<KaCanOpenBaudrate>, invalid<KaCanOpenBaudrate>,
            missing<KaCanOpenUsbOptions>, invalid<KaCanOpenUsbOptions>,
            missing<UsbSerial>, duplicate<NetworkConfig, std::string>>;

inline auto growNetwork(Networks previous,
                        YAML::const_iterator::value_type subnet,
                        std::unordered_set<std::string> &serial)
    -> NetworkVariant {
  if (!subnet.second["Baudrate"]) {
    return missing<KaCanOpenBaudrate>{};
  }
  if (!subnet.second["Driver"]) {
    return missing<KaCanOpenUsbOptions>{};
  }
  if (!subnet.second["USB Serial"]) {
    return missing<UsbSerial>{};
  }

  return subnet.second["Baudrate"].as<Baudrate>().match(
      passAlong<invalid<KaCanOpenBaudrate>, NetworkVariant>{},
      [&subnet, &serial, &previous](KaCanOpenBaudrate baudr) -> NetworkVariant {
        return subnet.second["Driver"].as<UsbOptions>().match(
            passAlong<invalid<KaCanOpenUsbOptions>, NetworkVariant>{},
            [&previous, &serial, &subnet,
             &baudr](KaCanOpenUsbOptions usbopt) -> NetworkVariant {
              auto key = subnet.first.as<std::string>();
              std::string temp_serial;
              try {
                temp_serial = subnet.second["USB Serial"].as<std::string>();

              } catch (const YAML::BadConversion &e) {
                return missing<UsbSerial>{};
              }
              if (serial.emplace(temp_serial).second == false) {
                return duplicate<NetworkConfig, std::string>{"Usb Serial"};
              }
              if (previous
                      .emplace(key,
                               NetworkConfig(key, usbopt, baudr, temp_serial))
                      .second == false) {
                return {duplicate<NetworkConfig, std::string>{key}};
              }
              return {previous};
            });
      });
};

namespace YAML {

template <> struct convert<Baudrate> {
  static bool decode(Node const &node, Baudrate &baud) {
    std::string baudString;
    try {
      baudString = node.as<std::string>();
    } catch (const YAML::BadConversion &e) {
      baud = invalid<KaCanOpenBaudrate>{};
      return true;
    }

    if (baudString == "10k")
      baud = KaCanOpenBaudrate::Baud10k;
    else if (baudString == "20k")
      baud = KaCanOpenBaudrate::Baud20k;
    else if (baudString == "50k")
      baud = KaCanOpenBaudrate::Baud50k;
    else if (baudString == "100k")
      baud = KaCanOpenBaudrate::Baud100k;
    else if (baudString == "125k")
      baud = KaCanOpenBaudrate::Baud125k;
    else if (baudString == "250k")
      baud = KaCanOpenBaudrate::Baud250k;
    else if (baudString == "500k")
      baud = KaCanOpenBaudrate::Baud500k;
    else if (baudString == "800k")
      baud = KaCanOpenBaudrate::Baud800k;
    else if (baudString == "1M")
      baud = KaCanOpenBaudrate::Baud1M;
    else
      baud = invalid<KaCanOpenBaudrate>{};
    return true;
  }
};

template <> struct convert<UsbOptions> {
  static bool decode(Node const &node, UsbOptions &usbopt) {
    std::string usboptString;
    try {
      usboptString = node.as<std::string>();
    } catch (const YAML::BadConversion &e) {
      usbopt = invalid<KaCanOpenUsbOptions>{};
      return true;
    }

    if (usboptString == "usbtin")
      usbopt = KaCanOpenUsbOptions::USBTIN;
    else if (usboptString == "peak_linux")
      usbopt = KaCanOpenUsbOptions::PEAK;
    else
      usbopt = invalid<KaCanOpenUsbOptions>{};
    return true;
  }
};

template <> struct convert<NetworkVariant> {
  static bool decode(Node const &node, NetworkVariant &network) {
    if (!node["Network"]) {
      network = missing<Networks>{};
      return true;
    }
    Node newnode = node["Network"];
    std::unordered_set<std::string> serials;
    network = std::accumulate(
        newnode.begin(), newnode.end(), NetworkVariant{},
        [&serials](NetworkVariant nw, YAML::const_iterator::value_type subnet) {
          return nw.match(
              [&subnet, &serials](empty<Networks>) -> NetworkVariant {
                return growNetwork(Networks{}, subnet, serials);
              },
              [&subnet, &serials](Networks previous) -> NetworkVariant {
                return growNetwork(previous, subnet, serials);
              },
              passAlong<missing<Networks>, NetworkVariant>{},
              passAlong<missing<KaCanOpenBaudrate>, NetworkVariant>{},
              passAlong<invalid<KaCanOpenBaudrate>, NetworkVariant>{},
              passAlong<missing<KaCanOpenUsbOptions>, NetworkVariant>{},
              passAlong<invalid<KaCanOpenUsbOptions>, NetworkVariant>{},
              passAlong<missing<UsbSerial>, NetworkVariant>{},
              passAlong<duplicate<NetworkConfig, std::string>,
                        NetworkVariant>{});
        });
    return true;
  }
};

}; // end of namespace
