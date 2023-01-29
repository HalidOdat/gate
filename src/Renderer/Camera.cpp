#include "glm/gtc/matrix_transform.hpp"

#include "Renderer/Camera.hpp"

namespace Game {

  OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
    : Camera{glm::ortho(left, right, bottom, top, zNear, zFar), Mat4(1.0f)}
  {}

  void OrthographicCamera::setProjection(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) {
    this->mProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
    this->mProjectionViewMatrix = this->mProjectionMatrix * this->mViewMatrix;
  }

  void OrthographicCamera::setPosition(const Vec3& position) {
    this->mPosition = position;
    recalculateProjectionViewMatrix();
  }

  void OrthographicCamera::setRotation(const f32 rotation) {
    this->mRotation = rotation;
    recalculateProjectionViewMatrix();
  }

  void OrthographicCamera::recalculateProjectionViewMatrix() {
    auto transform = glm::translate(
      Mat4(1.0f),
      mPosition
    ) * glm::rotate(
      Mat4(1.0f),
      glm::radians(mRotation),
      Vec3(0, 0, 1)
    );

    this->mViewMatrix = glm::inverse(transform);
    this->mProjectionViewMatrix = this->mProjectionMatrix * this->mViewMatrix;
  }

} // namespace Game