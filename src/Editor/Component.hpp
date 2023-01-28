#pragma once

#include "Core/Base.hpp"

#include "Renderer/Renderer.hpp"

namespace Game {

  struct Component {
    String name;

    void render(Renderer& renderer);
  };

}
