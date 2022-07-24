#pragma once

#include "Core/Assert.hpp"
#include "Core/Type.hpp"

namespace Game {

  class Event {
  public:
    enum class Type : u8 {
      KeyPressed, KeyReleased,
      MouseMove, MouseScroll, MouseButtonPressed, MouseButtonReleased,
      WindowResize, WindowClose,
    };

  public:
    inline Type getType() const { return this->type; }

    template<typename T, typename U>
    bool dispatch(bool (U::*fn)(const T&), U* self) const {
      if (this->getType() == T::TYPE) {
        GAME_DEBUG_ASSERT_WITH_MESSAGE(!this->handled, "Trying to handle an already handled event");
        this->handled = (self->*fn)(*(T*)this);
        return true;
      }
      return false;
    }

    template<typename T, typename F>
    bool dispatch(F fn) const {
      if (this->getType() == T::TYPE) {
        GAME_DEBUG_ASSERT_WITH_MESSAGE(!this->handled, "Trying to handle an already handled event");
        this->handled = fn(*(T*)this);
        return true;
      }
      return false;
    }

    inline bool isHandled() const { return this->handled; }
    const char* getName() const;

  protected:
    Event(const Type type) 
      : type{type}
    {}


  private:
    Type type;
    mutable bool handled = false;
  };

} // namespace Game