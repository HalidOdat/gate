#include "Physics/AABB.hpp"

#undef min
#undef max
#include <glm/gtx/component_wise.hpp>

namespace Game::Physics {

  Intersection AABB::intersectAABB(const AABB& other) const {
    Vec3 distance1 = other.mMin - mMax;
    Vec3 distance2 = mMin - other.mMax;
    Vec3 distances = glm::max(distance1, distance2);
    
    f32 maxDistance = glm::compMax(distances);
    return Intersection{maxDistance < 0, distances};
  }

}