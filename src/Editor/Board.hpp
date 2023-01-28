#pragma once

#include "Editor/Line.hpp"
#include "Editor/Component.hpp"

#include "Renderer/Renderer.hpp"

namespace Game {

  struct Board {
    ~Board();

    std::vector<Component*> components;
    std::vector<std::vector<u32>> connections;
    std::vector<Line> lines;

    void push_line(const Line& line);
    u32 push_component(Component* component);
    void render(Renderer& renderer);
  };

}
