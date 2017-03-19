#pragma once

#include "roboy_can/MaxonConfig.hpp"
#include "yaml-cpp/yaml.h"

template <typename T> struct empty {};

template <typename T> struct invalid {};
template <> struct invalid<uint32_t> { uint32_t outOfBoundInput; };
template <> struct invalid<uint16_t> { uint16_t outOfBoundInput; };
template <> struct invalid<int16_t> { int16_t outOfBoundInput; };
template <> struct invalid<int32_t> { int32_t outOfBoundInput; };
template <> struct invalid<uint8_t> { uint8_t outOfBoundInput; };

template <typename T> struct missing {};

template <typename T, typename Key> struct duplicate { Key key; };

template <typename Case, typename Variant> struct passAlong {
  auto operator()(Case c) -> Variant { return Variant{c}; }
};

template <typename T>
variant<invalid<T>, T> withinBounds(T vari, T lower, T upper) {
  if (vari < lower || vari > upper) {
    return invalid<T>{vari};
  }
  return {vari};
};
