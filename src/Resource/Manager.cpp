#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Manager.hpp"

#include <string>
#include <vector>

namespace Game {

  constexpr const auto         ASSETS_DIRECTORY = "./assets/";
  constexpr const auto  SHADER_ASSETS_DIRECTORY = "./assets/shaders/";
  constexpr const auto TEXTURE_ASSETS_DIRECTORY = "./assets/textures/";

  using Refcount = u32;
  constexpr const auto CELL_OCCUPIED = 0xFF'FF'FF'FF;

  struct TextureResource {
    u32             refcount;   
    std::string     path;
    Texture2D::Data texture;
  };

  struct ShaderResource {
    std::string vertexPath;
    std::string fragmentPath;
    Ref<Shader> shader;
  };

  struct ResourceManagerData {
    std::vector<TextureResource> textures;
    std::vector<u32>             texturesFreeList;

    std::vector<ShaderResource>  shaders;
  };

  static ResourceManagerData manager;

  void ResourceManager::incrementRefCount(Resource::Id id) {
    switch (id.type) {
      case Resource::Type::None:
        break;
      case Resource::Type::Texture:
        manager.textures[id.index].refcount += 1;
        break;
      default:
        GAME_UNREACHABLE("Unknown resource type!");
    }
  }

  void ResourceManager::decrementRefCount(Resource::Id id) {
    switch (id.type) {
      case Resource::Type::None:
        break;
      case Resource::Type::Texture:
        manager.textures[id.index].refcount -= 1;
        if (!manager.textures[id.index].refcount) {
          Logger::trace("ResourceManager: Texture(%u) destroyed!", manager.textures[id.index].texture.id);
          Texture2D::destroy(manager.textures[id.index].texture);
          manager.texturesFreeList.push_back(id.index);
          manager.textures[id.index].refcount = CELL_OCCUPIED;
        }
        break;
      default:
        GAME_UNREACHABLE("Unknown resource type!");
    }
  }

  void ResourceManager::Initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::Shutdown() {
    for (usize i = 0; i < manager.textures.size(); ++i) {
      if (manager.textures[i].refcount != CELL_OCCUPIED) {
        Logger::trace("ResourceManager: Texture(%u) destroyed!", manager.textures[i].texture.id);
        Texture2D::destroy(manager.textures[i].texture);
      }
    }
    manager.textures.clear();
    manager.texturesFreeList.clear();

    manager.shaders.clear();
    Logger::info("ResourceManager: Shutdown!");
  }

  const Texture2D::Data& ResourceManager::getTextureData(Resource::Id id) {
    return manager.textures[id.index].texture;
  }

  Texture2D::Data ResourceManager::generateMissingTexture() {
    static const u8 defaultTextureData[] = {
        0x00, 0x00, 0x00, 0xFF,   0xFF, 0x00, 0xFF, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF,   0x00, 0x00, 0x00, 0xFF,
      };

    return Texture2D::fromBytes(defaultTextureData, 2, 2, 4, false);
  }

  Texture2D ResourceManager::loadTexture(const StringView& filepath) {
    auto path   = std::string(TEXTURE_ASSETS_DIRECTORY) + filepath.data();
    auto data = Texture2D::create(path);

    auto index = (u32)manager.textures.size();

    if (data.id) {
      Logger::info("ResourceManager: Loaded texture: %s", path.c_str());
    } else {
      Logger::error("ResourceManager: Couldn't loaded texture: %s", path.c_str());
      data = ResourceManager::generateMissingTexture();
    }
    manager.textures.emplace_back(1, std::move(path), data);

    return Texture2D({Resource::Type::Texture, index});
  }

  Texture2D ResourceManager::textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, bool linear) {
    auto data  = Texture2D::fromBytes(bytes, width, height, channels, linear);
    auto index = (u32)manager.textures.size();
    manager.textures.emplace_back(1, "", data);
    Logger::info("ResourceManager: Loaded texture from memory");

    return Texture2D({Resource::Type::Texture, index});
  }

  Ref<Shader> ResourceManager::loadShader(const StringView& vFilepath, const StringView& fFilepath) {
    auto vertexPath   = std::string(SHADER_ASSETS_DIRECTORY) + vFilepath.data();
    auto fragmentPath = std::string(SHADER_ASSETS_DIRECTORY) + fFilepath.data();

    auto result = Shader::create(vertexPath.c_str(), fragmentPath.c_str());

    if (result) {
      Logger::info("ResourceManager: Loaded shader vertex:   %s", vertexPath.c_str());
      Logger::info("ResourceManager: Loaded shader fragment: %s", fragmentPath.c_str());
      manager.shaders.emplace_back(std::move(vertexPath), std::move(fragmentPath), result);
    } else {
      Logger::info("ResourceManager: Couldn't load shader vertex:   %s", vertexPath.c_str());
      Logger::info("ResourceManager: Couldn't load shader fragment: %s", fragmentPath.c_str());
    }

    return result;
  }

  // TODO: Check for change in files
  void ResourceManager::reloadTextures() {
    Logger::info("ResourceManager: Realoading All Textures");
    for (auto& texture : manager.textures) {
      // Ignore non-file based texture
      if (!texture.path.empty()) {
        Texture2D::destroy(texture.texture);
        texture.texture = Texture2D::create(texture.path);
        if (texture.texture.id == 0) {
          // TODO: Use a cached texture, instead of creating a new one
          texture.texture = ResourceManager::generateMissingTexture();
        }
      }
    }
  }

} // namespace Game
