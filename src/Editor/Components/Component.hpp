#pragma once

#include "Core/Base.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Editor/Point.hpp"
#include "Editor/Pin.hpp"
#include "Serializer/Serializer.hpp"

class Component;

namespace Gate {

#define GATE_COMPONENT_IMPLEMENTATION(name)               \
  Serializer::Node name::encode() const {                 \
    using namespace Serializer;                           \
                                                          \
    auto node = Node::object();                           \
    node["type"]     = String(#name);                     \
    node["position"] = Convert<Point>::encode(mPosition); \
    return node;                                          \
  }

  class Component {
  public:
    enum class Category {
      Input,
      Gate,
    };

    enum class Type {
      Switch,

      AndGate,
      OrGate,
      XorGate,
      NotGate,
    };

  public:
    inline Point getPosition() const { return mPosition; }
    inline std::vector<Pin>& getInputPins() { return mInputPins; }
    inline const std::vector<Pin>& getInputPins() const { return mInputPins; }
    inline std::vector<Pin>& getOutputPins() { return mOutputPins; }
    inline const std::vector<Pin>& getOutputPins() const { return mOutputPins; }

    inline bool isVisited() const { return mVisited; }
    inline void setVisited(bool value) { mVisited = value; }
    bool areAllInputPinsVisited() const;
    void resetVisited();

    inline Category getCategory() const { return mCategory; }

    Mat4 computeModel(f32 size) const;

  public:
    virtual ~Component();
    virtual void click() {}
    virtual bool update() = 0;

    virtual void renderBody(Renderer2D& renderer) = 0;
    virtual void renderConnectors(Renderer2D&);

    virtual void renderBody(Renderer3D&, u32 id) { (void)id; }
    virtual void renderConnectors(Renderer3D&, u32 id);

    virtual Serializer::Node encode() const = 0;
    static Component* decode(const Serializer::Node& node);

  protected:
    Component(Category category, Type type, Point position)
      : mCategory{category}, mType{type}, mPosition{position}
    {}

  protected:
    Category mCategory;
    Type mType;

    Point mPosition;
    std::vector<Pin> mInputPins;
    std::vector<Pin> mOutputPins;

    bool mVisited = false;
  };

}
