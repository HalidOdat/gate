#include "Core/Base.hpp"
#include "Serializer/Serializer.hpp"

#include <sstream>
#include <iomanip>

namespace Game::Serializer {

  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
  template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

  String Node::toString() const {
    std::stringstream ss;
    toStringWithIndent(ss, 0);
    return ss.str();
  }

  void Node::toStringWithIndent(std::stringstream& ss, u32 level) const {
    Style style = this->mStyle;
    std::visit(overloaded {
      [&](Null) { ss << "null"; },
      [&](Integer value) { ss << value; },
      [&](Float value) { ss << std::fixed << value; },
      [&](const String& value) { ss << std::quoted(value); },
      [&](const Array& array) {
        ss << "[ ";
        if (style == Node::Style::Expanded) {
          ss << '\n';
        }
        const auto size = array.size();
        usize i = 0;
        for (auto& value : array) {
          if (style == Node::Style::Expanded) {
            ss << std::setw(2 * (level + 1)) << "";
          }
          value.toStringWithIndent(ss, level + 1);
          if (i + 1 != size) {
            ss << ", ";
          }
          if (style == Node::Style::Expanded) {
            ss << '\n';
          }
          i++;
        }
        if (style == Node::Style::Expanded) {
          ss << std::setw(2 * level) << "" << ']';
        } else {
          ss << " ]";
        }
      },
      [&](const Object& object) {
        ss << "{ ";
        if (style == Node::Style::Expanded) {
          ss << '\n';
        }
        const auto size = object.size();
        usize i = 0;
        for (auto&[key, value] : object) {
          if (style == Node::Style::Expanded) {
            ss << std::setw(2 * (level + 1)) << "";
          }
          ss << key << ": ";
          value.toStringWithIndent(ss, level + 1);
          if (i + 1 != size) {
            ss << ", ";
          }
          if (style == Node::Style::Expanded) {
            ss << "\n";
          }
          i++;
        }
        if (style == Node::Style::Expanded) {
          ss << std::setw(2 * level) << "" << '}';
        } else {
          ss << " }";
        }
      },
    }, mValue);
  }

  Node& Node::operator[](usize i) {
    GAME_DEBUG_ASSERT(is<Node::Array>(), "");
    Node::Array& array = as<Node::Array>();
    GAME_ASSERT(i < array.size());
    return array[i];
  }

  Node& Node::operator[](const String& string) {
    GAME_DEBUG_ASSERT(is<Node::Object>(), "");
    Node::Object& object = as<Node::Object>();
    return object[string];
  }

  bool Node::contains(const String& key) const {
    GAME_DEBUG_ASSERT(is<Node::Object>(), "");
    const Node::Object& object = as<Node::Object>();
    return object.contains(key);
  }

  Node& Node::push(Node node) {
    GAME_DEBUG_ASSERT(is<Node::Array>(), "");
    Node::Array& array = as<Node::Array>();
    return array.emplace_back(std::move(node));
  }

  void Node::pop() {
    GAME_DEBUG_ASSERT(is<Node::Array>(), "");
    Node::Array& array = as<Node::Array>();
    GAME_ASSERT(array.size() != 0);
    array.pop_back();
  }

  usize Node::size() const {
    GAME_ASSERT(is<Node::Array>() || is<Node::Object>());
    if (const Node::Array* value = as_if<Node::Array>(); value) {
      return value->size();
    } else if (const Node::Object* value = as_if<Node::Object>(); value) {
      return value->size();
    }

    GAME_UNREACHABLE("should be array or object!");
  }

} // namespace Game::Serializer
