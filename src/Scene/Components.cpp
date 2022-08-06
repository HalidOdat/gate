#include "Scene/Components.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Game {
  
  Mat4 TransformComponent::getTranformMatrix() const {
      Mat4 transform{1.0f};
      transform = glm::translate(transform, translation);
      transform = glm::rotate(transform, rotation.x, Vec3(1.0f, 0.0f, 0.0f));
      transform = glm::rotate(transform, rotation.y, Vec3(0.0f, 1.0f, 0.0f));
      transform = glm::rotate(transform, rotation.z, Vec3(0.0f, 0.0f, 1.0f));
      transform = glm::scale(transform, scale);
      return transform;
  }

} // namespace Game
