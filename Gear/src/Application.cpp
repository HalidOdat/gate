#include <Gear/Application.hpp>

namespace Gear {

  void Application::start() {
    this->onCreate();

    while (!this->window.shouldClose()) {
      this->onUpdate();

      this->window.update();
    }

    this->onDestroy();
  }

  void Application::quit() {
    this->window.setShouldClose();
  }

} // namespace Gear