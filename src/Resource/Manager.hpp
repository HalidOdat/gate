#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"

namespace Gate {

  class ResourceManager {
  private:
    ResourceManager() = delete;

    static void initialize();
    static void shutdown();

  private:
    friend class Application;
  };

} // namespace Gate
