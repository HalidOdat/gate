#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class IndexBuffer {
  public:
    [[nodiscard]] static Ref<IndexBuffer> create(Slice<const u32> slice);
    DISALLOW_COPY_AND_ASSIGN(IndexBuffer);

    ~IndexBuffer();

    void bind();
    void unbind();
  
  private:
    IndexBuffer(Slice<const u32> slice);

  private:
    u32 id;
  };

} // namespace Game


