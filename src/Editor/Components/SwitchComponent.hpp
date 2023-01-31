#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class SwitchComponent : public Component {
  public:
    static const constexpr u32 OUTPUT_INDEX = 0;
  public:
    SwitchComponent(Point position);
    virtual void renderBody(Renderer& renderer) override;
    virtual void click() override;
    virtual void update() override;

  public:
    inline void toggle() { mPins[OUTPUT_INDEX].active = !mPins[OUTPUT_INDEX].active; }
  };

}
