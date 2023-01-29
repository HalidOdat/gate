#include <algorithm>

#include "Core/Input.hpp"
#include "Renderer/CameraController.hpp"

#include "Application.hpp"

namespace Gate {

  OrthographicCameraController::OrthographicCameraController(f32 aspectRatio, f32 zNear, f32 zFar)
    : mAspectRatio{aspectRatio},
    // mCamera(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel, zNear, zFar)
    mCamera(0.0f, Application::getWindow().getWidth(), Application::getWindow().getHeight(), 0.0f, zNear, zFar)
  {}

  void OrthographicCameraController::onUpdate([[maybe_unused]] Timestep ts) {
    // TODO: Will be used in the future to make smooth camera movement
  }

  void OrthographicCameraController::resize(u32 width, u32 height) {
    mAspectRatio = (f32)width / (f32)height;
		// mCamera.setProjection(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel);
    mCamera.setProjection(0.0f, Application::getWindow().getWidth(), Application::getWindow().getHeight(), 0.0f);
  }

  void OrthographicCameraController::setZoomLevel(const f32 zoomLevel) {
		mZoomLevel = std::clamp(zoomLevel, mZoomLevelMin, mZoomLevelMax);
		mCamera.setProjection(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel);
  }

  void OrthographicCameraController::setRotation(const f32 rotation) {
    mCameraRotation = rotation;
    mCamera.setRotation(mCameraRotation);
  }

  void OrthographicCameraController::offsetPosition(const Vec3 offset) {
    mCameraPosition += offset;
    mCamera.setPosition(mCameraPosition);
  }

  void OrthographicCameraController::setPosition(const Vec3 position) {
    mCameraPosition = position;
    mCamera.setPosition(mCameraPosition);
  }

  void OrthographicCameraController::offsetZoomLevel(const f32 offset) {
    setZoomLevel(mZoomLevel + offset);
  }

  void OrthographicCameraController::offsetRotation(const f32 offset) {
    mCameraRotation += offset;
    mCamera.setRotation(mCameraRotation);
  }

} // namespace Gate