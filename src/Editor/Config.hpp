#pragma once

#include "Core/Base.hpp"

namespace Gate {

  struct Config {
    struct Selector {
      Vec2 position = { 0.0f, 0.0f };
      Vec2 size     = { 7.0f, 7.0f };
      Vec4 color    = Color::BLUE;
    };
    
    struct Component {
      struct Connector {
        Vec2 size;
        Vec4 color;
      };

      Connector input = { { 6.5f, 6.5f }, Color::GREEN };
      Connector output = { { 6.5f, 6.5f }, Color::RED };
    };

    struct Wire {
      f32  width{3.0f};

      Vec4 activeColor   = Color::RED;
      Vec4 inactiveColor = Color::BLACK;
      Vec4 invalidColor  = Color::PURPLE;
    };

    struct Grid {
      struct Cell {
        u32 size = 15;
      };

      Cell cell;
      Vec4 color = rgba(60, 60, 60);
    };

    Selector selector;
    Component component;
    Wire wire;
    Grid grid;

    String title = "Logic Gate Simulator";
  };

  // Global settings
  extern Config config;

}