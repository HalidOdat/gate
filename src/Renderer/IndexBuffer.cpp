#include "Core/OpenGL.hpp"
#include "Renderer/IndexBuffer.hpp"

namespace Gate {

  IndexBuffer::Handle IndexBuffer::create(Slice<const u32> slice) {
    u32 id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW);
    return std::make_shared<IndexBuffer>(id, u32(slice.size()));
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

} // namespace Gate