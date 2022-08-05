#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Manager.hpp"

#include <string>

namespace Game {

  constexpr const auto         ASSETS_DIRECTORY = "assets/";
  constexpr const auto  SHADER_ASSETS_DIRECTORY = "assets/shaders/";
  constexpr const auto TEXTURE_ASSETS_DIRECTORY = "assets/textures/";
  constexpr const auto MESH_ASSETS_DIRECTORY    = "assets/objects/";

  ResourceManager ResourceManager::sInstance;

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::shutdown() {
    sInstance.getFactory<Mesh>().clear();
    sInstance.getFactory<Texture2D>().clear();
    sInstance.getFactory<Shader>().clear();
    Logger::info("ResourceManager: Shutdown!");
  }

  Texture2D::Handle ResourceManager::loadTexture(const StringView& filepath, Texture2D::Specification specification) {
    auto path = std::string(TEXTURE_ASSETS_DIRECTORY) + filepath.data();
    Option<Texture2D::Data> data = Texture2D::fromFile(path, specification);

    if (!data) {
      Logger::error("ResourceManager: Couldn't loaded texture: %s", path.c_str());
      data = Texture2D::generateMissingDataPlaceholder();
      return ResourceManager::getFactory<Texture2D>().emplace(*data);
    }

    Logger::info("ResourceManager: Loaded texture: %s", path.c_str());

    return ResourceManager::getFactory<Texture2D>().emplace(*data);
  }

  Texture2D::Handle ResourceManager::textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, Texture2D::Specification specification) {
    auto data  = Texture2D::fromBytes(bytes, width, height, channels, specification);
    Logger::info("ResourceManager: Loaded texture from memory: 0x%x", bytes);
    return ResourceManager::getFactory<Texture2D>().emplace(data);
  }

  Shader::Handle ResourceManager::loadShader(const StringView& filepath) {
    auto path   = std::string(SHADER_ASSETS_DIRECTORY) + filepath.data();
    Option<Shader::Data> result = Shader::fromFile(path);

    if (!result) {
      Logger::error("ResourceManager: Couldn't load shader: %s", path.c_str());
      GAME_ASSERT(false);
      return Shader::Handle();
    }

    Logger::info("ResourceManager: Loaded shader: %s", path.c_str());

    return ResourceManager::getFactory<Shader>().emplace(*result);
  }

  Mesh::Handle ResourceManager::loadMesh(const StringView& filepath) {
    auto path   = std::string(MESH_ASSETS_DIRECTORY) + filepath.data();
    Option<Mesh::Data> result = Mesh::fromFile(path);

    if (!result) {
      Logger::error("ResourceManager: Couldn't load mesh: %s", path.c_str());
      GAME_ASSERT(false);
      // TODO: Return invalid shader
      return Mesh::Handle();
    }
    Logger::info("ResourceManager: Loaded mesh: %s", path.c_str());

    return ResourceManager::getFactory<Mesh>().emplace(*result);
  }

  Mesh::Handle ResourceManager::cubeMesh() {
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

    Logger::info("ResourceManager: Loaded cube mesh");

    auto result = Mesh::fromVertices(vertices, indices);
    return ResourceManager::getFactory<Mesh>().emplace(result);
  }

  CubeMap::Handle ResourceManager::loadCubeMap(const CubeMap::FilePaths& filepaths) {
    auto data = CubeMap::fromFile(filepaths);
    Logger::info("ResourceManager: Loaded CubeMap");
    return ResourceManager::getFactory<CubeMap>().emplace(*data);
  }

} // namespace Game
