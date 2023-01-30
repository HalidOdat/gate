#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

namespace Gate {

  struct Component {
    String name;
    void render(Renderer& renderer);
  };

}
