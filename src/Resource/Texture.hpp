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

    enum class Source {
      Memory,
      File,
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
  
    inline const Specification& getSpecification() const { return mData.specification; }

    bool reload();


    inline const Option<String> getFilePath() const { return mData.filePath; }

  public:
    static constexpr const bool hasMissingDataPlaceholder = true;

    struct Data {
      u32 id;
      u32 width;
      u32 height;

      Specification specification;
      Option<String> filePath;
    };

    Texture2D(Data data)
      : mData{data}
    {}


  private:
    static Option<Data> fromFile(const StringView& filepath, Specification specification = {});
    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Specification specification = {});
    static Data generateMissingDataPlaceholder();

  private:
    Data mData;

    friend class ResourceManager;

    template<typename T>
    friend class ResourceFactory;
  };

  class CubeMap {
  public:
    using Handle = Resource<CubeMap>;

    using FilePaths = Array<String, 6>;

  public:
    DISALLOW_MOVE_AND_COPY(CubeMap);
    ~CubeMap();

    void bind() const;

    u32 getId() const;
    bool reload();

    inline const FilePaths& getFilePath() const { return mData.filePaths; }

  public:
    struct Data {
      u32 id;
      FilePaths filePaths;
    };

    CubeMap(Data data)
      : mData{data}
    {}


  private:
    static Option<Data> fromFile(FilePaths paths);
    
  private:
    Data mData;

    friend class ResourceManager;

    template<typename T>
    friend class ResourceFactory;
  };

} // namespace Game

