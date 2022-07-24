#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Manager.hpp"

namespace Game {

  Texture2D::Data Texture2D::fromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels) {
    GAME_ASSERT_WITH_MESSAGE(channels == 4 || channels == 3, "Unknown channel");
    GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4) {
      internalFormat = GL_RGBA8;
      dataFormat = GL_RGBA;
    } else if (channels == 3) {
      internalFormat = GL_RGB8;
      dataFormat = GL_RGB;
    }

    GAME_ASSERT_WITH_MESSAGE(internalFormat & dataFormat, "Format not supported!");

    u32 texture;
    GAME_GL_CHECK(glGenTextures(1, &texture));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, bytes));
    GAME_GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    return { texture, width, height };
  }

  Texture2D::Data Texture2D::create(const StringView& filepath) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data = stbi_load(filepath.data(), &width, &height, &channels, 0);
        
    if (!data) {
      Logger::error("couldn't load image file '%s'", filepath.data());
      return {0, 0, 0};
    }

    const auto result = Texture2D::fromBytes(data, width, height, channels);
    stbi_image_free(data);
    return result;
  }

  const Texture2D::Data& Texture2D::getData() const {
    return ResourceManager::getTextureData(this->id);
  }

  void Texture2D::destroy(Texture2D::Data& data) { 
    GAME_GL_CHECK(glDeleteTextures(1, &data.id));
  }

  void Texture2D::bind(const usize slot) noexcept {
    u32 textureId = this->getData().id;
    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureId));
  }

  u32 Texture2D::getId() const {
    return this->getData().id;
  }

  u32 Texture2D::getWidth() const {
    return this->getData().width;
  }

  u32 Texture2D::getHeight() const {
    return this->getData().height;
  }

} // namespace Game