#include <cstdlib>

#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Core/Type.hpp"

namespace Game {

  void CheckOpenGLError(const char* stmt, const char* fname, u32 line) {
    bool hadError = false;
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        Logger::error("OpenGL error %08x, at %s:%u - for %s\n", err, fname, line, stmt);
        hadError = true;
    }
    if (hadError) {
      std::exit(1);
    }
  }
  
} // namespace Game
