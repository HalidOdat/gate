#include "Core/Base.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Texture.hpp"

#include <string>

namespace Gate {

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
  }

  void ResourceManager::shutdown() {
    Texture::destroyAllTextures();
    Logger::info("ResourceManager: Shutdown!");
  }

} // namespace Gate
