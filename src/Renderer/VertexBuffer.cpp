#include <glad/glad.h>

#include "Core/Log.hpp"
#include "Renderer/VertexBuffer.hpp"

namespace Game {

  VertexBuffer::Handle VertexBuffer::withSize(const usize size) {
    return VertexBuffer::Handle(new VertexBuffer(size));
  }

  VertexBuffer::Handle VertexBuffer::create(Slice<const void> slice) {
    return VertexBuffer::Handle(new VertexBuffer(slice));
  }

  VertexBuffer::VertexBuffer(Slice<const void> slice) {
    GAME_GL_CHECK(glGenBuffers(1, &this->id));
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GAME_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, slice.sizeInBytes(), slice.data(), GL_STATIC_DRAW));
  }

  VertexBuffer::VertexBuffer(const usize size) {
    GAME_GL_CHECK(glGenBuffers(1, &this->id));
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GAME_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
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

  void VertexBuffer::set(const Slice<const void> slice) {
    GAME_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    GAME_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, slice.sizeInBytes(), slice.data()));
  }

} // namespace Game
