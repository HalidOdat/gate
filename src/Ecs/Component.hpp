#pragma once

#include "Core/Type.hpp"

namespace Game {

  namespace internals {
    
    extern u32 currentId;

  } // namespace internals

  struct BaseComponent {
    using Id = u32;
    
    u32 entity;
    // using componentDestroyFn = void(void*);
  };

  template<typename T>
  struct Component : BaseComponent {

    // static void componentDestroy(void* ptr) noexcept {
    //   reinterpret_cast<T*>(ptr)->~T();
    // }

    static u32 getId() {
      static Id id = internals::currentId++;
      return id;
    }
  };

} // namespace Game
