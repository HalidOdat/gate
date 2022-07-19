#pragma once

#include "Core/Type.hpp"

namespace Game {

  class Texture2D {
  public:
    [[nodiscard]] static Ref<Texture2D> create(const StringView& filepath);
    [[nodiscard]] static Ref<Texture2D> fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4);
    DISALLOW_COPY_AND_ASSIGN(Texture2D);
    ~Texture2D() noexcept;

    void bind() noexcept;

    inline u32 getId() const { return this->id; }

  private:
    Texture2D(u32 id, u32 width, u32 height)
      : id{id}, width{width}, height{height}
    {}

  private:
    u32 id;
    u32 width;
    u32 height;
  };

} // namespace Game

