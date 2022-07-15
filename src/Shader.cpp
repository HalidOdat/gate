#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Shader.hpp"

namespace Game {

  static u32 shaderTypeToOpenGLType(Shader::Type type) {
    switch (type) {
      case Shader::Type::Vertex:   return GL_VERTEX_SHADER;
      case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
    }

    GAME_UNREACHABLE("unknown shader type!");
    return 0;
  }

  static const char* shaderTypeToString(Shader::Type type) {
    switch (type) {
      case Shader::Type::Vertex:   return "Vertex";
      case Shader::Type::Fragment: return "Fragment";
    }

    GAME_UNREACHABLE("unknown shader type!");
    return 0;
  }

  std::optional<std::string> fileToString(const char* filepath) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      file.open(filepath);
      std::stringstream stream;
      stream << file.rdbuf();
      file.close();
      return stream.str();
    } catch (std::ifstream::failure e) {
      Logger::error("Couldn't load file '%s': %s", filepath, e.what());
      return std::nullopt;
    }
  }

  u32 Shader::compile(Type type, const char* source) noexcept {
    u32 id;
    GAME_GL_CHECK(id = glCreateShader(shaderTypeToOpenGLType(type)));
    GAME_GL_CHECK(glShaderSource(id, 1, &source, NULL));
    GAME_GL_CHECK(glCompileShader(id));

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        Logger::error("%s Shader Compilation:\n%s\n", shaderTypeToString(type), infoLog);
        return NULL_SHADER;
    }

    return id;
  }

  Ref<Shader> Shader::Create(const char* vFilepath, const char* fFilepath) noexcept {
    const auto vertexShaderSourceString = fileToString(vFilepath);
    if (!vertexShaderSourceString) {
      return nullptr;
    }
    u32 vertexShader = Shader::compile(Type::Vertex, vertexShaderSourceString->c_str());
    if (!vertexShader) {
      return nullptr;
    }

    // fragment shader
    const auto fragmentShaderSourceString = fileToString(fFilepath);
    if (!fragmentShaderSourceString) {
      GAME_GL_CHECK(glDeleteShader(vertexShader));
      return nullptr;
    }
    u32 fragmentShader = Shader::compile(Type::Fragment, fragmentShaderSourceString->c_str());
    if (!fragmentShader) {
      return nullptr;
    }

    // link shaders
    u32 shaderProgram;
    GAME_GL_CHECK(shaderProgram = glCreateProgram());
    GAME_GL_CHECK(glAttachShader(shaderProgram, vertexShader));
    GAME_GL_CHECK(glAttachShader(shaderProgram, fragmentShader));
    GAME_GL_CHECK(glLinkProgram(shaderProgram));

    // check for linking errors
    int success;
    char infoLog[512];
    GAME_GL_CHECK(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));

    if (!success) {
        GAME_GL_CHECK(glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog));
        Logger::error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    GAME_GL_CHECK(glDeleteShader(vertexShader));
    GAME_GL_CHECK(glDeleteShader(fragmentShader));

    if (!success) {
      glDeleteProgram(shaderProgram);
      return nullptr;
    }

    return Ref<Shader>( new Shader(shaderProgram));
  }

  Shader::~Shader() noexcept {
    GAME_GL_CHECK(glDeleteProgram(this->id));
  }

  void Shader::bind() noexcept {
    GAME_GL_CHECK(glUseProgram(this->id));
  }

  void Shader::unbind() noexcept {
    GAME_GL_CHECK(glUseProgram(NULL_SHADER));
  }

} // namespace Game