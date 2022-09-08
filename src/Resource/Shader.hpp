#pragma once

#include "Core/Math.hpp"
#include "Resource/Resource.hpp"

#include <unordered_map>

namespace Game {

  // TODO: shader specification of definitions
  class Shader {
    friend class ResourceManager;
    
  public:
    enum class Type {
      Vertex = 0,
      Fragment,
      Geometry,
      Compute,
    };

    enum Version {
      Es300,
      Core450, 
    };

    using Handle = Resource<Shader>;

    class Builder {
    public:
      Builder& version(Shader::Version version);
      Builder& define(String name, String content = "");
      Shader::Handle build();

    private:
      Builder() = default;

    private:
      StringView mFilePath;
      Version mVersion = Version::Es300;
      std::unordered_map<String, String> mDefinitions;

      friend class Shader;
    };
  
  public:
    static constexpr const u32 SHADER_TYPE_COUNT = 4;

  public:
    static bool globalDefine(String name, String content);

    static Shader::Builder load(const StringView& filepath);
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

    i32 getUniformLocation(StringView string);

  private:
    static std::unordered_map<String, String> sGlobalDefines;

  private:
    u32 id;
    Option<String> mFilePath;
    std::unordered_map<StringView, i32> mUniformLocations;

  private:
    friend class Builder;
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(Shader)

} // namespace Game
