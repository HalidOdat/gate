#include <vector>

#include "Core/Log.hpp"
#include "Core/Assert.hpp"
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
       && (point.y <= position.y && point.y >= (position.y - size.y))
      ) {
        return true;
      }

      return false;
    }

  private:
    Vec2 position;
    Vec2 size;
  };

  Vec2 Ui::Layout::nextAvailablePosition() {
    switch (this->type) {
      case Type::Horizontal:
        return this->position + (this->size + Vec2(this->padding)) * Vec2(1.0f, 0.0f);
      case Type::Vertical:
        return this->position + (this->size + Vec2(this->padding)) * Vec2(0.0f, 1.0f);
      default:
        GAME_UNREACHABLE("Unknown Layout type!");
    }
  }

  void Ui::Layout::pushWidget(const Vec2& widgetSize) {
    switch (this->type) {
      case Type::Horizontal:
        this->size.x = this->size.x + widgetSize.x + this->padding;
        this->size.y = std::max(this->size.y, widgetSize.y);
        break;
      case Type::Vertical:
        this->size.x = std::max(this->size.x, widgetSize.x);
        this->size.y = this->size.y + widgetSize.y + this->padding;
        break;
      default:
        GAME_UNREACHABLE("Unknown Layout type!");
    }
  }

  Ui::Ui(f32 left, f32 right, f32 bottom, f32 top)
    : camera{left, right, bottom, top}
  {}

  void Ui::begin(const Vec2& position, f32 padding) {
    // Clear previous state (if any)
    // this->has_active = false;
    // this->active     = 0;

    Layout layout;
    layout.type     = Layout::Type::Horizontal;
    layout.position = position;
    layout.size     = Vec2(0.0f);
    layout.padding  = padding;
  
    this->layouts.push_back(layout);

    Renderer::begin(this->camera);
  }

  void Ui::beginLayout(Layout::Type type, f32 padding) {
    GAME_UNREACHABLE("todo");
  }

  bool Ui::button(const Vec3& color, u32 id) {
    auto& layout = this->layouts.back();

    const auto position = layout.nextAvailablePosition();
    const auto size     = Vec2(0.15f, 0.1f);

    const auto rectangle = AABB(position, size);

    Vec3 c = color;
    bool clicked = false;
    if (this->has_active && this->active == id) {
      if (!this->mouseButton) {
        c = Vec3(0.0f, 1.0f, 1.0f);
        if (rectangle.contains(this->mousePosition)) {
          clicked = true;
        }
        this->has_active = false;
      }
    } else {
      if (this->mouseButton && rectangle.contains(this->mousePosition)) {
        if (!this->has_active) {
          this->has_active = true;
          this->active     = id;
        }
      }
    }

    Renderer::drawQuad(position, size, Vec4(c, 1.0f));

    layout.pushWidget(size);
    return clicked;
  }

  void Ui::endLayout() {
    GAME_UNREACHABLE("todo");
  }

  void Ui::end() {
    GAME_DEBUG_ASSERT(this->layouts.size() == 1);
    this->layouts.pop_back();
    Renderer::end();
  }

  void Ui::onEvent(const Event& event) {
    event.dispatch(&Ui::onWindowResizeEvent, this);
    event.dispatch(&Ui::onMouseMoveEvent, this);
    event.dispatch(&Ui::onMouseButtonPressedEvent, this);
    event.dispatch(&Ui::onMouseButtonReleasedEvent, this);
  }

  bool Ui::onWindowResizeEvent(const WindowResizeEvent& event) {
    GAME_UNREACHABLE("todo");
    return false;
  }

  bool Ui::onMouseMoveEvent(const MouseMoveEvent& event) {
    this->mousePosition = {event.getX(), event.getY()};
    return false;
  }

  bool Ui::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) {
    this->mouseButton = true;
    return false;  
  }

  bool Ui::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    this->mouseButton = false;
    return false;  
  }

} // namespace Game
