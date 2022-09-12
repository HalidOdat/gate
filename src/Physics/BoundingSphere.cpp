#include "Physics/BoundingSphere.hpp"

namespace Game::Physics {
  Intersection BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) const {
    f32 radiusDistance = mRadius + other.mRadius;
    Vec3 direction = mCenter - other.mCenter;
    f32 centerDistance = glm::length(direction);
    direction /= centerDistance;

    f32 distance = centerDistance < radiusDistance;
    return Intersection{distance < 0, direction * distance};
  }
}