#pragma once
#include "roboy_can/MaxonConfig.hpp"
#include "roboy_can/Types.hpp"

template <typename T, typename DuplicateReason>
using erroneous =
    variant<empty<T>, invalid<T>, missing<T>, duplicate<T, DuplicateReason>>;

template <typename... Components> struct invalid_component {
  using component_t = variant<erroneous<Components>...>;
  component_t component;

  template <typename T> struct visitor {
    auto operator()(T const &err) { return err.to_string(); }
  };

  auto to_string() -> std::string {
    return component.match(visitor<Components>{}...);
  }
};

template <> class invalid<MotorConfigs> {
  using reason_t =
      invalid_component<SensorConfig, Networks, MaxonControllers, motorNames>;
  reason_t reason;
  auto to_string() -> std::string { return reason.to_string(); };

  // using MotorConfigVariant = variant<erroneous<MotorConfigs>, MotorConfigs>;
