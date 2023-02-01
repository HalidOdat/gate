#include "Core/OpenGL.hpp"

#include "Core/Base.hpp"
#include "Renderer/IndexBuffer.hpp"

#include "Resource/Factory.hpp"

namespace Gate {

  GAME_FACTORY_IMPLEMENTATION(IndexBuffer, factory)

  IndexBuffer::Handle IndexBuffer::create(Slice<const u32> slice) {
    u32 id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW);
    return factory.emplace(id, u32(slice.size()));
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