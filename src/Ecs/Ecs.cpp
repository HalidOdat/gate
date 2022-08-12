#include "Ecs/Ecs.hpp"

#include <cstdlib>
#include <algorithm>

namespace Game::Ecs {

  // ComponentPool implementation
  ComponentPool::~ComponentPool() {
    GAME_PROFILE_FUNCTION();
    if (this->storage) {
      if (this->destoryFunction != nullptr) {
        for (usize i = 0; i < this->size; ++i) {
          this->destoryFunction(this->storage + i * this->componentSize);
        }
      }
      free(this->storage);
    }
  }
  
  u32 ComponentPool::allocateComponent() {
    GAME_PROFILE_FUNCTION();
    if (this->capacity != 0) {
      this->capacity--;
      return this->size++;
    }

    // TODO: better allocation
    this->storage = (u8*)std::realloc(this->storage, (this->size + 1) * this->componentSize);
    return this->size++;
  }

  void* ComponentPool::create(const u32 i) {
    GAME_PROFILE_FUNCTION();
    u32 index = this->allocateComponent();

    if (this->sparse.size() <= i) {
      // TODO: put invalid entity value
      this->sparse.resize(i + 1);
    }

    this->sparse[i] = index;

    BaseComponent* c = this->getComponentAtIndex(index);
    c->entity = i;
    return c;
  }

  void ComponentPool::destory(const u32 i) {
    GAME_PROFILE_FUNCTION();
    u32 index = this->sparse[i];

    if (index == this->size - 1) {
      this->size--;
      this->capacity++;
      return;
    }

    u8* componentPtr = this->storage + index * this->componentSize;
    if (this->destoryFunction != nullptr) {
      this->destoryFunction(componentPtr);
    }
    std::memcpy(
      componentPtr,
      this->storage + (this->size - 1) * this->componentSize,
      this->componentSize
    );

    index = this->getComponentAtIndex(index)->entity;
    this->sparse[index] = i;
    
    this->size--;
    this->capacity++;
  }

  void* ComponentPool::get(const u32 i) {
    GAME_PROFILE_FUNCTION();
    return this->storage + this->sparse[i] * this->componentSize;
  }

  // Registry implementation

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
