#include <cassert>

#include <glad/glad.h>

#include "Renderer/VertexArray.hpp"

namespace Game {
  
  VertexArray::VertexArray() {
    GAME_GL_CHECK(glCreateVertexArrays(1, &this->id));
  }

  VertexArray::~VertexArray() {
    GAME_GL_CHECK(glDeleteVertexArrays(1, &this->id));
  }

  void VertexArray::bind() {
    GAME_GL_CHECK(glBindVertexArray(this->id));
  }

  void VertexArray::unbind() {
    GAME_GL_CHECK(glBindVertexArray(0));
  }

  void VertexArray::addVertexBuffer(Ref<VertexBuffer> buffer) {
    this->bind();
    buffer->bind();

    const auto layout = buffer->getLayout();
    for (const auto& element : layout.getElements()) {
      switch (element.getType()) {
        case BufferElement::Type::Float:
        case BufferElement::Type::Float2:
        case BufferElement::Type::Float3:
        case BufferElement::Type::Float4:
          GAME_GL_CHECK(glEnableVertexAttribArray(this->vertexAttributeIndex));
	        GAME_GL_CHECK(glVertexAttribPointer(
            this->vertexAttributeIndex,
            (GLint)element.getComponentCount(),
            GL_FLOAT,
            element.isNormalized() ? GL_TRUE : GL_FALSE,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          ));
          this->vertexAttributeIndex++;
          break;
        case BufferElement::Type::Int:
        case BufferElement::Type::Int2:
        case BufferElement::Type::Int3:
        case BufferElement::Type::Int4:
        case BufferElement::Type::Bool:
          GAME_GL_CHECK(glEnableVertexAttribArray(this->vertexAttributeIndex));
	        GAME_GL_CHECK(glVertexAttribIPointer(
            this->vertexAttributeIndex,
            (GLint)element.getComponentCount(),
            element.getType() == BufferElement::Type::Bool ? GL_BOOL : GL_INT,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          ));
          this->vertexAttributeIndex++;
          break;
      }
    }

    this->buffers.push_back(buffer);
  }

  void VertexArray::setIndexBuffer(Ref<IndexBuffer> buffer) {
    this->bind();
    buffer->bind();
    this->indexBuffer = buffer;
  }

  void VertexArray::draw() {
    this->bind();
    if (this->indexBuffer) {
      GAME_GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    } else {
      GAME_GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
    }
  }

} // namespace Game
