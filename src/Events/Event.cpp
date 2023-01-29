#include "Core/Base.hpp"

#include "Events/Event.hpp"

namespace Gate {
  
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
      case Type::WindowMinimized:     return "WindowMinimized";
    }

    GATE_UNREACHABLE("Unknown event type");
  }

} // namespace Gate
