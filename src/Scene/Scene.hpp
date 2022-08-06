#pragma once

#include "Core/Type.hpp"
#include "Core/Timestep.hpp"
#include "Ecs/Ecs.hpp"
#include "Scene/Components.hpp"

namespace Game {
  
  class Entity;

  class Scene {
  public:
    Scene(String name);

    inline const String& getName() const { return mName; }

    Entity createEntity(String tag = "Entity");

    void render();
    void onUpdate(Timestep ts);
  private:
    String mName;
    Ecs::Registry mRegistry;

  private:
    friend class Entity;
  };

  class Entity {
  public:
    template<typename T, typename ...Args>
    T& add(Args&&... args) {
      return mScene->mRegistry.assign<T>(mEntity, std::forward<Args>(args)...);
    }

  private:
    Entity(Ecs::Entity entity, Scene* scene)
      : mEntity{entity}, mScene{scene}
    {}

  private:
    Ecs::Entity mEntity;
    Scene* mScene;

  private:
    friend class Scene;
  };

} // namespace Game
