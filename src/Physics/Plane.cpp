#pragma once

#include "Physics/Plane.hpp"
#include "Physics/BoundingSphere.hpp"

namespace Game::Physics {

  Intersection Plane::intersectBoundingSphere(const BoundingSphere& other) const {
    f32 distanceFromSphereCenter = glm::abs(glm::dot(mNormal, other.getCenter()) + mDistance);
    f32 distanceFromSphere = distanceFromSphereCenter - other.getRadius();
    return Intersection{distanceFromSphere < 0, mNormal * distanceFromSphere};
  }
  
}