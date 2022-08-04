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

  using Refcount = u32;
  constexpr const auto CELL_OCCUPIED = 0xFF'FF'FF'FF;

  struct TextureResource {
    u32             refcount;
    std::string     path;
    Texture2D::Data texture;
  };

  struct ShaderResource {
    u32          refcount;
    std::string  path;
    Shader::Data shader;
  };

  struct ResourceManagerData {
    std::vector<TextureResource> textures;
    std::vector<u32>             texturesFreeList;

    std::vector<ShaderResource>  shaders;
    std::vector<u32>             shadersFreeList;
  };

  static ResourceManagerData manager;

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::shutdown() {
    sInstance.getFactory<Mesh>().clear();
    sInstance.getFactory<Texture2D>().clear();
    sInstance.getFactory<Shader>().clear();
    Logger::info("ResourceManager: Shutdown!");
  }

  Texture2D::Data ResourceManager::generateMissingTexture() {
    static const u8 defaultTextureData[] = {
        0x00, 0x00, 0x00, 0xFF,   0xFF, 0x00, 0xFF, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF,   0x00, 0x00, 0x00, 0xFF,
      };

    return Texture2D::fromBytes(defaultTextureData, 2, 2, 4, false);
  }

  Resource<Texture2D> ResourceManager::loadTexture(const StringView& filepath, bool linear, bool verticalFlip) {
    auto path = std::string(TEXTURE_ASSETS_DIRECTORY) + filepath.data();
    auto data = Texture2D::create(path, linear, verticalFlip);

    if (!data.id) {
      Logger::error("ResourceManager: Couldn't loaded texture: %s", path.c_str());
      data = ResourceManager::generateMissingTexture();
      return ResourceManager::getFactory<Texture2D>().emplace(data);
    }

    Logger::info("ResourceManager: Loaded texture: %s", path.c_str());

    return ResourceManager::getFactory<Texture2D>().emplace(data);
  }

  Resource<Texture2D> ResourceManager::textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, bool linear) {
    auto data  = Texture2D::fromBytes(bytes, width, height, channels, linear);
    Logger::info("ResourceManager: Loaded texture from memory");
    return ResourceManager::getFactory<Texture2D>().emplace(data);
  }

  Resource<Shader> ResourceManager::loadShader(const StringView& filepath) {
    auto path   = std::string(SHADER_ASSETS_DIRECTORY) + filepath.data();
    auto result = Shader::create(path);

    if (!result.id) {
      Logger::info("ResourceManager: Couldn't load shader: %s", path.c_str());
      GAME_ASSERT(false);
      return Resource<Shader>();
    }

    return ResourceManager::getFactory<Shader>().emplace(result);
  }

  Resource<Mesh> ResourceManager::loadMesh(const StringView& filepath) {
    auto path   = std::string(MESH_ASSETS_DIRECTORY) + filepath.data();
    auto result = Mesh::fromFileSource(Mesh::FileFormat::Obj, path);

    if (!result.vertexArray) {
      Logger::info("ResourceManager: Couldn't load mesh: %s", path.c_str());
      GAME_ASSERT(false);
      // TODO: Return invalid shader
      return Resource<Mesh>();
    }

    return ResourceManager::getFactory<Mesh>().emplace(result);
  }

  Resource<Mesh> ResourceManager::cubeMesh() {
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

    auto result = Mesh::fromVertices(vertices, indices);
    return ResourceManager::getFactory<Mesh>().emplace(result);
  }

  // TODO: Check for change in files
  void ResourceManager::reloadTextures() {
    Logger::info("ResourceManager: Realoading All Textures");
    for (auto& texture : manager.textures) {
      // TODO: implement this
      // if (!texture.path.empty()) {
      //   Texture2D::destroy(texture.texture);
      //   texture.texture = Texture2D::create(texture.path);
      //   if (texture.texture.id == 0) {
      //     // TODO: Use a cached texture, instead of creating a new one
      //     texture.texture = ResourceManager::generateMissingTexture();
      //   }
      // }
    }
  }

} // namespace Game
