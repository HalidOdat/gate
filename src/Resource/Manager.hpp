#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"

namespace Game {
  
  class Application;

  class ResourceManager {
    friend class Application;
    friend class Resource;

  public:
    static Texture2D loadTexture(const StringView& filepath);
    static Texture2D textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4);

    static Ref<Shader> loadShader(const StringView& vFilepath, const StringView& fFilepath);

    static void reloadTextures();

    ResourceManager() = delete;
  public:
    static const Texture2D::Data& getTextureData(Resource::Id id);

  private:
    static void incrementRefCount(Resource::Id id);
    static void decrementRefCount(Resource::Id id);

    static void Initialize();
    static void Shutdown();
  };

} // namespace Game
