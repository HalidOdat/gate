#pragma once

#include "Resource/Resource.hpp"

namespace Gate {

  // TODO: Support indices of different types, like unsigned short
  class IndexBuffer {
  public:
    using Handle = Resource<IndexBuffer>;

  public:
    [[nodiscard]] static IndexBuffer::Handle create(Slice<const u32> slice);
    DISALLOW_COPY_AND_ASSIGN(IndexBuffer);

    ~IndexBuffer();

    void bind();
    void unbind();

    inline u32 getCount() const { return this->count; }

  private:
    IndexBuffer(u32 id, u32 count)
      : id{id}, count{count}
    {}

  private:
    u32 id;
    u32 count;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(IndexBuffer)

} // namespace Gate


