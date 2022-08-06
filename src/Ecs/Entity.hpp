#pragma once

#include "Core/Type.hpp"

namespace Game::Ecs {
  
  class Registry;

  class Entity final {
    friend Registry;

  public:
    using Id = u32;
  
  public:
    Entity(Entity::Id id)
      : id{id}
    {}

    inline Id getId() const { return id; }

  private:
    Id id;
  };

} // namespace Game::Ecs
