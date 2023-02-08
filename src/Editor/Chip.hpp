#pragma once

#include "Core/Base.hpp"
#include "Editor/Config.hpp"
#include "Editor/Wire.hpp"
#include "Editor/Components.hpp"
#include "Editor/Connection.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Serializer/Serializer.hpp"

#include <unordered_map>

namespace Gate {

  enum class WirePushState {
    Valid,
    Connected,
    Invalid,
  };

  class Chip {
  public:
    using Handle = Ref<Chip>;

  public:
    static Chip::Handle create(String name = "Unnamed chip");

  public:
    Chip(String name);
    ~Chip();
    DISALLOW_MOVE_AND_COPY(Chip);

  public:
    void render(Renderer2D& renderer);
    void render(Renderer3D& renderer);
    bool pushComponent(Component* component);
    void removeComponent(Point position);
    void removeWire(Point position);
    WirePushState pushWire(Wire wire);
    bool click(Point position);
    bool click(u32 id);
    void tick();

    const std::vector<Wire> getWires() const { return mWires; }
    const std::vector<Component*> getComponents() const { return mComponents; }
    const String& getName() const { return mName; }
    void setName(String name) { mName = std::move(name); }

  private:
    void renderComponentBodys(Renderer2D& renderer);
    void renderComponentConnectors(Renderer2D& renderer);
    void renderWires(Renderer2D& renderer);
    void renderGrid(Renderer2D& renderer);

    void renderComponentBodys(Renderer3D& renderer);
    void renderComponentConnectors(Renderer3D& renderer);
    void renderWires(Renderer3D& renderer);
    void renderGrid(Renderer3D& renderer);

    ConnectionState getConnectionState(Connection& connection);
    ConnectionResult pushWireConnection(Point position, u32 wireIndex);
    ConnectionResult pushComponentConnection(Point position, u32 componentIndex, u32 pinIndex);

  private:
    String mName;

    std::vector<Component*> mComponents;
    std::vector<Wire> mWires;
    std::vector<std::vector<Connection>> mConnections;
    std::unordered_map<Point, u32> mConnectionsIndexByPoint;
  };

}

namespace Gate::Serializer {

  template<>
  struct Convert<Chip> {
    static Node encode(Chip& value);
    static bool decode(const Node& node, Chip& value);
  };

}

