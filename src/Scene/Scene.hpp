#pragma once

#include "Core/Type.hpp"
#include "Core/Timestep.hpp"
#include "Ecs/Ecs.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Components.hpp"
#include "Serializer/Serializer.hpp"

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

  private:
    friend class Entity;
    friend struct Serializer::Convert<Scene>;

    friend class EditorLayer;
  };

  template<typename T, typename ...Args>
  T& Entity::add(Args&&... args) {
    return mScene->mRegistry.assign<T>(
      Ecs::Entity(mEntityId),
      std::forward<Args>(args)...
    );
  }

  template<typename T, typename ...Ts>
  bool Entity::has() {
    return mScene->mRegistry.hasComponent<T, Ts...>(Ecs::Entity(mEntityId));
  }

  template<typename T, typename ...Ts>
  T& Entity::get() {
    return mScene->mRegistry.get<T, Ts...>(Ecs::Entity(mEntityId));
  }

} // namespace Game
