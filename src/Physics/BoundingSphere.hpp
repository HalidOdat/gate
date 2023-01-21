#pragma once

#include "Core/Base.hpp"
#include "Core/Math.hpp"

#include "Physics/Intersection.hpp"

namespace Game::Physics {

  class BoundingSphere {
  public:
    BoundingSphere() = default;
    BoundingSphere(const Vec3& center, f32 radius)
      : mCenter{center}, mRadius{radius}
    {}

    Intersection intersectBoundingSphere(const BoundingSphere& other) const;

    void offsetTranslation(const Vec3& offset) { mCenter += offset; }

    inline const Vec3& getCenter() const { return mCenter; } 
    inline f32 getRadius() const { return mRadius; } 
  private:
    Vec3 mCenter;
    f32  mRadius;
  };

}