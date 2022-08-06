#include "Scene/Scene.hpp"

#include "Core/Assert.hpp"
#include "Renderer/Renderer.hpp"

namespace Game {
  
  Scene::Scene(String name)
      : mName{name}
  {}

  void Scene::render() {
    auto view = mRegistry.view<TransformComponent, MeshSourceComponent, MeshRendererComponent>();
    for (auto[entity, tc, ms, mr] : view) {
      Renderer::submit(mr.shader, ms.mesh, mr.material, tc.getTranformMatrix());
    }
  }

  void Scene::onUpdate(Timestep ts) {
    render();
  }

  Entity Scene::createEntity(String tag) {
    Ecs::Entity entity = mRegistry.create();
    mRegistry.assign<TagComponent>(entity, std::move(tag));
    return Entity(entity, this);
  }

} // namespace Game
