#pragma once

#include "Core/Base.hpp"

namespace Gate {

  struct Buffer {
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
  };

  class BufferElement {
  public:
    enum class Type {
      Float,
      Float2,
      Float3,
      Float4,
      Int,
      Int2,
      Int3,
      Int4,
      Uint,
      Uint4,
      Bool,
      Mat3,
      Mat4,
    };

  public:
    BufferElement(Type type, u32 attributeDivisor, bool normalized = false)
      : mType{type}, mAttributeDivisor{attributeDivisor},  mNormalized{normalized}
    {}

    inline Type getType() const { return mType; }

    usize getComponentCount() const;
    usize getSize() const;

    inline usize getOffset() const { return mOffset; }
    inline void  setOffset(usize offset) { mOffset = offset; }

    inline usize getAttributeDivisor() const { return mAttributeDivisor; }

    inline bool isNormalized() const { return mNormalized; }
  private:
    Type  mType;
    usize mOffset = 0;
    u32   mAttributeDivisor;
    bool  mNormalized;
  };

  class BufferLayout {
  public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> elements)
      : elements{elements}
    {
      calculateStride();
    }

    BufferLayout(std::vector<BufferElement> elements)
      : elements{elements}
    {
      calculateStride();
    }

    inline usize getStride() const { return this->stride; }
    inline const std::vector<BufferElement>& getElements() const { return this->elements; }

  private:
    void calculateStride();

  private:
    usize                      stride = 0;
    std::vector<BufferElement> elements;
  };

  class VertexBuffer {
  public:
    using Handle = std::shared_ptr<VertexBuffer>;

    class Builder {
    public:
      Builder& data(const void* inData, u32 inSize);
      Builder& data(const Slice<const void> slice);
      Builder& size(u32 inSize);
      Builder& layout(BufferElement::Type type, u32 attributeDivisor = 0);
      Builder& storage(Buffer::StorageType type);
      Builder& access(Buffer::AccessType type);
      VertexBuffer::Handle build();

    private:
      Builder() = default;

    private:
      const void* mData = nullptr;
      u32 mSize = 0;
      std::vector<BufferElement> mLayout;

      Buffer::StorageType mStorage = Buffer::StorageType::Static;
      Buffer::AccessType  mAccess  = Buffer::AccessType::Draw;

      friend class VertexBuffer;
    };

  public:
    [[nodiscard]] static VertexBuffer::Builder builder();
    DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
    ~VertexBuffer();

    void bind();
    void unbind();

    void set(const Slice<const void> slice);

    inline const BufferLayout& getLayout() const { return mLayout; }
    inline void setLayout(BufferLayout layout) { mLayout = std::move(layout); }

  public:
    // DO NOT USE! Use the builder!
    //
    // NOTE: It has to be public so it can be constructed by std::make_shared.
    VertexBuffer(u32 id, BufferLayout layout)
      : mId{id}, mLayout{std::move(layout)}
    {}

  private:
    u32 mId;
    BufferLayout mLayout;
  };

} // namespace Gate