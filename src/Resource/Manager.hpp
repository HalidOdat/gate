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
  static Texture2D textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, bool linear = true);

    static Shader loadShader(const StringView& filepath);

    static void reloadTextures();

    ResourceManager() = delete;
  public:
    static const Texture2D::Data& getTextureData(Resource::Id id);
    static const Shader::Data&    getShaderData(Resource::Id id);

  private:
    static void Initialize();
    static void Shutdown();

    static void incrementRefCount(Resource::Id id);
    static void decrementRefCount(Resource::Id id);

    static Texture2D::Data generateMissingTexture();
  };

} // namespace Game
