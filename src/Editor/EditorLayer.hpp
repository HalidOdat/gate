#pragma once

#include "Core/Base.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/CameraController.hpp"

#include "Editor/Config.hpp"
#include "Editor/Wire.hpp"

#include "Editor/Components.hpp"

#include <unordered_map>

namespace Gate {

  class EditorLayer {
  public:
    EditorLayer();

    void onAttach();
    void onDetach();
    void onUpdate(Timestep ts);
    void onUiRender(Ui& ui);
    void onEvent(const Event& event);
  
    Vec2 gridAlginPosition(Vec2 position);
    Vec2 getGridAlignedMousePosition();

  private:
    bool push_component(Component* component);
    bool push_wire(Wire wire);
    void tick();
    void tickConnections(std::vector<u32>& connectionIndexes, bool pin);

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
      };

  private:
    // Camera
    OrthographicCameraController mEditorCameraController;

    // State
    bool mClicked = false;
    Vec2 mLastMousePosition{0.0f};
    Mode mMode = Mode::Select;

    // Selector
    Vec2 mSelectorPosition = { 0.0f, 0.0f };

    // Wire drawing
    Vec2 mWireStartPosition{0.0f};
    Vec2 mWireEndPosition{0.0f};

    // Grid drawing & caching
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;

    // Board parts/components
    struct Connection {
      /// The type that describes the connector type and direction.
      enum Type {
        WireTo,
        WireFrom,
        ComponentIn,
        ComponentOut,
      };

      Type type;
      u32 componentIndex;
      // The index can either be an index to a wire in mWires,
      // or an index in the component's in or out pins.
      u32 index;
    };
    std::vector<Component*> mComponents;
    std::vector<Wire> mWires;
    std::vector<Connection> mConnections;
    std::unordered_map<Point, std::vector<u32>> mConnectionsByPoint;
  };

} // namespace Gate
