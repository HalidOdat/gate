#include "Core/OpenGL.hpp"

#include "Core/Base.hpp"
#include "Events/Event.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/FileDropEvent.hpp"
#include "Window.hpp"

namespace Gate {

  static void GLFWErrorCallback(int error, const char* description) {
    Logger::error("GLFW Error (%d): %s", error, description);
  }

  void Window::initializeWindowSystem() {
    glfwSetErrorCallback(GLFWErrorCallback);

    Logger::info("GLFW Version: %s", glfwGetVersionString());
    if (!glfwInit()) {
      std::exit(EXIT_FAILURE);
    } else {
      Logger::trace("GLFW initialized!");
    }
  }

  void Window::deinitializeWindowSystem() {
    glfwTerminate();
    Logger::trace("GLFW Terminated!");
  }

#ifndef GATE_PLATFORM_WEB
  static void APIENTRY openGLMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
  ) {
    (void)id;
    (void)length;
    (void)userParam;

    const char* sourceString;
    switch (source) {
      case GL_DEBUG_SOURCE_API:             sourceString = "API"; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceString = "Window System"; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceString = "Shader Compiler"; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceString = "Third Party"; break;
      case GL_DEBUG_SOURCE_APPLICATION:     sourceString = "Application"; break;
      case GL_DEBUG_SOURCE_OTHER:           sourceString = "Other"; break;
      default:
        GATE_UNREACHABLE("");
    }

    const char* typeString;
    switch (type) {
      case GL_DEBUG_TYPE_ERROR:               typeString = "Error"; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "Deprecated Behaviour"; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeString = "Undefined Behaviour"; break;
      case GL_DEBUG_TYPE_PORTABILITY:         typeString = "Portability"; break;
      case GL_DEBUG_TYPE_PERFORMANCE:         typeString = "Performance"; break;
      case GL_DEBUG_TYPE_MARKER:              typeString = "Marker"; break;
      case GL_DEBUG_TYPE_PUSH_GROUP:          typeString = "Push Group"; break;
      case GL_DEBUG_TYPE_POP_GROUP:           typeString = "Pop Group"; break;
      case GL_DEBUG_TYPE_OTHER:               typeString = "Other"; break;
      default:
        GATE_UNREACHABLE("");
    }
    
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
          Logger::error("OpenGL: Source = %s, Type = %s, Message = %s", sourceString, typeString, message);
          break;
        case GL_DEBUG_SEVERITY_MEDIUM:
          Logger::warn("OpenGL: Source = %s, Type = %s, Message = %s", sourceString, typeString, message);
          break;
        case GL_DEBUG_SEVERITY_LOW:
          Logger::info("OpenGL: Source = %s, Type = %s, Message = %s", sourceString, typeString, message);
          break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
          Logger::trace("OpenGL: Source = %s, Type = %s, Message = %s", sourceString, typeString, message);
          break;
        default:
          GATE_UNREACHABLE("");
    }
  }
#endif

  Ref<Window> Window::create(const char* title, u32 width, u32 height) {
    initializeWindowSystem();

    Logger::trace("Creating window...");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    #if defined(GATE_DEBUG_MODE) && !defined(GATE_PLATFORM_WEB)
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    #endif

    Data data;
    data.window = nullptr;
    data.width = width;
    data.height = height;
    data.eventCallback = [](auto&) {};

    data.window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!data.window) {
      return nullptr;
    }
    auto result = Ref<Window>( new Window(data) );

    glfwMakeContextCurrent(result->data.window);

    // We dont need to do this, for web since ems
    #ifndef GATE_PLATFORM_WEB
      if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
      {
        Logger::error("Glad: Failed to initialize");
        return nullptr;
      }
    #endif

    Logger::info("OpenGL Version: %s", glGetString(GL_VERSION));

    #if defined(GATE_DEBUG_MODE) && !defined(GATE_PLATFORM_WEB)
      i32 flags;
      glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
      if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        Logger::info("Glfw: Debug context initialized!");
      } else {
        Logger::warn("Glfw: Failed to initialize the debug context");
      }
    #endif

    glfwSetWindowUserPointer(result->data.window, &result->data);

    glfwSetWindowSizeCallback(result->data.window, [](GLFWwindow* window, int width, int height) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      if (width == 0 && height == 0) { // window minimized
        WindowMinimizedEvent event;
        data.eventCallback(event);
        return;
      }

      data.width  = width;
      data.height = height;
      WindowResizeEvent event(width, height);
      data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(result->data.window, [](GLFWwindow* window) {
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      WindowCloseEvent event;
      data.eventCallback(event);
    });

    glfwSetKeyCallback(result->data.window, [](GLFWwindow* window, int keyCode, int scancode, int action, int mods) {
      (void)scancode;

      auto& data = *(Data*)glfwGetWindowUserPointer(window);

      const auto key      = (Key)keyCode;
      const auto modifier = (KeyModifier)mods;
      switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, modifier, false);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, modifier, true);
					data.eventCallback(event);
					break;
				}
			}
    });

    glfwSetMouseButtonCallback(result->data.window, [](GLFWwindow* window, int button, int action, int mods) {
      (void)mods;

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
      MouseMoveEvent event(static_cast<f32>(x), static_cast<f32>(y));
      data.eventCallback(event);
    });

    glfwSetDropCallback(result->data.window, [](GLFWwindow* window, int count, const char** paths) {
      (void) window;
      if (count <= 0) {
        return;
      }
      std::vector<String> result;
      for (int i = 0; i < count; ++i) {
        result.emplace_back(paths[i]);
      }
      auto& data = *(Data*)glfwGetWindowUserPointer(window);
      FileDropEvent event(result);
      data.eventCallback(event);
    });

    // Center window, if possible.
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor) {
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      if (mode) {
        i32 x = mode->width  / 2 - width  / 2;
        i32 y = mode->height / 2 - height / 2;
        glfwSetWindowPos(
          result->data.window,
          x >= 0 ? x : 0,
          y >= 0 ? y : 0
        );
      }
    }

    return result;
  }

  void Window::setSize(u32 width, u32 height) {
    glfwSetWindowSize(this->data.window, width, height);
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

  void Window::setTitle(const String& title) {
    glfwSetWindowTitle(data.window, title.c_str());
  }

  void Window::setEventCallback(EventCallback callback) {
    this->data.eventCallback = callback;
  }

  bool Window::isKeyPressed(Key key) const {
    auto state = glfwGetKey(data.window, (int)key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }

  void Window::enableCursor(bool yes) {
    if (yes) {
      glfwSetInputMode(this->data.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(this->data.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
  }
}

