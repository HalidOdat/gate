#include "Physics/Collider.hpp"

namespace Game::Physics {
  Collider::Collider(const BoundingSphere& sphere)
    : mType{Type::Sphere}
  {
    mData.sphere = sphere;
  }
  Collider::Collider(const Plane& plane)
    : mType{Type::Plane}
  {
    mData.plane = plane;
  }
  Collider::Collider(const AABB& aabb)
    : mType{Type::AABB}
  {
    mData.aabb = aabb;
  }

  const BoundingSphere& Collider::getBoundingSphere() const {
    GAME_DEBUG_ASSERT(mType == Type::Sphere);
    return mData.sphere;
  }
  const Plane& Collider::getPlane() const {
    GAME_DEBUG_ASSERT(mType == Type::Plane);
    return mData.plane;
  }
  const AABB& Collider::getAABB() const {
    GAME_DEBUG_ASSERT(mType == Type::AABB);
    return mData.aabb;
  }

  Intersection Collider::intersect(const Collider& other) const {
    if (mType == Type::Sphere && other.mType == Type::Sphere) {
      return getBoundingSphere().intersectBoundingSphere(other.getBoundingSphere());
    }

    GAME_TODO("Not implemented");
  }

  void Collider::offsetTranslation(const Vec3& offset) {
    if (mType == Type::Sphere) {
      mData.sphere.offsetTranslation(offset);
      return;
    }

    GAME_TODO("Not implemented");
  }
}