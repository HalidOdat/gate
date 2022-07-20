#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class Timestep {
  public:
    Timestep(f32 time = 0.0f)
      : time{time}
    {}

    inline operator f32() const { return this->time; }

  private:
    f32 time;
  };

} // namespace Game
