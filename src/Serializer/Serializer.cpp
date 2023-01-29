#include "Core/Base.hpp"
#include "Serializer/Serializer.hpp"

#include <sstream>
#include <iomanip>

namespace Gate::Serializer {

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
      [&](Boolean value) { ss << (value ? "true" : "false"); },
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
            value.toStringWithIndent(ss, level + 1);
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
            value.toStringWithIndent(ss, level + 1);
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
            ss << std::quoted(key) << ": ";
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

  Node Node::array() {
    return { Node::Array{} };
  }
  Node Node::object() {
    return { Node::Object{} };
  }

  bool Node::isNull() const {
    return std::holds_alternative<Node::Null>(mValue);
  }
  bool Node::isBoolean() const {
    return std::holds_alternative<Node::Boolean>(mValue);
  }
  bool Node::isFloat() const {
    return std::holds_alternative<Node::Float>(mValue);
  }
  bool Node::isInteger() const {
    return std::holds_alternative<Node::Integer>(mValue);
  }
  bool Node::isString() const {
    return std::holds_alternative<Node::String>(mValue);
  }
  bool Node::isArray() const {
    return std::holds_alternative<Node::Array>(mValue);
  }
  bool Node::isObject() const {
    return std::holds_alternative<Node::Object>(mValue);
  }

  bool Node::isPrimitive() const {
    return !isArray() && !isObject();
  }

  Node::Boolean* Node::asBoolean() {
    return std::get_if<Node::Boolean>(&mValue);
  }
  Node::Float*   Node::asFloat() {
    return std::get_if<Node::Float>(&mValue);
  }
  Node::Integer* Node::asInteger() {
    return std::get_if<Node::Integer>(&mValue);
  }
  Node::String*  Node::asString() {
    return std::get_if<Node::String>(&mValue);
  }
  Node::Array*   Node::asArray() {
    return std::get_if<Node::Array>(&mValue);
  }
  Node::Object*  Node::asObject() {
    return std::get_if<Node::Object>(&mValue);
  }
  const Node::Boolean* Node::asBoolean() const {
    return std::get_if<Node::Boolean>(&mValue);
  }
  const Node::Float*   Node::asFloat() const {
    return std::get_if<Node::Float>(&mValue);
  }
  const Node::Integer* Node::asInteger() const {
    return std::get_if<Node::Integer>(&mValue);
  }
  const Node::String*  Node::asString() const {
    return std::get_if<Node::String>(&mValue);
  }
  const Node::Array*   Node::asArray() const {
    return std::get_if<Node::Array>(&mValue);
  }
  const Node::Object*  Node::asObject() const {
    return std::get_if<Node::Object>(&mValue);
  }

  const Node& Node::operator[](usize i) const {
    const auto* array = asArray();
    GAME_DEBUG_ASSERT(array != nullptr);
    GAME_ASSERT(i < array->size());
    return (*array)[i];
  }

  Node& Node::operator[](usize i) {
    auto* array = asArray();
    GAME_DEBUG_ASSERT(array != nullptr);
    GAME_ASSERT(i < array->size());
    return (*array)[i];
  }

  Node& Node::operator[](const String& string) {
    GAME_DEBUG_ASSERT(isObject());
    auto* object = asObject();
    return (*object)[string];
  }

  const Node* Node::get(const String& key) const {
    GAME_DEBUG_ASSERT(isObject());
    const Node::Object* object = asObject();
    if (!object->contains(key)) {
      return nullptr;
    }
    return &object->at(key);
  }

  bool Node::contains(const String& key) const {
    GAME_DEBUG_ASSERT(isObject());
    const Node::Object* object = asObject();
    return object->contains(key);
  }

  Node& Node::push(Node node) {
    GAME_DEBUG_ASSERT(isArray());
    Node::Array* array = asArray();
    return array->emplace_back(std::move(node));
  }

  void Node::pop() {
    GAME_DEBUG_ASSERT(isArray());
    Node::Array* array = asArray();
    GAME_ASSERT(array->size() != 0);
    array->pop_back();
  }

  usize Node::size() const {
    GAME_ASSERT(isArray() || isObject());
    if (const Node::Array* array = asArray(); array) {
      return array->size();
    } else if (const Node::Object* object = asObject(); object) {
      return object->size();
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
    Option<Node> result;
    switch (mLexer.current().getType()) {
      case Utils::Token::Type::OpenBracket: return parseArray();
      case Utils::Token::Type::OpenBrace:   return parseObject();
      case Utils::Token::Type::CloseBracket:
        Logger::error("Json parser: Line %llu: unexpected closing bracket token", mLexer.current().getLineNumber());
        result = None;
        break;
      case Utils::Token::Type::CloseBrace:
        Logger::error("Json parser: Line %llu: unexpected closing bracket token", mLexer.current().getLineNumber());
        result = None;
        break;
      case Utils::Token::Type::Integer:
        result = mLexer.current().getInteger();
        break;
      case Utils::Token::Type::Minus: {
        if (!mLexer.next()) {
          Logger::error("Json parser: unexpected token '-' prefix");
          return None;
        }
        switch (mLexer.current().getType()) {
          case Utils::Token::Type::Integer:
            result = - (i64)mLexer.current().getInteger();
            break;
          case Utils::Token::Type::Float:
            result = - mLexer.current().getFloat();
            break;
          default:
            Logger::error("Json parser: unexpected token '-' prefix");
        }
      }
        break;
      case Utils::Token::Type::Float:
        result = mLexer.current().getFloat();
        break;
      case Utils::Token::Type::String:
        result = mLexer.current().getString();
        break;
      case Utils::Token::Type::Identifier:
        if (mLexer.current().getRaw() == "null") {
          result = Node();
        } else if (mLexer.current().getRaw() == "true") {
          result = Node(true);
        } else if (mLexer.current().getRaw() == "false") {
          result = Node(false);
        } else {
          Logger::error("Json parser: Line %llu: unexpected name token", mLexer.current().getLineNumber());
          result = None;
        }
        break;
      case Utils::Token::Type::Eof:
        Logger::error("Json parser: Line %llu: unexpected EOF", mLexer.current().getLineNumber());
        result = None;
        break;
      default:
        Logger::error("Json parser: Line %llu: unexpected token", mLexer.current().getLineNumber());
        result = None;
        break;
    }

    if (result.has_value()) {
      if (!mLexer.next()) {
        Logger::error("Json: couldn't lex source");
        return None;
      }
    }

    return result;
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

    return node;
  }
  Option<Node> Json::parseObject() {
    if (!matchToken(Utils::Token::Type::OpenBrace)) {
      return None;
    }

    Node node = Node::object();
    while (!isToken(Utils::Token::Type::Eof) && !isToken(Utils::Token::Type::CloseBrace)) {
      String key = mLexer.current().getString();
      if (!matchToken(Utils::Token::Type::String)) {
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

      node[key] = *value;
      if (!matchToken(Utils::Token::Type::Comma)) {
        break;
      }
    }

    if (!matchToken(Utils::Token::Type::CloseBrace)) {
      Logger::error("Json parser: Line %llu: expected closing brace", mLexer.current().getLineNumber());
      return None;
    }

    return node;
  }

  Option<Node> Json::parse(StringView source) {
    auto parser = Json(source);
    auto result = parser.parseNode();
    if (parser.mLexer.hasError()) {
      Logger::error("Json: Lexer: error");
      return None;
    }
    return result;
  }

} // namespace Gate::Serializer
