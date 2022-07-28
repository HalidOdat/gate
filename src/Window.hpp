#pragma once

#include <functional>

#include "Core/Type.hpp"
#include "Events/Event.hpp"


// Forward declare
struct GLFWwindow;

namespace Game {

  // TODO: put events in a queue
  class Window final {
  public:
    using EventCallback = std::function<void(const Event&)>;

  public:
    static Ref<Window> create(const char* title, const u32 width, const u32 height);
    DISALLOW_MOVE_AND_COPY(Window);
    ~Window();

    bool shouldClose();
    void setShouldClose();
    void update();

    void setVSync(bool enable);
    void setEventCallback(EventCallback callback);

    inline u32 getWidth()  const { return this->data.width; }
    inline u32 getHeight() const { return this->data.height; }
    inline f32 getAspectRatio() const { return (f32)this->data.width / (f32)this->data.height; }

    bool isKeyPressed(Key key) const;

  private:
    struct Data {
      GLFWwindow* window;
      u32 width;
      u32 height;
      EventCallback eventCallback;
    };

  private:
    Window(Data data)
      : data{ data }
    {}

    static u32 windowCount;
    static void initializeWindowSystem();
    static void deinitializeWindowSystem();

  private:
    Data data;
  };

} // namespace Game
