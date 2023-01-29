#include "Core/Input.hpp"

#include "Application.hpp"

namespace Gate {

  bool Input::isKeyPressed(Key key) {
    return Application::get().getWindow().isKeyPressed(key);
  }

} // namespace Gate
