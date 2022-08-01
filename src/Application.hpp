#pragma once

#include "Ui/Ui.hpp"
#include "Window.hpp"
#include "Core/KeyCode.hpp"
#include "Events/WindowEvent.hpp"
#include "Layers/LayerStack.hpp"

namespace Game {

  class Application {
  protected:
    Application(const char* title = "Game Title", const u32 width = 900, const u32 height = 600);
    DISALLOW_MOVE_AND_COPY(Application);
    ~Application();

  public:
    inline static Application& get() { return *sInstance; }
    inline static Window& getWindow() { return *sInstance->window; }

    void start();
    void quit();

    void pushLayer(Layer* layer);

  private:
    void onEvent(const Event& event);
    bool onWindowCloseEvent(const WindowCloseEvent& event);
    bool onWindowResizeEvent(const WindowResizeEvent& event);

  private:
    static Application* sInstance;

  private:
    bool running = true;
    Ref<Window> window = nullptr;
    Ui* ui;
    LayerStack layerStack;
    f32 lastFrameTime = 0.0f;
  };
  
}