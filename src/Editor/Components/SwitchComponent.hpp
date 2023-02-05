#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class SwitchComponent : public Component {
  public:
    static const constexpr u32 OUTPUT_INDEX = 0;
  public:
    SwitchComponent(Point position);
    virtual bool update() override;
    virtual void click() override;
    virtual void renderBody(Renderer2D& renderer) override;
    virtual void renderBody(Renderer3D& renderer) override;

  private:
    inline void toggle() { mOutputPins[OUTPUT_INDEX].active = !mOutputPins[OUTPUT_INDEX].active; }
  };

}
