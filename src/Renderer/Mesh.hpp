#pragma once

#include "Core/Base.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/IndexBuffer.hpp"
#include "Renderer/Texture.hpp"

namespace Gate {

  class Mesh {
    friend class Application;
    friend class Renderer3D;
  public:
    enum class Type {
      Cube,
      File,
    };

    using Handle = std::shared_ptr<Mesh>;

  public:
    static Mesh::Handle cube();
    static Mesh::Handle load(const std::string& file);
    DISALLOW_MOVE_AND_COPY(Mesh);

    const VertexArray::Handle getVertexArray() const;

    bool reload();

    inline const Option<String>& getFilePath() const { return mData.filePath; }
    inline Type getType() const { return mData.type; }
  private:
    struct Data {
      VertexArray::Handle  vertexArray;
      VertexBuffer::Handle vertexBuffer;
      IndexBuffer::Handle  indexBuffer;
      Option<String>       filePath;
      Type                 type;

      bool hasInstanced = false;
    };

  private:
    static void destroyAllMeshes();
    static Mesh::Data fromVertices(const Slice<const void> vertices, const Slice<const u32> indices);

  public:
    // DO NOT USE! Use the constructor functions.
    //
    // NOTE: Has to be public to be constructed with std::make_shared
    Mesh(Data data)
      : mData{std::move(data)}
    {}

    ~Mesh();

  private:
    Data mData;
  };

} // namespace Game

namespace std {

  template<>
  struct hash<Gate::Mesh::Handle> {
    size_t operator()(Gate::Mesh::Handle const& handle) const noexcept {
      return hash<Gate::usize>{}((Gate::usize)handle.get());
    }
  };

}