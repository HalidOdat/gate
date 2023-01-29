#pragma once

#include "Core/Type.hpp"

namespace Gate {
  
  class Application;

  class Timestep {
  public:
    inline static Timestep get() { return timestep; }
    inline operator f32() const { return this->time; }

  private:
    Timestep(f32 time = 0.0f)
      : time{time}
    {}

  private:
    static Timestep timestep;

  private:
    f32 time;

    friend class Application;
  };

} // namespace Gate
