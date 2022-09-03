#pragma once

#include <cstdint>
#include <cstddef>

#include <array>
#include <string>
#include <string_view>
#include <memory>
#include <optional>

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

  using usize = size_t;
  using isize = intptr_t;

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

  template<typename T, usize N>
  using Array      = std::array<T, N>;

  using String     = std::string;
  using StringView = std::string_view;

  template<typename T>
  using Option     = std::optional<T>;
  static constexpr const std::nullopt_t None = std::nullopt;

  template<typename T>
  class Slice {
  public:
    template <typename U = T, usize N,
      std::enable_if_t<
        !std::is_same_v<std::remove_cv_t<U>, void> &&
         std::is_same_v<std::remove_cv_t<U>, std::remove_cv_t<T>>,
      bool> = true
    >
    inline constexpr Slice(U (&array)[N])
      : _data{array}, _size{N}
    {}

    template <typename U = T, usize N,
      std::enable_if_t<
        !std::is_same_v<std::remove_cv_t<U>, void> &&
         std::is_same_v<std::remove_cv_t<T>, void>,
      bool> = true
    >
    inline constexpr Slice(U (&array)[N])
      : _data{array}, _size{N * sizeof(U)}
    {}

    template <typename U = T,
      std::enable_if_t<
        !std::is_same_v<std::remove_cv_t<U>, void> &&
         std::is_same_v<std::remove_cv_t<U>, std::remove_cv_t<T>>,
      bool> = true
    >
    inline constexpr Slice(U* data, usize length)
      : _data{data}, _size{length}
    {}

    template <typename U = T,
      std::enable_if_t<
        !std::is_same_v<std::remove_cv_t<U>, void> &&
         std::is_same_v<std::remove_cv_t<T>, void>,
      bool> = true
    >
    inline constexpr Slice(U* data, usize length)
      : _data{data}, _size{length * sizeof(U)}
    {}

    inline constexpr const T* data()   const { return this->_data; }
    inline constexpr T* data() { return this->_data; }
    inline constexpr usize size() const { return this->_size; }
    
    template <typename U = T, std::enable_if_t<!std::is_same_v<std::remove_cv_t<U>, void>, bool> = true>
    inline constexpr usize sizeInBytes() const { return sizeof(U) * this->_size; }

    template <typename U = T, std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, void>, bool> = true>
    inline constexpr usize sizeInBytes() const { return this->_size; }
    

    inline constexpr T* begin() { return _data; }
    inline constexpr T* end()   { return _data + _size; }
    inline constexpr const T* cbegin() const { return _data; }
    inline constexpr const T* cend()   const { return _data + _size; }

  private:
    T*    _data;
    usize _size;
  };

} // namespace Game
