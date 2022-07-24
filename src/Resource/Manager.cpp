#include "Core/Log.hpp"
#include "Resource/Manager.hpp"

#include <string>
#include <vector>

namespace Game {

  constexpr const auto         ASSETS_DIRECTORY = "./assets/";
  constexpr const auto  SHADER_ASSETS_DIRECTORY = "./assets/shaders/";
  constexpr const auto TEXTURE_ASSETS_DIRECTORY = "./assets/textures/";

  struct TextureResource {
    std::string path;
    Ref<Texture2D> texture;
  };

  struct ShaderResource {
    std::string vertexPath;
    std::string fragmentPath;
    Ref<Shader> shader;
  };

  struct ResourceManagerData {
    std::vector<TextureResource> textures;
    std::vector<ShaderResource> shaders;
  };

  static ResourceManagerData manager;

  void ResourceManager::Initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::Shutdown() {
    manager.textures.clear();
    manager.shaders.clear();
    Logger::info("ResourceManager: Shutdown!");
  }

  Ref<Texture2D> ResourceManager::loadTexture(const StringView& filepath) {
    auto path   = std::string(TEXTURE_ASSETS_DIRECTORY) + filepath.data();
    auto result = Texture2D::create(path);


    if (result) {
      manager.textures.emplace_back(std::move(path), result);
      Logger::info("ResourceManager: Loaded texture: %s", path.c_str());
    } else {
      Logger::error("ResourceManager: Couldn't loaded texture: %s", path.c_str());
    }

    return result;
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

} // namespace Game
