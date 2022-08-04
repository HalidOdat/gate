#pragma once

#include <vector>

#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Resource/Resource.hpp"
#include "Resource/Texture.hpp"

namespace Game {

  class Mesh {
  public:
    using Handle = Resource<Mesh>;

    struct MaterialData {
      enum class Type {
        Diffuse,
        Specular,
        Emission,
      };

      Type type;
      Texture2D::Handle texture;
    };

  public:
    DISALLOW_MOVE_AND_COPY(Mesh);
    const Ref<VertexArray> getVertexArray() const;

    const std::vector<MaterialData>& getMaterialData() const;
    void addMaterialData(MaterialData::Type type, Texture2D::Handle textue);

  public:
    struct Data {
      Ref<VertexArray>          vertexArray;
      Ref<VertexBuffer>         vertexBuffer;
      Ref<IndexBuffer>          indexBuffer;
      std::vector<MaterialData> material;
    };

    Mesh(Data data)
      : mData(std::move(data))
    {}

  private:
    enum class FileFormat {
      Obj,
    };

  private:
    static Data fromFileSource(FileFormat format, const std::string& source);
    static Data fromVertices(const Slice<const void> vertices, const Slice<const u32> indices);

  private:
    Data mData;

    friend class ResourceManager;
  };

} // namespace Game
