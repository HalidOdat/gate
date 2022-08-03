#pragma once

#include <vector>

#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Resource/Resource.hpp"
#include "Resource/Texture.hpp"

namespace Game {

  class Mesh : public Resource {
  public:
    struct MaterialData {
      enum class Type {
        Diffuse,
        Specular,
        Emission,
      };

      Type type;
      Texture2D texture;
    };

  public:
    const Ref<VertexArray> getVertexArray() const;

    const std::vector<MaterialData>& getMaterialData() const;
    void addMaterialData(MaterialData::Type type, Texture2D textue);

  public:
    struct Data {
      Ref<VertexArray>          vertexArray;
      Ref<VertexBuffer>         vertexBuffer;
      Ref<IndexBuffer>          indexBuffer;
      std::vector<MaterialData> material;
    };

  private:
    enum class FileFormat {
      Obj,
    };

  private:
    Mesh(Resource::Id id)
      : Resource(id)
    {}

    const Data& getData() const;
    Data& getData();

    static Data fromFileSource(FileFormat format, const std::string& source);
    static Data fromVertices(const Slice<const void> vertices, const Slice<const u32> indices);
    static void destroy(Data& data);

  private:
    friend class ResourceManager;
  };

} // namespace Game
