
#pragma once

#include "Scene/Scene.hpp"

namespace Game {

  class SceneSerializer {
  public:
    static String serialize(Scene& scene);
  };

} // namespace Game
