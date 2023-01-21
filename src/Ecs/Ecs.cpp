#include "Ecs/Ecs.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace Game::Ecs {

  Registry::Registry() {
    this->pools.resize(MAX_COMPONENTS);
  }

  Entity Registry::create() {
    auto entity = Entity((u32)this->entities.size());
    EntityDescriptor descriptor {
      Requirement()
    };
    this->entities.push_back(descriptor);
    return entity;
  }

} // namespace Game::Ecs
