#include "Ecs/Ecs.hpp"

#include <cstdlib>
#include <algorithm>

namespace Game {

  // // ComponentPool implementation
  ComponentPool::~ComponentPool() {
    free(this->storage);
  }
  
  u32 ComponentPool::allocateComponent() {
    if (this->capacity != 0) {
      this->capacity--;
      return this->size++;
    }

    // TODO: better allocation
    this->storage = (u8*)std::realloc(this->storage, (this->size + 1) * this->componentSize);
    return this->size++;
  }

  void* ComponentPool::create(const u32 i) {
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
    u32 index = this->sparse[i];

    if (index == this->size - 1) {
      this->size--;
      this->capacity++;
      return;
    }

    std::memcpy(
      this->storage + index * this->componentSize,
      this->storage + (this->size - 1) * this->componentSize,
      this->componentSize
    );

    index = this->getComponentAtIndex(index)->entity;
    this->sparse[index] = i;
    
    this->size--;
    this->capacity++;
  }

  void* ComponentPool::get(const u32 i) {
    return this->storage + this->sparse[i] * this->componentSize;
  }

  // Registry implementation

  Registry::Registry()
  {
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

  bool Registry::hasComponent(const Entity entity, const Requirement requirements) const {
    return (this->entities[entity.id].mask & requirements) == requirements;
  }

} // namespace Game
