#include <glad/glad.h>

#include "Core/Assert.hpp"
#include "Renderer/IndexBuffer.hpp"

namespace Game {
  IndexBuffer::IndexBuffer(Slice<u32> slice) {
    GAME_GL_CHECK(glGenBuffers(1, &this->id));
    GAME_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    GAME_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW));
  }
  
  IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &this->id);
  }

  void IndexBuffer::bind() {
    GAME_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
  }

  void IndexBuffer::unbind() {
    GAME_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }

} // namespace Game