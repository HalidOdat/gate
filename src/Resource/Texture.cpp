#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Factory.hpp"

namespace Game {

  Texture2D::Builder& Texture2D::Builder::load(const StringView& file) {
    mIsBuffer = false;
    mFile = file;
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::color(u8 r, u8 g, u8 b, u8 a) {
    mIsBuffer = false;
    mFile = "";

    u32 color =  r; color <<= 8;
        color |= g; color <<= 8;
        color |= b; color <<= 8;
        color |= a;

    mColor = color;
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::color(u32 color) {
    mIsBuffer = false;
    mFile = "";
    mColor = color;
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::buffer(u32 width, u32 height) {
    mIsBuffer = true;
    mWidth = width;
    mHeight = height;
    return *this;
  }

  Texture2D::Builder& Texture2D::Builder::wrapping(Texture::WrappingMode mode) {
    mSpecification.wrapping = mode;
    return *this; 
  }
  Texture2D::Builder& Texture2D::Builder::filtering(Texture::Filtering filtering) {
    mSpecification.filtering = filtering;
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::mipmap(Texture::MipmapMode mode) {
    mSpecification.mipmap = mode;
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::verticalFlipOnLoad(bool yes) {
    if (yes) {
      mSpecification.verticalFlip = Texture::VerticalFlip::True;
    } else {
      mSpecification.verticalFlip = Texture::VerticalFlip::False;
    }
    return *this;
  }
  Texture2D::Builder& Texture2D::Builder::gammaCorrected(bool yes) {
    mSpecification.gammaCorrected = yes;
    return *this;
  }
  Texture2D::Handle Texture2D::Builder::build() {
    if (mIsBuffer) {
      GAME_ASSERT(mWidth != 0 || mHeight != 0);
      return Texture2D::buffer(mWidth, mHeight, mSpecification);
    }
    if (!mFile.empty()) {
      return Texture2D::load(String(mFile), mSpecification);
    } else {
      return Texture2D::color(mColor);
    }
  }

  Texture2D::Builder Texture2D::builder() {
    return Texture2D::Builder();
  }


  // TODO: key should also have specification
  static std::unordered_map<String, Texture2D::Handle> cachedImageTexture2D; // Key: Path,  Value: Texture
  static std::unordered_map<u32,    Texture2D::Handle> cachedColorTexture2D; // Key: Color, Value: Texture

  GAME_FACTORY_IMPLEMENTATION(Texture2D, texture2DFactory)

  template<>
  struct FactoryCallback<Texture2D> {
    inline static void postDecrement(const Resource<Texture2D>& resource) {
      if (resource.getReferenceCount() == 1) {
        switch (resource->getType()) {
          case Texture::Type::Image:
            cachedImageTexture2D.erase(*resource->getFilePath());
            break;
          case Texture::Type::Color:
            cachedColorTexture2D.erase(resource->getColor());
            break;
          case Texture::Type::Buffer:
            break;
          default:
            GAME_UNREACHABLE("unknown texture type!");
        }  
      }
    }
    inline static void created(Texture2D& texture, u32 id) {
      switch (texture.getType()) {
        case Texture::Type::Image:
          Logger::trace("Texture #%u loaded from file: %s", id, texture.getFilePath()->c_str());
          break;
        case Texture::Type::Color:
          Logger::trace("Texture #%u created color: #%08X", id, texture.getColor());
          break;
        case Texture::Type::Buffer:
          Logger::trace("Texture2D #%u created buffer: width=%u, height=%u", id, texture.getWidth(), texture.getHeight());
          break;
        default:
          GAME_UNREACHABLE("unknown texture type!");
      }
    }
    inline static void destroyed(Texture2D& texture, u32 id) {
      switch (texture.getType()) {
        case Texture::Type::Image:
          Logger::trace("Texture2D #%u destroyed: %s", id, texture.getFilePath()->c_str());
          break;
        case Texture::Type::Color:
          Logger::trace("Texture2D #%u destroyed color: #%08X", id, texture.getColor());
          break;
        case Texture::Type::Buffer:
          Logger::trace("Texture2D #%u destroyed buffer: width=%u, height=%u", id, texture.getWidth(), texture.getHeight());
          break;
        default:
          GAME_UNREACHABLE("unknown texture type!");
      }
    }
    inline static void clear() {
      cachedImageTexture2D.clear();
      cachedColorTexture2D.clear();
    }
  };

  void Texture2D::destroyAllTextures() {
    cachedImageTexture2D.clear();
    cachedColorTexture2D.clear();
    texture2DFactory.clear();
  }

  void Texture2D::reloadAll() {
    GAME_PROFILE_FUNCTION();

    GAME_TODO("not implemented yet!");
  }

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
    GAME_UNREACHABLE("unknown texture wrapping type!");
  }

  static GLenum TextureFilteringToOpenGL(Texture::FilteringMode filtering) {
    switch (filtering) {
      case Texture::FilteringMode::Linear:  return GL_LINEAR;
      case Texture::FilteringMode::Nearest: return GL_NEAREST;
    }
    GAME_UNREACHABLE("unknown texture filtering type!");
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
        GAME_UNREACHABLE("unknown texture filtering type!");
    }

    GAME_UNREACHABLE("unknown texture mipmap type!");
  }

  Texture2D::Data Texture2D::fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, Specification specification) {
    GAME_PROFILE_FUNCTION();

    GAME_ASSERT_WITH_MESSAGE(channels == 4 || channels == 3, "Unknown channel");
    GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4) {
      internalFormat = GL_RGBA8;
      dataFormat = GL_RGBA;
    } else if (channels == 3) {
      internalFormat = GL_RGB8;
      dataFormat = GL_RGB;
    }

    if (specification.gammaCorrected) {
      if (internalFormat == GL_RGBA8) {
        internalFormat = GL_SRGB8_ALPHA8;
      } else if (internalFormat == GL_RGB8) {
        internalFormat = GL_SRGB8;
      }
    }

    GAME_ASSERT_WITH_MESSAGE(internalFormat && dataFormat, "Format not supported!");

    auto repeat    = TextureWrappingToOpenGL(specification.wrapping);
    auto magFilter = TextureFilteringToOpenGL(specification.filtering.mag);
    auto minFilter = TextureFilteringMipmapToOpenGL(specification.filtering.min, specification.mipmap);

    u32 texture;
    GAME_GL_CHECK(glGenTextures(1, &texture));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat));	
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));

    GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, bytes));
    if (specification.mipmap != Texture::MipmapMode::None) {
      GAME_GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    }

    return Data{texture, width, height, specification};
  }

  Texture2D::Handle Texture2D::color(u32 color) {
    GAME_PROFILE_FUNCTION();

    if (cachedColorTexture2D.contains(color)) {
      return cachedColorTexture2D.at(color);
    }

    Texture2D::Specification specification;
    specification.mipmap = Texture::MipmapMode::None;
    specification.filtering = Texture::FilteringMode::Nearest;

    u8 a = (color >> 0 ) & 0xFF;
    u8 b = (color >> 8 ) & 0xFF;
    u8 g = (color >> 16) & 0xFF;
    u8 r = (color >> 24) & 0xFF;

    const u8 bytes[] = {r, g, b, a};
    auto data = Texture2D::fromBytes(bytes, 1, 1, 4, specification);
    data.type  = Texture::Type::Color;
    data.color = color;

    auto result = texture2DFactory.emplace(std::move(data));
    cachedColorTexture2D.emplace(color, result);
    return result;
  }

  Texture2D::Handle Texture2D::color(u8 r, u8 g, u8 b, u8 a) {
    GAME_PROFILE_FUNCTION();

    u32 color =  r; color <<= 8;
        color |= g; color <<= 8;
        color |= b; color <<= 8;
        color |= a;

    return Texture2D::color(color);
  }

  Texture2D::Handle Texture2D::buffer(u32 width, u32 height, Specification specification) {
    auto data = Texture2D::fromBytes(nullptr, width, height, 3, specification);
    data.type = Texture::Type::Buffer;
    return texture2DFactory.emplace(std::move(data));
  }

  Texture2D::Handle Texture2D::load(const String& path, Specification specification) {
    GAME_PROFILE_FUNCTION();

    if (cachedImageTexture2D.contains(path)) {
      return cachedImageTexture2D.at(path);
    }

    stbi_set_flip_vertically_on_load(specification.verticalFlip == Texture::VerticalFlip::True);

    auto filepath = "assets/textures/" + String(path);

    int width, height, channels;
    stbi_uc* bytes = stbi_load(filepath.data(), &width, &height, &channels, 0);
        
    if (!bytes) {
      Logger::error("Couldn't load image file '%s'", filepath.data());
      return {};
    }

    auto data = Texture2D::fromBytes(bytes, width, height, channels, specification);
    data.type     = Texture::Type::Image;
    data.filePath = path;

    stbi_image_free(bytes);
    
    auto result = texture2DFactory.emplace(data);
    cachedImageTexture2D.emplace(String(path), result);
    return result;
  }

  Texture2D::Handle Texture2D::generateMissingDataPlaceholder() {
    GAME_PROFILE_FUNCTION();

    Texture2D::Specification specification;
    specification.filtering.mag = Texture::FilteringMode::Nearest;
    specification.filtering.min = Texture::FilteringMode::Linear;
    specification.mipmap        = Texture::MipmapMode::Linear;

    return texture2DFactory.emplace(Texture2D::fromBytes(defaultTextureData, 2, 2, 4, specification));
  }

  bool Texture2D::reload() {
    GAME_PROFILE_FUNCTION();

    if (!mData.filePath.has_value()) {
      return true;
    }

    stbi_set_flip_vertically_on_load(mData.specification.verticalFlip == Texture::VerticalFlip::True);
    int width, height, channels;
    auto file = "assets/textures/" + *mData.filePath;
    u8* bytes = stbi_load(mData.filePath->c_str(), &width, &height, &channels, 0);

    if (!bytes) {
      Logger::error("Couldn't reload image file '%s'", mData.filePath->c_str());
      return false;
    }

    auto data = Texture2D::fromBytes(bytes, width, height, channels, mData.specification);
    stbi_image_free(bytes);

    Logger::trace("Reloaded texture: %s", mData.filePath.value().c_str());
    GAME_GL_CHECK(glDeleteTextures(1, &mData.id));
    data.filePath = std::move(mData.filePath);
    mData = std::move(data);
    return true;
  }

  Texture2D::~Texture2D() {
    GAME_PROFILE_FUNCTION();

    GAME_GL_CHECK(glDeleteTextures(1, &mData.id));
  }

  void Texture2D::bind(const usize slot) const {
    GAME_PROFILE_FUNCTION();

    // TODO: debug check if max texture slot reached
    u32 textureId = mData.id;
    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0 + (GLenum)slot));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureId));
  }

  u32 Texture2D::getId() const {
    return mData.id;
  }

  u32 Texture2D::getWidth() const {
    return mData.width;
  }

  u32 Texture2D::getHeight() const {
    return mData.height;
  }

  // ---------------------------- CubeMap ---------------------------

  GAME_FACTORY_IMPLEMENTATION(CubeMap, cubeMapFactory)

  CubeMap::Handle CubeMap::load(FilePaths paths) {
    GAME_PROFILE_FUNCTION();

    GAME_ASSERT(paths.size() == 6);

    u32 texture;
    GAME_GL_CHECK(glGenTextures(1, &texture));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));

    int width, height, nrChannels;
    for (u32 i = 0; i < paths.size(); ++i) {
      stbi_set_flip_vertically_on_load(false);
      auto path = "assets/textures/" + paths[i];
      u8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
      if (data) {
        GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        stbi_image_free(data);
      } else {
        GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultTextureData));
      }
    }

    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE)); 

    return cubeMapFactory.emplace(texture, std::move(paths));

  }

  CubeMap::~CubeMap() {
    GAME_PROFILE_FUNCTION();

    GAME_GL_CHECK(glDeleteTextures(1, &id));
  }

  void CubeMap::bind() const {
    GAME_PROFILE_FUNCTION();

    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  }

  bool CubeMap::reload() {
    GAME_PROFILE_FUNCTION();

    // TODO: implement reloading
    GAME_TODO("not implemented");
    return false;
  }


} // namespace Game