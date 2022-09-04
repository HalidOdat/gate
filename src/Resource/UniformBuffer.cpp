#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Resource/Factory.hpp"
#include "Resource/UniformBuffer.hpp"

namespace Game {

  namespace {
    static GLenum bufferStorageAndAccessTypeToOpenGL(UniformBuffer::StorageType storage, UniformBuffer::AccessType access) {
      switch (storage) {
        case UniformBuffer::StorageType::Stream:
          switch (access) {
            case UniformBuffer::AccessType::Draw: return GL_STREAM_DRAW;
            case UniformBuffer::AccessType::Read: return GL_STREAM_READ;
            case UniformBuffer::AccessType::Copy: return GL_STREAM_COPY;
          }
          break;
        case UniformBuffer::StorageType::Static:
          switch (access) {
            case UniformBuffer::AccessType::Draw: return GL_STATIC_DRAW;
            case UniformBuffer::AccessType::Read: return GL_STATIC_READ;
            case UniformBuffer::AccessType::Copy: return GL_STATIC_COPY;
          }
          break;
        case UniformBuffer::StorageType::Dynamic:
          switch (access) {
            case UniformBuffer::AccessType::Draw: return GL_DYNAMIC_DRAW;
            case UniformBuffer::AccessType::Read: return GL_DYNAMIC_READ;
            case UniformBuffer::AccessType::Copy: return GL_DYNAMIC_COPY;
          }
          break;
      }

      GAME_UNREACHABLE("unknown storage or access type!");
    }

  }

  GAME_FACTORY_IMPLEMENTATION(UniformBuffer, factory)

  UniformBuffer::Builder& UniformBuffer::Builder::data(const void* inData, u32 inSize) {
    mData = inData;
    mSize = inSize;
    return *this;
  }
  UniformBuffer::Builder& UniformBuffer::Builder::data(const Slice<const void> slice) {
    mData = slice.data();
    mSize = (u32)slice.sizeInBytes();
    return *this;
  }
  UniformBuffer::Builder& UniformBuffer::Builder::size(u32 inSize) {
    mSize = inSize;
    return *this;
  }
  UniformBuffer::Builder& UniformBuffer::Builder::storage(UniformBuffer::StorageType type) {
    mStorage = type;
    return *this;
  }
  UniformBuffer::Builder& UniformBuffer::Builder::access(UniformBuffer::AccessType type) {
    mAccess = type;
    return *this;
  }
  UniformBuffer::Handle UniformBuffer::Builder::build() {
    GLenum storageAndAccess = bufferStorageAndAccessTypeToOpenGL(mStorage, mAccess);

    u32 id;
    glGenBuffers(1, &id);
    glBindBufferBase(GL_UNIFORM_BUFFER, mBinding, id);
    if (mData) {
      glBufferData(GL_UNIFORM_BUFFER, mSize, mData, storageAndAccess);
    } else {
      glBufferData(GL_UNIFORM_BUFFER, mSize, nullptr, storageAndAccess);
    }
    return factory.emplace(id, mSize, mBinding);
  }

  UniformBuffer::Builder UniformBuffer::builder(u32 binding) {
    return UniformBuffer::Builder(binding);
  }

  UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &mId);
  }

  void UniformBuffer::bind() {
    glBindBuffer(GL_UNIFORM_BUFFER, mId);
  }

  void UniformBuffer::unbind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void UniformBuffer::set(const Slice<const void> slice) {
    glBindBuffer(GL_UNIFORM_BUFFER, mId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, slice.sizeInBytes(), slice.data());
  }

} // namespace Game
