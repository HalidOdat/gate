#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"

namespace Game {
  
  class Application;

  class ResourceManager {
    friend class Application;

  public:
    static Ref<Texture2D> loadTexture(const StringView& filepath);
    static Ref<Shader>    loadShader(const StringView& vFilepath, const StringView& fFilepath);

    ResourceManager() = delete;
  private:
    static void Initialize();
    static void Shutdown();
  };

} // namespace Game
