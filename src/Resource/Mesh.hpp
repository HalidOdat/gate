#pragma once

#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Resource/Texture.hpp"

#include <vector>

namespace Game {

  class Mesh {
  public:
    enum class Type {
      Cube,
      File,
    };

    using Handle = Resource<Mesh>;

  public:
    static Mesh::Handle cube();
    static Mesh::Handle load(const std::string& file);
    DISALLOW_MOVE_AND_COPY(Mesh);

    const VertexArray::Handle getVertexArray() const;

    bool reload();

    inline const Option<String>& getFilePath() const { return mData.filePath; }

  private:
    enum class FileFormat {
      Obj,
    };

  private:
    struct Data {
      VertexArray::Handle  vertexArray;
      VertexBuffer::Handle vertexBuffer;
      IndexBuffer::Handle  indexBuffer;
      Option<String>       filePath;
    };

  private:
    static Mesh::Data fromVertices(const Slice<const void> vertices, const Slice<const u32> indices);

    Mesh(Data data)
      : mData{std::move(data)}
    {}

  private:
    Data mData;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(Mesh)

} // namespace Game
