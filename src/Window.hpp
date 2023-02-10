#pragma once

#include <functional>

#include "Core/Type.hpp"
#include "Core/KeyCode.hpp"
#include "Events/Event.hpp"

// Forward declare
struct GLFWwindow;

namespace Gate {

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

    void setTitle(const String& title);

    void setSize(u32 width, u32 height);
    inline u32 getWidth()  const { return this->data.width; }
    inline u32 getHeight() const { return this->data.height; }
    inline f32 getAspectRatio() const { return (f32)this->data.width / (f32)this->data.height; }

    bool isKeyPressed(Key key) const;

    void enableCursor(bool yes = true);

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

    static void initializeWindowSystem();
    static void deinitializeWindowSystem();

  private:
    Data data;
  };

} // namespace Gate
