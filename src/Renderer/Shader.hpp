#pragma once

#include <glm/glm.hpp>

#include "Core/Type.hpp"

namespace Game {
  
  class Shader {
  public:
    enum class Type {
      Vertex,
      Fragment,
    };
  
  public:
    static constexpr const u32 NULL_SHADER = 0;

  public:
    [[nodiscard]] static Ref<Shader> Create(const char* vFilepath, const char* fFilepath) noexcept;

    DISALLOW_MOVE_AND_COPY(Shader);
    ~Shader() noexcept;

    void bind() noexcept;
    void unbind() noexcept;

    void setFloat(StringView name, const f32 value);
    void setVec2(StringView name,  const glm::vec2& value);
    void setVec3(StringView name,  const glm::vec3& value);
    void setVec4(StringView name,  const glm::vec4& value);
    void setMat2(StringView name,  const glm::mat2& value);
    void setMat3(StringView name,  const glm::mat3& value);
    void setMat4(StringView name,  const glm::mat4& value);
    void setInt(StringView name,   const i32 value);

  protected:
    Shader(u32 id)
      : id{id}
    {}

  private:
      static u32 compile(Type type, const char* source) noexcept;
  private:
    u32 id;

    const char* filepath;
  };

} // namespace Game
