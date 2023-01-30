#pragma once

#include "Core/Base.hpp"
#include "Utils/Color.hpp"

namespace Gate {

  struct Config {
    struct Selector {
      Vec2 position = { 0.0f, 0.0f };
      Vec2 size     = { 7.0f, 7.0f };
      Vec4 color    = Color::BLUE;
    };
    
    struct Wire {
      f32  width{3.0f};

      Vec4 activeColor   = Color::RED;
      Vec4 inactiveColor = Color::BLACK;
    };

    struct Grid {
      struct Cell {
        u32 size = 15;
      };

      Cell cell;
      Vec4 color = Color::BLACK;
    };

    Selector selector;
    Wire wire;
    Grid grid;

    String title = "Logic Gate Simulator";
  };

  // Global settings
  extern Config config;

}