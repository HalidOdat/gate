#pragma once

#include "Core/Base.hpp"

#ifdef GATE_PLATFORM_WEB
# include <emscripten/emscripten.h>
# define GL_GLEXT_PROTOTYPES
# define EGL_EGLEXT_PROTOTYPES
#else
# include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
