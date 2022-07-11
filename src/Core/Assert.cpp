#include <cstdlib>

#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Core/Type.hpp"

namespace Game {

  static_assert(sizeof(GLuint) == sizeof(u32), "GLuints should be unsigned 32-bit numbers");
  static_assert(sizeof(GLint)  == sizeof(i32), "GLuints should be singned 32-bit numbers");

  void CheckOpenGLError(const char* stmt, const char* fname, int line) {
    bool hadError = false;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        Logger::error("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
        hadError = true;
    }
    if (hadError) {
      std::exit(1);
    }
  }

} // namespace Game
