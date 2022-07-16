#pragma once

#include "Core/Type.hpp"
#include "Events/Event.hpp"
#include "Events/KeyCode.hpp"

namespace Game {

  class KeyEvent : public Event {
  public:
    Key getKey()     const { return this->key; }
    i32 getKeyCode() const { return (i32)this->key; }

  protected:
    KeyEvent(Event::Type type, const Key key)
      : Event{type}, key{key}
    {}

  protected:
    Key key;
  };

  class KeyPressedEvent : public KeyEvent {    
  public:
    static constexpr const auto TYPE = Event::Type::KeyPressed;

  public:
    KeyPressedEvent(const Key key, bool repeat)
      : KeyEvent(TYPE, key), repeat{ repeat }
    {}

    inline bool isRepeat() const { return this->repeat; }

  private:
    bool repeat = false;
  };

  class KeyReleasedEvent : public KeyEvent {
  public:
    static constexpr const auto TYPE = Event::Type::KeyReleased;

  public:
    KeyReleasedEvent(const Key key)
      : KeyEvent(TYPE, key)
    {}
  };

} // namespace Game