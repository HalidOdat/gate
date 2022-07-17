#pragma once

#include "Window.hpp"
#include "Events/WindowEvent.hpp"
#include "Layers/LayerStack.hpp"

namespace Game {

  class Application {
  protected:
    Application(const char* title = "Game Title", const u32 width = 800, const u32 height = 600);
    DISALLOW_MOVE_AND_COPY(Application);
    ~Application();

  public:
    void start();
    void quit();

    void pushLayer(Layer* layer);

  private:
    void onEvent(const Event& event);
    bool onWindowCloseEvent(const WindowCloseEvent& event);
    bool onWindowResizeEvent(const WindowResizeEvent& event);

  private:
    bool running = true;
    Ref<Window> window = nullptr;
    LayerStack layerStack;
  };
  
}