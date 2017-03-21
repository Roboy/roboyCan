#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"
#include "roboy_can/errorHandling.hpp"
#include "yaml-cpp/yaml.h"
#include <numeric>

using Baudrate = variant<invalid<KaCanOpenBaudrate>, KaCanOpenBaudrate>;
using UsbOptions = variant<invalid<KaCanOpenUsbOptions>, KaCanOpenUsbOptions>;

using Network =
    variant<empty<Networks>, Networks, missing<KaCanOpenBaudrate>,
            invalid<KaCanOpenBaudrate>, missing<KaCanOpenUsbOptions>,
            invalid<KaCanOpenUsbOptions>, missing<std::string>,
            duplicate<NetworkConfig, std::string>>;

auto growNetwork(Networks previous, YAML::const_iterator::value_type subnet)
    -> Network {
  if (!subnet.second["Baudrate"]) {
    return missing<KaCanOpenBaudrate>{};
  }
  if (!subnet.second["Driver"]) {
    return missing<KaCanOpenUsbOptions>{};
  }
  if (!subnet.second["USB Serial"]) {
    return missing<std::string>{};
  }

  return subnet.second["Baudrate"].as<Baudrate>().match(
      passAlong<invalid<KaCanOpenBaudrate>, Network>{},
      [&subnet, &previous](KaCanOpenBaudrate baudr) -> Network {
        return subnet.second["Driver"].as<UsbOptions>().match(
            passAlong<invalid<KaCanOpenUsbOptions>, Network>{},
            [&previous, &subnet,
             &baudr](KaCanOpenUsbOptions usbopt) -> Network {
              auto serial = subnet.second["USB Serial"].as<std::string>();
              auto key = subnet.first.as<std::string>();
              if (previous
                      .emplace(key, NetworkConfig(key, usbopt, baudr, serial))
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
    std::string baudString = node.as<std::string>();

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
    std::string usboptString = node.as<std::string>();

    if (usboptString == "usbtin")
      usbopt = KaCanOpenUsbOptions::USBTIN;
    else if (usboptString == "peak_linux")
      usbopt = KaCanOpenUsbOptions::PEAK;
    else
      usbopt = invalid<KaCanOpenUsbOptions>{};
    return true;
  }
};

template <> struct convert<Network> {
  static bool decode(Node const &node, Network &network) {
    network = std::accumulate(
        node.begin(), node.end(), Network{},
        [](Network nw, YAML::const_iterator::value_type subnet) {
          return nw.match(
              [&subnet](empty<Networks>) -> Network {
                return growNetwork(Networks{}, subnet);
              },
              [&subnet](Networks previous) -> Network {
                return growNetwork(previous, subnet);
              },
              passAlong<missing<KaCanOpenBaudrate>, Network>{},
              passAlong<invalid<KaCanOpenBaudrate>, Network>{},
              passAlong<missing<KaCanOpenUsbOptions>, Network>{},
              passAlong<invalid<KaCanOpenUsbOptions>, Network>{},
              passAlong<missing<std::string>, Network>{},
              passAlong<duplicate<NetworkConfig, std::string>, Network>{});
        });
    return true;
  }
};

}; // end of namespace
