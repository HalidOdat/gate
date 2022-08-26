#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/VertexBuffer.hpp"

#include "Resource/Factory.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(VertexBuffer, factory)

  VertexBuffer::Handle VertexBuffer::withSize(const u32 size) {
    u32 id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    return factory.emplace(id);
  }

  VertexBuffer::Handle VertexBuffer::create(Slice<const void> slice) {
    u32 id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW);
    return factory.emplace(id);
  }

  VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &mId);
  }

  void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, mId);
  }

  void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void VertexBuffer::set(const Slice<const void> slice) {
    glBindBuffer(GL_ARRAY_BUFFER, mId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, slice.sizeInBytes(), slice.data());
  }

} // namespace Game
