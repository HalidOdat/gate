#pragma once

#include "Core/Math.hpp"

namespace Game {
  
  class Camera {
  public:
    inline const Mat4& getProjectionViewMatrix() const { return this->projectionViewMatrix; }

  protected:
    Camera() = default;

  protected:
    Mat4 projectionViewMatrix;
  };

  class PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera(Vec3 position, f32 fov, f32 aspect, f32 zNear, f32 zFar);

  private:
    Vec3 position;
    Vec3 forward;
    Vec3 up;

    Mat4 projection;
  };

  class OrthographicCamera : public Camera {
  public:
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 zNear = -1.0f, f32 zFar = 1.0f);

    void setProjection(f32 left, f32 right, f32 bottom, f32 top, f32 zNear = -1.0f, f32 zFar = 1.0f);

    inline void offsetPosition(const Vec3& offset) { this->setPosition(this->position + offset); }
    inline void offsetRotation(const f32   offset) { this->setRotation(this->rotation + offset); }

    void setPosition(const Vec3& position);
    void setRotation(const f32 rotation);

    inline f32 getRotation() const { return this->rotation; }

  private:
    void recalculateProjectionViewMatrix();

  private:
    Mat4 projection;
    Mat4 view;

    Vec3 position = {0.0f, 0.0f, 0.0f};
    f32  rotation = 0.0f; // rotation on the z-axis
  };

} // namespace Game
