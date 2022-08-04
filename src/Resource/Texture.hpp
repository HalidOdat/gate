#pragma once

#include "Resource/Resource.hpp"

namespace Game {

  class Texture2D {
  public:
    DISALLOW_MOVE_AND_COPY(Texture2D);
    ~Texture2D();

    void bind(const usize slot = 0) const;

    u32 getId() const;
    u32 getWidth() const;
    u32 getHeight() const;

  public:
    struct Data {
      u32 id;
      u32 width;
      u32 height;
    };

    Texture2D(Data data)
      : mData{data}
    {}

  private:
    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, bool linear = true);
    static Data create(const StringView& filepath, bool linear = false, bool verticalFlip = true);

  private:
    Data mData;

    friend class ResourceManager;
  };

} // namespace Game

