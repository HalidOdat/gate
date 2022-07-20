#pragma once

#include <vector>

#include "Events/Event.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Renderer/Camera.hpp"

namespace Game {
  
  class Ui {
  public:

    class Layout {
      friend Ui;

    public:
      enum class Type {
        Horizontal,
        Vertical,
      };

    private:
      Vec2 nextAvailablePosition();

    private:
      Type type;
      Vec2 position;
      Vec2 size;
    };

  public:
    Ui(f32 left, f32 right, f32 bottom, f32 top);

    void begin(const Vec2& position);
    void beginLayout(Layout::Type type);
    bool button(const Vec3& color, u32 id = 0);
    void endLayout();
    void end();

    void onEvent(const Event& event);

  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

  private:
    OrthographicCamera camera;

    bool has_active = false;
    u32  active;

    Vec2 mousePosition = Vec2{0.0f, 0.0f};
    bool mouseButton   = false;

    std::vector<Layout> layouts;
  };

} // namespace Game
