#pragma once

#include <vector>

#include "Core/Type.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"

namespace Game {

  class VertexArray {
  public:
    [[nodiscard]] static Ref<VertexArray> create();
    
    DISALLOW_MOVE_AND_COPY(VertexArray);
    ~VertexArray();

    void bind();
    void unbind();

    void addVertexBuffer(Ref<VertexBuffer> buffer);
    void setIndexBuffer(Ref<IndexBuffer> buffer);

    inline u32 getId() { return id; }

    void draw();

  private:
    VertexArray();

  private:
    u32 id;
    u32 vertexAttributeIndex = 0;
    std::vector<Ref<VertexBuffer>> buffers;
    Ref<IndexBuffer> indexBuffer;
  };

} // namespace Game