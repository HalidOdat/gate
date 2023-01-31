#pragma once

#include "Editor/Point.hpp"

#include "Renderer/Renderer.hpp"

namespace Gate {
  
  struct Wire {
    Point from;
    Point to;

    bool active = false;
    bool visited = false;

    void render(Renderer& renderer);
  };

}