#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer.hpp"

namespace Gate {

  struct Pin {
    static const constexpr u32 NULL_CONNECTION = UINT32_MAX;

    Point position;

    u32 connectionIndex{NULL_CONNECTION};
    bool active = false;
    bool visited = false;

    void render(Renderer& renderer, bool isOutput);
  };

}
