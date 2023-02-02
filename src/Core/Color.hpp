#pragma once

#include "Core/Type.hpp"

namespace Gate {
  
  constexpr Vec4 rgba(u8 r, u8 g, u8 b, u8 a = 255) {
    return {
      r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f
    };
  }
  constexpr Vec4 rgba(u32 hex) {
    u8 a = (hex >>  0) & 0xFF;
    u8 b = (hex >>  8) & 0xFF;
    u8 g = (hex >> 16) & 0xFF;
    u8 r = (hex >> 24) & 0xFF;
    return {
      r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f
    };
  }

  class Color {
  public:
    static constexpr const auto WHITE     = rgba(0xFFFFFFFF);
    static constexpr const auto BLACK     = rgba(0x000000FF);
    static constexpr const auto DARK_GRAY = rgba(0xA9A9A9FF);
    static constexpr const auto RED       = rgba(0xFF0000FF);
    static constexpr const auto GREEN     = rgba(0x00FF00FF);
    static constexpr const auto BLUE      = rgba(0x0000FFFF);
    static constexpr const auto DARK_BLUE = rgba(0x0000FFFF);
    static constexpr const auto PURPLE    = rgba(0x800080FF);
    static constexpr const auto ORANGE    = rgba(0xFFA500FF);
  };

} // namespace Gate

