#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Core/Log.hpp"
#include "Renderer/Texture.hpp"

namespace Game {

  Ref<Texture2D> Texture2D::create(const StringView& filepath) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data = stbi_load(filepath.data(), &width, &height, &channels, 0);
        
    if (!data) {
      Logger::error("couldn't load image file '%s'", filepath.data());
      return nullptr;
    }

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

    GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data));
    GAME_GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(data);

    return Ref<Texture2D>( new Texture2D(texture, width, height) );
  }

  Texture2D::~Texture2D() noexcept {
    GAME_GL_CHECK(glDeleteTextures(1, &this->id));
  }

  void Texture2D::bind() noexcept {
    GAME_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, this->id));
  }

} // namespace Game