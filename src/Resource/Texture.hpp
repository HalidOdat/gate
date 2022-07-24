#pragma once

#include "Resource/Resource.hpp"

namespace Game {

  class Texture2D : public Resource {
    friend class ResourceManager;

  public:
    void bind(const usize slot = 0) noexcept;

    inline u32 getId() const;
    inline u32 getWidth() const;
    inline u32 getHeight() const;

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

    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4);
    static Data create(const StringView& filepath);

    static void destroy(Data& data);
  };

} // namespace Game

