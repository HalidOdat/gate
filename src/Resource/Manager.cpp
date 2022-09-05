#include "Core/Base.hpp"
#include "Resource/Manager.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/Material.hpp"

#include <string>

namespace Game {

  void ResourceManager::initialize() {
    Logger::info("ResourceManager: Initialized!");
    Material::initialize();
  }

  void ResourceManager::shutdown() {
    // sInstance.getFactory<Mesh>().clear();
    // sInstance.getFactory<Texture2D>().clear();
    // sInstance.getFactory<Shader>().clear();
    Material::destroy();
    Texture2D::destroyAllTextures();
    Mesh::destroyAllMeshes();
    Logger::info("ResourceManager: Shutdown!");
  }

} // namespace Game
