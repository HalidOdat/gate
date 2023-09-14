#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Core/OpenGL.hpp"

#include "Renderer/Texture.hpp"
#include "Renderer/Shader.hpp"
#include "Application.hpp"

namespace Gate {

  // TODO: key should also have specification
  // FIXME: Delete texture when it reaches 1 refcount for hashmap
  static std::unordered_map<String, Texture::Handle> cachedImageTexture; // Key: Path,  Value: Texture
  static std::unordered_map<u32,    Texture::Handle> cachedColorTexture; // Key: Color, Value: Texture

  static const u8 defaultTextureData[] = {
    0x00, 0x00, 0x00, 0xFF,   0xFF, 0x00, 0xFF, 0xFF,
    0xFF, 0x00, 0xFF, 0xFF,   0x00, 0x00, 0x00, 0xFF,
  };

  static GLenum TextureWrappingToOpenGL(Texture::WrappingMode wrapping) {
    switch (wrapping) {
      case Texture::WrappingMode::Repeat:         return GL_REPEAT;
      case Texture::WrappingMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
      case Texture::WrappingMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
      case Texture::WrappingMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
    }
    GATE_UNREACHABLE("unknown texture wrapping type!");
  }

  static GLenum TextureFilteringToOpenGL(Texture::FilteringMode filtering) {
    switch (filtering) {
      case Texture::FilteringMode::Linear:  return GL_LINEAR;
      case Texture::FilteringMode::Nearest: return GL_NEAREST;
    }
    GATE_UNREACHABLE("unknown texture filtering type!");
  }

  static GLenum TextureFilteringMipmapToOpenGL(Texture::FilteringMode filtering, Texture::MipmapMode mipmap) {
    switch (filtering) {
      case Texture::FilteringMode::Linear:
        switch (mipmap) {
          case Texture::MipmapMode::None:    return GL_LINEAR;
          case Texture::MipmapMode::Linear:  return GL_LINEAR_MIPMAP_LINEAR;
          case Texture::MipmapMode::Nearest: return GL_LINEAR_MIPMAP_NEAREST;
        }
        break;
      case Texture::FilteringMode::Nearest:
        switch (mipmap) {
          case Texture::MipmapMode::None:    return GL_NEAREST;
          case Texture::MipmapMode::Linear:  return GL_NEAREST_MIPMAP_LINEAR;
          case Texture::MipmapMode::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
        }
        break;
      default:
        GATE_UNREACHABLE("unknown texture filtering type!");
    }

    GATE_UNREACHABLE("unknown texture mipmap type!");
  }

  static GLenum TextureDataTypeToOpenGL(Texture::DataType format) {
    switch (format) {
      case Texture::DataType::UnsignedByte:     return GL_UNSIGNED_BYTE;
      case Texture::DataType::UnsignedInt:      return GL_UNSIGNED_INT;
      case Texture::DataType::Float:            return GL_FLOAT;
      case Texture::DataType::HalfFloat:        return GL_HALF_FLOAT;
      case Texture::DataType::UnsignedInt_24_8: return GL_UNSIGNED_INT_24_8;
    }
    GATE_UNREACHABLE("unknown data type type!");
  }

  static GLenum TextureDataFormatToOpenGL(Texture::DataFormat format) {
    switch (format) {
      case Texture::DataFormat::Red:          return GL_RED;
      case Texture::DataFormat::Rg:           return GL_RG;
      case Texture::DataFormat::Rgb:          return GL_RGB;
      case Texture::DataFormat::Bgr:          return GL_BGR;
      case Texture::DataFormat::Rgba:         return GL_RGBA;
      case Texture::DataFormat::Bgra:         return GL_BGRA;
      case Texture::DataFormat::RedInteger:   return GL_RED_INTEGER;
      case Texture::DataFormat::RgbaInteger:  return GL_RGBA_INTEGER;
      case Texture::DataFormat::DepthStencil: return GL_DEPTH_STENCIL;
    }
    GATE_UNREACHABLE("unknown data format type!");
  }

  static GLenum TextureInternalFormatToOpenGL(Texture::Format format) {
    switch (format) {
      case Texture::Format::Rgb8:            return GL_RGB8;
      case Texture::Format::Rgba8:           return GL_RGBA8;
      case Texture::Format::Rgba8UI:         return GL_RGBA8UI;
      case Texture::Format::Srgb8:           return GL_SRGB8;
      case Texture::Format::Srgb8Alpha8:     return GL_SRGB8_ALPHA8;
      case Texture::Format::Rgb32F:          return GL_RGB32F;
      case Texture::Format::Rgb16F:          return GL_RGB16F;
      case Texture::Format::Rgba32F:         return GL_RGBA32F;
      case Texture::Format::Rgba16F:         return GL_RGBA16F;
      case Texture::Format::R11FG11FB10F:    return GL_R11F_G11F_B10F;
      case Texture::Format::R32UI:           return GL_R32UI;
      case Texture::Format::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
    }
    GATE_UNREACHABLE("unknown internal format type!");
  }

