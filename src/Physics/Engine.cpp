#include "Physics/Engine.hpp"

#include "Scene/Components.hpp"

namespace Game::Physics {

  Engine::Engine() {}

  void Engine::tick(Timestep ts, Ecs::Registry& registry) {
    // simulation
    {
      auto view = registry.view<VelocityComponent, TransformComponent>();
      for (auto[entity, velocity, transform] : view) {
        Vec3 offset = velocity.velocity * (f32)ts;
        transform.offsetTranslation(offset);

        if (registry.hasComponent<CollisionComponent>(entity)) {
          auto& collisionComponent = registry.get<CollisionComponent>(entity);
          collisionComponent.collider.offsetTranslation(offset);
        }
      }
    }

    // Collision detection
    auto view1 = registry.view<CollisionComponent, TransformComponent>();
    for (auto[entity1, collision1, transform1] : view1) {
      auto view2 = registry.view<CollisionComponent, TransformComponent>();
      for (auto[entity2, collision2, transform2] : view2) {
        if (entity1.getId() != entity2.getId()) {
          auto instersection = collision1.collider.intersect(collision2.collider);
          if (instersection.isIntersecting) {
            if (registry.hasComponent<VelocityComponent>(entity1)) {
              registry.get<VelocityComponent>(entity1).velocity *= -1;
            }
          }
        }
      }
    }

    // Collision response
  }

}