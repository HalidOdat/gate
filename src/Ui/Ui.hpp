#pragma once

#include <vector>

#include "Events/Event.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Renderer/CameraController.hpp"
#include "Renderer/Renderer.hpp"

#include "Resource/Manager.hpp"

namespace Game {

  class Application;

  class Ui {
  public:
    struct Config {
      struct Margin {
        f32 left   = 1.0f;
        f32 top    = 0.5f;
        f32 right  = 1.0f;
        f32 bottom = 0.5f;
      };

      struct Color {
        Vec3 inactive = { 0.15f, 0.15f, 0.15f };
        Vec3 hot      = { 0.20f, 0.20f, 0.20f };
        Vec3 active   = { 0.25f, 0.25f, 0.25f };
      };

      struct Text {
        Vec3 color = Vec3(1.0f, 1.0f, 1.0f);
      };

      struct Button {
        Margin margin;
        Color color;
        Vec2 size { 60, 18 };
        Text text;
      };

      struct Slider {
        Margin margin;
        Color color;
        Vec2 size { 65, 18 };
        Text text;
      };

      struct Checkbox {
        Margin margin;
        Color color;
        Vec2 size { 18, 18 };
      };

      struct Dock {
        Vec3 backgroundColor = {0.1f, 0.1f, 0.1f};
      };

      struct Window {
        Vec2 size;
      };

      Window window;
      Dock dock;
      Button button;
      Slider slider;
      Checkbox checkbox;
    };

    enum class Dock {
      Left,
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
      Vec2 nextAvailablePosition(bool withPadding = true);
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
    void beginDock(Ui::Dock type, f32 size = 25.0f);
    void endDock();
    void endLayout();
    void end();

    void label(const StringView& text, const Vec3& foreground = {1.0f, 1.0f, 1.0f}, const Vec4& background = {1.0f, 1.0f, 1.0f, 0.0f});
    void image(const Texture::Handle& texture, u32 width, u32 height);
    bool button(const StringView& text, u64 id = 0);
    bool slider(f32& value, const f32 min, const f32 max);
    bool slider(Vec3& value, const Vec3& mins, const Vec3& maxs);
    bool checkbox(bool& value);

    Config& getConfig() { return mConfig; }
    const Config& getConfig() const { return mConfig; }

    void onEvent(const Event& event);
  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onMouseMoveEvent(const MouseMoveEvent& event);
    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);

    void prepareFrame();
    void endFrame();

  private:
    void drawQuad(const Vec2& position, const Vec2& size, const Vec3& color);
    void drawQuad(const Vec2& position, const Vec2& size, const Vec4& color);
    void drawText(const StringView& text, const Vec2& position, f32 size, const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));

  private:
    OrthographicCamera mCamera;

    bool hasHot    = false;
    bool hasActive = false;
    u64  hot       = 0;
    u64  active    = 0;

    Vec2 mousePosition = Vec2{0.0f, 0.0f};
    bool mouseButton   = false;
    Vec2 mLastMousePosition;

    std::vector<Layout> layouts;

    Config mConfig;

    f32 mLayerOffset = 1.0f;

    friend class Application;
  };

} // namespace Game
