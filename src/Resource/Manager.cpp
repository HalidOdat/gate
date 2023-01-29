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
    // sInstance.getFactory<Texture2D>().clear();
    // sInstance.getFactory<Shader>().clear();
    Texture2D::destroyAllTextures();
    Mesh::destroyAllMeshes();
    Logger::info("ResourceManager: Shutdown!");
  }

} // namespace Game
