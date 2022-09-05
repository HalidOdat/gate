#pragma once

#include "Core/Type.hpp"
#include "Core/Timestep.hpp"
#include "Ecs/Ecs.hpp"
#include "Scene/Components.hpp"
#include "Serializer/Serializer.hpp"
#include "Physics/Engine.hpp"

namespace Game {
  
  class Entity;

  class Scene {
  public:
    Scene(String name);

    inline const String& getName() const { return mName; }

    Entity createEntity(String tag = "Entity");

    void render(const PerspectiveCameraController& camera);
    void onSimulateUpdate(Timestep ts, const PerspectiveCameraController& camera);
    void onUpdate(Timestep ts);

  private:
    String mName;
    Ecs::Registry mRegistry;
    Physics::Engine mPhysicsEngine;

  private:
    friend class Entity;
    friend struct Serializer::Convert<Scene>;

    friend class EditorLayer;
  };

  class Entity {
  public:
    Entity()
      : mEntityId{0}, mScene{nullptr}
    {}

    template<typename T, typename ...Args>
    T& add(Args&&... args) {
      return mScene->mRegistry.assign<T>(
        Ecs::Entity(mEntityId),
        std::forward<Args>(args)...
      );
    }

    template<typename T, typename ...Ts>
    bool has() {
      return mScene->mRegistry.hasComponent<T, Ts...>(Ecs::Entity(mEntityId));
    }

    template<typename T, typename ...Ts>
    auto& get() {
      return mScene->mRegistry.get<T, Ts...>(Ecs::Entity(mEntityId));
    }

    inline bool isValid() const { return mScene; }

  private:
    Entity(Ecs::Entity entity, Scene* scene)
      : mEntityId{entity.getId()}, mScene{scene}
    {}

  private:
    Ecs::Entity::Id mEntityId;
    Scene* mScene;

  private:
    friend class EditorLayer;
    friend class  Scene;
    friend struct Serializer::Convert<Scene>;
  };

} // namespace Game
