#pragma once

#include "Core/Type.hpp"
#include "Core/KeyCode.hpp"
#include "Events/Event.hpp"

namespace Game {

  class KeyEvent : public Event {
  public:
    Key getKey()      const { return this->mKey; }
    i32 getKeyCode()  const { return (i32)this->mKey; }

  protected:
    KeyEvent(Event::Type type, Key key)
      : Event{type}, mKey{key}
    {}

  protected:
    Key mKey;
  };

  class KeyPressedEvent : public KeyEvent {    
  public:
    static constexpr const auto TYPE = Event::Type::KeyPressed;

  public:
    KeyPressedEvent(Key key, KeyModifier modifier, bool repeat)
      : KeyEvent(TYPE, key), mModifier{modifier}, mRepeat{repeat}
    {}

    inline KeyModifier getModifier() const { return this->mModifier; }
    inline bool isRepeat() const { return this->mRepeat; }

  private:
    KeyModifier mModifier;
    bool mRepeat = false;
  };

  class KeyReleasedEvent : public KeyEvent {
  public:
    static constexpr const auto TYPE = Event::Type::KeyReleased;

  public:
    KeyReleasedEvent(Key key)
      : KeyEvent(TYPE, key)
    {}
  };

} // namespace Game