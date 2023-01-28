#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Renderer/Renderer.hpp"

#include "Ui/Ui.hpp"

namespace Game {

  class AABB {
  public:
    AABB(Vec2 position, Vec2 size)
      : position{position}, size{size}
    {}

    bool contains(const Vec2& point) const {
      if ((point.x >= position.x && point.x <= (position.x + size.x))
       && (point.y >= position.y && point.y <= (position.y + size.y))
      ) {
        return true;
      }

      return false;
    }

  private:
    Vec2 position;
    Vec2 size;
  };

  Vec2 Ui::Layout::nextAvailablePosition(bool withPadding) {
    (void)withPadding;

    switch (this->type) {
      case Type::Horizontal:
        return this->position + this->size * Vec2(1.0f, 0.0f);
      case Type::Vertical:
        return this->position + this->size * Vec2(0.0f, 1.0f);
      default:
        GAME_UNREACHABLE("Unknown Layout type!");
    }
  }

  void Ui::Layout::pushWidget(const Vec2& widgetSize) {
#   undef max
    switch (this->type) {
      case Type::Horizontal:
        this->size.x = this->size.x + widgetSize.x;
        this->size.y = std::max(this->size.y, widgetSize.y);
        break;
      case Type::Vertical:
        this->size.x = std::max(this->size.x, widgetSize.x);
        this->size.y = this->size.y + widgetSize.y;
        break;
      default:
        GAME_UNREACHABLE("Unknown Layout type!");
    }
  }

  Ui::Ui(u32 width, u32 height)
    : mCamera{0.0f, (f32)width, 0.0f, (f32)height}
  {
    mConfig.window.size = { (f32)width, (f32)height };
  }

  void Ui::prepareFrame() {
  }

  void Ui::endFrame() {
  }

  void Ui::drawQuad(const Vec2& position, const Vec2& size, const Vec4& color) {
    mRenderer.drawQuad(Vec2(position.x, mConfig.window.size.y - position.y), {size.x, -size.y}, color);
  }

  void Ui::drawQuad(const Vec2& position, const Vec2& size, const Vec3& color) {
    drawQuad(position, size, Vec4(color, 1.0f));
  }

  void Ui::drawText(const StringView& text, const Vec2& position, f32 size, const Vec3& color) {
    mRenderer.drawText(text, Vec2(position.x, mConfig.window.size.y - position.y), size, Vec4(color, 1.0f));
  }

  void Ui::begin(const Vec2& position, f32 padding) {
    Layout layout;
    layout.type     = Layout::Type::Vertical;
    layout.size     = Vec2(0.0f);
    layout.padding  = padding;
    layout.position = position;
  
    this->layouts.push_back(layout);

    mRenderer.begin(mCamera);
  }

  void Ui::beginLayout(Layout::Type type, f32 padding) {
    Layout layout;
    layout.type     = type;
    layout.position = layouts.back().nextAvailablePosition(false);
    layout.size     = Vec2(0.0f);
    layout.padding  = padding;

    this->layouts.push_back(layout);
  }

  void Ui::beginDock(Ui::Dock type, f32 size) {
    mRenderer.begin(mCamera);

    switch (type) {
      case Dock::Left: {
        Layout layout;
        layout.type     = Layout::Type::Vertical;
        layout.padding  = 0.0f;
        layout.position = {0.0f, 0.0f};
        layout.size     = {0.0f, 0.0f};
        this->layouts.push_back(layout);

        drawQuad(
          this->layouts[this->layouts.size() - 1].position,
          {
            mConfig.window.size.x / (100.0f / size),
            mConfig.window.size.y,
          },
          mConfig.dock.backgroundColor
        );
      }
        break;
      default:
        GAME_UNREACHABLE("");
    }
  }

  void Ui::endDock() {
    this->layouts.pop_back();
  }

