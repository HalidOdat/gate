#pragma once

#include "Resource/Resource.hpp"

namespace Game {

  class Texture2D : public Resource {
    friend class ResourceManager;

  public:
    void bind(const usize slot = 0) noexcept;

    u32 getId() const;
    u32 getWidth() const;
    u32 getHeight() const;

  public:
    struct Data {
      u32 id;
      u32 width;
      u32 height;
    };

  private:
    Texture2D(Resource::Id id)
      : Resource(id)
    {}

  private:
    const Data& getData() const;

    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, bool linear = true);
    static Data create(const StringView& filepath, bool linear = false, bool verticalFlip = true);

    static void destroy(Data& data);
  };

} // namespace Game

