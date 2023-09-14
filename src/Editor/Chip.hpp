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

  class Board;

  enum class WirePushState {
    Valid,
    Connected,
    Invalid,
  };

  class Chip {
  public:
    using Handle = Ref<Chip>;

  public:
  static Chip::Handle create(u32 index);

  public:
    Chip(u32 index);
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

    u32 getOptimalCellSize() const { return mOptimalCellSize; }
    u32 getIndex() const { return mIndex; }

    std::pair<std::vector<SwitchComponent*>, std::vector<OutputComponent*>> getPinComponents() const;

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

    void calculateOptimalCellSize(Point position);

  private:
    String mName;

    // Component and wire states
    std::vector<Component*> mComponents;
    std::vector<Wire> mWires;
    std::vector<std::vector<Connection>> mConnections;
    std::unordered_map<Point, u32> mConnectionsIndexByPoint;

    // Size
    u32 mOptimalCellSize;

    u32 mIndex = 0;
  };

}

namespace Gate::Serializer {

  template<>
  struct Convert<Chip> {
    static Node encode(Chip& value);
    static bool decode(const Node& node, Chip& value, Board& board);
  };

}

