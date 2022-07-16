#pragma once

#include <functional>

namespace Game {

  class Event {
  public:
    enum class Type {
      KeyPressed, KeyReleased,
      MouseMove, MouseScroll, MouseButtonPressed, MouseButtonReleased,
      WindowResize, WindowClose,
    };

  public:
    inline Type getType() const { return this->type; }

    template<typename T, typename U>
    bool dispatch(void (U::*fn)(const T&), U* self) const {
      if (this->getType() == T::TYPE) {
        (self->*fn)(*(T*)this);
        return true;
      }
      return false;
    }

    const char* getName() const;

  protected:
    Event(const Type type) 
      : type{type}
    {}

  private:
    Type type;
  };

} // namespace Game
