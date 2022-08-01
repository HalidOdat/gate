#pragma once

#include <vector>

#include "Events/Event.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Renderer/CameraController.hpp"

#include "Resource/Manager.hpp"

namespace Game {
  
  class Ui {
  public:
    struct Config {
      struct Margin {
        f32 left   = 4.0f;
        f32 top    = 2.0f;
        f32 right  = 4.0f;
        f32 bottom = 2.0f;
      };

      struct Color {
        Vec3 inactive = { 0.15f, 0.15f, 0.15f };
        Vec3 hot      = { 0.20f, 0.20f, 0.20f };
        Vec3 active   = { 0.25f, 0.25f, 0.25f };
      };

      struct Button {
        Margin margin;
        Color color;
        Vec2 size { 80, 20 };
      };

      Button  button;
    };

    class Layout {
      friend Ui;
    public:
      static constexpr const float DEFAULT_PADDING = 10.0f;

    public:
      enum class Type {
        Horizontal,
        Vertical,
      };

    private:
      Vec2 nextAvailablePosition();
      void pushWidget(const Vec2& widgetSize);

    private:
      Type type;
      f32  padding;
      Vec2 position;
      Vec2 size;
    };

  public:
    Ui(u32 width, u32 height);

    void begin(const Vec2& position, f32 padding = Layout::DEFAULT_PADDING);
    void beginLayout(Layout::Type type, f32 padding = Layout::DEFAULT_PADDING);
    bool button(const StringView& text, u32 id = 0);
    void endLayout();
    void end();

    Config& getConfig() { return mConfig; }
    const Config& getConfig() const { return mConfig; }

    void onEvent(const Event& event);

  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

  private:
    OrthographicCamera mCamera;

    bool hasHot    = false;
    u32  hot       = 0;
    bool hasActive = false;
    u32  active    = 0;

    Vec2 mousePosition = Vec2{0.0f, 0.0f};
    bool mouseButton   = false;

    std::vector<Layout> layouts;

    Config mConfig;
  };

} // namespace Game
