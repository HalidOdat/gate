#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"

namespace Game {

  Entity Entity::duplicate() {
    GAME_ASSERT_WITH_MESSAGE(mScene, "Scene shouldn't be a nullptr");

    if (!has<TagComponent>()) {
      GAME_UNREACHABLE("Entity should always have a tag component");
    }
    Entity newEntity = mScene->createEntity(get<TagComponent>().tag + "(clone)");
    if (has<TransformComponent>()) {
      auto component = get<TransformComponent>();
      newEntity.add<TransformComponent>(component);
    }
    if (has<MeshSourceComponent>()) {
      auto component = get<MeshSourceComponent>();
      newEntity.add<MeshSourceComponent>(component);
    }
    if (has<MeshRendererComponent>()) {
      auto component = get<MeshRendererComponent>();
      newEntity.add<MeshRendererComponent>(component);
    }
    if (has<VelocityComponent>()) {
      auto component = get<VelocityComponent>();
      newEntity.add<VelocityComponent>(component);
    }
    return newEntity;
  }

}