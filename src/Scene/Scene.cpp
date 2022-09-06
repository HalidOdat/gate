#include "Scene/Scene.hpp"

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

namespace Game {
  
  Scene::Scene(String name)
      : mName{name}
  {}

  void Scene::render(const PerspectiveCameraController& camera) {
    GAME_PROFILE_FUNCTION();

    Renderer::begin3D(camera);
    auto view = mRegistry.view<TransformComponent, MeshSourceComponent, MeshRendererComponent>();
    for (auto[entity, tc, ms, mr] : view) {
      Renderer::submit(ms.mesh, mr.material, tc.getTransform());
    }
  }

  void Scene::onSimulateUpdate(Timestep ts, const PerspectiveCameraController& camera) {
    GAME_PROFILE_FUNCTION();

    mPhysicsEngine.tick(ts, mRegistry);
    render(camera);
  }

  void Scene::onUpdate(Timestep ts) {
    (void)ts;

    GAME_PROFILE_FUNCTION();
    
    // render();
    GAME_TODO("");
  }

  Entity Scene::createEntity(String tag) {
    GAME_PROFILE_FUNCTION();

    Ecs::Entity entity = mRegistry.create();
    mRegistry.assign<TagComponent>(entity, std::move(tag));
    return Entity(entity, this);
  }

} // namespace Game
