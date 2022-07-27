#include "Renderer/CameraController.hpp"

namespace Game {

  OrthographicCameraController::OrthographicCameraController(f32 aspectRatio, f32 zNear, f32 zFar)
    : mAspectRatio{aspectRatio},
    mCamera(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel, zNear, zFar)
  {}

  void OrthographicCameraController::enableRotation(bool yes) {
    mCameraRotationEnabled = yes;
  }

  void OrthographicCameraController::onUpdate(Timestep ts) {
  }

  void OrthographicCameraController::onEvent(const Event& event) {
    event.dispatch(&OrthographicCameraController::onWindowResizeEvent, this);
    event.dispatch(&OrthographicCameraController::onMouseScrollEvent, this);
  }

  bool OrthographicCameraController::onWindowResizeEvent(const WindowResizeEvent& event) {
    return false;
  }

  bool OrthographicCameraController::onMouseScrollEvent(const MouseScrollEvent& event) {
    return false;
  }


} // namespace Game