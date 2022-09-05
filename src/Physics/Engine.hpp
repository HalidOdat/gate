#pragma once

#include "Core/Base.hpp"
#include "Core/Timestep.hpp"
#include "Ecs/Ecs.hpp"

namespace Game::Physics {

  class Engine {
  public:
    Engine();
    void tick(Timestep ts, Ecs::Registry& registry);

  private:
  };

}