#pragma once

#include "Core/Base.hpp"

namespace Gate {

  namespace Theme {

    struct HexColor {
      u32 value;

      inline operator u32() const { return value; }
    };
    
    struct Board {
      HexColor background;
      HexColor points;
    };

    struct Wire {
      HexColor active;
      HexColor inactive;
      HexColor invalid;
    };

    struct Text {
      HexColor color;
    };

    struct Settings {
      Board board;
      Wire wire;
      Text text;

      static Option<Settings> load(const char* filepath);
    };

  } // namespace Theme

} // namespace Gate