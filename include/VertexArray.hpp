#pragma once

#include <vector>

#include "Core/Type.hpp"
#include "VertexBuffer.hpp"

namespace Game {

  class VertexArray {
  public:
    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();

    void addVertexBuffer(VertexBuffer&& buffer);

    inline u32 getId() { return id; }

    void draw();

  private:
    u32 id;
  };

} // namespace Game