  static Texture::DataFormat TextureBaseDataFormatOfInternalFomat(Texture::Format format) {
    switch (format) {
      case Texture::Format::Rgb8:            return Texture::DataFormat::Rgb;
      case Texture::Format::Rgba8:           return Texture::DataFormat::Rgba;
      case Texture::Format::Rgba8UI:         return Texture::DataFormat::RgbaInteger;
      case Texture::Format::Srgb8:           return Texture::DataFormat::Rgb;
      case Texture::Format::Srgb8Alpha8:     return Texture::DataFormat::Rgba;
      case Texture::Format::Rgb32F:          return Texture::DataFormat::Rgb;
      case Texture::Format::Rgb16F:          return Texture::DataFormat::Rgb;
      case Texture::Format::Rgba32F:         return Texture::DataFormat::Rgba;
      case Texture::Format::Rgba16F:         return Texture::DataFormat::Rgba;
      case Texture::Format::R11FG11FB10F:    return Texture::DataFormat::Rgb;
      case Texture::Format::R32UI:           return Texture::DataFormat::RedInteger;
      case Texture::Format::Depth24Stencil8: return Texture::DataFormat::DepthStencil;
    }
    GATE_UNREACHABLE("unknown internal format type!");
  }

  static Texture::DataType TextureDataTypeOfInternalFomat(Texture::Format format) {
    switch (format) {
      case Texture::Format::Rgb8:            return Texture::DataType::UnsignedByte;
      case Texture::Format::Rgba8:           return Texture::DataType::UnsignedByte;
      case Texture::Format::Rgba8UI:         return Texture::DataType::UnsignedByte;
      case Texture::Format::Srgb8:           return Texture::DataType::UnsignedByte;
      case Texture::Format::Srgb8Alpha8:     return Texture::DataType::UnsignedByte;
      case Texture::Format::Rgb32F:          return Texture::DataType::Float;
      case Texture::Format::Rgb16F:          return Texture::DataType::HalfFloat;
      case Texture::Format::Rgba32F:         return Texture::DataType::Float;
      case Texture::Format::Rgba16F:         return Texture::DataType::HalfFloat;
      case Texture::Format::R11FG11FB10F:    return Texture::DataType::HalfFloat;
      case Texture::Format::R32UI:           return Texture::DataType::UnsignedInt;
      case Texture::Format::Depth24Stencil8: return Texture::DataType::UnsignedInt_24_8;
    }
    GATE_UNREACHABLE("unknown internal format type!");
  }

