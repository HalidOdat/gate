#include "Scene/Components.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Game {
  
  Mat4 TransformComponent::getTranformMatrix() const {
      Mat4 transform{1.0f};
      transform = glm::translate(transform, translation);
      // TODO: rotation
      // transform = glm::rotate(transform, rotation);
      transform = glm::scale(transform, scale);
      return transform;
  }

} // namespace Game
