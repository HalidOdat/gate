#pragma once

#include "Core/Math.hpp"
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

    enum class DataType : u8 {
      UnsignedByte,
      Float,
      HalfFloat,
      UnsignedInt_24_8,
    };

    enum class DataFormat : u8 {
      Red,
      Rg,
      Rgb,
      Bgr,
      Rgba,
      Bgra,

      DepthStencil,
    };

    enum class Format : u8 {
      Rgb8,
      Rgba8,
      Srgb8,
      Srgb8Alpha8,
      Rgb32F,
      Rgba32F,
      Rgb16F,
      Rgba16F,
      R11FG11FB10F,

      Depth24Stencil8,

      DefaultDepthStencil = Depth24Stencil8,
    };

    enum class Type : u8 {
      Image,
      Color,
      Buffer,
    };

  private:
    Texture() = delete;
  };

  class Texture2D {
  public:
    using Handle = Resource<Texture2D>;

    struct Specification {
      Specification() {}

      Texture::Type           type           = Texture::Type::Color;
      Texture::Format         internalFormat = Texture::Format::Rgba8;
      Texture::WrappingMode   wrapping       = Texture::WrappingMode::Repeat;
      Texture::Filtering      filtering      = Texture::FilteringMode::Linear;
      Texture::MipmapMode     mipmap         = Texture::MipmapMode::Linear;
      bool                    verticalFlip   = true;
      bool                    gammaCorrected = true;
      u32                     samples        = 0;
    };

    class Builder {
    public:
      Builder& format(Texture::Format internalFormat);
      Builder& wrapping(Texture::WrappingMode mode);
      Builder& filtering(Texture::Filtering filtering);
      Builder& mipmap(Texture::MipmapMode mode);
      Builder& verticalFlipOnLoad(bool yes = true);
      Builder& gammaCorrected(bool yes = true);
      Builder& samples(u32 inSamples);
      Texture2D::Handle build();

    private:
      Builder() = default;

    private:
      Texture::Type mType = Texture::Type::Color;

      u32 mWidth = 1;
      u32 mHeight = 1;
      Specification mSpecification;

      u32 mColor = 0xFFFFFFFF;

      StringView mFile;

      const void*         mData = nullptr;
      Texture::DataFormat mDataFormat = Texture::DataFormat::Rgba;
      Texture::DataType   mDataType = Texture::DataType::UnsignedByte;

    private:
      friend class Texture2D;
    };

  public:
    [[nodiscard]] static Texture2D::Builder builder();
    [[nodiscard]] static Texture2D::Builder load(const String& filepath, bool verticalFlipOnLoad = true);
    [[nodiscard]] static Texture2D::Builder buffer(u32 width, u32 height);
    [[nodiscard]] static Texture2D::Builder buffer(const void* inData, u32 width, u32 height, Texture::DataFormat dataFormat, Texture::DataType dataType);
    [[nodiscard]] static Texture2D::Builder color(u32 color);
    [[nodiscard]] static Texture2D::Builder color(u8 r, u8 g, u8 b, u8 a = 0xFF);
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

