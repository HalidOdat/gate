
#pragma once

#include "Scene/Scene.hpp"

namespace Game {

  class SceneSerializer {
  public:
    static String serialize(Scene& scene);
    static bool deserializeFromFile(const StringView& path, Scene& scene);
  };

} // namespace Game
