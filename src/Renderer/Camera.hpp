#pragma once

#include "Core/Math.hpp"

namespace Game {

  // TODO: Camera should only contain projection and view matrix

  class Camera {
  public:
    inline Mat4 getProjectionViewMatrix() const { return mProjectionViewMatrix; }
    inline const Mat4& getProjectionMatrix() const  { return mProjectionMatrix; }
    inline const Mat4& getViewMatrix() const { return mViewMatrix; }

  protected:
    inline Camera(Mat4 projectionMatrix, Mat4 viewMatrix)
    : mProjectionMatrix{projectionMatrix},
      mViewMatrix{viewMatrix},
      mProjectionViewMatrix{mProjectionMatrix * mViewMatrix}
    {}

  protected:
    Mat4 mProjectionMatrix;
    Mat4 mViewMatrix;
    Mat4 mProjectionViewMatrix;
  };

  class OrthographicCamera : public Camera {
  public:
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 zNear = -1.0f, f32 zFar = 1.0f);

    void setProjection(f32 left, f32 right, f32 bottom, f32 top, f32 zNear = -1.0f, f32 zFar = 1.0f);

    inline void offsetPosition(const Vec3& offset) { this->setPosition(mPosition + offset); }
    inline void offsetRotation(const f32   offset) { this->setRotation(mRotation + offset); }

    void setPosition(const Vec3& position);
    void setRotation(const f32 rotation);

    inline f32 getRotation() const { return mRotation; }

  private:
    void recalculateProjectionViewMatrix();

  private:
    Vec3 mPosition = {0.0f, 0.0f, 0.0f};
    f32  mRotation = 0.0f; // rotation on the z-axis
  };

  class PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera(const Vec3& position, const Vec3& target, const Vec3& up, f32 fov, f32 aspect, f32 zNear = 0.1f, f32 zFar = 100.0f);

    void setProjection(f32 fov, f32 aspect, f32 zNear = 0.1f, f32 zFar = 100.0f);
    void lookAt(const Vec3& position, const Vec3& target, const Vec3& up = Vec3{0.0f, 0.0f, 1.0f});
  };

} // namespace Game
