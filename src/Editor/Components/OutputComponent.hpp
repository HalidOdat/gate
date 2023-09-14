#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"

#include "Editor/Components/Component.hpp"

namespace Gate {
 
  class OutputComponent : public Component {
  public:
    static const constexpr u32 INPUT_INDEX = 0;
  public:
    OutputComponent(Point position);
    virtual bool update() override;
    virtual bool deletable() override;
    virtual void setDeletable(bool value) override;
    virtual void renderBody(Renderer2D& renderer) override;
    virtual void renderBody(Renderer3D& renderer, u32 id) override;

    virtual Serializer::Node encode() const override;
  
  private:
    bool mDeletable = true;
  };

}
