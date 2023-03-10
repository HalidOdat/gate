#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"

namespace Gate {

  struct Pin {
    static const constexpr u32 NULL_CONNECTION = UINT32_MAX;

    Point position;

    u32 connectionIndex{NULL_CONNECTION};
    bool active = false;
    bool visited = false;

    void render(Renderer2D& renderer, bool isOutput);
    void render(Renderer3D& renderer, bool isOutput, u32 id);
  };

}
