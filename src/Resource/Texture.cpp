#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Manager.hpp"

namespace Game {

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
      default:
        GAME_UNREACHABLE("unknown texture wrapping type!");
        return 0;
    }
  }

  static GLenum TextureFilteringToOpenGL(Texture::FilteringMode filtering) {
    switch (filtering) {
      case Texture::FilteringMode::Linear:  return GL_LINEAR;
      case Texture::FilteringMode::Nearest: return GL_NEAREST;
      default:
        GAME_UNREACHABLE("unknown texture filtering type!");
        return 0;
    }
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
        return 0;
    }

    GAME_UNREACHABLE("unknown texture mipmap type!");
  }

  Texture2D::Data Texture2D::fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels, Specification specification) {
    GAME_ASSERT_WITH_MESSAGE(channels == 4 || channels == 3, "Unknown channel");
    GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4) {
      internalFormat = GL_RGBA8;
      dataFormat = GL_RGBA;
    } else if (channels == 3) {
      internalFormat = GL_RGB8;
      dataFormat = GL_RGB;
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
    GAME_GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    return { texture, width, height, specification };
  }

  Option<Texture2D::Data> Texture2D::fromFile(const StringView& filepath, Specification specification) {
    stbi_set_flip_vertically_on_load(specification.verticalFlip == Texture::VerticalFlip::True);

    int width, height, channels;
    stbi_uc* data = stbi_load(filepath.data(), &width, &height, &channels, 0);
        
    if (!data) {
      Logger::error("couldn't load image file '%s'", filepath.data());
      return None;
    }

    auto result = Texture2D::fromBytes(data, width, height, channels, specification);
    result.filePath = filepath;
    stbi_image_free(data);
    return result;
  }

  Texture2D::Data Texture2D::generateMissingDataPlaceholder() {
    Texture2D::Specification specification;
    specification.filtering.mag = Texture::FilteringMode::Nearest;
    specification.filtering.min = Texture::FilteringMode::Linear;
    specification.mipmap        = Texture::MipmapMode::Linear;

    return Texture2D::fromBytes(defaultTextureData, 2, 2, 4, specification);
  }

  bool Texture2D::reload() {
    if (!mData.filePath.has_value()) {
      return false;
    }

    auto data = Texture2D::fromFile(*mData.filePath, mData.specification);
    if (data) {
      Logger::trace("Reloaded texture: %s", mData.filePath.value().c_str());

      GAME_GL_CHECK(glDeleteTextures(1, &mData.id));
      mData = *data;
      return true;
    }

    return false;
  }

  Texture2D::~Texture2D() {
    GAME_GL_CHECK(glDeleteTextures(1, &mData.id));
  }

  void Texture2D::bind(const usize slot) const {
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

  Option<CubeMap::Data> CubeMap::fromFile(FilePaths paths) {
    GAME_ASSERT(paths.size() == 6);

    u32 texture;
    GAME_GL_CHECK(glGenTextures(1, &texture));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));

    int width, height, nrChannels;
    for (u32 i = 0; i < paths.size(); ++i) {
      stbi_set_flip_vertically_on_load(false);
      u8* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
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

    return Data{texture, std::move(paths)};
    
  }

  CubeMap::~CubeMap() {
    GAME_GL_CHECK(glDeleteTextures(1, &mData.id));
  }

  void CubeMap::bind() const {
    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, mData.id));
  }

  bool CubeMap::reload() {
    // TODO: implement reloading
    GAME_TODO("not implemented");
    return false;
  }


} // namespace Game