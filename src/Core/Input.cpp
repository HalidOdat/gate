#include "Core/Input.hpp"

#include "Application.hpp"

namespace Game {

  bool Input::isKeyPressed(Key key) {
    return Application::get().getWindow().isKeyPressed(key);
  }

} // namespace Game
