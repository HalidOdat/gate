#pragma once

#include "Core/Type.hpp"

#include <algorithm>

namespace Game::Ecs {

  namespace internals {
    
    extern u32 currentId;

  } // namespace internals

  struct BaseComponent {
    using Id = u32;
    using DestroyFunction = auto(*)(void*) -> void;
    using MoveFunction = auto(*)(void*, void*, usize) -> void;
    
    u32 entity;
  };

  template<typename T>
  struct Component : BaseComponent {
    static u32 getId() {
      static Id id = internals::currentId++;
      return id;
    }
  };

} // namespace Game::Ecs
