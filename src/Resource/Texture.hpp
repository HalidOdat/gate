#pragma once

#include "Resource/Resource.hpp"

namespace Game {

  class Texture {
  public:
    enum class WrappingMode : u8 {
      Repeat,
      MirroredRepeat,
      ClampToEdge,
      ClampToBorder,
    };

    enum class FilteringMode : u8 {
      Linear,
      Nearest,
    };

    enum class MipmapMode : u8 {
      None,
      Nearest,
      Linear,
    };

    enum class VerticalFlip : u8 {
      False = 0,
      True  = 1,
    };

    struct Filtering {
      Filtering(FilteringMode filtering = Texture::FilteringMode::Linear)
        : min{filtering}, mag{filtering}
      {}

      Filtering(FilteringMode min, FilteringMode mag)
        : min{min}, mag{mag}
      {}

      Texture::FilteringMode min;
      Texture::FilteringMode mag;
    };

  private:
    Texture() = delete;
  };

  class Texture2D {
  public:
    using Handle = Resource<Texture2D>;

    struct Specification {
      Texture::WrappingMode wrapping     = Texture::WrappingMode::Repeat;
      Texture::Filtering    filtering    = Texture::FilteringMode::Linear;
      Texture::MipmapMode   mipmap       = Texture::MipmapMode::Linear;
      Texture::VerticalFlip verticalFlip = Texture::VerticalFlip::True;
    };

  public:
    DISALLOW_MOVE_AND_COPY(Texture2D);
    ~Texture2D();

    void bind(const usize slot = 0) const;

    u32 getId() const;
    u32 getWidth() const;
    u32 getHeight() const;
  
    const Specification& getSpecification() const { return mData.specification; }

  public:
    struct Data {
      u32 id;
      u32 width;
      u32 height;

      Specification specification;
    };

    Texture2D(Data data)
      : mData{data}
    {}

  private:
    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Specification specification = {});
    static Data create(const StringView& filepath, Specification specification = {});

  private:
    Data mData;

    friend class ResourceManager;
  };

} // namespace Game

