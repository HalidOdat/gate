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
        std::stringstream s(line.substr(2));
        Vec3 result;
        s >> result.x;
        s >> result.y;
        s >> result.z;
        vertices.push_back(result);
      } else if (line.starts_with("vt ")) {
        std::stringstream s(line.substr(3));
        Vec2 result;
        s >> result.x;
        s >> result.y;
        textures.push_back(result);
      } else if (line.starts_with("vn ")) {
        std::stringstream s(line.substr(3));
        Vec3 result;
        s >> result.x;
        s >> result.y;
        s >> result.z;
        normals.push_back(result);
      } else if (line.starts_with("f ")) {
        if (first) {
          result.resize(vertices.size());
          first = false;
        }
        std::stringstream s(line.substr(2));

        for (std::string line; std::getline(s, line, ' '); ) {
          std::stringstream temp(line);
          u32 partCount = 0;
          u32 parts[3];
          for (std::string part; std::getline(temp, part, '/');) {
            parts[partCount++] = std::stoul(part);
          }

          GAME_DEBUG_ASSERT(partCount == 3);

          u32 vertexNumber = parts[0] - 1;

          auto vertex = Vertex {
            vertices[vertexNumber],
            textures[parts[1] - 1],
            normals[parts[2] - 1],
          };

          result[vertexNumber] = vertex;
          indices.push_back(vertexNumber);
        }
      }
    }
    
    return {result, indices};
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

    return factory.emplace(vao, vbo, ibo, file);
  }

  Mesh::Handle Mesh::fromVertices(const Slice<const void> vertices, const Slice<const u32> indices) {
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

    return factory.emplace(vao, vbo, ibo);
  }

  bool Mesh::reload() {
    if (!mFilePath.has_value()) {
      return false;
    }

    GAME_TODO("");
    auto data = Mesh::load(*mFilePath);
    if (data) {
      Logger::trace("Reloaded mesh: %s", mFilePath.value().c_str());
      // mData = *data;
      return true;
    }

    return false;
  }

  const VertexArray::Handle Mesh::getVertexArray() const {
    return mVertexArray;
  }

  Mesh::Handle Mesh::cube() {
    static const f32 vertices[] = {
      // positions          // normals           // texture coords
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

    return Mesh::fromVertices(vertices, indices);
  }

} // namespace Game