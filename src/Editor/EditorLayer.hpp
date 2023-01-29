#pragma once

#include "Core/Base.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/CameraController.hpp"

#include "Editor/Line.hpp"
#include "Editor/Component.hpp"

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
    void renderAll(Renderer& renderer);
    void renderComponents(Renderer& renderer);
    void renderLines(Renderer& renderer);
    void renderGrid(Renderer& renderer);

    void invalidate(u32 width, u32 height);

    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

  private:
      enum class Mode {
        Select,
        LineDraw,
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
    Vec2 mSelectorSize     = { 8.0f, 8.0f };
    Vec4 mSelectorColor    = Color::BLUE;

    // Line drawing
    f32  mLineWidth{5.0f};
    Vec2 mLineStartPosition{0.0f};
    Vec2 mLineEndPosition{0.0f};
    Vec2 mLineSize{0.0f};

    // Grid drawing
    u32                 mGridCellSize = 20;
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;

    // Board parts/components
    std::vector<Component*>       mComponents;
    std::vector<std::vector<u32>> mConnections;
    std::vector<Line>             mLines;
  };

} // namespace Gate
