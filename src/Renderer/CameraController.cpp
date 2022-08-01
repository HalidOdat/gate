#include <algorithm>

#include "Core/Input.hpp"
#include "Renderer/CameraController.hpp"

namespace Game {

  OrthographicCameraController::OrthographicCameraController(f32 aspectRatio, f32 zNear, f32 zFar)
    : mAspectRatio{aspectRatio},
    mCamera(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel, zNear, zFar)
  {}

  void OrthographicCameraController::onUpdate(Timestep ts) {
    // TODO: Will be used in the future to make smooth camera movement
  }

  void OrthographicCameraController::resize(u32 width, u32 height) {
    mAspectRatio = (f32)width / (f32)height;
		mCamera.setProjection(-mAspectRatio * mZoomLevel, mAspectRatio * mZoomLevel, -mZoomLevel, mZoomLevel);
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

  PerspectiveCameraController::PerspectiveCameraController(Vec3 position, f32 fov, f32 aspectRatio, f32 zNear, f32 zFar)
    : mPosition{position}, mFov{fov}, mAspectRatio{aspectRatio}, mZNear{zNear}, mZFar{zFar},
      mFront{0.0f, 0.0f, -1.0f}, mUp{0.0f, 1.0f, 0.0f}, mWorldUp{mUp},
      mCamera{mPosition, mFront, mUp, mFov, mAspectRatio, mZNear, mZFar}
  {
    updateCameraVectors();
  }

  void PerspectiveCameraController::setPosition(const Vec3 position) {
    mPosition = position;
    mCamera.lookAt(mPosition, mPosition + mFront, mUp);
  }

  void PerspectiveCameraController::offsetPosition(const Vec3 offset) {
    mPosition += offset;
    mCamera.lookAt(mPosition, mPosition + mFront, mUp);
  }

  void PerspectiveCameraController::resize(u32 width, u32 height) {
    mAspectRatio = (f32)width / (f32)height;
    mCamera.setProjection(mFov, mAspectRatio, mZNear, mZFar);
  }

  void PerspectiveCameraController::updateCameraVectors() {
    Vec3 front = {
      glm::cos(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch)),
      glm::sin(glm::radians(mPitch)),
      glm::sin(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch)),
    };

    mFront = glm::normalize(front);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp    = glm::normalize(glm::cross(mRight, mFront));

    mCamera.lookAt(mPosition, mPosition + mFront, mUp);
  }

  void PerspectiveCameraController::onUpdate(Timestep ts) {
    f32 velocity = mMovmentSpeed * ts;
    if (Input::isKeyPressed(Key::W)) {
        mPosition += mFront * velocity;
    } else if (Input::isKeyPressed(Key::S)) {
        mPosition -= mFront * velocity;
    }

    if (Input::isKeyPressed(Key::A)) {
        mPosition -= mRight * velocity;
    } else if (Input::isKeyPressed(Key::D)) {
        mPosition += mRight * velocity;
    }

    if (Input::isKeyPressed(Key::Space)) {
        mPosition.y += velocity;
    } else if (Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift)) {
        mPosition.y -= velocity;
    }

    setPosition(mPosition);
  }

  void PerspectiveCameraController::onEvent(const Event& event) {
    event.dispatch(&PerspectiveCameraController::onWindowResizeEvent, this);
    event.dispatch(&PerspectiveCameraController::onKeyPressedEvent, this);
    event.dispatch(&PerspectiveCameraController::onMouseMoveEvent, this);
  }
  bool PerspectiveCameraController::onWindowResizeEvent(const WindowResizeEvent& event) {
    resize(event.getWidth(), event.getHeight());
    return false;
  }
  bool PerspectiveCameraController::onKeyPressedEvent(const KeyPressedEvent& event) {
    
    return false;
  }

  bool PerspectiveCameraController::onMouseMoveEvent(const MouseMoveEvent& event) {
    auto[x, y] = event.getPosition();
    if (mFristMove) {
      mLastMousePosition = {x, y};
      mFristMove = false;
    }

    const auto ts = Timestep::get();

    f32 xOffset = (x - mLastMousePosition.x) * mMouseSensitivity * ts;
    f32 yOffset = (mLastMousePosition.y - y) * mMouseSensitivity * ts;

    mLastMousePosition = {x, y};

    mYaw   += xOffset;
    mPitch += yOffset;

    const auto constrainPitch = true;
    if (constrainPitch) {
      mPitch = std::clamp(mPitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
    return true;
  }

} // namespace Game