#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Manager.hpp"

#include <sstream>
#include <fstream>
#include <optional>

namespace Game {

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

  Option<Mesh::Data> Mesh::fromFile(const std::string& file) {
    FileFormat format = FileFormat::Obj;
    auto source = fileToString(file);
    if (!source) {
      return None;
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

    return Data{vao, vbo, ibo, file};
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

    return {vao, vbo, ibo};
  }

  bool Mesh::reload() {
    if (!mData.filePath.has_value()) {
      return false;
    }

    auto data = Mesh::fromFile(*mData.filePath);
    if (data) {
      Logger::trace("Reloaded mesh: %s", mData.filePath.value().c_str());
      mData = *data;
      return true;
    }

    return false;
  }

  const Ref<VertexArray> Mesh::getVertexArray() const {
    return mData.vertexArray;
  }

} // namespace Game