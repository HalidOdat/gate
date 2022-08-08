#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  // TODO: Support indices of different types, like unsigned short
  class IndexBuffer {
  public:
    using Handle = Ref<IndexBuffer>;

  public:
    [[nodiscard]] static IndexBuffer::Handle create(Slice<const u32> slice);
    DISALLOW_COPY_AND_ASSIGN(IndexBuffer);

    ~IndexBuffer();

    void bind();
    void unbind();

    inline u32 getCount() const { return this->count; }
  
  private:
    IndexBuffer(Slice<const u32> slice);

  private:
    u32 id;
    u32 count;
  };

} // namespace Game


