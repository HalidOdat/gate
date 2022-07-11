#pragma once

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
    static [[nodiscard]] Ref<Shader> Create(const char* vFilepath, const char* fFilepath) noexcept;

    DISALLOW_MOVE_AND_COPY(Shader);
    ~Shader() noexcept;

    void bind() noexcept;
    void unbind() noexcept;

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
