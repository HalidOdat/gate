#pragma once

#include "Window.hpp"
#include "Core/KeyCode.hpp"
#include "Events/WindowEvent.hpp"

#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Editor/EditorLayer.hpp"

namespace Gate {

  class Application {
  public:
    Application(StringView title, u32 width, u32 height);
    DISALLOW_MOVE_AND_COPY(Application);
    ~Application();

    inline static Application& get() { return *sInstance; }
    inline static Window& getWindow() { return *sInstance->window; }
    inline static Renderer2D& getRenderer2D() { return *sInstance->mRenderer2D; }
    static Renderer3D& getRenderer3D();

    static void saveFile(const String& name, const String& content);
    static void renameChip();

    static void renameChip(const String& name);

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
    
    Renderer2D* mRenderer2D = nullptr;
    Renderer3D* mRenderer3D = nullptr;
    EditorLayer *layer = nullptr;
    f32 lastFrameTime = 0.0f;

    String mTitle;
  };
  
}
