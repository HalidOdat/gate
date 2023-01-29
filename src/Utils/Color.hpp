#pragma once

#include "Core/Math.hpp"

namespace Gate {
  
  class Color {
  public:
    static constexpr const auto WHITE = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    static constexpr const auto BLACK = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    static constexpr const auto RED   = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
    static constexpr const auto GREEN = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
    static constexpr const auto BLUE  = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
  };

} // namespace Gate
