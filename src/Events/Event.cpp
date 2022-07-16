#include "Core/Assert.hpp"

#include "Events/Event.hpp"

namespace Game {
  
  const char* Event::getName() const {
    switch (this->type) {
      case Type::KeyPressed:          return "KeyPressed";
      case Type::KeyReleased:         return "KeyReleased";
      case Type::MouseMove:           return "MouseMove";
      case Type::MouseScroll:         return "MouseScroll";
      case Type::MouseButtonPressed:  return "MouseButtonPressed";
      case Type::MouseButtonReleased: return "MouseButtonReleased";
      case Type::WindowResize:        return "WindowResize";
      case Type::WindowClose:         return "WindowClose";
    }

    GAME_UNREACHABLE("Unknown event type");
    return "";
  }

} // namespace Game
