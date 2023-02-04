#include "Renderer/Mesh.hpp"
#include "Utils/File.hpp"

#include <sstream>
#include <fstream>
#include <optional>
#include <unordered_map>
#include <cstring>

namespace Gate {

  static std::unordered_map<String, Mesh::Handle> cachedMeshes;
  static Mesh::Handle cachedCube;

  void Mesh::destroyAllMeshes() {
    cachedMeshes.clear();
    cachedCube = nullptr;
  }

  struct Vertex {
    Vec3 position;
    Vec2 texture;
    Vec3 normals;
  };

  static std::pair<std::vector<Vertex>, std::vector<u32>> parseObjFile(const std::string& source) {
    std::stringstream stream(source);

    std::vector<Vec3> vertices;
    std::vector<Vec2> textures;
    std::vector<Vec3> normals;

    std::vector<Vertex> result;
    std::vector<u32>    indices;
    bool first = true;
    for (std::string line; std::getline(stream, line); ) {
      if (strncmp(line.c_str(), "v ", 2) == 0) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "v %f %f %f", &result.x, &result.y, &result.z);
        GATE_ASSERT(count == 3);
        vertices.push_back(result);
      } else if (strncmp(line.c_str(), "vt ", 3) == 0) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "vt %f %f", &result.x, &result.y);
        GATE_ASSERT(count == 2);
        textures.push_back(result);
      } else if (strncmp(line.c_str(), "vn ", 3) == 0) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "vn %f %f %f", &result.x, &result.y, &result.z);
        GATE_ASSERT(count == 3);
        normals.push_back(result);
      } else if (strncmp(line.c_str(), "f ", 2) == 0) {
        if (first) {
          result.resize(vertices.size());
          first = false;
        }

        u32 faces[9];
        auto count = std::sscanf(
          line.c_str(),
          "f %u/%u/%u %u/%u/%u %u/%u/%u",
          &faces[0], &faces[1], &faces[2],
          &faces[3], &faces[4], &faces[5],
          &faces[6], &faces[7], &faces[8]
        );

        GATE_ASSERT(count == 9);

        for (u32 i = 0; i < 9; i += 3) {
          u32 vertexNumber = faces[i + 0] - 1;

          auto vertex = Vertex {
            vertices[vertexNumber],
            textures[faces[i + 1] - 1],
            normals[faces[i + 2] - 1],
          };

          result[vertexNumber] = vertex;
          indices.push_back(vertexNumber);
        }
      }
    }
    
    return {std::move(result), std::move(indices)};
  }

  Mesh::Handle Mesh::load(const std::string& file) {
    if (auto it = cachedMeshes.find(file); it != cachedMeshes.end()) {
      return it->second;
    }

    auto source = Utils::fileToString(file);
    if (!source) {
      Logger::error("Couldn't load file '%s'", file.c_str());
      return {};
    }

    auto[vertices, indices] = parseObjFile(source);
    free(source);

    auto vao = VertexArray::create();
    auto vbo = VertexBuffer::builder()
      .data({vertices.data(), vertices.size()})
      .layout(BufferElement::Type::Float3, "position")
      .layout(BufferElement::Type::Float2, "texcoords")
      .layout(BufferElement::Type::Float3, "normals")
      .build();
    vao->addVertexBuffer(vbo);
    auto ibo = IndexBuffer::create({indices.data(), indices.size()});
    vao->setIndexBuffer(ibo);
    vao->unbind();

    auto result = std::make_shared<Mesh>(Data{vao, vbo, ibo, file, Type::File});
    cachedMeshes[file] = result;
    Logger::trace("Mesh %p created: %s", result, file.c_str());
    return result;
  }

  Mesh::~Mesh() {
    Logger::trace("Mesh %p destroyed: %s", this, this->getFilePath()->c_str());
  }

  Mesh::Data Mesh::fromVertices(const Slice<const void> vertices, const Slice<const u32> indices) {
    auto vao = VertexArray::create();
    auto vbo = VertexBuffer::builder()
      .data(vertices)
      .layout(BufferElement::Type::Float3, "position")
      .layout(BufferElement::Type::Float2, "texcoords")
      .layout(BufferElement::Type::Float3, "normals")
      .build();
    vao->addVertexBuffer(vbo);
    auto ibo = IndexBuffer::create(indices);
    vao->setIndexBuffer(ibo);
    vao->unbind();

    return Data{vao, vbo, ibo, "", Type::Cube};
  }

  bool Mesh::reload() {
    if (!mData.filePath.has_value()) {
      return true;
    }

    auto source = Utils::fileToString(*mData.filePath);
    if (!source) {
      return false;
    }

    auto[vertices, indices] = parseObjFile(source);
    free(source);
    auto data = Mesh::fromVertices({vertices.data(), vertices.size()}, {indices.data(), indices.size()});
    data.filePath = std::move(mData.filePath);
    data.type = mData.type;
    mData = std::move(data);
    Logger::trace("Reloaded mesh: %s", mData.filePath.value().c_str());
    return true;
  }

  const VertexArray::Handle Mesh::getVertexArray() const {
    return mData.vertexArray;
  }

  Mesh::Handle Mesh::cube() {
    if (cachedCube) {
      return cachedCube;
    }

    static const f32 vertices[] = {
      // positions          // normals     // texture coords
      -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
       0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
       0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
      -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
      -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

      -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f, 1.0f,
       0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f, 1.0f,

      -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f, 
      -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f, 
      -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f, 
      -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f, 
      -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f, 
      -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f, 

      0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
      0.5f,  0.5f, -0.5f,    1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
      0.5f, -0.5f, -0.5f,    0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
      0.5f, -0.5f,  0.5f,    0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,    1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
       0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
       0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
       0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
      -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

      -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
       0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
    };

    static const u32 indices[] = {
       0,
       1,
       2,
       3,
       4,
       5,

       6,
       7,
       8,
       9,
      10,
      11,

      12,
      13,
      14,
      15,
      16,
      17,

      18,
      19,
      20,
      21,
      22,
      23,

      24,
      25,
      26,
      27,
      28,
      29,

      30,
      31,
      32,
      33,
      34,
      35,
    };

    Data data = Mesh::fromVertices(vertices, indices);
    data.type = Type::Cube;
    cachedCube = std::make_shared<Mesh>(std::move(data));
    return cachedCube;
  }

} // namespace Gate
