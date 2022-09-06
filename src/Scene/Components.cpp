#include "Scene/Components.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Game {
  
  TransformComponent::TransformComponent(
    const Vec3& translation,
    const Vec3& rotation,
    const Vec3& scale
  ) : mTranslation{translation}, mRotation{rotation}, mScale{scale}
  {
    recalculateTransform();
  }

  void TransformComponent::recalculateTransform() {
    mTransform = Mat4(1.0f);
    mTransform = glm::translate(mTransform, mTranslation);
    mTransform = glm::rotate(mTransform, mRotation.x, Vec3(1.0f, 0.0f, 0.0f));
    mTransform = glm::rotate(mTransform, mRotation.y, Vec3(0.0f, 1.0f, 0.0f));
    mTransform = glm::rotate(mTransform, mRotation.z, Vec3(0.0f, 0.0f, 1.0f));
    mTransform = glm::scale(mTransform, mScale);
  }

  void TransformComponent::setTranslation(const Vec3& value) {
    mTranslation = value;
    recalculateTransform();
  }
  void TransformComponent::setRotation(const Vec3& value) {
    mRotation = value;
    recalculateTransform();
  }
  void TransformComponent::setScale(const Vec3& value) {
    mScale = value;
    recalculateTransform();
  }

} // namespace Game