  void Ui::label(const StringView& text, const Vec3& foreground, const Vec4& background) {
    auto& layout = this->layouts.back();

    const auto position = layout.nextAvailablePosition() + Vec2(mConfig.slider.margin.left, mConfig.slider.margin.top);
    const auto size     = Vec2(mConfig.slider.size.y * (f32)text.size());

    drawQuad(position, size, background);
    drawText(text, { position.x + 3, position.y + size.y}, size.x / (f32)text.size() - 3, foreground);

    layout.pushWidget(
      size
        + Vec2(mConfig.slider.margin.left,  mConfig.slider.margin.top)
        + Vec2(mConfig.slider.margin.right, mConfig.slider.margin.bottom)
    );
  }

  void Ui::image(const Texture2D::Handle& texture, u32 width, u32 height) {
    (void)texture;
    (void)width;
    (void)height;

    GAME_TODO("");
  }

  bool Ui::button(const StringView& text, u64 id) {
    auto& layout = this->layouts.back();

    const auto position = layout.nextAvailablePosition() + Vec2(mConfig.button.margin.left, mConfig.button.margin.top);
    const auto size     = mConfig.button.size;

    const auto rectangle = AABB(
      position, 
      size
    );

    Vec3 color = mConfig.button.color.inactive;
    bool clicked = false;
    if (this->hasActive && this->active == id) {
      if (!this->mouseButton) {
        this->hasActive = false;

        if (rectangle.contains(this->mousePosition)) {
          clicked = true;
        }
      } else {
        color = mConfig.button.color.active;
      }
    } else if (this->hasHot && this->hot == id) {
      color = mConfig.button.color.hot;

      if (!rectangle.contains(this->mousePosition)) {
        this->hasHot = false;
      } else if (this->mouseButton && !this->hasActive) {
        this->hasActive = true;
        this->active    = id;
      }
    } else {
      if (!this->hasActive && rectangle.contains(this->mousePosition)) {
        this->hasHot = true;
        this->hot    = id;
      }
    }

    drawQuad(position, size, color);
    drawText(text, position + Vec2(4.0f, size.y / 1.2f), size.y / 2.0f, mConfig.button.text.color);

    layout.pushWidget(
      size
        + Vec2(mConfig.button.margin.left, mConfig.button.margin.top)
        + Vec2(mConfig.button.margin.right, mConfig.button.margin.bottom)
    );
    return clicked;
  }

  static f32 scale(f32 value, std::pair<f32, f32> src, std::pair<f32, f32> dst) {
    return ((value - src.first) / (src.second - src.first)) * (dst.second - dst.first) + dst.first;
  }

  bool Ui::slider(f32& value, const f32 min, const f32 max) {
    const u64 id = (u64)(void*)&value;

    auto& layout = this->layouts.back();

    const auto position = layout.nextAvailablePosition() + Vec2(mConfig.slider.margin.left, mConfig.slider.margin.top);
    const auto size     = mConfig.slider.size;

    const auto rectangle = AABB(
      position, 
      size
    );

    Vec3 color = mConfig.slider.color.inactive;

    bool changed = false;
    if (this->hasActive && this->active == id) {
      color = mConfig.slider.color.active;
      if (!this->mouseButton) {
        this->hasActive = false;
        this->hasHot    = false;
      }
      
      if (mLastMousePosition.x != mousePosition.x) {
        mLastMousePosition = mousePosition;
        changed = true;
      }
    } else if (this->hasHot && this->hot == id) {
      color = mConfig.slider.color.hot;

      if (rectangle.contains(this->mousePosition)) {
        if (this->mouseButton && !this->hasActive) {
          this->hasActive    = true;
          this->active       = id;
          this->hasHot       = false;
          mLastMousePosition = mousePosition;
        }
      } else {
        this->hasHot = false;
      }
    } else {
      if (!this->hasActive && rectangle.contains(this->mousePosition)) {
        this->hasHot = true;
        this->hot    = id;
      }
    }

    if (changed) {
      f32 x = std::clamp(mLastMousePosition.x - position.x, 0.0f, size.x) / size.x;
      value = scale(x, {0.0f, 1.0f}, {min, max});
    }

    drawQuad(position, size, color);
    std::stringstream ss;
    ss.precision(4);
    ss << std::fixed << value;
    drawText(ss.str(), position + Vec2(4.0f, size.y / 1.2f), size.y / 2.0f, mConfig.slider.text.color);

    layout.pushWidget(
      size
        + Vec2(mConfig.slider.margin.left, mConfig.slider.margin.top)
        + Vec2(mConfig.slider.margin.right, mConfig.slider.margin.bottom)
    );

    return changed;
  }

