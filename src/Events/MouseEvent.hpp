#pragma once

#include "Core/Type.hpp"
#include "Events/Event.hpp"

namespace Gate {

  class MouseEvent : public Event {
  protected:
    MouseEvent(Event::Type type)
      : Event(type)
    {}
  };

  class MouseMoveEvent : public Event {
  public:
    struct Position {
      f32 x;
      f32 y;
    };

  public:
    inline static constexpr const auto TYPE = Event::Type::MouseMove;

  public:
    MouseMoveEvent(const f32 x, const f32 y)
      : Event(TYPE), mPosition{x, y}
    {}

    const Position& getPosition() const { return mPosition; }
    f32 getX() const { return mPosition.x; }
    f32 getY() const { return mPosition.y; }

    Position getOpenGLScreenCoordinates(u32 width, u32 height) const {
      return {
        mPosition.x     / ((f32)width / 2.0f) - 1,
        1 - mPosition.y / ((f32)height / 2.0f)
      };
    }

  private:
    Position mPosition;
  };

  class MouseScrollEvent : public Event {
  public:
    struct Offset {
      f32 x;
      f32 y;
    };

  public:
    inline static constexpr const auto TYPE = Event::Type::MouseScroll;

  public:
    MouseScrollEvent(const f32 xOffset, const f32 yOffset)
      : Event(TYPE), offset{xOffset, yOffset}
    {}

    const Offset& getOffset() const { return this->offset; }
    f32 getXOffset() const { return this->offset.x; }
    f32 getYOffset() const { return this->offset.y; }
    
  private:
    Offset offset;
  };

  enum class MouseButton {
    _1     = 0,
    _2     = 1,
    _3     = 2,
    _4     = 3,
    _5     = 4,
    _6     = 5,
    _7     = 6,
    _8     = 7,
    Last   = MouseButton::_8,
    Left   = MouseButton::_1,
    Right  = MouseButton::_2,
    Middle = MouseButton::_3,
  };

  class MouseButtonEvent : public MouseEvent {
  public:
    inline MouseButton getButton()    const { return this->button; }
    inline i32        getButtonCode() const { return (i32)this->button; }

  protected:
    MouseButtonEvent(Event::Type type, MouseButton button)
      : MouseEvent(type), button{ button }
    {}
    
  private:
    MouseButton button;
  };

  class MouseButtonPressedEvent : public MouseButtonEvent {
  public:
    inline static constexpr const auto TYPE = Event::Type::MouseButtonPressed;

  public:
    MouseButtonPressedEvent(const MouseButton button)
      : MouseButtonEvent(TYPE, button)
    {}
  };

  class MouseButtonReleasedEvent : public MouseButtonEvent {
  public:
    inline static constexpr const auto TYPE = Event::Type::MouseButtonReleased;
    
  public:
    MouseButtonReleasedEvent(const MouseButton button)
      : MouseButtonEvent(TYPE, button)
    {}
  };

} // namespace Gate