#pragma once

#include "Core/Base.hpp"
#include "Editor/Config.hpp"
#include "Serializer/Serializer.hpp"

#include <functional>

namespace Gate {

  struct Point {
    u32 x;
    u32 y;
    u32 z = 0;

    Point() = default;
    Point(Vec2 position) {
      this->x = (u32)position.x;
      this->y = (u32)position.y;
    }
    Point(Vec3 position) {
      this->x = (u32)position.x;
      this->y = (u32)position.y;
      this->z = (u32)position.z;
    }
    Point(u32 x_, u32 y_) {
      this->x = x_;
      this->y = y_;
    }
    Point(u32 x_, u32 y_, u32 z_) {
      this->x = x_;
      this->y = y_;
      this->z = z_;
    }

    Vec2 toVec2() const {
      return { (f32)x, (f32)y };
    }
    Vec3 toVec3() const {
      return { (f32)x, (f32)y, (f32)z };
    }

    Vec3 to3DWorldPosition() const {
      return toVec3() * config.grid.cell.size3d;
    }

    inline bool operator==(const Point& other) const { return x == other.x && y == other.y; }
  };

}

template<>
struct std::hash<Gate::Point> {
  std::size_t operator()(Gate::Point const& point) const noexcept {
    std::size_t h1 = std::hash<Gate::u32>{}(point.x);
    std::size_t h2 = std::hash<Gate::u32>{}(point.y);
    return h1 ^ (h2 << 1);
  }
};

namespace Gate::Serializer {

  template<>
  struct Convert<Point> {
    static Node encode(const Point& value);
    static bool decode(const Node& node, Point& value);
  };

}
