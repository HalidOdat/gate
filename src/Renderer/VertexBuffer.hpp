#pragma once

#include <vector>

#include "Core/Assert.hpp"
#include "Core/Type.hpp"

#include "Resource/Resource.hpp"

namespace Game {

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
    };

  public:
    BufferElement(Type type, bool normalized = false)
      : mType{type}, mNormalized{normalized}
    {
    }

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

  public:
    [[nodiscard]] static VertexBuffer::Handle create(Slice<const void> slice);
    [[nodiscard]] static VertexBuffer::Handle withSize(const u32 size);
    DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
    ~VertexBuffer();

    void bind();
    void unbind();

    void set(const Slice<const void> slice);

    inline const BufferLayout& getLayout() const { return mLayout; }
    inline void setLayout(BufferLayout layout) { mLayout = std::move(layout); }

  private:
    VertexBuffer(u32 id)
      : mId{id}
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