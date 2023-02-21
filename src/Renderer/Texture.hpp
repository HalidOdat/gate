#pragma once

#include "Core/Base.hpp"

namespace Gate {

  class Texture {
    friend class Application;
  public:
    using Handle = std::shared_ptr<Texture>;

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
      UnsignedInt,
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
      RgbaInteger,
      Bgra,
      RedInteger,

      DepthStencil,
    };

    enum class Format : u8 {
      Rgb8,
      Rgba8,
      Rgba8UI,
      Srgb8,
      Srgb8Alpha8,
      Rgb32F,
      Rgba32F,
      Rgb16F,
      Rgba16F,
      R11FG11FB10F,

      R32UI,

      Depth24Stencil8,

      DefaultDepthStencil = Depth24Stencil8,
    };

    enum class Type : u8 {
      Image,
      Color,
      Buffer,
    };

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
      Texture::Handle build();

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
      friend class Texture;
    };

  public:
    [[nodiscard]] static Texture::Builder builder();
    [[nodiscard]] static Texture::Builder load(const String& filepath, bool verticalFlipOnLoad = true);
    [[nodiscard]] static Texture::Builder buffer(u32 width, u32 height);
    [[nodiscard]] static Texture::Builder buffer(const void* inData, u32 width, u32 height, Texture::DataFormat dataFormat, Texture::DataType dataType);
    [[nodiscard]] static Texture::Builder color(u32 color);
    [[nodiscard]] static Texture::Builder color(u8 r, u8 g, u8 b, u8 a = 0xFF);
    [[nodiscard]] static Texture::Handle generateMissingDataPlaceholder();
    DISALLOW_MOVE_AND_COPY(Texture);
    ~Texture();

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
      Option<String> filePath = std::nullopt;
      u32 color{};
    };

    static Data fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Specification specification = {});

    static void destroyAllTextures();

  public:
    // DO NOT USE! Use the builder!
    //
    // NOTE: It has to be public so it can be constructed by std::make_shared.
    Texture(Data data)
      : mData{std::move(data)}
    {}

  private:
    Data mData;
  };

  class SubTexture {
  public:
    SubTexture() = default;
    SubTexture(Texture::Handle texture, const Vec2& from = Vec2{0.0f, 0.0f}, const Vec2& to = Vec2{1.0f, 1.0f})
      : mFrom{from}, mTo{to}, mTexture{texture}
    {}

    const Vec2& getTo() const { return mTo; }
    const Vec2& getFrom() const { return mFrom; }
    const Texture::Handle& getTexture() const { return mTexture; }

  private:
    Vec2 mFrom{0.0f, 0.0f};
    Vec2 mTo{1.0f, 1.0f};
    Texture::Handle mTexture;
  };

  class TextureAtlas {
  public:
    TextureAtlas() = default;
    TextureAtlas(const Texture::Handle& texture, u32 cellSize);
    TextureAtlas(const Texture::Handle& texture, u32 cellWidth, u32 cellHeight);

    SubTexture get(u32 index);

  private:
    Texture::Handle mTexture = nullptr;
    u32 mCellWidth{};
    u32 mCellHeight{};

    u32 mRowCellCount{};
    u32 mColumnCellCount{};
  };

  class CubeMap {
    friend class Application;
  public:
    using Handle = std::shared_ptr<CubeMap>;

  public:
    static CubeMap::Handle load(std::array<String, 6> paths);
    static CubeMap::Handle load(const char* filepath);
    DISALLOW_MOVE_AND_COPY(CubeMap);
    ~CubeMap();

    void bind() const;

    u32 getId() const;
    bool reload();

  public:
    CubeMap(u32 id)
      : mId{id}
    {}

  private:
    u32 mId;
  };

} // namespace Gate

