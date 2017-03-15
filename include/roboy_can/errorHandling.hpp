#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "yaml-cpp/yaml.h"

template <typename T> struct empty {};

template <typename T> struct invalid {};
template <> struct invalid<SensorConfig> { std::string missingParameter; };

template <typename T> struct missing {};
template <> struct missing<SensorConfig> { std::string missingParameter; };

template <typename T, typename Key> struct duplicate { Key key; };

template <typename Case, typename Variant> struct passAlong {
  auto operator()(Case c) -> Variant { return Variant{c}; }
};

template <typename T> struct foo {
  YAML::Node yn;
  T lower_bound;
  T upper_bound;
};
