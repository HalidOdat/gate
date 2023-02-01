#pragma once

#include "Renderer/Renderer.hpp"
#include "Editor/Point.hpp"

namespace Gate {
  
  struct Wire {
    Point from;
    Point to;

    std::vector<u32> connectionIndexes{};

    bool active = false;
    bool visited = false;

    void render(Renderer& renderer);
  };

}