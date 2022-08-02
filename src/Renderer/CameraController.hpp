#pragma once

#include "Core/Timestep.hpp"

#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Events/KeyEvent.hpp"

#include "Renderer/Camera.hpp"

namespace Game {
  
  class OrthographicCameraController {
  public:
    OrthographicCameraController(f32 aspectRatio, f32 zNear = -1.0f, f32 zFar = 100.0f);

    const OrthographicCamera& getCamera() const { return mCamera; }
    OrthographicCamera& getCamera() { return mCamera; }

    inline f32 getAspectRatio() const { return mAspectRatio; }
    inline f32 getZoomLevel() const { return mZoomLevel; }
    inline f32 getZoomLevelMin() const { return mZoomLevelMin; }
    inline f32 getZoomLevelMax() const { return mZoomLevelMax; }
    inline f32 getRotation() const { return mCameraRotation; }
    inline const Vec3& getPosition() const { return mCameraPosition; }

    void setRotation(const f32 rotation);
    void offsetRotation(const f32  offset);
    void setZoomLevel(const f32 zoomLevel);
    void offsetZoomLevel(const f32 offset);
    void setPosition(const Vec3 position);
    void offsetPosition(const Vec3 offset);

    void resize(u32 width, u32 height);
    void onUpdate(Timestep ts);

  private:
    f32 mAspectRatio;
    f32 mZoomLevelMin = 0.25f;
    f32 mZoomLevelMax = 10.0f;
    f32 mZoomLevel    = 1.0f;

    OrthographicCamera mCamera;
    Vec3               mCameraPosition = {0.0f, 0.0f, 0.0f};
    // f32                mCameraTranspationSpeed = 1.0f;
    // bool               mCameraRotationEnabled = true;
    f32                mCameraRotation = 0.0f;
    // f32                mCameraRotationSpeed = 100.0f;
  };

  class PerspectiveCameraController {
  public:
    PerspectiveCameraController(Vec3 position, f32 fov, f32 aspect, f32 zNear = 0.1f, f32 zFar = 100.0f);

    const PerspectiveCamera& getCamera() const { return mCamera; }
    PerspectiveCamera& getCamera() { return mCamera; }

    inline f32 getAspectRatio() const { return mAspectRatio; }
    inline f32 getFov() const { return mFov; }
    inline const Vec3& getPosition() const { return mPosition; }
    inline const Vec3& getFront() const { return mFront; }

    void setPosition(const Vec3 position);
    void offsetPosition(const Vec3 offset);

    void resize(u32 width, u32 height);
    void onUpdate(Timestep ts);

    void onEvent(const Event& event);

  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);

    void updateCameraVectors();

  private:
    Vec3 mPosition = {0.0f, 0.0f, 0.0f};
    f32  mAspectRatio;
    f32  mFov;
    f32  mZNear;
    f32  mZFar;

    Vec3 mFront;
    Vec3 mUp;
    Vec3 mRight;
    Vec3 mWorldUp;

    f32 mYaw = -90.0f;
    f32 mPitch = 0.0f;

    f32 mZoomLevel = 45.0f;
    f32 mMovmentSpeed = 6.5f;
    f32 mMouseSensitivity = 15.0f;
    bool mFristMove = true;
    Vec2 mLastMousePosition;

    PerspectiveCamera mCamera;
  };

} // namespace Game
