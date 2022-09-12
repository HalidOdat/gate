#include "Physics/Engine.hpp"

#include "Scene/Components.hpp"

namespace Game::Physics {

  Engine::Engine() {}

  void Engine::tick(Timestep ts, Ecs::Registry& registry) {
    auto view = registry.view<VelocityComponent, TransformComponent>();
    for (auto[entity, velocity, transform] : view) {
      transform.offsetTranslation(velocity.velocity * (f32)ts);
    }
  }

}