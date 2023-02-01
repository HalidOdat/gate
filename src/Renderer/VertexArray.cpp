#include <cassert>

#include "Core/OpenGL.hpp"

#include "Core/Log.hpp"
#include "Renderer/VertexArray.hpp"

#include "Resource/Factory.hpp"

namespace Gate {
  
  GAME_FACTORY_IMPLEMENTATION(VertexArray, factory);

  VertexArray::Handle VertexArray::create() {
    u32 id;
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);
    return factory.emplace(id);
  }

  VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &mId);
  }

  void VertexArray::bind() {
    glBindVertexArray(mId);
  }

  void VertexArray::unbind() {
    glBindVertexArray(0);
  }

  void VertexArray::addVertexBuffer(VertexBuffer::Handle buffer) {
    this->bind();
    buffer->bind();

    const auto layout = buffer->getLayout();
    for (const auto& element : layout.getElements()) {
      switch (element.getType()) {
        case BufferElement::Type::Float:
        case BufferElement::Type::Float2:
        case BufferElement::Type::Float3:
        case BufferElement::Type::Float4:
          glEnableVertexAttribArray(mVertexAttributeIndex);
	        glVertexAttribPointer(
            mVertexAttributeIndex,
            (GLint)element.getComponentCount(),
            GL_FLOAT,
            element.isNormalized() ? GL_TRUE : GL_FALSE,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          );
          mVertexAttributeIndex++;
          break;
        case BufferElement::Type::Int:
        case BufferElement::Type::Int2:
        case BufferElement::Type::Int3:
        case BufferElement::Type::Int4:
        case BufferElement::Type::Uint:
        case BufferElement::Type::Bool: {
          GLenum type;
          switch (element.getType()) {
            case BufferElement::Type::Bool:
              type = GL_BOOL;
              break;
            case BufferElement::Type::Int:
            case BufferElement::Type::Int2:
            case BufferElement::Type::Int3:
            case BufferElement::Type::Int4:
              type = GL_INT;
              break;
            case BufferElement::Type::Uint:
              type = GL_UNSIGNED_INT;
              break;
            default:
              GATE_UNREACHABLE("Unknown buffer element type!");
          }
          
          glEnableVertexAttribArray(mVertexAttributeIndex);
	        glVertexAttribIPointer(
            mVertexAttributeIndex,
            (GLint)element.getComponentCount(),
            type,
            (GLsizei)layout.getStride(),
            (const void*)element.getOffset()
          );
          if (element.getAttributeDivisor() != 0) {
            glVertexAttribDivisor(mVertexAttributeIndex, element.getAttributeDivisor());
          }
          mVertexAttributeIndex++;
          break;
        }
        case BufferElement::Type::Mat3:
        case BufferElement::Type::Mat4: {
          auto count = element.getComponentCount();
          for (usize i = 0; i < count; i++) {
            glEnableVertexAttribArray(mVertexAttributeIndex);
            glVertexAttribPointer(mVertexAttributeIndex,
              (GLint)count,
              GL_FLOAT,
              element.isNormalized() ? GL_TRUE : GL_FALSE,
              layout.getStride(),
              (const void*)(element.getOffset() + sizeof(float) * count * i)
            );
            if (element.getAttributeDivisor() != 0) {
              glVertexAttribDivisor(mVertexAttributeIndex, element.getAttributeDivisor());
            }
            mVertexAttributeIndex++;
          }
          break;
        }
        default:
          GATE_UNREACHABLE("Unknown buffer element type!");
      }
    }

    mBuffers.push_back(buffer);
  }

  void VertexArray::setIndexBuffer(IndexBuffer::Handle buffer) {
    bind();
    buffer->bind();
    mIndexBuffer = buffer;
  }

  void VertexArray::drawIndices() {
    const auto count = mIndexBuffer->getCount();
    this->drawIndices(count);
  }

  void VertexArray::drawIndices(const u32 count) {
    this->bind();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
  }

  void VertexArray::drawArrays(u32 count) {
    glDrawArrays(GL_TRIANGLES, 0, count);
  }

} // namespace Gate
