#pragma once

#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Resource/Resource.hpp"

namespace Game {

  class Mesh : public Resource {
  public:
    const Ref<VertexArray> getVertexArray() const;

  public:
    struct Data {
      Ref<VertexArray>  vertexArray;
      Ref<VertexBuffer> vertexBuffer;
      Ref<IndexBuffer>  indexBuffer;
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

    static Data fromFileSource(FileFormat format, const std::string& source);
    static void destroy(Data& data);

  private:
    friend class ResourceManager;
  };

} // namespace Game
