#pragma once

#include "yaml-cpp/yaml.h"

template <typename T> struct empty {};

template <typename T> struct invalid { std::string paramName; };

template <typename T> struct missing { std::string paramName; };

template <typename T, typename Key> struct duplicate { Key key; };

template <typename Case, typename Variant> struct passAlong {
  auto operator()(Case c) -> Variant { return Variant{c}; }
};

template <typename T>
variant<invalid<T>, T> withinBounds(YAML::Node nn, std::string key, T lower,
                                    T upper) {
  T vari = nn[key].as<T>();
  if (vari < lower || vari > upper) {
    return invalid<T>{key};
  }
  return {vari};
};
