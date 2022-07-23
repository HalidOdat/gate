#pragma once

#include <vector>
#include <array>
#include <bitset>

#include "Ecs/Entity.hpp"
#include "Ecs/Component.hpp"

namespace Game {

  constexpr const auto MAX_COMPONENTS = 32;

  using Requirement = std::bitset<MAX_COMPONENTS>;
  
  class Registry;

  template<typename ...Ts>
  class Iterator {
  public:
    Iterator(Registry* registry, Requirement requirements, u32 index)
      : registry{registry}, requirements{requirements}, index{index}
    {}


    Iterator& operator++() {
      do {
        this->index++;
      } while (
        this->index < this->registry->entities.size()
        && (this->registry->entities[this->index].mask & this->requirements) != this->requirements
      );
      return *this;
    }

    Iterator operator++(int) {
      Iterator result = *this;
      ++(*this);
      return result;
    }

    const Entity operator*() const {
      return Entity(this->index);
    }

    bool operator==(const Iterator& other) const {
      return this->index == other.index || this->index == this->registry->entities.size();
    }

    bool operator!=(const Iterator& other) const {
      return this->index != other.index && this->index != this->registry->entities.size();
    }

  public:
    Registry* registry;
    Requirement requirements;
    u32 index;
  };

  template<typename ...Ts>
  class RegistryView {
  public:
    RegistryView(Registry* registry)
      : registry{registry}
    {
      (this->requirements.set(Component<Ts>::getId()),...);
    }

    Iterator<Ts...> begin() {
      u32 firstIndex = 0;
      while (firstIndex < this->registry->entities.size() &&
        (this->requirements != (this->requirements & this->registry->entities[firstIndex].mask))
      )
      {
        firstIndex++;
      }
      return Iterator<Ts...>(this->registry, this->requirements, firstIndex);
    }

    Iterator<Ts...> end() {
      return Iterator<Ts...>(this->registry, this->requirements, this->registry->entities.size());
    }

  private:
    Registry* registry;
    Requirement requirements;
  };

  class ComponentPool {
  public:
    ComponentPool() {}

    ComponentPool(u32 componentSize)
      : storage{nullptr}, size{0}, capacity{0}, componentSize{componentSize}, initialized{true}
    {}

    ~ComponentPool();
    
    u32 allocateComponent();

    /// Must not have the component
    void* create(const u32 i);

    // Must have the component
    void destory(const u32 i);

    // Must have the component
    void* get(const u32 i);


    bool isInitialzed() { return initialized; }

  private:
    BaseComponent* getComponentAtIndex(const u32 index) {
      return reinterpret_cast<BaseComponent*>(
        this->storage + index * this->componentSize
      );
    }

  private:
    u8* storage = nullptr;
    u32 size;
    u32 capacity;
    u32 componentSize;
    bool initialized = false;
    
    std::vector<u32> sparse;
  };

  class Registry {
    template<typename ...Ts>
    friend class Iterator;

    template<typename ...Ts>
    friend class RegistryView;
  public:
    static constexpr const auto MAX_COMPONENTS = 32;

  public:
    Registry();

    Entity create();

    // TODO: staticaly check if ...ts inherit from Component type
    template<typename ...Ts>
    bool hasComponent(const Entity entity) const {
      Requirement requirements;

      (requirements.set(Component<Ts>::getId()),...);
      this->hasComponent(entity, requirements);
    }

    template<typename T, typename ...Args>
    void assign(Entity entity, Args&& ...args) {
      u32 cid = Component<T>::getId();

      // TODO: check if already assigned
      this->entities[entity.id].mask.set(cid);

      auto pool = &this->pools[cid];
      if (!pool->isInitialzed()) {
        *pool = ComponentPool(sizeof(T));
      }
      new (pool->create(entity.id)) T{std::forward<Args>(args)...};
    }

    template<typename ...Ts>
    RegistryView<Ts...> view() {
      return RegistryView<Ts...>(this);
    }

  private:

    struct EntityDescriptor {
      Requirement mask;
    };
  
  private:
    bool hasComponent(const Entity entity, const Requirement requirements) const;

  private:
    std::vector<EntityDescriptor> entities;
    std::vector<ComponentPool> pools;
  };

} // namespace Game
