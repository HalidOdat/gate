#pragma once

#include "Core/Base.hpp"
#include "Core/Math.hpp"

#include "Physics/Intersection.hpp"

namespace Game::Physics {

  class BoundingSphere;
  
  class Plane {
  public:
    Plane() = default;
    Plane(const Vec3& normal, f32 distance)
      : mNormal{normal}, mDistance{distance}
    {}

    inline const Vec3& getNormal() const { return mNormal; }
    inline f32 getDistance() const { return mDistance; }

    Intersection intersectBoundingSphere(const BoundingSphere& other) const;

  private:
    Vec3 mNormal;
    f32  mDistance;
  };
}