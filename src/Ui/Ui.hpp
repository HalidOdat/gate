#pragma once

#include "Core/Math.hpp"

namespace Game {
  
  class Ui {
  public:
    enum class LayoutType {
      Horizontal,
      Vertical,
    };

  public:
    Ui() = delete;

    static void begin(const Vec2& position);
    static void beginLayout(LayoutType type);
    static bool button(const Vec4& color, u32 id = 0);
    static void endLayout();
    static void end();
  };

} // namespace Game
