#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class AndComponent : public Component {
  public:
    static const constexpr u32 A_INPUT_INDEX = 0;
    static const constexpr u32 B_INPUT_INDEX = 1;
    static const constexpr u32 OUTPUT_INDEX  = 0;
  public:
    AndComponent(Point position);
    virtual bool update() override;
    virtual void renderBody(Renderer2D& renderer) override;
    virtual void renderBody(Renderer3D& renderer, u32 id) override;
    virtual void renderConnectors(Renderer3D&, [[maybe_unused]] u32 id) override {}

    virtual Serializer::Node encode() const override;
  };

}
