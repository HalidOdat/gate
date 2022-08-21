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

  void Node::toStringWithIndent(std::stringstream& ss, u32 level, bool isArrayParent) const {
    Style style = this->mStyle;
    std::visit(overloaded {
      [&](Null) { ss << "null"; },
      [&](Boolean value) { ss << value ? "true" : "false"; },
      [&](Integer value) { ss << value; },
      [&](Float value) { ss << std::fixed << value; },
      [&](const String& value) { ss << std::quoted(value); },
      [&](const Array& array) {
        const auto size = array.size();
        usize i = 0;
        if (style == Node::Style::Expanded) {
          ss << "[\n";
          for (auto& value : array) {
            ss << std::setw(2 * (level + 1)) << "";
            value.toStringWithIndent(ss, level + 1, true);
            if (i + 1 != size) {
              ss << ",";
            }
            ss << "\n";
            i++;
          }
          ss << std::setw(2 * level) << "" << ']';
        } else {
          ss << "[ ";
          for (auto& value : array) {
            value.toStringWithIndent(ss, level + 1, true);
            if (i + 1 != size) {
              ss << ", ";
            }
            i++;
          }
          ss << " ]";
        }
      },
      [&](const Object& object) {
        const auto size = object.size();
        usize i = 0;
        if (style == Node::Style::Expanded) {
          ss << "{\n";
          for (auto&[key, value] : object) {
            ss << std::setw(2 * (level + 1)) << "";
            ss << key << ": ";
            value.toStringWithIndent(ss, level + 1);
            if (i + 1 != size) {
              ss << ",";
            }
            ss << "\n";
            i++;
          }
          ss << std::setw(2 * level) << "" << '}';
        } else {
          ss << "{ ";
          for (auto&[key, value] : object) {
            ss << key << ":";
            if (value.getStyle() != Style::Flat) {
              ss << '\n';
            } else {
              ss << ' ';
            }
            value.toStringWithIndent(ss, level + 1);
            if (i + 1 != size) {
              ss << ", ";
            }
            i++;
          }
          ss << " }";
        }
      },
    }, mValue);
  }

  bool Node::isPrimitive() const {
    return is<Node::Null>() || is<Node::Integer>() || is<Node::Float>() || is<Node::String>();
  }

  Node& Node::operator[](usize i) {
    GAME_DEBUG_ASSERT(is<Node::Array>());
    Node::Array& array = as<Node::Array>();
    GAME_ASSERT(i < array.size());
    return array[i];
  }

  Node& Node::operator[](const String& string) {
    GAME_DEBUG_ASSERT(is<Node::Object>());
    Node::Object& object = as<Node::Object>();
    return object[string];
  }

  bool Node::contains(const String& key) const {
    GAME_DEBUG_ASSERT(is<Node::Object>());
    const Node::Object& object = as<Node::Object>();
    return object.contains(key);
  }

  Node& Node::push(Node node) {
    GAME_DEBUG_ASSERT(is<Node::Array>());
    Node::Array& array = as<Node::Array>();
    return array.emplace_back(std::move(node));
  }

  void Node::pop() {
    GAME_DEBUG_ASSERT(is<Node::Array>());
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

  Json::Json(StringView source)
    : mLexer{source}
  {}

  bool Json::matchToken(Utils::Token::Type type) {
    if (mLexer.current().is(type)) {
      if (!mLexer.next()) {
        Logger::error("Json: couldn't lex source");
      }
      return true;
    }

    return false;
  }
  bool Json::isToken(Utils::Token::Type type) {
    return mLexer.current().is(type);
  }

  Option<Node> Json::parseNode() {
    switch (mLexer.current().getType()) {
      case Utils::Token::Type::Eof:         return None;
      case Utils::Token::Type::OpenBracket: return parseArray();
      case Utils::Token::Type::OpenBrace:   return parseObject();
      case Utils::Token::Type::CloseBracket:
        Logger::error("Json parser: Line %llu: unexpected closing bracket token", mLexer.current().getLineNumber());
        return None;
      case Utils::Token::Type::CloseBrace:
        Logger::error("Json parser: Line %llu: unexpected closing bracket token", mLexer.current().getLineNumber());
        return None;
      case Utils::Token::Type::Integer:     return mLexer.current().getInteger();
      case Utils::Token::Type::Float:       return mLexer.current().getFloat();
      case Utils::Token::Type::String:      return mLexer.current().getString();
      case Utils::Token::Type::Identifier:
        if (mLexer.current().getRaw() == "null") {
          return Node();
        } else if (mLexer.current().getRaw() == "true") {
          return Node(true);
        } else if (mLexer.current().getRaw() == "false") {
          return Node(false);
        } 
        Logger::error("Json parser: Line %llu: unexpected name token", mLexer.current().getLineNumber());
        return None;
      default:
        Logger::error("Json parser: Line %llu: unexpected token", mLexer.current().getLineNumber());
        return None;
    }
  }

  Option<Node> Json::parseArray() {
    if (!matchToken(Utils::Token::Type::OpenBracket)) {
      return None;
    }

    Node node = Node::array();
    while (!isToken(Utils::Token::Type::Eof) && !isToken(Utils::Token::Type::CloseBracket)) {
      Option<Node> value = parseNode();
      if (!value) {
        return None;
      }
      node.push(*value);
      if (!matchToken(Utils::Token::Type::Comma)) {
        break;
      }
    }

    if (!matchToken(Utils::Token::Type::CloseBracket)) {
      Logger::error("Json parser: Line %llu: expected closing bracket", mLexer.current().getLineNumber());
      return None;
    }

    return std::move(node);
  }
  Option<Node> Json::parseObject() {
    if (!matchToken(Utils::Token::Type::OpenBrace)) {
      return None;
    }

    Node node = Node::object();
    while (!isToken(Utils::Token::Type::Eof) && !isToken(Utils::Token::Type::CloseBrace)) {
      StringView key = mLexer.current().getRaw();
      if (!matchToken(Utils::Token::Type::Identifier)) {
        Logger::error("Json parser: Line %llu: expected key name", mLexer.current().getLineNumber());
        return None;
      }
      if (!matchToken(Utils::Token::Type::Colon)) {
        Logger::error("Json parser: Line %llu: expected colon token after key", mLexer.current().getLineNumber());
        return None;
      }

      Option<Node> value = parseNode();
      if (!value) {
        return None;
      }

      node[String(key)] = *value;
      if (!matchToken(Utils::Token::Type::Comma)) {
        break;
      }
    }

    if (!matchToken(Utils::Token::Type::CloseBrace)) {
      Logger::error("Json parser: Line %llu: expected closing brace", mLexer.current().getLineNumber());
      return None;
    }

    return std::move(node);
  }

  Option<Node> Json::parse(StringView source) {
    auto parser = Json(source);
    auto result = parser.parseNode();
    if (parser.mLexer.hasError()) {
      return None;
    }
    return std::move(result);
  }

} // namespace Game::Serializer
