#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"
#include "Editor/Point.hpp"

namespace Gate {

  struct Pin {
    static const constexpr u32 NULL_CONNECTION = UINT32_MAX;
    enum class Type {
      Input,
      Output,
    };
    Type type;
    Point position;

    u32 connectionIndex{NULL_CONNECTION};
    bool active = false;
  };

}