  bool Ui::slider(Vec3& value, const Vec3& mins, const Vec3& maxs) {
    bool changed = false;
    this->beginLayout(Layout::Type::Horizontal);
      label("X", Vec3{1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}); changed = this->slider(value.x, mins.x, maxs.x) || changed;
      label("Y", Vec3{1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}); changed = this->slider(value.y, mins.y, maxs.y) || changed;
      label("Z", Vec3{1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}); changed = this->slider(value.z, mins.z, maxs.z) || changed;
    this->endLayout();
    return changed;
  }

  bool Ui::checkbox(bool& value) {
    u64 id = (u64)(void*)&value;

    auto& layout = this->layouts.back();

    const auto position = layout.nextAvailablePosition() + Vec2(mConfig.checkbox.margin.left, mConfig.checkbox.margin.top);
    const auto size     = mConfig.checkbox.size;

    const auto rectangle = AABB(
      position, 
      size
    );

    Vec3 color = mConfig.checkbox.color.inactive;
    if (this->hasActive && this->active == id) {
      if (!this->mouseButton) {
        this->hasActive = false;

        if (rectangle.contains(this->mousePosition)) {
          value = !value;
        }
      } else {
        color = mConfig.checkbox.color.active;
      }
    } else if (this->hasHot && this->hot == id) {
      color = mConfig.checkbox.color.hot;

      if (!rectangle.contains(this->mousePosition)) {
        this->hasHot = false;
      } else if (this->mouseButton && !this->hasActive) {
        this->hasActive = true;
        this->active    = id;
      }
    } else {
      if (!this->hasActive && rectangle.contains(this->mousePosition)) {
        this->hasHot = true;
        this->hot    = id;
      }
    }

    drawQuad(position, size, color);
    if (value) {
      drawQuad(position + Vec2(5.0f), size - Vec2(10.0f), Vec3(0.1f, 0.9f, 0.3f));
    } else {
      drawQuad(position + Vec2(5.0f), size - Vec2(10.0f), Vec3(0.3f, 0.3f, 0.3f));
    }

    layout.pushWidget(
      size
        + Vec2(mConfig.checkbox.margin.left, mConfig.checkbox.margin.top)
        + Vec2(mConfig.checkbox.margin.right, mConfig.checkbox.margin.bottom)
    );
    return value;
  }

  void Ui::endLayout() {
    auto size = this->layouts.back().size;
    this->layouts.pop_back();
    this->layouts.back().pushWidget(size);
  }

  void Ui::end() {
    GAME_DEBUG_ASSERT(this->layouts.size() == 1);
    // Layout layout = this->layouts.back();
    this->layouts.pop_back();

    mRenderer.end();
  }

  void Ui::onEvent(const Event& event) {
    event.dispatch(&Ui::onWindowResizeEvent, this);
    event.dispatch(&Ui::onMouseMoveEvent, this);
    event.dispatch(&Ui::onMouseButtonPressedEvent, this);
    event.dispatch(&Ui::onMouseButtonReleasedEvent, this);
  }

  bool Ui::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mConfig.window.size = { (f32)width, (f32)height };
    mCamera.setProjection(0.0f, (f32)width, 0.0f, (f32)height);
    return false;
  }

  bool Ui::onMouseMoveEvent(const MouseMoveEvent& event) {
    this->mousePosition = {event.getX(), event.getY()};
    return this->hasHot || this->hasActive;
  }

  bool Ui::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) {
    (void)event;
    this->mouseButton = true;
    return this->hasHot || this->hasActive;
  }

  bool Ui::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    (void)event;
    this->mouseButton = false;
    return  this->hasHot || this->hasActive;
  }

} // namespace Game
