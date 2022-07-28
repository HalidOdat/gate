#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Events/Event.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Window.hpp"

namespace Game {

  static void GLFWErrorCallback(int error, const char* description) {
    Logger::error("GLFW Error (%d): %s", error, description);
  }

  u32 Window::windowCount = 0;

  void Window::initializeWindowSystem() {
    if (windowCount == 0) {
      glfwSetErrorCallback(GLFWErrorCallback);

      Logger::info("GLFW Version: %s", glfwGetVersionString());
      if (!glfwInit()) {
        std::exit(EXIT_FAILURE);
      } else {
        Logger::trace("GLFW initialized!");
      }
    }
    
    windowCount++;
  }

  void Window::deinitializeWindowSystem() {
    GAME_ASSERT_WITH_MESSAGE(windowCount >= 1, "Should not call deinit before init");
    windowCount--;
    if (windowCount == 0) {
      glfwTerminate();
      Logger::trace("GLFW Terminated!");
    }    
  }


  Ref<Window> Window::create(const char* title, u32 width, u32 height) {
    initializeWindowSystem();

    Logger::trace("Creating window...");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    Data data;
    data.window = nullptr;
    data.width = width;
    data.height = height;
    data.eventCallback = [](auto& e) {};

    data.window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!data.window) {
      return nullptr;
    }
    auto result = Ref<Window>( new Window(data) );

    glfwMakeContextCurrent(result->data.window);
    GAME_ASSERT(glfwGetError(NULL) == 0);

    if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
    {
      Logger::error("Glad failed");
      return nullptr;
    }

    Logger::info("OpenGL Version: %s", glGetString(GL_VERSION));

    glfwSwapInterval(1);

    glfwSetWindowUserPointer(result->data.window, &result->data);

    glfwSetWindowSizeCallback(result->data.window, [](GLFWwindow* window, int width, int height) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      WindowResizeEvent event(width, height);
      data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(result->data.window, [](GLFWwindow* window) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      WindowCloseEvent event;
      data.eventCallback(event);
    });

    glfwSetKeyCallback(result->data.window, [](GLFWwindow* window, int keyCode, int scancode, int action, int mods) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);

      const auto key = (Key)keyCode;
      switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, false);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, true);
					data.eventCallback(event);
					break;
				}
			}
    });

    glfwSetMouseButtonCallback(result->data.window, [](GLFWwindow* window, int button, int action, int mods) {
			auto& data = *(Data*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event((MouseButton)button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event((MouseButton)button);
					data.eventCallback(event);
					break;
				}
			}
		});

    glfwSetScrollCallback(result->data.window, [](GLFWwindow* window, double xOffset, double yOffset) {
			auto& data = *(Data*)glfwGetWindowUserPointer(window);

			MouseScrollEvent event(static_cast<f32>(xOffset), static_cast<f32>(yOffset));
			data.eventCallback(event);
		});

    glfwSetCursorPosCallback(result->data.window, [](GLFWwindow* window, double x, double y) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      MouseMoveEvent event(
        static_cast<f32>(x) / (data.width / 2.0f) - 1,
        1 - static_cast<f32>(y) / (data.height / 2.0f)
      );
      data.eventCallback(event);
    });
    return result;
  }

  bool Window::shouldClose() {
    return (bool)glfwWindowShouldClose(this->data.window);
  }

  void Window::update() {
    glfwPollEvents();
    glfwSwapBuffers(this->data.window);
  }

  Window::~Window() {
    glfwDestroyWindow(this->data.window);
    deinitializeWindowSystem();
  }

  void Window::setShouldClose() {
    glfwSetWindowShouldClose(this->data.window, true);
  }

  void Window::setVSync(bool enable) {
    if (enable) {
      glfwSwapInterval(1);
    } else {
      glfwSwapInterval(0);
    }
  }

  void Window::setEventCallback(EventCallback callback) {
    this->data.eventCallback = callback;
  }

  bool Window::isKeyPressed(Key key) const {
    auto state = glfwGetKey(data.window, (int)key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }
}