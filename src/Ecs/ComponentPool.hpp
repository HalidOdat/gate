#pragma once

#include <vector>
#include <array>
#include <bitset>

#include "Core/Base.hpp"

#include "Ecs/Entity.hpp"
#include "Ecs/Component.hpp"

namespace Game::Ecs {

  class IComponentPool {
  public:
    virtual ~IComponentPool();
    virtual void destory(const u32 i) = 0;
    virtual void* get(const u32 i) = 0;
  };

  template<typename T>
  class ComponentPool : public IComponentPool {
  public:
    ComponentPool() = default;

    template<typename ...Args>
    T* construct(const u32 i, Args&& ...args) {
      GAME_PROFILE_FUNCTION();
      u32 index = (u32)components.size();
      components.emplace_back(std::forward<Args>(args)...);

      if (this->sparse.size() <= i) {
        // TODO: put invalid entity value
        this->sparse.resize(i + 1);
      }

      this->sparse[i] = index;

      T* c = &components[index];
      reinterpret_cast<BaseComponent*>(c)->entity = i;
      return c;
    }

    // Must have the component
    void destory(const u32 i) override {
      GAME_PROFILE_FUNCTION();
      u32 index = this->sparse[i];

      if (index + 1 == components.size()) {
        components.pop_back();
        return;
      }

      components.erase(components.begin() + index);

      index = this->getComponentAtIndex(index)->entity;
      this->sparse[index] = i;
    }

    // Must have the component
    void* get(const u32 i) override {
      GAME_PROFILE_FUNCTION();
      return &components[this->sparse[i]];
    }

  private:
    BaseComponent* getComponentAtIndex(const u32 index) {
      return reinterpret_cast<BaseComponent*>(&components[index]);
    }

  private:
    std::vector<u32> sparse;
    std::vector<T> components;
  };

} // namespace Game::Ecs
