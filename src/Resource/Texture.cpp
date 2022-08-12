#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Factory.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(Texture2D, texture2DFactory)

  template<>
  struct FactoryCallback<Texture2D> {
    inline static void created(Texture2D& texture, u32 id) {
      if (texture.getFilePath()) {
        Logger::trace("Texture #%u loaded from file: %s", id, texture.getFilePath()->c_str());
      } else {
        Logger::trace("Texture #%u created color: #%08x", id, texture.getColor());
      }
    }
    inline static void destroyed(Texture2D& texture, u32 id) {
     if (texture.getFilePath()) {
        Logger::trace("Texture #%u destroyed: %s", id, texture.getFilePath()->c_str());
      } else {
        Logger::trace("Texture #%u destroyed color: #%08x", id, texture.getColor());
      }
    }
  };

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

    u8 a = color & 0xFF; color >>= 8;
    u8 b = color & 0xFF; color >>= 8;
    u8 g = color & 0xFF; color >>= 8;
    u8 r = color & 0xFF;
    return Texture2D::color(r, g, b, a);
  }

  Texture2D::Handle Texture2D::color(u8 r, u8 g, u8 b, u8 a) {
    GAME_PROFILE_FUNCTION();

    Texture2D::Specification specification;
    specification.mipmap = Texture::MipmapMode::None;
    specification.filtering = Texture::FilteringMode::Nearest;

    const u8 bytes[] = {r, g, b, a};
    return texture2DFactory.emplace(Texture2D::fromBytes(bytes, 1, 1, 4, specification));
  }

  Texture2D::Handle Texture2D::load(const StringView& path, Specification specification) {
    GAME_PROFILE_FUNCTION();

    stbi_set_flip_vertically_on_load(specification.verticalFlip == Texture::VerticalFlip::True);

    auto filepath = "assets/textures/" + String(path);

    int width, height, channels;
    stbi_uc* data = stbi_load(filepath.data(), &width, &height, &channels, 0);
        
    if (!data) {
      Logger::error("Couldn't load image file '%s'", filepath.data());
      return {};
    }

    auto result = Texture2D::fromBytes(data, width, height, channels, specification);
    result.filePath = filepath;
    stbi_image_free(data);
    return texture2DFactory.emplace(result);
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