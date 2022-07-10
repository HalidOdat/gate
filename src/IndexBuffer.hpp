#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class IndexBuffer : public CreateObject<IndexBuffer> {
  public:
    DISALLOW_COPY_AND_ASSIGN(IndexBuffer);

    IndexBuffer(Slice<u32> slice);
    ~IndexBuffer();

    void bind();
    void unbind();

  private:
    u32 id;
  };

} // namespace Game


