#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Renderer/IndexBuffer.hpp"

#include "Resource/Factory.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(IndexBuffer, factory)

  IndexBuffer::Handle IndexBuffer::create(Slice<const u32> slice) {
    u32 id;
    GAME_GL_CHECK(glGenBuffers(1, &id));
    GAME_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
    GAME_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW));
    return factory.emplace(id, u32(slice.size()));
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