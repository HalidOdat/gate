#include "Core/Log.hpp"
#include "VertexBuffer.hpp"

#include <glad/glad.h>

namespace Game {

  VertexBuffer::VertexBuffer(Slice<void> slice) {
    GAME_GL_CHECK(glGenBuffers(1, &this->id));
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GAME_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW));
  }

  VertexBuffer::~VertexBuffer() {
    GAME_GL_CHECK(glDeleteBuffers(1, &this->id));
  }

  void VertexBuffer::bind() {
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
  }

  void VertexBuffer::unbind() {
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  }

} // namespace Game
