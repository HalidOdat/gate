#include "Core/Log.hpp"
#include "VertexBuffer.hpp"

#include <glad/glad.h>

namespace Game {

  VertexBuffer::VertexBuffer(Slice<void> slice) {
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
    glBufferData(GL_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW);
  }

  VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &this->id);
  }

  void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, this->id);
  }

  void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

} // namespace Game
