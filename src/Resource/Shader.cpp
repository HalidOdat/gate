#include <fstream>
#include <sstream>
#include <string>
#include <optional>

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Shader.hpp"

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

  Ref<Shader> Shader::create(const char* vFilepath, const char* fFilepath) noexcept {
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

  void Shader::setFloat(StringView name, const f32 value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform1f(uniform_id, value));
  }

  void Shader::setVec2(StringView name, const Vec2& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform2f(uniform_id, value.x, value.y));
  }

  void Shader::setVec3(StringView name, const Vec3& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform3f(uniform_id, value.x, value.y, value.z));
  }

  void Shader::setVec4(StringView name, const Vec4& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform4f(uniform_id, value.x, value.y, value.z, value.w));
  }

  void Shader::setMat2(StringView name, const Mat2& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniformMatrix2fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setMat3(StringView name, const Mat3& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniformMatrix3fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setMat4(StringView name, const Mat4& value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setInt(StringView name, const i32 value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform1i(uniform_id, value));
  }

  void Shader::setUint(StringView name, const u32 value) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform1ui(uniform_id, value));
  }

  void Shader::setIntArray(StringView name, const i32* value, u32 count) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform1iv(uniform_id, count, value));
  }

  void Shader::setUintArray(StringView name, const u32* value, u32 count) {
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(this->id, name.data()));
    GAME_GL_CHECK(glUniform1uiv(uniform_id, count, value));
  }

} // namespace Game