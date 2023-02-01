#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

#include "Editor/Point.hpp"
#include "Editor/Pin.hpp"

namespace Gate {

  class Component {
  public:
    enum class Category {
      Input,
      Gate,
    };

  public:
    inline Point getPosition() const { return mPosition; }
    inline std::vector<Pin>& getPins() { return mPins; }
    inline const std::vector<Pin>& getPins() const { return mPins; }

    inline bool isVisited() const { return mVisited; }
    inline void setVisited(bool value) { mVisited = value; }
    bool areAllInputPinsVisited() const;
    void resetVisited();

    inline Category getCategory() const { return mCategory; }

  public:
    virtual ~Component();
    virtual void renderBody(Renderer& renderer) = 0;
    virtual void renderConnectors(Renderer&);
    virtual void click() {}
    virtual bool update() = 0;

  protected:
    Component(Category category, Point position)
      : mCategory{category}, mPosition{position}
    {}

  protected:
    Category mCategory;
    Point mPosition;
    std::vector<Pin> mPins;

    bool mVisited = false;
  };

}
