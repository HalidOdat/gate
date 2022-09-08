#pragma once

#include "Ecs/Entity.hpp"

namespace Game {

  namespace Serializer {
    template<typename T>
    struct Convert;    
  }

  class Scene;

  class Entity {
  public:
    Entity()
      : mEntityId{0}, mScene{nullptr}
    {}

    template<typename T, typename ...Args>
    T& add(Args&&... args);

    template<typename T, typename ...Ts>
    bool has();

    template<typename T, typename ...Ts>
    T& get();

    Entity duplicate();

    inline bool isValid() const { return mScene; }
    inline operator bool() const { return isValid(); }
  private:
    Entity(Ecs::Entity entity, Scene* scene)
      : mEntityId{entity.getId()}, mScene{scene}
    {}

  private:
    Ecs::Entity::Id mEntityId;
    Scene* mScene;

  private:
    friend class Scene;
    friend class EditorLayer;

    template<typename T>
    friend struct Serializer::Convert;
  };

} // namespace Game