  Texture::Builder& Texture::Builder::format(Texture::Format internalFormat) {
    mSpecification.internalFormat = internalFormat;
    return *this;
  }
  Texture::Builder& Texture::Builder::wrapping(Texture::WrappingMode mode) {
    mSpecification.wrapping = mode;
    return *this; 
  }
  Texture::Builder& Texture::Builder::filtering(Texture::Filtering filtering) {
    mSpecification.filtering = filtering;
    return *this;
  }
  Texture::Builder& Texture::Builder::mipmap(Texture::MipmapMode mode) {
    mSpecification.mipmap = mode;
    return *this;
  }
  Texture::Builder& Texture::Builder::verticalFlipOnLoad(bool yes) {
    mSpecification.verticalFlip = yes;
    return *this;
  }
  Texture::Builder& Texture::Builder::gammaCorrected(bool yes) {
    mSpecification.gammaCorrected = yes;
    return *this;
  }
  Texture::Builder& Texture::Builder::samples(u32 inSamples) {
    mSpecification.samples = inSamples;
    return *this; 
  }
  Texture::Handle Texture::Builder::build() {
    String filepath = String(mFile);
    switch (mType) {
      case Texture::Type::Color:
        if (cachedColorTexture.find(mColor) != cachedColorTexture.end()) {
          return cachedColorTexture.at(mColor);
        }
        break;
      case Texture::Type::Buffer:
        if (!mData) {
          mDataType   = TextureDataTypeOfInternalFomat(mSpecification.internalFormat);
          mDataFormat = TextureBaseDataFormatOfInternalFomat(mSpecification.internalFormat);
        }
        break;
      case Texture::Type::Image:
        if (cachedImageTexture.find(filepath) != cachedImageTexture.end()) {
          return cachedImageTexture.at(filepath);
        }
        break;
      default:
        GATE_UNREACHABLE("unknown texture type!");
    }

    mSpecification.type = mType;

    if (mType == Texture::Type::Image) {
      stbi_set_flip_vertically_on_load(mSpecification.verticalFlip);

      int width, height, channels;
      stbi_uc* bytes = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

      if (!bytes) {
        Logger::error("Couldn't load image file '%s'", filepath.c_str());
        return {};
      }

      mWidth  = width;
      mHeight = height;
      mData   = bytes;
      mDataType = Texture::DataType::UnsignedByte;

      GATE_ASSERT_WITH_MESSAGE(channels == 4 || channels == 3, "Unknown channel");
      if (channels == 4) {
        mDataFormat = Texture::DataFormat::Rgba;
      } else if (channels == 3) {
        mDataFormat = Texture::DataFormat::Rgb;
      }
    }

    GATE_ASSERT(mWidth != 0 || mHeight != 0);

    if (mSpecification.gammaCorrected) {
      if (mSpecification.internalFormat == Texture::Format::Rgba8) {
        mSpecification.internalFormat = Texture::Format::Srgb8Alpha8;
      } else if (mSpecification.internalFormat == Texture::Format::Rgb8) {
        mSpecification.internalFormat = Texture::Format::Srgb8;
      }
    }

    auto dataType       = TextureDataTypeToOpenGL(mDataType);
    auto dataFormat     = TextureDataFormatToOpenGL(mDataFormat);
    auto internalFormat = TextureInternalFormatToOpenGL(mSpecification.internalFormat);
    auto wrapping       = TextureWrappingToOpenGL(mSpecification.wrapping);
    auto magFilter      = TextureFilteringToOpenGL(mSpecification.filtering.mag);
    auto minFilter      = TextureFilteringMipmapToOpenGL(mSpecification.filtering.min, mSpecification.mipmap);

    GLenum target;
    if (mSpecification.samples == 0) {
      target = GL_TEXTURE_2D;
    } else {
      target = GL_TEXTURE_2D_MULTISAMPLE;
    }

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(target, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapping);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

    u8 colorBytes[4];
    switch (mType) {
      case Texture::Type::Color: {
        colorBytes[3] = (mColor >> 0)  & 0xFF;
        colorBytes[2] = (mColor >> 8)  & 0xFF;
        colorBytes[1] = (mColor >> 16) & 0xFF;
        colorBytes[0] = (mColor >> 24) & 0xFF;
        mData = colorBytes;
        break;
      }
      case Texture::Type::Buffer:
        break;
      case Texture::Type::Image:
        break;
      default:
        GATE_UNREACHABLE("unknown texture type!");
    }

    if (mSpecification.samples == 0) {
      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, dataFormat, dataType, mData);
    } else {
      #ifndef GATE_PLATFORM_WEB
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mSpecification.samples, internalFormat, mWidth, mHeight, 0);
      #else
        GATE_TODO("glTexImage2DMultisample not implemented");
      #endif
    }

    if (mSpecification.mipmap != Texture::MipmapMode::None && mSpecification.internalFormat != Texture::Format::R32UI) {
      glGenerateMipmap(target);
    }

    if (mType == Texture::Type::Image) {
      stbi_image_free((stbi_uc*)mData);
    }

    auto data = Data{texture, mWidth, mHeight, mSpecification, mType, filepath, mColor};
    auto handle = std::make_shared<Texture>(std::move(data));
    switch (mType) {
      case Texture::Type::Color:
        Logger::trace("Texture #%u loaded from file: %s", handle->getId(), handle->getFilePath()->c_str());
        cachedColorTexture.emplace(mColor, handle);
        break;
      case Texture::Type::Image:
        Logger::trace("Texture #%u created color: #%08X", handle->getId(), handle->getColor());
        cachedImageTexture.emplace(std::move(filepath), handle);
        break;
      case Texture::Type::Buffer:
        Logger::trace("Texture #%u created buffer: width=%u, height=%u", handle->getId(), handle->getWidth(), handle->getHeight());
        break;
      default:
        GATE_UNREACHABLE("unknown texture type!");
    }
    return handle;
  }

  void Texture::destroyAllTextures() {
    cachedImageTexture.clear();
    cachedColorTexture.clear();
  }

  void Texture::reloadAll() {
    GATE_TODO("not implemented yet!");
  }

  Texture::Data Texture::fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, Specification specification) {
    GATE_ASSERT_WITH_MESSAGE(channels == 4 || channels == 3, "Unknown channel");
    GLenum dataFormat = 0;
    if (channels == 4) {
      dataFormat = GL_RGBA;
    } else if (channels == 3) {
      dataFormat = GL_RGB;
    }

    if (specification.gammaCorrected) {
      if (specification.internalFormat == Texture::Format::Rgba8) {
        specification.internalFormat = Texture::Format::Srgb8Alpha8;
      } else if (specification.internalFormat == Texture::Format::Rgb8) {
        specification.internalFormat = Texture::Format::Srgb8;
      }
    }

    auto dataType       = TextureDataTypeToOpenGL(Texture::DataType::UnsignedByte);
    auto internalFormat = TextureInternalFormatToOpenGL(specification.internalFormat);
    auto wrapping       = TextureWrappingToOpenGL(specification.wrapping);
    auto magFilter      = TextureFilteringToOpenGL(specification.filtering.mag);
    auto minFilter      = TextureFilteringMipmapToOpenGL(specification.filtering.min, specification.mipmap);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, bytes);
    if (specification.mipmap != Texture::MipmapMode::None) {
      glGenerateMipmap(GL_TEXTURE_2D);
    }

    return Data{texture, width, height, specification, Texture::Type::Color};
  }
  Texture::Builder Texture::color(u32 color) {
    Builder builder;
    builder.mType = Texture::Type::Color;
    builder.mColor = color;
    builder.mSpecification.mipmap = Texture::MipmapMode::None;
    builder.mSpecification.filtering.min = Texture::FilteringMode::Nearest;
    builder.mSpecification.filtering.mag = Texture::FilteringMode::Nearest;
    return builder;
  }
  Texture::Builder Texture::color(u8 r, u8 g, u8 b, u8 a) {
    u32 color =  r; color <<= 8;
        color |= g; color <<= 8;
        color |= b; color <<= 8;
        color |= a;
    return Texture::color(color);
  }
  Texture::Builder Texture::buffer(u32 width, u32 height) {
    Builder builder;
    builder.mType = Texture::Type::Buffer;
    builder.mWidth = width;
    builder.mHeight = height;
    return builder;
  }
  Texture::Builder Texture::buffer(const void* inData, u32 width, u32 height, Texture::DataFormat dataFormat, Texture::DataType dataType) {
    Builder builder;
    builder.mType = Texture::Type::Buffer;
    builder.mData = inData;
    builder.mWidth = width;
    builder.mHeight = height;
    builder.mDataFormat = dataFormat;
    builder.mDataType = dataType;
    return builder;
  }
  Texture::Builder Texture::load(const String& path, bool verticalFlipOnLoad) {
    Builder builder;
    builder.mType = Texture::Type::Image;
    builder.mFile = path;
    builder.mSpecification.verticalFlip = verticalFlipOnLoad;
    return builder;
  }
  Texture::Handle Texture::generateMissingDataPlaceholder() {
    return Texture::buffer(defaultTextureData, 2, 2, Texture::DataFormat::Rgba, Texture::DataType::UnsignedByte)
      .filtering({Texture::FilteringMode::Nearest, Texture::FilteringMode::Nearest})
      .mipmap(Texture::MipmapMode::None)
      .build();
  }
  bool Texture::reload() {
    if (mData.type != Texture::Type::Image) {
      return true;
    }

    stbi_set_flip_vertically_on_load(mData.specification.verticalFlip);
    int width, height, channels;
    auto file = *mData.filePath;
    u8* bytes = stbi_load(mData.filePath->c_str(), &width, &height, &channels, 0);

    if (!bytes) {
      Logger::error("Couldn't reload image file '%s'", mData.filePath->c_str());
      return false;
    }

    auto data = Texture::fromBytes(bytes, width, height, channels, mData.specification);
    stbi_image_free(bytes);

    Logger::trace("Reloaded texture: %s", mData.filePath.value().c_str());
    glDeleteTextures(1, &mData.id);
    data.filePath = std::move(mData.filePath);
    mData = std::move(data);
    return true;
  }
  Texture::~Texture() {
    switch (getType()) {
      case Texture::Type::Image:
        Logger::trace("Texture #%u destroyed: %s", getId(), getFilePath()->c_str());
        break;
      case Texture::Type::Color:
        Logger::trace("Texture #%u destroyed color: #%08X", getId(), getColor());
        break;
      case Texture::Type::Buffer:
        Logger::trace("Texture #%u destroyed buffer: width=%u, height=%u", getId(), getWidth(), getHeight());
        break;
      default:
        GATE_UNREACHABLE("unknown texture type!");
    }
    glDeleteTextures(1, &mData.id);
  }
  void Texture::bind(const usize slot) const {
    // TODO: debug check if max texture slot reached
    u32 textureId = mData.id;
    glActiveTexture(GL_TEXTURE0 + (GLenum)slot);
    glBindTexture(GL_TEXTURE_2D, textureId);
  }
  u32 Texture::getId() const {
    return mData.id;
  }
  u32 Texture::getWidth() const {
    return mData.width;
  }
  u32 Texture::getHeight() const {
    return mData.height;
  }

  TextureAtlas::TextureAtlas(const Texture::Handle& texture, u32 cellSize)
    : mTexture{texture}, mCellWidth{cellSize}, mCellHeight{cellSize}
  {
    const auto textureWidth = texture->getWidth();
    const auto textureHeight = texture->getHeight();

    mRowCellCount    = textureWidth  / mCellWidth;
    mColumnCellCount = textureHeight / mCellHeight;
  }

  TextureAtlas::TextureAtlas(const Texture::Handle& texture, u32 cellWidth, u32 cellHeight)
    : mTexture{texture}, mCellWidth{cellWidth}, mCellHeight{cellHeight}
  {
    const auto textureWidth = texture->getWidth();
    const auto textureHeight = texture->getHeight();
    mRowCellCount    = textureWidth  / mCellWidth;
    mColumnCellCount = textureHeight / mCellHeight;
  }

  SubTexture TextureAtlas::get(u32 _index) {
    u32 level = _index / mRowCellCount;
    u32 index = _index % mRowCellCount;

    GATE_ASSERT(index < mRowCellCount);
    GATE_ASSERT(level < mColumnCellCount);

    const auto textureWidth  = mTexture->getWidth();
    const auto textureHeight = mTexture->getHeight();

    const auto widthRatio  = f32(mCellWidth) / textureWidth;
    const auto heightRatio = f32(mCellHeight) / textureHeight;

    return SubTexture(
      mTexture,
      { // from
        widthRatio * index,
        heightRatio * (mColumnCellCount - level - 1)
      },
      { // to
        widthRatio * (index + 1),
        heightRatio * (mColumnCellCount - level)
      }
    );
  }

  CubeMap::Handle CubeMap::load(std::array<String, 6> paths) {
    GATE_ASSERT(paths.size() == 6);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (u32 i = 0; i < paths.size(); ++i) {
      stbi_set_flip_vertically_on_load(false);
      auto path = paths[i];
      u8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

      if (data) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
      } else {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8_ALPHA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultTextureData);
      }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return std::make_shared<CubeMap>(texture);
  }

  CubeMap::Handle CubeMap::load(const char* filepath) {
    Logger::trace("CubeMap: Loading %s", filepath);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(filepath, &width, &height, &nrComponents, 0);
    if (!data) {
      Logger::error("Failed to load HDR image: %s", filepath);
      return nullptr;
    }

    Logger::info("Width: %d", width);
    Logger::info("Height: %d", height);

    // Set up projection and view matrices for capturing data onto the 6 cubemap face directions
    const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // Load shader
    Shader::Handle equirectangularToCubemapShader = Shader::load("assets/3D/shaders/EquirectangularToCubemap.glsl").build();
    equirectangularToCubemapShader->bind();
    equirectangularToCubemapShader->setInt("uEquirectangularMap", 0);
    equirectangularToCubemapShader->setMat4("uProjection", captureProjection);

    // Setup framebuffer
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, height, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    
    // Create hdr texture
    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    // Setup cubemap to render to and attach to framebuffer
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, height, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    auto renderCube = [&]() {
      if (cubeVAO == 0) {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
      }
      // render Cube
      glBindVertexArray(cubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
    };

    // Convert HDR equirectangular environment map to cubemap equivalent
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, height, height); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        equirectangularToCubemapShader->setMat4("uView", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto scrWidth = Application::getWindow().getWidth();
    auto scrHeight = Application::getWindow().getHeight();
    glViewport(0, 0, scrWidth, scrHeight);

    return std::make_shared<CubeMap>(envCubemap);
  }

  CubeMap::~CubeMap() {
    glDeleteTextures(1, &mId);
  }

  void CubeMap::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
  }

  bool CubeMap::reload() {
    // TODO: implement reloading
    GATE_TODO("not implemented");
    return false;
  }

} // namespace Gate
