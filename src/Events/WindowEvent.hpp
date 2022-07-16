#pragma once

#include "Core/Type.hpp"
#include "Events/Event.hpp"

namespace Game {

  class WindowEvent : public Event {
  protected:
    WindowEvent(Event::Type type)
      : Event{ type }
    {}
  };

  class WindowResizeEvent : public Event {
  public:
    struct WindowSize {
      u32 width;
      u32 height;
    };

  public:
    inline static constexpr const auto TYPE = Event::Type::WindowResize;

  public:
    WindowResizeEvent(const u32 width, const u32 height)
      : Event(TYPE), size{ width, height }
    {}

    inline WindowSize getSize() const { return this->size; }
    inline u32 getWidth() const { return this->size.width; }
    inline u32 getHeight() const { return this->size.height; }
    
  private:
    WindowSize size;
  };

  class WindowCloseEvent : public Event {
  public:
    inline static constexpr const auto TYPE = Event::Type::WindowClose;

  public:
    WindowCloseEvent()
      : Event{TYPE}
    {}
  };

} // namespace Game