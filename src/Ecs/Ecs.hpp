#pragma once

#include <vector>
#include <array>
#include <bitset>

#include "Core/Assert.hpp"

#include "Ecs/Entity.hpp"
#include "Ecs/Component.hpp"

namespace Game {

  constexpr const auto MAX_COMPONENTS = 32;

  using Requirement = std::bitset<MAX_COMPONENTS>;
  
  class Registry;

  template<typename ...Ts>
  class RegistryView {
    friend class Registry;
  public:
    class Iterator {
      friend class RegistryView;
    public:
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
        auto result = *this;
        ++(*this);
        return result;
      }

      std::tuple<Entity, Ts&...> operator*() const {
        auto entity = Entity(this->index);
        return std::make_tuple(entity, std::ref(this->registry->get<Ts>(entity))...);
      }

      bool operator==(const Iterator& other) const {
        return this->index == other.index || this->index == this->registry->entities.size();
      }

      bool operator!=(const Iterator& other) const {
        return this->index != other.index && this->index != this->registry->entities.size();
      }
    
    private:
      Iterator(Registry* registry, Requirement requirements, u32 index)
        : registry{registry}, requirements{requirements}, index{index}
      {}

    public:
      Registry* registry;
      Requirement requirements;
      u32 index;
    };

  public:
    Iterator begin() {
      u32 firstIndex = 0;
      while (firstIndex < this->registry->entities.size() &&
        (this->requirements != (this->requirements & this->registry->entities[firstIndex].mask))
      )
      {
        firstIndex++;
      }
      return Iterator(this->registry, this->requirements, firstIndex);
    }

    Iterator end() {
      return Iterator(this->registry, this->requirements, (u32)this->registry->entities.size());
    }

  private:
    RegistryView(Registry* registry)
      : registry{registry}
    {
      (this->requirements.set(Component<Ts>::getId()),...);
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
    Registry();

    Entity create();

    // TODO: staticaly check if ...ts inherit from Component type
    template<typename ...Ts>
    bool hasComponent(const Entity entity) const {
      Requirement requirements;
      (requirements.set(Component<Ts>::getId()),...);
      return (this->entities[entity.id].mask & requirements) == requirements;
    }

    template<typename T, typename ...Args>
    T& assign(Entity entity, Args&& ...args) {
      u32 cid = Component<T>::getId();

      // TODO: check if already assigned
      this->entities[entity.id].mask.set(cid);

      auto pool = &this->pools[cid];
      if (!pool->isInitialzed()) {
        *pool = ComponentPool(sizeof(T));
      }
      return *new (pool->create(entity.id)) T{std::forward<Args>(args)...};
    }

    template<typename T>
    T& get(Entity entity) {
      const auto cid = Component<T>::getId();
      if (!this->entities[entity.id].mask.test(cid)) {
        GAME_ASSERT(false);
      }

      return *reinterpret_cast<T*>(this->pools[cid].get(entity.id));
    }

    template<typename T0, typename T1, typename ...Ts>
    std::tuple<T0&, T1&, Ts&...> get(Entity entity) {
      return std::tie(this->get<T0>(entity), this->get<T1>(entity), this->get<Ts>(entity)...);
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
    std::vector<EntityDescriptor> entities;
    std::vector<ComponentPool> pools;
  };

} // namespace Game
