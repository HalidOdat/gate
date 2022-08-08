#pragma once

#include <vector>

#include "Core/Type.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"

namespace Game {

  class VertexArray {
  public:
    using Handle = Ref<VertexArray>;

  public:
    [[nodiscard]] static VertexArray::Handle create();
    DISALLOW_MOVE_AND_COPY(VertexArray);
    ~VertexArray();

    void bind();
    void unbind();

    void addVertexBuffer(VertexBuffer::Handle buffer);
    void setIndexBuffer(IndexBuffer::Handle buffer);

    inline u32 getId() { return id; }

    void drawIndices();
    void drawIndices(const u32 count);

    void drawArrays(u32 count);

  private:
    VertexArray();

  private:
    u32 id;
    u32 vertexAttributeIndex = 0;
    std::vector<VertexBuffer::Handle> buffers;
    IndexBuffer::Handle indexBuffer;
  };

} // namespace Game