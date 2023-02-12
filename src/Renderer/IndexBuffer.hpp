#pragma once

#include "Core/Base.hpp"

namespace Gate {

  class IndexBuffer {
  public:
    using Handle = std::shared_ptr<IndexBuffer>;

  public:
    [[nodiscard]] static IndexBuffer::Handle create(Slice<const u32> slice);
    DISALLOW_COPY_AND_ASSIGN(IndexBuffer);

    ~IndexBuffer();

    void bind();
    void unbind();

    inline u32 getCount() const { return this->count; }

  public:
    // DO NOT USE! Use the builder!
    //
    // NOTE: It has to be public so it can be constructed by std::make_shared.
    IndexBuffer(u32 id, u32 count)
      : id{id}, count{count}
    {}

  private:
    u32 id;
    u32 count;
  };

} // namespace Gate


