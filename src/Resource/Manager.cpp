#include "Core/Base.hpp"
#include "Resource/Manager.hpp"

#include <string>

namespace Game {

  constexpr const auto         ASSETS_DIRECTORY = "assets/";
  constexpr const auto  SHADER_ASSETS_DIRECTORY = "assets/shaders/";
  constexpr const auto TEXTURE_ASSETS_DIRECTORY = "assets/textures/";
  constexpr const auto MESH_ASSETS_DIRECTORY    = "assets/objects/";

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::shutdown() {
    // sInstance.getFactory<Mesh>().clear();
    // sInstance.getFactory<Texture2D>().clear();
    // sInstance.getFactory<Shader>().clear();
    Logger::info("ResourceManager: Shutdown!");
  }

} // namespace Game
