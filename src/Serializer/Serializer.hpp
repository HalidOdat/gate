#pragma once

#include "Core/Type.hpp"
#include "Utils/Parsing/Lexer.hpp"
#include <variant>
#include <unordered_map>
#include <sstream>

namespace Game::Serializer {

  template<typename T>
  struct Convert;

  class Node {
  public:
    using Null    = std::nullptr_t;
    using Boolean = bool;
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

    static Node array();
    static Node object();

    bool isNull() const;
    bool isBoolean() const;
    bool isFloat() const;
    bool isInteger() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    bool isPrimitive() const;

    Node::Boolean* asBoolean();
    Node::Float*   asFloat();
    Node::Integer* asInteger();
    Node::String*  asString();
    Node::Array*   asArray();
    Node::Object*  asObject();
    const Node::Boolean* asBoolean() const;
    const Node::Float*   asFloat() const;
    const Node::Integer* asInteger() const;
    const Node::String*  asString() const;
    const Node::Array*   asArray() const;
    const Node::Object*  asObject() const;

    const Node& operator[](usize i) const;
    Node& operator[](usize i);
    Node& operator[](const String& string);

    bool contains(const String& key) const;
    const Node* get(const String& key) const;

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
      Boolean,
      Integer,
      Float,
      String,
      Array,
      Object
    > mValue;

    Style mStyle = Style::Expanded;
  
  private:

    friend struct Convert<Node::Boolean>;
    friend struct Convert<Node::Integer>;
    friend struct Convert<Node::Float>;
    friend struct Convert<f32>;
    friend struct Convert<Node::String>;
    friend struct Convert<Node::Array>;
    friend struct Convert<Node::Object>;
  };

  template<typename T>
  struct Convert {
    static Node encode(const T& value) {
      (void)value;
      static_assert(std::is_same_v<T, void>, "Unknown type");
      return {};
    }
    static bool decode(const Node& node, T& value) {
      (void)node;
      (void)value;
      static_assert(std::is_same_v<T, void>, "Unknown type");
      return false;
    }
  };

  template<>
  struct Convert<Node> {
    static Node encode(Node value) {
      return value;
    }
    static bool decode(const Node& node, Node& value) {
      value = node;
      return true;
    }
  };

  template<>
  struct Convert<Node::Boolean> {
    static Node encode(Node::Boolean value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::Boolean& value) {
      if (node.isBoolean()) {
        value = *node.asBoolean();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<Node::Integer> {
    static Node encode(Node::Integer value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::Integer& value) {
      if (node.isInteger()) {
        value = *node.asInteger();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<u64> {
    static Node encode(u64 value) {
      return (Node::Integer)value;
    }
    static bool decode(const Node& node, u64& value) {
      if (node.isInteger()) {
        value = *node.asInteger();
        return true;
      }
      return false;
    }
  };


  template<>
  struct Convert<Node::Float> {
    static Node encode(Node::Float value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::Float& value) {
      if (node.isFloat()) {
        value = *node.asFloat();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<f32> {
    static Node encode(f32 value) {
      Node node;
      node.mValue = (Node::Float)value;
      return node;
    }
    static bool decode(const Node& node, f32& value) {
      if (node.isFloat()) {
        value = (f32)*node.asFloat();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<Node::String> {
    static Node encode(Node::String value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::String& value) {
      if (node.isString()) {
        value = *node.asString();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<Node::Array> {
    static Node encode(const Node::Array& value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::Array& value) {
      if (node.isArray()) {
        value = *node.asArray();
        return true;
      }
      return false;
    }
  };

  template<>
  struct Convert<Node::Object> {
    static Node encode(const Node::Object& value) {
      Node node;
      node.mValue = value;
      return node;
    }
    static bool decode(const Node& node, Node::Object& value) {
      if (node.isObject()) {
        value = *node.asObject();
        return true;
      }
      return false;
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
    static bool decode(const Node& node, Option<T>& value) {
      if (node.isNull()) {
        value = None;
        return true;
      }

      return Convert<T>::decode(node, *value);
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

  class Json {
  public:
    static Option<Node> parse(StringView source);
  
  private:
    Json(StringView source);

    bool matchToken(Utils::Token::Type type);
    bool isToken(Utils::Token::Type type);

    void nextToken();

    Option<Node> parseNode();
    Option<Node> parseArray();
    Option<Node> parseObject();

  private:
    Utils::Lexer mLexer;
  };
  
} // namespace Game::Serializer
