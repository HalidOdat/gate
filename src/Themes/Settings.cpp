#include "Utils/File.hpp"
#include "Themes/Settings.hpp"
#include "Serializer/Serializer.hpp"

#include <cstdlib>

namespace Gate {

  namespace Serializer {

    template<>
    struct Convert<Theme::HexColor> {
      static Node encode(Theme::HexColor value) {
        (void)value;
        GATE_TODO("implement HexColor serialization");
        Node node;
        return node;
      }
      static bool decode(const Node& node, Theme::HexColor& color) {
        const auto* string = node.asString();
        if (!string) {
          return false;
        }
        if (string->size() != 7) {
          return false;
        }
        if (string->at(0) != '#') {
          return false;
        }
        color.value = (u32)strtol(string->c_str() + 1, NULL, 16);
        // set alpha to 255
        color.value <<= 8;
        color.value |= 0xFF;
        return true;
      }
    };
    template<>
    struct Convert<Theme::Board> {
      static Node encode(Theme::Board value) {
        (void)value;
        GATE_TODO("implement board theme serialization");
        Node node;
        return node;
      }
      static bool decode(const Node& node, Theme::Board& board) {
        if (!node.isObject()) {
          return false;
        }

        if (const auto* background = node.get("background"); background) {
          if (!Convert<Theme::HexColor>::decode(*background, board.background)) return false;
        }
        if (const auto* points = node.get("points"); points) {
          if (!Convert<Theme::HexColor>::decode(*points, board.points)) return false;
        }
        return true;
      }
    };
    template<>
    struct Convert<Theme::Wire> {
      static Node encode(Theme::Wire value) {
        (void)value;
        GATE_TODO("implement wire theme serialization");
        Node node;
        return node;
      }
      static bool decode(const Node& node, Theme::Wire& wire) {
        if (!node.isObject()) {
          return false;
        }

        if (const auto* active = node.get("active"); active) {
          if (!Convert<Theme::HexColor>::decode(*active, wire.active)) return false;
        }
        if (const auto* inactive = node.get("inactive"); inactive) {
          if (!Convert<Theme::HexColor>::decode(*inactive, wire.inactive)) return false;
        }
        if (const auto* invalid = node.get("invalid"); invalid) {
          if (!Convert<Theme::HexColor>::decode(*invalid, wire.invalid)) return false;
        }
        return true;
      }
    };
    template<>
    struct Convert<Theme::Text> {
      static Node encode(Theme::Text value) {
        (void)value;
        GATE_TODO("implement Text theme serialization");
        Node node;
        return node;
      }
      static bool decode(const Node& node, Theme::Text& text) {
        if (!node.isObject()) {
          return false;
        }
        if (const auto* color = node.get("color"); color) {
          if (!Convert<Theme::HexColor>::decode(*color, text.color)) return false;
        }
        return true;
      }
    };
    template<>
    struct Convert<Theme::Settings> {
      static Node encode(Theme::Settings value) {
        (void)value;
        GATE_TODO("implement theme settings serialization");
        Node node;
        return node;
      }
      static bool decode(const Node& node, Theme::Settings& settings) {
        if (!node.isObject()) {
          return false;
        }
        
        if (const auto* board = node.get("board"); board) {
          if (!Convert<Theme::Board>::decode(*board, settings.board)) return false;
        }
        if (const auto* wire = node.get("wire"); wire) {
          if (!Convert<Theme::Wire>::decode(*wire, settings.wire)) return false;
        }
        if (const auto* text = node.get("text"); text) {
          if (!Convert<Theme::Text>::decode(*text, settings.text)) return false;
        }
        return true;
      }
    };

  } // namespace Serializer

  namespace Theme {

    Option<Settings> Settings::load(const char* filepath) {
      auto* content = Utils::fileToString(filepath);
      if (!content) {
        Logger::info("cannot open file %s", filepath);
        return None;
      }
      const auto node = Serializer::Json::parse(content);
      free(content);
      if (!node) {
        Logger::error("Json: invalid json");
        return None;
      }
      Settings settings;
      if (!Serializer::Convert<Settings>::decode(node, settings)) {
        Logger::error("Theme: invalid settings");
        return None;
      }
      return settings;
    }

  } // namespace Theme

} // namespace Gate