#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Resource/Factory.hpp"
#include "Renderer/VertexBuffer.hpp"

namespace Game {

  namespace {
    static GLenum bufferStorageAndAccessTypeToOpenGL(Buffer::StorageType storage, Buffer::AccessType access) {
      switch (storage) {
        case Buffer::StorageType::Stream:
          switch (access) {
            case Buffer::AccessType::Draw: return GL_STREAM_DRAW;
            case Buffer::AccessType::Read: return GL_STREAM_READ;
            case Buffer::AccessType::Copy: return GL_STREAM_COPY;
          }
          break;
        case Buffer::StorageType::Static:
          switch (access) {
            case Buffer::AccessType::Draw: return GL_STATIC_DRAW;
            case Buffer::AccessType::Read: return GL_STATIC_READ;
            case Buffer::AccessType::Copy: return GL_STATIC_COPY;
          }
          break;
        case Buffer::StorageType::Dynamic:
          switch (access) {
            case Buffer::AccessType::Draw: return GL_DYNAMIC_DRAW;
            case Buffer::AccessType::Read: return GL_DYNAMIC_READ;
            case Buffer::AccessType::Copy: return GL_DYNAMIC_COPY;
          }
          break;
      }

      GAME_UNREACHABLE("unknown storage or access type!");
    }

  }

  GAME_FACTORY_IMPLEMENTATION(VertexBuffer, factory)

  VertexBuffer::Builder& VertexBuffer::Builder::data(const void* inData, u32 inSize) {
    mData = inData;
    mSize = inSize;
    return *this;
  }
  VertexBuffer::Builder& VertexBuffer::Builder::data(const Slice<const void> slice) {
    mData = slice.data();
    mSize = (u32)slice.sizeInBytes();
    return *this;
  }
  VertexBuffer::Builder& VertexBuffer::Builder::size(u32 inSize) {
    mSize = inSize;
    return *this;
  }
  VertexBuffer::Builder& VertexBuffer::Builder::layout(BufferElement::Type type, String name) {
    mLayout.push_back({type, /* name */});
    return *this;
  }
  VertexBuffer::Builder& VertexBuffer::Builder::storage(Buffer::StorageType type) {
    mStorage = type;
    return *this;
  }
  VertexBuffer::Builder& VertexBuffer::Builder::access(Buffer::AccessType type) {
    mAccess = type;
    return *this;
  }
  VertexBuffer::Handle VertexBuffer::Builder::build() {
    GLenum storageAndAccess = bufferStorageAndAccessTypeToOpenGL(mStorage, mAccess);

    u32 id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    if (mData) {
      glBufferData(GL_ARRAY_BUFFER, mSize, mData, storageAndAccess);
    } else {
      glBufferData(GL_ARRAY_BUFFER, mSize, nullptr, storageAndAccess);
    }
    return factory.emplace(id, BufferLayout(mLayout));
  }

  VertexBuffer::Builder VertexBuffer::builder() {
    return VertexBuffer::Builder();
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
