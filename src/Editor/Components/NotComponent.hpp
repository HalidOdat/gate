#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class NotComponent : public Component {
  public:
    static const constexpr u32 INPUT_INDEX  = 0;
    static const constexpr u32 OUTPUT_INDEX = 0;
  public:
    NotComponent(Point position);
    virtual bool update() override;
    virtual void renderBody(Renderer2D& renderer) override;
    virtual void renderBody(Renderer3D& renderer, u32 id) override;
    
    virtual Serializer::Node encode() const override;
  };

}
