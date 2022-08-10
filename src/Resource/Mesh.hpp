#pragma once

#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Resource/Texture.hpp"

#include <vector>

namespace Game {

  class Mesh {
  public:
    using Handle = Resource<Mesh>;

  public:
    static Mesh::Handle cube();
    static Mesh::Handle load(const std::string& file);
    static Mesh::Handle fromVertices(const Slice<const void> vertices, const Slice<const u32> indices);
    DISALLOW_MOVE_AND_COPY(Mesh);

    const VertexArray::Handle getVertexArray() const;

    bool reload();

  private:
    enum class FileFormat {
      Obj,
    };

  private:
    Mesh(
      VertexArray::Handle vertexArray,
      VertexBuffer::Handle vertexBuffer,
      IndexBuffer::Handle  indexBuffer,
      Option<String>       filePath = None
    )
      : mVertexArray{std::move(vertexArray)}
      , mVertexBuffer{std::move(vertexBuffer)}
      , mIndexBuffer{std::move(indexBuffer)}
      , mFilePath(std::move(filePath))
    {}
    

  private:
    VertexArray::Handle  mVertexArray;
    VertexBuffer::Handle mVertexBuffer;
    IndexBuffer::Handle  mIndexBuffer;
    Option<String>       mFilePath;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(Mesh)

} // namespace Game
