#pragma once

#include "Window.hpp"

#include <memory>

namespace Game {

  class Application {
  protected:
    Application(const char* title = "Game Title", const u32 width = 800, const u32 height = 600);
    ~Application();

  public:
    void start();
    void quit();

    virtual void onCreate() {};
    virtual void onUpdate() = 0;
    virtual void onDestroy() {};

  private:
    std::unique_ptr<Window> window = nullptr;
  };
  
}