#include <vector>

#include "Core/Assert.hpp"
#include "Renderer/Renderer.hpp"

#include "Ui/Ui.hpp"

namespace Game {

  class AABB {
  public:
    AABB(Vec2 positon, Vec2 size)
      : min{positon}, max{positon + size}
    {}

    bool contains(const Vec2& point) const {
      if ( point.x > this->min.x && point.x < this->max.x
        && point.y > this->min.y && point.y < this->max.y
      ) {
        return true;
      }

      return false;
    }

  private:
    Vec2 min;
    Vec2 max;
  };

  struct Layout {
    Ui::LayoutType type;
    Vec2           position;
    Vec2           size;

    Vec2 nextAvailablePosition() {
      switch (this->type) {
        case Ui::LayoutType::Horizontal:
          return this->position + this->size * Vec2(1.0f, 0.0f);
        case Ui::LayoutType::Vertical:
          return this->position + this->size * Vec2(0.0f, 1.0f);
        default:
          GAME_UNREACHABLE("Unknown Layout type!");
      }
    }
  };

  struct MouseState {
    Vec2 position;
    bool button;
  };

  struct UiContext {
    std::vector<Layout> layouts;
    MouseState          mouse;
    bool                has_active = false;
    u32                 active;
  };

  static UiContext ui;

  void Ui::begin(const Vec2& position) {
    Layout layout;
    layout.type     = Ui::LayoutType::Horizontal;
    layout.position = position;
    layout.size     = Vec2(0.0f);
  
    ui.layouts.push_back(layout);
  }

  void Ui::beginLayout(LayoutType type) {
    GAME_UNREACHABLE("");
  }

  bool Ui::button(const Vec4& color, u32 id) {
    const auto position = ui.layouts.back().nextAvailablePosition();
    const auto size     = Vec2(0.1f, 0.1f);

    const auto rectangle = AABB(position, size);

    bool clicked = false;
    if (ui.has_active && ui.active == id) {
      if (!ui.mouse.button) {
        ui.has_active = false;
        clicked = true;
      }
    } else {
      if (ui.mouse.button && rectangle.contains(ui.mouse.position)) {
        if (!ui.has_active) {
          ui.has_active = true;
          ui.active     = id;
        }
      }
    }

    return clicked;
  }

  void Ui::endLayout() {
    GAME_UNREACHABLE("");
  }

  void Ui::end() {
    GAME_UNREACHABLE("");
  }


} // namespace Game
