#pragma once

#include <cstdint>
#include <cstddef>

#include <memory>

namespace Game {

  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  using i8  = std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using f32 = float;
  using f64 = double;

  using usize = std::size_t;

  #define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;      \
    TypeName& operator=(const TypeName&) = delete

  #define DISALLOW_MOVE_AND_ASSIGN(TypeName)  \
    TypeName(TypeName&&) = delete;      \
    TypeName& operator=(TypeName&&) = delete

  #define DISALLOW_MOVE_AND_COPY(TypeName) \
    DISALLOW_MOVE_AND_ASSIGN(TypeName);    \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

  template<typename T>
  using Ref = std::shared_ptr<T>;

  template<typename T>
  class CreateObject {
  public:
    template<typename ...Args>
    inline static Ref<T> Create(Args&&... args) {
      return Ref<T>(new T(std::forward<Args>(args)...));
    }
  };

} // namespace Game
