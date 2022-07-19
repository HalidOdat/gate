#pragma once

#include <vector>

#include "Core/Assert.hpp"
#include "Core/Type.hpp"

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
      Bool,
    };

  public:
    BufferElement(Type type, bool normalized = false)
      : type{type}, normalized{normalized}
    {
    }

    inline Type getType() const { return this->type; }

    inline constexpr usize getComponentCount() const {
      switch (type) {
        case Type::Float:  return 1;
        case Type::Float2: return 2;
        case Type::Float3: return 3;
        case Type::Float4: return 4;
        case Type::Int:    return 1;
        case Type::Int2:   return 2;
        case Type::Int3:   return 3;
        case Type::Int4:   return 4;
        case Type::Bool:   return 1;
      }

      GAME_UNREACHABLE("Unknown shader data type!");
      return 0;
    }

    inline usize getSize() const {
      switch (type) {
        case Type::Float:  return sizeof(float) * 1;
        case Type::Float2: return sizeof(float) * 2;
        case Type::Float3: return sizeof(float) * 3;
        case Type::Float4: return sizeof(float) * 4;
        case Type::Int:    return sizeof(int)   * 1;
        case Type::Int2:   return sizeof(int)   * 2;
        case Type::Int3:   return sizeof(int)   * 3;
        case Type::Int4:   return sizeof(int)   * 4;
        case Type::Bool:   return sizeof(bool);
      }

      GAME_UNREACHABLE("Unknown shader data type!");
      return 0;
    }

    inline usize getOffset() const { return this->offset; }
    inline void  setOffset(usize offset) { this->offset = offset; }

    inline bool isNormalized() const { return this->normalized; }
  private:
    Type  type;
    usize offset = 0;
    bool  normalized;
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
    [[nodiscard]] static Ref<VertexBuffer> create(Slice<const void> slice);
    DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
    ~VertexBuffer();

    void bind();
    void unbind();

    inline const BufferLayout& getLayout() const { return this->layout; }
    inline void setLayout(BufferLayout layout) { this->layout = std::move(layout); }

  private:
    VertexBuffer(Slice<const void> slice);

  private:
    u32 id;
    BufferLayout layout;
  };

} // namespace Game