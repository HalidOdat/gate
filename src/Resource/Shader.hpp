#pragma once

#include "Core/Math.hpp"
#include "Resource/Resource.hpp"

namespace Game {

  class Shader : public Resource {
    friend class ResourceManager;
    
  public:
    enum class Type {
      Vertex = 0,
      Fragment,
      Compute,
    };
  
  public:
    static constexpr const u32 SHADER_TYPE_COUNT = 3;

  public:
    void bind() noexcept;
    void unbind() noexcept;

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

  public:
    struct Data {
      u32 id;
    };
  
  private:
    static constexpr const u32 NULL_SHADER = 0;

  private:
    Shader(Resource::Id id)
      : Resource{id}
    {}

    static u32  compile(Type type, const char* source) noexcept;
    static Data create(const StringView& filepath);
    static void destroy(Data& data);

    const Data& getData() const;
  };

} // namespace Game
