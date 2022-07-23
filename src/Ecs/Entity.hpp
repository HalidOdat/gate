#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class Registry;

  class Entity final {
    friend Registry;

  public:
    using Id = u32;
  
  public:
    Entity(Entity::Id id)
      : id{id}
    {}

  private:
    Id id;
  };

} // namespace Game
