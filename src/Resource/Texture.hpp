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

    enum class Type {
      Image,
      Color,
    };

  private:
    Texture() = delete;
  };

  class Texture2D {
  public:
    using Handle = Resource<Texture2D>;

    struct Specification {
      Specification() {}
      Texture::WrappingMode wrapping       = Texture::WrappingMode::Repeat;
      Texture::Filtering    filtering      = Texture::FilteringMode::Linear;
      Texture::MipmapMode   mipmap         = Texture::MipmapMode::Linear;
      Texture::VerticalFlip verticalFlip   = Texture::VerticalFlip::True;
      bool                  gammaCorrected = false;
    };

  public:
    [[nodiscard]] static Texture2D::Handle load(const String& filepath, Specification specification = {});
    [[nodiscard]] static Texture2D::Handle color(u32 color);
    [[nodiscard]] static Texture2D::Handle color(u8 r, u8 g, u8 b, u8 a = 0xFF);
    [[nodiscard]] static Texture2D::Handle generateMissingDataPlaceholder();
    DISALLOW_MOVE_AND_COPY(Texture2D);
    ~Texture2D();

    void bind(const usize slot = 0) const;

    u32 getId() const;
    u32 getWidth() const;
    u32 getHeight() const;
  
    bool reload();

    inline const Specification&  getSpecification() const { return mData.specification; }
    inline const Option<String>& getFilePath() const { return mData.filePath; }
    inline Texture::Type getType() const { return mData.type; }
    inline u32 getColor() const { return mData.color; }

    static void reloadAll();

  private:
    struct Data {
      u32 id;
      u32 width;
      u32 height;

      Specification specification;
      Texture::Type type;
      Option<String> filePath;
      u32 color;
    };

    Texture2D(Data data)
      : mData{std::move(data)}
    {}

    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Specification specification = {});

    static void destroyAllTextures();

  private:
    Data mData;

  private:
    template<typename T>
    friend class ResourceFactory;
    friend class ResourceManager;
  };

  GAME_FACTORY_HEADER(Texture2D)

  class CubeMap {
  public:
    using Handle = Resource<CubeMap>;
    using FilePaths = Array<String, 6>;

  public:
    static CubeMap::Handle load(FilePaths paths);
    DISALLOW_MOVE_AND_COPY(CubeMap);
    ~CubeMap();

    void bind() const;

    u32 getId() const;
    bool reload();

    inline const FilePaths& getFilePath() const { return filePaths; }

  private:
    CubeMap(u32 id, FilePaths filePaths)
      : id{id}
      , filePaths{std::move(filePaths)}
    {}

  private:
    u32 id;
    FilePaths filePaths;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(CubeMap)

} // namespace Game

