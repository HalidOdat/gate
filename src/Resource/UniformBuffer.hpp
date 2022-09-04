#pragma once

#include "Core/Base.hpp"
#include "Resource/Resource.hpp"

#include <vector>

namespace Game {

  class UniformBuffer {
  public:
    using Handle = Resource<UniformBuffer>;

    enum class StorageType : u8 {
      /// The data store contents will be modified once and used at most a few times.
      Stream,

      /// The data store contents will be modified once and used many times. 
      Static,

      // The data store contents will be modified repeatedly and used many times.
      Dynamic,
    };

    enum class AccessType : u8 {
      /// The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
      Draw,
      /// The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
      Read,
      /// The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
      Copy,
    };

    class Builder {
    public:
      Builder& data(const void* inData, u32 inSize);
      Builder& data(const Slice<const void> slice);
      Builder& size(u32 inSize);
      Builder& storage(StorageType type);
      Builder& access(AccessType type);
      UniformBuffer::Handle build();

    private:
      Builder(u32 binding)
        : mBinding(binding)
      {}

    private:
      const void* mData = nullptr;
      u32 mSize = 0;

      StorageType mStorage = StorageType::Static;
      AccessType  mAccess  = AccessType::Draw;

      u32 mBinding;

      friend class UniformBuffer;
    };

  public:
    [[nodiscard]] static UniformBuffer::Builder builder(u32 binding);
    DISALLOW_COPY_AND_ASSIGN(UniformBuffer);
    ~UniformBuffer();

    void bind();
    void unbind();

    void set(const Slice<const void> slice);

  private:
    UniformBuffer(u32 id, u32 size, u32 binding)
      : mId{id}, mSize(size), mBinding(binding)
    {}

  private:
    u32 mId;
    u32 mSize;
    u32 mBinding;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(UniformBuffer)

} // namespace Game