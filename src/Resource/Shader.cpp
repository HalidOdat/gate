#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Core/Assert.hpp"
#include "Resource/Shader.hpp"
#include "Utils/String.hpp"

namespace Game {

  static u32 shaderTypeToOpenGLType(Shader::Type type) {
    switch (type) {
      case Shader::Type::Vertex:   return GL_VERTEX_SHADER;
      case Shader::Type::Fragment: return GL_FRAGMENT_SHADER;
      case Shader::Type::Compute:  return GL_COMPUTE_SHADER;
    }

    GAME_UNREACHABLE("unknown shader type!");
    return 0;
  }

  static const char* shaderTypeToString(Shader::Type type) {
    switch (type) {
      case Shader::Type::Vertex:   return "Vertex";
      case Shader::Type::Fragment: return "Fragment";
      case Shader::Type::Compute:  return "Compute";
    }

    GAME_UNREACHABLE("unknown shader type!");
    return 0;
  }

  static std::optional<Shader::Type> stringToShaderType(const StringView& string) {
    if (Utils::stringIsEqualIgnoreCase(string, shaderTypeToString(Shader::Type::Vertex))) {
      return Shader::Type::Vertex;
    } else if (Utils::stringIsEqualIgnoreCase(string, shaderTypeToString(Shader::Type::Fragment))) {
      return Shader::Type::Fragment;
    } else if (Utils::stringIsEqualIgnoreCase(string, shaderTypeToString(Shader::Type::Compute))) {
      return Shader::Type::Compute;
    }

    return std::nullopt;
  }

  static std::vector<std::pair<Shader::Type, std::string>> parse(const StringView& filename) {
    const StringView typeDelimiter = "@type ";

    std::vector<std::pair<Shader::Type, std::string>> result;

    std::optional<std::string> common = std::nullopt;

    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      file.open(filename.data());
      std::stringstream stream;
      stream << file.rdbuf();
      file.close();

      std::string type;
      std::string content;
      for (std::string line; std::getline(stream, line); ) {
        if (line.starts_with(typeDelimiter)) {
          if (!common) {
            common = content + '\n';
          } else {
            auto shaderType = stringToShaderType(type);
            if (!shaderType) {
              return {};
            }

            if (std::any_of(result.begin(), result.end(), [shaderType](const auto& value){
              return value.first == shaderType;
            })) {
              Logger::error("Duplicate shader type '%s' in %s", shaderTypeToString(*shaderType), filename.data());
              return {};
            }
            
            result.emplace_back(std::make_pair(*shaderType, *common + content));
          }

          content.clear();

          type = line.substr(typeDelimiter.size());
          continue;
        }

        content += line;
        content += '\n';
      }

      auto shaderType = stringToShaderType(type);
      if (!shaderType) {
        return {};
      }

      if (std::any_of(result.begin(), result.end(), [shaderType](const auto& value){
        return value.first == shaderType;
      })) {
        Logger::error("Duplicate shader type '%s' in %s", shaderTypeToString(*shaderType), filename.data());
        return {};
      }

      result.emplace_back(std::make_pair(*shaderType, *common + content));
    } catch (std::ifstream::failure e) {
      Logger::error("Couldn't open file '%s': %s", filename.data(), e.what());
      return {};
    }

    if (!std::any_of(result.begin(), result.end(), [](const auto& value){
        return value.first == Shader::Type::Vertex;
    }) || !std::any_of(result.begin(), result.end(), [](const auto& value){
        return value.first == Shader::Type::Fragment;
    })) {
      Logger::error("Shader program '%s' must contain a vertex and fragment shaders", filename.data());
      return {};
    }

    // Logger::trace("-------------------------------------");
    // for (auto&[type, content] : result) {
    //   Logger::trace("Type: '%s'", shaderTypeToString(type));
    //   Logger::trace("Content: '%s'", content.c_str());
    // }
    // Logger::trace("-------------------------------------");

    return result;
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

  Option<Shader::Data> Shader::fromFile(const StringView& filepath) {
    auto content = parse(filepath);
    if (content.empty()) {
      return None;
    }

    std::array<u32, SHADER_TYPE_COUNT> shaders = {0, 0, 0};
    for (auto&[type, content] : content) {
      u32 id = Shader::compile(type, content.c_str());;

      shaders[(usize)type] = id;

      if (id == 0) {
        for (auto shader : shaders) {
          if (shader != 0) {
            GAME_GL_CHECK(glDeleteShader(shader));
          }
        }
        return None;
      }
    }

    // link shaders
    u32 shaderProgram;
    GAME_GL_CHECK(shaderProgram = glCreateProgram());

    for (auto shader : shaders) {
      if (shader != 0) {
        GAME_GL_CHECK(glAttachShader(shaderProgram, shader));
      }
    }

    GAME_GL_CHECK(glLinkProgram(shaderProgram));

    // check for linking errors
    int success;
    char infoLog[512];
    GAME_GL_CHECK(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));

    if (!success) {
        GAME_GL_CHECK(glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog));
        Logger::error("Shader program linking failed:\n%s\n", infoLog);
    }

    for (auto shader : shaders) {
      if (shader != 0) {
        GAME_GL_CHECK(glDetachShader(shaderProgram, shader));
        GAME_GL_CHECK(glDeleteShader(shader));
      }
    }

    if (!success) {
      glDeleteProgram(shaderProgram);
      return None;
    }

    return Data{ shaderProgram, String(filepath) };
  }

  Shader::~Shader() {
    GAME_GL_CHECK(glDeleteProgram(mData.id));
  }

  bool Shader::reload() {
    if (!mData.filePath.has_value()) {
      return false;
    }

    auto data = Shader::fromFile(*mData.filePath);
    if (data) {
      Logger::trace("Reloaded shader: %s", mData.filePath.value().c_str());

      GAME_GL_CHECK(glDeleteProgram(mData.id));
      mData = *data;
      return true;
    }

    return false;
  }

  void Shader::bind() noexcept {
    u32 programId = mData.id;
    GAME_GL_CHECK(glUseProgram(programId));
  }

  void Shader::unbind() noexcept {
    GAME_GL_CHECK(glUseProgram(NULL_SHADER));
  }

  void Shader::setFloat(StringView name, const f32 value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform1f(uniform_id, value));
  }

  void Shader::setVec2(StringView name, const Vec2& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform2f(uniform_id, value.x, value.y));
  }

  void Shader::setVec3(StringView name, const Vec3& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform3f(uniform_id, value.x, value.y, value.z));
  }

  void Shader::setVec4(StringView name, const Vec4& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform4f(uniform_id, value.x, value.y, value.z, value.w));
  }

  void Shader::setMat2(StringView name, const Mat2& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniformMatrix2fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setMat3(StringView name, const Mat3& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniformMatrix3fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setMat4(StringView name, const Mat4& value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value[0][0]));
  }

  void Shader::setInt(StringView name, const i32 value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform1i(uniform_id, value));
  }

  void Shader::setUint(StringView name, const u32 value) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform1ui(uniform_id, value));
  }

  void Shader::setIntArray(StringView name, const i32* value, u32 count) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform1iv(uniform_id, count, value));
  }

  void Shader::setUintArray(StringView name, const u32* value, u32 count) {
    u32 programId = mData.id;
    u32 uniform_id;
    GAME_GL_CHECK(uniform_id = glGetUniformLocation(programId, name.data()));
    GAME_GL_CHECK(glUniform1uiv(uniform_id, count, value));
  }

} // namespace Game