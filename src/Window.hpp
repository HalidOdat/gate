#pragma once

#include <functional>

#include "Core/Type.hpp"
#include "Events/Event.hpp"


// Forward declare
struct GLFWwindow;

namespace Game {

  class Window {
  public:
    using EventCallback = std::function<void(const Event&)>;

  public:
    static Ref<Window> create(const char* title, const u32 width, const u32 height);
    DISALLOW_MOVE_AND_COPY(Window);
    ~Window();

    bool shouldClose();
    void setShouldClose();
    void update();

    void setEventCallback(EventCallback callback);

    inline u32 getWidth()  const { return this->data.width; }
    inline u32 getHeight() const { return this->data.height; }

  private:
    struct Data {
      GLFWwindow* window;
      u32 width;
      u32 height;
      EventCallback eventCallback;
    };

  protected:
    Window(Data data)
      : data{ data }
    {}

  private:
    Data data;
  };

} // namespace Game
