#pragma once

#include "Core/Base.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/CameraController.hpp"

#include "Editor/Config.hpp"
#include "Editor/Wire.hpp"

#include "Editor/Components.hpp"
#include "Editor/Connection.hpp"

#include <unordered_map>

namespace Gate {

  class EditorLayer {
  public:
    EditorLayer();
    ~EditorLayer();

    void onUpdate(Timestep ts);
    void onUiRender(Ui& ui);
    void onEvent(const Event& event);
  
    Vec2 gridAlginPosition(Vec2 position);
    Vec2 getGridAlignedMousePosition();

  private:
    ConnectionState getConnectionState(Connection& connection);
    ConnectionResult push_wire_connection(Point position, u32 wireIndex);
    ConnectionResult push_component_connection(Point position, u32 componentIndex, u32 pinIndex);
    bool push_component(Component* component);
    bool push_wire(Wire wire);
    void tick();

    void renderAll(Renderer& renderer);
    void renderComponentBodys(Renderer& renderer);
    void renderComponentConnectors(Renderer& renderer);
    void renderWires(Renderer& renderer);
    void renderGrid(Renderer& renderer);

    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

  private:
      enum class Mode {
        Select,
        WireDraw,
        AddComponent,
      };

      enum class ComponentType {
        Switch,
        Not,
        And,
        Or,
        Xor,
      };

      static StringView componentTypeToString(ComponentType type) {
        switch (type) {
          case ComponentType::Switch: return "Switch";
          case ComponentType::Not:    return "NOT Gate";
          case ComponentType::And:    return "AND Gate";
          case ComponentType::Or:     return "OR Gate";
          case ComponentType::Xor:    return "XOR Gate";
        }
        GATE_UNREACHABLE("invalid component type");
      }

  private:
    // Camera
    OrthographicCameraController mEditorCameraController;

    // State
    bool mClicked = false;
    Vec2 mLastMousePosition{0.0f};
    Mode mMode = Mode::Select;
    ComponentType mComponentType = ComponentType::Switch;

    // Selector
    Vec2 mSelectorPosition{ 0.0f, 0.0f };

    // Wire drawing
    Vec2 mWireStartPosition{0.0f};
    Vec2 mWireEndPosition{0.0f};

    // Grid drawing & caching
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;

    // Board parts/components
    std::vector<Component*> mComponents;
    std::vector<Wire> mWires;
    std::vector<std::vector<Connection>> mConnections;
    std::unordered_map<Point, u32> mConnectionsIndexByPoint;
  };

} // namespace Gate
