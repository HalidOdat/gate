#include "VertexArray.hpp"

#include <glad/glad.h>

namespace Game {
  
  VertexArray::VertexArray() {
    glCreateVertexArrays(1, &this->id);
  }

  VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &this->id);
  }

  void VertexArray::bind() {
    glBindVertexArray(this->id);
  }

  void VertexArray::unbind() {
    glBindVertexArray(0);
  }

  void VertexArray::addVertexBuffer(Ref<VertexBuffer> buffer) {
    this->bind();
    buffer->bind();

    // Assume that 2 position float x and y
    glEnableVertexAttribArray(0);
	  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    this->buffers.push_back(buffer);
  }

  void VertexArray::draw() {
    this->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

} // namespace Game
