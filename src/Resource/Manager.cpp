#include "Core/Base.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Texture.hpp"

#include <string>

namespace Game {

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::shutdown() {
    // sInstance.getFactory<Mesh>().clear();
    // sInstance.getFactory<Texture>().clear();
    // sInstance.getFactory<Shader>().clear();
    Texture::destroyAllTextures();
    Logger::info("ResourceManager: Shutdown!");
  }

} // namespace Game
