#pragma once

#include "Core/Base.hpp"
#include "Editor/Config.hpp"
#include "Editor/Wire.hpp"
#include "Editor/Components.hpp"
#include "Editor/Connection.hpp"

#include "Renderer/Renderer.hpp"

#include <unordered_map>

namespace Gate {

  enum class WirePushState {
    Valid,
    Connected,
    Invalid,
  };

  class Chip {
  public:
    Chip(String name);
    ~Chip();

  public:
    void render(Renderer& renderer);
    bool push_component(Component* component);
    WirePushState push_wire(Wire wire);
    bool click(Point position);
    void tick();

  private:
    void renderComponentBodys(Renderer& renderer);
    void renderComponentConnectors(Renderer& renderer);
    void renderWires(Renderer& renderer);
    void renderGrid(Renderer& renderer);

    ConnectionState getConnectionState(Connection& connection);
    ConnectionResult push_wire_connection(Point position, u32 wireIndex);
    ConnectionResult push_component_connection(Point position, u32 componentIndex, u32 pinIndex);

  private:
    String mName;

    std::vector<Component*> mComponents;
    std::vector<Wire> mWires;
    std::vector<std::vector<Connection>> mConnections;
    std::unordered_map<Point, u32> mConnectionsIndexByPoint;
  };

}