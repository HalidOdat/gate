#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/VertexBuffer.hpp"

namespace Game {

  Ref<VertexBuffer> VertexBuffer::create(Slice<const void> slice) {
    return Ref<VertexBuffer>(new VertexBuffer(slice));
  }

  VertexBuffer::VertexBuffer(Slice<const void> slice) {
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
