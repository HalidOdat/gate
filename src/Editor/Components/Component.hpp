#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

#include "Editor/Point.hpp"

namespace Gate {

  struct Pin {
    enum class Type {
      Input,
      Output,
    };
    Type type;
    Point position;

    bool active = false;
  };

  class Component {
  public:
    inline Point getPosition() const { return mPosition; }
    inline std::vector<Pin>& getPins() { return mPins; }
    inline const std::vector<Pin>& getPins() const { return mPins; }

    inline bool isVisited() const { return mVisited; }
    inline void setVisited(bool value) { mVisited = value; }

  public:
    virtual ~Component();
    virtual void renderBody(Renderer& renderer) = 0;
    virtual void renderConnectors(Renderer&);
    virtual void click() = 0;
    virtual void update() = 0;

  protected:
    Component(Point position)
      : mPosition{position}
    {}

  protected:
    Point mPosition;
    std::vector<Pin> mPins;

    bool mVisited = false;
  };

}
