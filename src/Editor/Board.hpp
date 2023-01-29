#pragma once

#include "Editor/Line.hpp"
#include "Editor/Component.hpp"

#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"

namespace Gate {

  struct Board {
    Board();
    ~Board();

    const static constexpr u32 GRID_SIZE = 20;

    FrameBuffer::Handle gridFrameBuffer;
    Texture::Handle gridTexture;

    std::vector<Component*> components;
    std::vector<std::vector<u32>> connections;
    std::vector<Line> lines;

    void push_line(const Line& line);
    u32 push_component(Component* component);

    void invalidate(u32 width, u32 height);

    void renderAll(Renderer& renderer);
    void render(Renderer& renderer);
    void renderGrid(Renderer& renderer);
  };

}
