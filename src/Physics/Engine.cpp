#include "Physics/Engine.hpp"

#include "Scene/Components.hpp"

namespace Game::Physics {

  Engine::Engine() {}

  void Engine::tick(Timestep ts, Ecs::Registry& registry) {
    auto view = registry.view<TransformComponent, VelocityComponent>();
    for (auto[entity, transform, velocity] : view) {
      transform.offsetTranslation(velocity.velocity * (f32)ts);
    }
  }

}