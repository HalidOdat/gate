#pragma once

#include "Core/Base.hpp"

#include <functional>

namespace Gate {

  struct Point {
    u32 x;
    u32 y;

    Point() = default;
    Point(Vec2 position) {
      this->x = (u32)position.x;
      this->y = (u32)position.y;
    }
    Point(u32 x_, u32 y_) {
      this->x = x_;
      this->y = y_;
    }

    Vec2 toVec2() const {
      return { (f32)x, (f32)y };
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
