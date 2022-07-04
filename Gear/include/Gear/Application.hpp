#pragma once

#include <Gear/Window.hpp>

namespace Gear {
  class Application {
  protected:
    Application(const char* title = "Game Title", const u32 width = 800, const u32 height = 600)
      : window{title, width, height}
    {
    }

  public:
    void start();
    void quit();

    virtual void onCreate() {};
    virtual void onUpdate() = 0;
    virtual void onDestroy() {};

  private:
    Window window;
  };
}