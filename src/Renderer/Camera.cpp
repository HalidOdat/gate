#include "glm/gtc/matrix_transform.hpp"

#include "Renderer/Camera.hpp"

namespace Game {

  PerspectiveCamera::PerspectiveCamera(Vec3 position, f32 fov, f32 aspect, f32 zNear, f32 zFar)
    : position{position},
    forward{0.0f, 0.0f, 1.0f},
    up{0.0f, 1.0f, 0.0f},
    projection{glm::perspective(fov, aspect, zNear, zFar)}
  {
    this->projectionViewMatrix = this->projection * glm::lookAt(this->position, this->position + this->forward, this->up);
  }

  OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
    : projection{glm::ortho(left, right, bottom, top, zNear, zFar)}, view{1.0f}
  {
    this->projectionViewMatrix = this->projection * this->view;
  }

  void OrthographicCamera::setProjection(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar) {
    this->projection = glm::ortho(left, right, bottom, top, zNear, zFar);
    this->projectionViewMatrix = this->projection * this->view;
  }

  void OrthographicCamera::setPosition(const Vec3& position) {
    this->position = position;
    recalculateProjectionViewMatrix();
  }

  void OrthographicCamera::setRotation(const f32 rotation) {
    this->rotation = rotation;
    recalculateProjectionViewMatrix();
  }

  void OrthographicCamera::recalculateProjectionViewMatrix() {
    auto transform = glm::translate(
      Mat4(1.0f),
      this->position
    ) * glm::rotate(
      Mat4(1.0f),
      glm::radians(this->rotation),
      Vec3(0, 0, 1)
    );

    this->view = glm::inverse(transform);
    this->projectionViewMatrix = this->projection * this->view;
  }

} // namespace Game