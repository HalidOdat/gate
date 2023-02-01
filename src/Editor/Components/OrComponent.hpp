#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class OrComponent : public Component {
  public:
    static const constexpr u32 A_INPUT_INDEX = 0;
    static const constexpr u32 B_INPUT_INDEX = 1;
    static const constexpr u32 OUTPUT_INDEX  = 2;
  public:
    OrComponent(Point position);
    virtual void renderBody(Renderer& renderer) override;
    virtual bool update() override;
  };

}
