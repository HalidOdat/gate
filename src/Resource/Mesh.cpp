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

        // Logger::info("v -> x: %f, y: %f, z: %f", result.x, result.y, result.z);
      } else if (line.starts_with("vt ")) {
        std::stringstream s(line.substr(3));
        Vec2 result;
        s >> result.x;
        s >> result.y;
        // result.y = 1 - result.y;

        textures.push_back(result);

        // Logger::info("vt %f %f", result.x, result.y);
      } else if (line.starts_with("vn ")) {
        std::stringstream s(line.substr(3));
        Vec3 result;
        s >> result.x;
        s >> result.y;
        s >> result.z;
        normals.push_back(result);

        // Logger::info("vn %f %f %f", result.x, result.y, result.z);
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


    Logger::error("Vertices: %zu", vertices.size());
    Logger::error("Indices: %zu", indices.size());
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

  Mesh::Data Mesh::fromFileSource(FileFormat format, const std::string& source) {
    auto[vertices, indices] = parseObjFile(*fileToString(source));
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

    return {vao, vbo, ibo};
  }

  void Mesh::destroy(Data& data) {
    // do nothing...
  }

  const Mesh::Data& Mesh::getData() const {
    return ResourceManager::getMeshData(id);
  }

  const Ref<VertexArray> Mesh::getVertexArray() const {
    auto data = getData();
    return data.vertexArray;
  }

} // namespace Game