#pragma once

#include "Core/Base.hpp"
#include "Resource/Resource.hpp"

#include <vector>

namespace Game {

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
      Bool,
      Mat3,
      Mat4,
    };

  public:
    BufferElement(Type type, bool normalized = false)
      : mType{type}, mNormalized{normalized}
    {}

    inline Type getType() const { return mType; }

    inline constexpr usize getComponentCount() const {
      switch (mType) {
        case Type::Float:  return 1;
        case Type::Float2: return 2;
        case Type::Float3: return 3;
        case Type::Float4: return 4;
        case Type::Int:    return 1;
        case Type::Int2:   return 2;
        case Type::Int3:   return 3;
        case Type::Int4:   return 4;
        case Type::Uint:   return 1;
        case Type::Bool:   return 1;

        case Type::Mat3:   return 3;
        case Type::Mat4:   return 4;
      }
      GAME_UNREACHABLE("Unknown shader data type!");
    }

    inline usize getSize() const {
      switch (mType) {
        case Type::Float:  return sizeof(float)        * 1;
        case Type::Float2: return sizeof(float)        * 2;
        case Type::Float3: return sizeof(float)        * 3;
        case Type::Float4: return sizeof(float)        * 4;
        case Type::Int:    return sizeof(int)          * 1;
        case Type::Int2:   return sizeof(int)          * 2;
        case Type::Int3:   return sizeof(int)          * 3;
        case Type::Int4:   return sizeof(int)          * 4;
        case Type::Uint:   return sizeof(unsigned int) * 1;
        case Type::Bool:   return sizeof(bool);
        case Type::Mat3:   return sizeof(float) * 3 * 3;
        case Type::Mat4:   return sizeof(float) * 4 * 4;
      }
      GAME_UNREACHABLE("Unknown shader data type!");
    }

    inline usize getOffset() const { return mOffset; }
    inline void  setOffset(usize offset) { mOffset = offset; }

    inline bool isNormalized() const { return mNormalized; }
  private:
    Type  mType;
    usize mOffset = 0;
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
    void calculateStride() {
			this->stride = 0;
      usize offset = 0;
			for (auto& element : this->elements) {
        element.setOffset(offset);

				offset       += element.getSize();
        this->stride += element.getSize();
			}
		}

  private:
    usize                      stride = 0;
    std::vector<BufferElement> elements;
  };

  class VertexBuffer {
  public:
    using Handle = Resource<VertexBuffer>;

    class Builder {
    public:
      Builder& data(const void* inData, u32 inSize);
      Builder& data(const Slice<const void> slice);
      Builder& size(u32 inSize);
      Builder& layout(BufferElement::Type type, String name);
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

  private:
    VertexBuffer(u32 id, BufferLayout layout)
      : mId{id}, mLayout{std::move(layout)}
    {}

  private:
    u32 mId;
    BufferLayout mLayout;

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(VertexBuffer)

} // namespace Game