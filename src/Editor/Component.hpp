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

  private:
    Point mOut;
  };

  // class AndComponent : public Component {
  // public:
  //   AndComponent()
  //   virtual void render(Renderer& renderer) override;

  // private:
  //   Point mA;
  //   Point mB;

  //   Point out;
  // };

}
