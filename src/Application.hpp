#pragma once

#include "Ui/Ui.hpp"
#include "Window.hpp"
#include "Core/KeyCode.hpp"
#include "Events/WindowEvent.hpp"

#include "Editor/EditorLayer.hpp"

namespace Gate {

  class Application {
  public:
    Application(StringView title, u32 width, u32 height);
    DISALLOW_MOVE_AND_COPY(Application);
    ~Application();

    inline static Application& get() { return *sInstance; }
    inline static Window& getWindow() { return *sInstance->window; }
    inline static Renderer& getRenderer() { return *sInstance->mRenderer; }

    void start();
    void quit();
  private:
    void onEvent(const Event& event);
    bool onWindowCloseEvent(const WindowCloseEvent& event);
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onWindowMinimizedEvent(const WindowMinimizedEvent& event);

    void startGameLoop();
    static void gameLoop();

  private:
    static Application* sInstance;

  public:
    bool running = true;
    bool mWindowMinimized = false;
    Ref<Window> window = nullptr;
    
    Renderer* mRenderer;
    Ui* ui;
    EditorLayer *layer;
    f32 lastFrameTime = 0.0f;

    String mTitle;
  };
  
}
