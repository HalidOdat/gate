#pragma once

#include "Core/Type.hpp"

namespace Game::Ecs {

  namespace internals {
    
    extern u32 currentId;

  } // namespace internals

  struct BaseComponent {
    using Id = u32;
    using DestroyFunction = auto(*)(void*) -> void;
    
    u32 entity;
  };

  template<typename T>
  struct Component : BaseComponent {
    static void destroyFunction(void* ptr) {
      reinterpret_cast<T*>(ptr)->~T();
    }

    static u32 getId() {
      static Id id = internals::currentId++;
      return id;
    }
  };

} // namespace Game::Ecs
