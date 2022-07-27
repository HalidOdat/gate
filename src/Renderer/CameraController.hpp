#pragma once

#include "Core/Timestep.hpp"

#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"

#include "Renderer/Camera.hpp"

namespace Game {
  
  class OrthographicCameraController {
  public:
    OrthographicCameraController(f32 aspectRatio, f32 zNear = -1.0f, f32 zFar = 1.0f);

    void enableRotation(bool yes = true);

    void onUpdate(Timestep ts);
    void onEvent(const Event& event);

    const OrthographicCamera& getCamera() const { return mCamera; }
    const f32 getAspectRatio() const { return mAspectRatio; }
    const f32 getZoomLevel() const { return mZoomLevel; }
    void      setZoomLevel(const f32 zoomLevel) { mZoomLevel = zoomLevel; }
    const f32 getRotationSpeed() const { return mZoomLevel; }
    void      setRotationSpeed(const f32 rotationSpeed) { mCameraRotationSpeed = rotationSpeed; }

  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);

  private:
    f32 mAspectRatio;
    f32 mZoomLevel;

    OrthographicCamera mCamera;
    Vec3               mCameraPosition = {0.0f, 0.0f, 0.0f};
    f32                mCameraTranspationSpeed = 1.0f;
    bool               mCameraRotationEnabled = true;
    f32                mCameraRotation = 0.0f;
    f32                mCameraRotationSpeed = 100.0f;
  };

  // TODO: Implement PerspectiveCameraController

} // namespace Game
