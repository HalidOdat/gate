#pragma once

#include "Layers/EditorLayer.hpp"
#include "Layers/GameLayer.hpp"

#include "Application.hpp"

namespace Game {
  
  class GameApplication : public Application {
  public:
    GameApplication() {
      this->pushLayer(new GameLayer());
      this->pushLayer(new EditorLayer());
    }
  };

} // namespace Game
