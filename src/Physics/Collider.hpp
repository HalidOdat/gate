#pragma once

#include "Physics/BoundingSphere.hpp"
#include "Physics/AABB.hpp"
#include "Physics/Plane.hpp"

namespace Game::Physics {

  class Collider {
  public:
    enum class Type : u8 {
      Sphere,
      Plane,
      AABB,
    };

  public:
    Collider(const BoundingSphere& sphere);
    Collider(const Plane& plane);
    Collider(const AABB& aabb);

    const BoundingSphere& getBoundingSphere() const;
    const Plane& getPlane() const;
    const AABB& getAABB() const;

    Intersection intersect(const Collider& other) const;
    void offsetTranslation(const Vec3& offset);

  private:
    union Data {
      Data() = default;
      BoundingSphere sphere;
      Plane plane;
      AABB aabb;
    };

  public:
    Type mType;
    Data mData;
  };

}