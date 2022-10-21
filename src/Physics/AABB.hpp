#pragma once

#include "Core/Base.hpp"
#include "Core/Math.hpp"

#include "Physics/Intersection.hpp"

namespace Game::Physics {

  class AABB {
  public:
    AABB() = default;
    AABB(const Vec3& min, const Vec3& max)
      : mMin{min}, mMax{max}
    {}

    inline const Vec3& getMin() const { return mMin; }
    inline const Vec3& getMax() const { return mMax; }

    Intersection intersectAABB(const AABB& other) const;

  private:
    Vec3 mMin;
    Vec3 mMax;
  };

}