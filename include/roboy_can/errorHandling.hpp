#pragma once

#include "yaml-cpp/yaml.h"

template <typename T> struct empty {};

// template <typename T> struct invalid { std::string reason; };

template <typename T> struct invalid {
  explicit invalid(std::string st) : reason{st} {};
  std::string reason;
};

template <typename T> struct missing { std::string reason; };

template <typename T, typename Key> struct duplicate { Key key; };

template <typename Case, typename Variant> struct passAlong {
  auto operator()(Case c) -> Variant { return Variant{c}; }
};

template <typename T>
variant<invalid<T>, T> withinBounds(YAML::Node nn, std::string key, T lower,
                                    T upper) {
  T vari;
  try {
    vari = nn[key].as<T>();
  } catch (const YAML::BadConversion &e) {
    return invalid<T>{key +
                      "Value missing or (unlikely: ) not of correct type."};
  }
  if (vari < lower || vari > upper) {
    return invalid<T>{key + " : Value out of bounds."};
  }
  return {vari};
};
