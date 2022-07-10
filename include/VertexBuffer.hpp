#pragma once

#include <vector>

#include "Core/Type.hpp"

namespace Game {

  class VertexBuffer {
  public:
    VertexBuffer(void* data, usize length);
    ~VertexBuffer();

    void bind();
    void unbind();

  private:
    u32 id;
  };

} // namespace Game