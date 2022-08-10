#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Factory.hpp"

#include <sstream>
#include <fstream>
#include <optional>

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(Mesh, factory)

  template<>
  struct FactoryCallback<Mesh> {
    inline static void created(Mesh& mesh, u32 id) {
      if (mesh.getFilePath()) {
        Logger::trace("Mesh #%u created: %s", id, mesh.getFilePath()->c_str());
      }
    }
    inline static void destroyed(Mesh& mesh, u32 id) {
      if (mesh.getFilePath()) {
        Logger::trace("Mesh #%u destroyed: %s", id, mesh.getFilePath()->c_str());
      }
    }
  };

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
      if (line.starts_with("v ")) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "v %f %f %f", &result.x, &result.y, &result.z);
        GAME_ASSERT(count == 3);
        vertices.push_back(result);
      } else if (line.starts_with("vt ")) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "vt %f %f", &result.x, &result.y);
        GAME_ASSERT(count == 2);
        textures.push_back(result);
      } else if (line.starts_with("vn ")) {
        Vec3 result;
        auto count = std::sscanf(line.c_str(), "vn %f %f %f", &result.x, &result.y, &result.z);
        GAME_ASSERT(count == 3);
        normals.push_back(result);
      } else if (line.starts_with("f ")) {
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

        GAME_ASSERT(count == 9);

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

  std::optional<std::string> fileToString(const StringView& filename) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      file.open(filename.data());
      std::stringstream stream;
      stream << file.rdbuf();
      file.close();
      return stream.str();
    } catch (std::ifstream::failure e) {
      Logger::error("Couldn't load file '%s': %s", filename.data(), e.what());
      return std::nullopt;
    }
  }

  Mesh::Handle Mesh::load(const std::string& filepath) {
    auto file = "assets/objects/" + String(filepath);
    FileFormat format = FileFormat::Obj;
    auto source = fileToString(file);
    if (!source) {
      return {};
    }

    auto[vertices, indices] = parseObjFile(*source);
    auto vao = VertexArray::create();
    auto vbo = VertexBuffer::create({vertices.data(), vertices.size()});
    vbo->setLayout({
      BufferElement::Type::Float3, // position
      BufferElement::Type::Float2, // texcoords
      BufferElement::Type::Float3, // normals
    });
    vao->addVertexBuffer(vbo);
    auto ibo = IndexBuffer::create({indices.data(), indices.size()});
    vao->setIndexBuffer(ibo);
    vao->unbind();

    return factory.emplace(Data{vao, vbo, ibo, file});
  }

  Mesh::Data Mesh::fromVertices(const Slice<const void> vertices, const Slice<const u32> indices) {
    auto vao = VertexArray::create();
    auto vbo = VertexBuffer::create(vertices);
    vbo->setLayout({
      BufferElement::Type::Float3, // position
      BufferElement::Type::Float2, // texcoords
      BufferElement::Type::Float3, // normals
    });
    vao->addVertexBuffer(vbo);
    auto ibo = IndexBuffer::create(indices);
    vao->setIndexBuffer(ibo);
    vao->unbind();

    return Data{vao, vbo, ibo};
  }

  bool Mesh::reload() {
    if (!mData.filePath.has_value()) {
      return true;
    }

    auto source = fileToString(*mData.filePath);
    if (!source) {
      return false;
    }

    auto[vertices, indices] = parseObjFile(*source);
    auto data = Mesh::fromVertices({vertices.data(), vertices.size()}, {indices.data(), indices.size()});
    data.filePath = std::move(mData.filePath);
    mData = std::move(data);
    Logger::trace("Reloaded mesh: %s", mData.filePath.value().c_str());
    return true;
  }

  const VertexArray::Handle Mesh::getVertexArray() const {
    return mData.vertexArray;
  }

  Mesh::Handle Mesh::cube() {
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

    return factory.emplace(Mesh::fromVertices(vertices, indices));
  }

} // namespace Game