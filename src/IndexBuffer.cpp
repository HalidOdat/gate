#include "IndexBuffer.hpp"

#include <glad/glad.h>

namespace Game {
  IndexBuffer::IndexBuffer(Slice<u32> slice) {
    glGenBuffers(1, &this->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW);
  }
  
  IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &this->id);
  }

  void IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
  }

  void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

} // namespace Game