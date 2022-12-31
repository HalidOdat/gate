#include "Ecs/Ecs.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace Game::Ecs {

  Registry::Registry() {
    GAME_PROFILE_FUNCTION();
    this->pools.resize(MAX_COMPONENTS);
  }

  Entity Registry::create() {
    GAME_PROFILE_FUNCTION();
    auto entity = Entity((u32)this->entities.size());
    EntityDescriptor descriptor {
      Requirement()
    };
    this->entities.push_back(descriptor);
    return entity;
  }

} // namespace Game::Ecs
