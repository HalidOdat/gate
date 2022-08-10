#pragma once

#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"

#include <vector>

namespace Game {

  class VertexArray {
  public:
    using Handle = Resource<VertexArray>;

  public:
    [[nodiscard]] static VertexArray::Handle create();
    DISALLOW_MOVE_AND_COPY(VertexArray);
    ~VertexArray();

    void bind();
    void unbind();

    void addVertexBuffer(VertexBuffer::Handle buffer);
    void setIndexBuffer(IndexBuffer::Handle buffer);

    inline u32 getId() { return mId; }

    void drawIndices();
    void drawIndices(const u32 count);

    void drawArrays(u32 count);

  private:
    VertexArray(u32 id)
      : mId(id)
    {}

  private:
    u32 mId;
    u32 mVertexAttributeIndex = 0;
    std::vector<VertexBuffer::Handle> mBuffers;
    IndexBuffer::Handle mIndexBuffer;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(VertexArray);

} // namespace Game