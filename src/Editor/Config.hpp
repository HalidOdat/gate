#pragma once

#include "Core/Base.hpp"
#include "Themes/Settings.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"

namespace Gate {

  struct Config {
    struct Selector {
      Vec2 position = { 0.0f, 0.0f };
      Vec2 size     = { 7.0f, 7.0f };
      Vec4 color    = Color::BLUE;
    };
    
    struct Component {
      struct Connector {
        f32  size;
        Vec4 color;
      };

      Connector input  = { 0.13f, Color::GREEN };
      Connector output = { 0.13f, Color::RED };
    };

    struct Wire {
      f32  width{ 0.18f };
      Vec2 endsSize{ 0.24f };

      Vec4 activeColor   = Color::RED;
      Vec4 inactiveColor = Color::BLACK;
      Vec4 invalidColor  = Color::PURPLE;
    };

    struct Grid {
      struct Cell {
        u32 size = 25;

        f32 size3d = 1.0f;
      };

      Cell cell;
      Vec4 color = rgba(60, 60, 60);
      Vec4 background = Color::WHITE;
    };

    struct Text {
      Vec4 color;
    };

    Selector selector;
    Component component;
    Wire wire;
    Grid grid;
    Text text;

    Mesh::Handle pinMesh;
    Material::Handle activeMaterial;
    Material::Handle inactiveMaterial;

    SubTexture andGate;
    SubTexture orGate;
    SubTexture xorGate;
    SubTexture notGate;

    void apply(Theme::Settings settins);
  };

  // Global settings
  extern Config config;

}