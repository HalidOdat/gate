#pragma once

#include "Core/Base.hpp"

namespace Gate {

  struct Point {
    u32 x;
    u32 y;

    Point() = default;
    Point(Vec2 position) {
      this->x = (u32)position.x;
      this->y = (u32)position.y;
    }

    Vec2 toVec2() const {
      return { (f32)x, (f32)y };
    }
  };

}