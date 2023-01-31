#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

#include "Editor/Point.hpp"

namespace Gate {

  class Component {
  public:
    virtual ~Component();
    virtual void renderBody(Renderer& renderer) = 0;
    virtual void renderConnectors(Renderer&) {}

  protected:
    Component(Point pos)
      : mPosition{pos}
    {}

  protected:
    Point mPosition;
  };
 
  class SwitchComponent : public Component {
  public:
    SwitchComponent(Point position);
    virtual void renderBody(Renderer& renderer) override;
    virtual void renderConnectors(Renderer& renderer) override;

    inline Point getOutPoint() const { return mOut; }

  private:
    Point mOut;
  };

}
