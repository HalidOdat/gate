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
      : Event(TYPE), mSize{ width, height }
    {}

    inline WindowSize getSize() const { return mSize; }
    inline u32 getWidth() const { return mSize.width; }
    inline u32 getHeight() const { return mSize.height; }
    inline f32 getAspectRatio() const { return (f32)mSize.width / (f32)mSize.height; }

  private:
    WindowSize mSize;
  };

  class WindowCloseEvent : public Event {
  public:
    inline static constexpr const auto TYPE = Event::Type::WindowClose;

  public:
    WindowCloseEvent()
      : Event{TYPE}
    {}
  };

  class WindowMinimizedEvent : public Event {
  public:
    inline static constexpr const auto TYPE = Event::Type::WindowMinimized;

  public:
    WindowMinimizedEvent()
      : Event{TYPE}
    {}
  };

} // namespace Game