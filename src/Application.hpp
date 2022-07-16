#pragma once

#include "Window.hpp"
#include "Events/KeyEvent.hpp"

namespace Game {

  class Application {
  protected:
    Application(const char* title = "Game Title", const u32 width = 800, const u32 height = 600);
    DISALLOW_MOVE_AND_COPY(Application);
    ~Application();

  public:
    void start();
    void quit();

    virtual void onCreate() {};
    virtual void onUpdate() = 0;
    virtual void onDestroy() {};

  private:
    void onEvent(const Event& event);

  private:
    Ref<Window> window = nullptr;
  };
  
}