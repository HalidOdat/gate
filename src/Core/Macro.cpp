#include <cstdlib>

#include <glad/glad.h>

#include "Core/Macro.hpp"
#include "Core/Log.hpp"
#include "Core/Type.hpp"

namespace Gate {

  static_assert(sizeof(GLuint) == sizeof(u32), "GLuints should be unsigned 32-bit numbers");
  static_assert(sizeof(GLint)  == sizeof(i32), "GLuints should be singned 32-bit numbers");

  const char* glErrorToString(GLenum value) {
    switch (value) {
      case GL_NO_ERROR:                      return "no error";
      case GL_INVALID_ENUM:                  return "invalid enum";
      case GL_INVALID_VALUE:                 return "invalid value";
      case GL_INVALID_OPERATION:             return "invalid Operation";
      case GL_INVALID_FRAMEBUFFER_OPERATION: return "invalid framebuffer operation";
      case GL_OUT_OF_MEMORY:                 return "out of memory";
      default: 
        GATE_UNREACHABLE("unknown OpengGL error type");
    }
  }

} // namespace Gate
