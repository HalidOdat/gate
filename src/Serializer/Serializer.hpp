#pragma once

#include "Core/Type.hpp"
#include <variant>
#include <unordered_map>
#include <sstream>

namespace Game::Serializer {

  template<typename T>
  struct Convert;

  class Node {
  public:
    using Null    = std::nullptr_t;
    using Integer = i64;
    using Float   = f64;
    using String  = std::string;
    using Array   = std::vector<Node>;
    using Object  = std::unordered_map<String, Node>;
  
    enum class Style {
      Expanded,
      Flat,
    };

  public:
    Node()
      : mValue{nullptr}
    {}

    template<typename T>
    Node(const T& value)
      : mValue(nullptr)
    {
      *this = Convert<T>::encode(value);
    }

    static Node array() {
      return { Node::Array{} };
    }
    static Node object() {
      return { Node::Object{} };
    }

    template<typename T>
    bool is() const {
      return std::holds_alternative<T>(mValue);
    }

    template<typename T>
    T& as() {
      return std::get<T>(mValue);
    }
    template<typename T>
    const T& as() const {
      return std::get<T>(mValue);
    }

    template<typename T>
    T* as_if() {
      return std::get_if<T>(&mValue);
    }
    template<typename T>
    const T* as_if() const {
      return std::get_if<T>(&mValue);
    }

    // TODO: implement decode
    // template<typename T>
    // bool decode(T& value) {
    //   return Convert<T>::decode(value);
    // }

    Node& operator[](usize i);
    Node& operator[](const String& string);

    bool contains(const String& key) const;

    Node& push(Node node);
    void pop();

    inline Style getStyle() const { return mStyle; }
    inline void setStyle(Style style) { mStyle = style; }

    usize size() const;

    String toString() const;

  private:
    void toStringWithIndent(std::stringstream& ss, u32 level) const;

  private:
    std::variant<
      Null,
      Integer,
      Float,
      String,
      Array,
      Object
    > mValue;

    Style mStyle = Style::Expanded;
  
  private:

    friend struct Convert<Node::Integer>;
    friend struct Convert<Node::Float>;
    friend struct Convert<f32>;
    friend struct Convert<Node::String>;
    friend struct Convert<Node::Array>;
    friend struct Convert<Node::Object>;
  };

  template<typename T>
  struct Convert {
    static Node encode(const T&) {
      static_assert(std::is_same_v<T, void>, "Unknown type");
    }
  };

  template<>
  struct Convert<Node> {
    static Node encode(Node value) {
      return value;
    }
  };

  template<>
  struct Convert<Node::Integer> {
    static Node encode(Node::Integer value) {
      Node node;
      node.mValue = value;
      return node;
    }
  };

  template<>
  struct Convert<Node::Float> {
    static Node encode(Node::Float value) {
      Node node;
      node.mValue = value;
      return node;
    }
  };

  template<>
  struct Convert<f32> {
    static Node encode(f32 value) {
      Node node;
      node.mValue = (Node::Float)value;
      return node;
    }
  };

  template<>
  struct Convert<Node::String> {
    static Node encode(Node::String value) {
      Node node;
      node.mValue = value;
      return node;
    }
  };

  template<>
  struct Convert<Node::Array> {
    static Node encode(const Node::Array& value) {
      Node node;
      node.mValue = value;
      return node;
    }
  };

  template<>
  struct Convert<Node::Object> {
    static Node encode(const Node::Object& value) {
      Node node;
      node.mValue = value;
      return node;
    }
  };

  template<typename T>
  struct Convert<Option<T>> {
    static Node encode(const Option<T>& value) {
      if (value) {
        return *value;
      }

      return Node{};
    }
  };

  template<typename T>
  struct Convert<std::vector<T>> {
    static Node encode(const std::vector<T>& value) {
      auto node = Node::array();
      for (auto& element : value) {
        node.push(element);
      }
      return node;
    }
  };

  template<typename V>
  struct Convert<std::unordered_map<String, V>> {
    static Node encode(const std::unordered_map<String, V>& value) {
      auto node = Node::object();
      for (auto&[key, element] : value) {
        node[key] = element;
      }
      return node;
    }
  };
  
} // namespace Game::Serializer
