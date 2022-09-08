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
	  	newEntity.add<TransformComponent>(get<TransformComponent>());
	  }
	  if (has<MeshSourceComponent>()) {
	  	newEntity.add<MeshSourceComponent>(get<MeshSourceComponent>());
	  }
	  if (has<MeshRendererComponent>()) {
	  	newEntity.add<MeshRendererComponent>(get<MeshRendererComponent>());
	  }
	  if (has<VelocityComponent>()) {
	  	newEntity.add<VelocityComponent>(get<VelocityComponent>());
	  }
	  return newEntity;
  }

}