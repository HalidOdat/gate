#include <cassert>

#include <glad/glad.h>

#include "VertexArray.hpp"

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

    const auto layout = buffer->getLayout();
    for (const auto& element : layout.getElements()) {
      switch (element.getType()) {
        case BufferElement::Type::Float:
        case BufferElement::Type::Float2:
        case BufferElement::Type::Float3:
        case BufferElement::Type::Float4:
          glEnableVertexAttribArray(this->vertexAttributeIndex);
	        glVertexAttribPointer(
            this->vertexAttributeIndex,
            (GLint)element.getComponentCount(),
            GL_FLOAT,
            element.isNormalized() ? GL_TRUE : GL_FALSE,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          );
          this->vertexAttributeIndex++;
          break;
        case BufferElement::Type::Int:
        case BufferElement::Type::Int2:
        case BufferElement::Type::Int3:
        case BufferElement::Type::Int4:
        case BufferElement::Type::Bool:
          glEnableVertexAttribArray(this->vertexAttributeIndex);
	        glVertexAttribIPointer(
            this->vertexAttributeIndex,
            (GLint)element.getComponentCount(),
            element.getType() == BufferElement::Type::Bool ? GL_BOOL : GL_INT,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          );
          this->vertexAttributeIndex++;
          break;
      }
    }

    this->buffers.push_back(buffer);
  }

  void VertexArray::draw() {
    this->bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

} // namespace Game
