#pragma once

#include "Core/Math.hpp"
#include "Resource/Resource.hpp"

namespace Game {

  // TODO: shader specification of definitions
  class Shader {
    friend class ResourceManager;
    
  public:
    enum class Type {
      Vertex = 0,
      Fragment,
      Compute,
    };

    using Handle = Resource<Shader>;
  
  public:
    static constexpr const u32 SHADER_TYPE_COUNT = 3;

  public:
    static Shader::Handle load(const StringView& filepath);
    DISALLOW_MOVE_AND_COPY(Shader);
    ~Shader();

    void bind() noexcept;
    void unbind() noexcept;

    bool reload();

    void setFloat(StringView name, const f32 value);
    void setVec2(StringView name,  const Vec2& value);
    void setVec3(StringView name,  const Vec3& value);
    void setVec4(StringView name,  const Vec4& value);
    void setMat2(StringView name,  const Mat2& value);
    void setMat3(StringView name,  const Mat3& value);
    void setMat4(StringView name,  const Mat4& value);
    void setInt(StringView name,   const i32 value);
    void setUint(StringView name,  const u32 value);
    void setIntArray(StringView name, const i32* value, u32 count);
    void setUintArray(StringView name, const u32* value, u32 count);
  
  private:
    static constexpr const u32 NULL_SHADER = 0;

  private:
    Shader(u32 id, Option<String> filePath = None)
      : id{id}, mFilePath{std::move(filePath)}
    {}

    [[nodiscard]] static u32 compile(Type type, const char* source) noexcept;

  private:
    u32 id;
    Option<String> mFilePath;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(Shader)

} // namespace Game
