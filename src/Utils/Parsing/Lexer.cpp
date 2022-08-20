#include "Utils/Parsing/Lexer.hpp"

namespace Game::Utils {

  Lexer::Lexer(const StringView& source)
    : mSource{source}
  {}

  void Lexer::enableIgnoreNewline(bool yes) {
    mIgnoreNewline = yes;
  }

  char Lexer::nextChar() {
    if (mIndex + 1 >= mSource.size()) {
      return '\0';
    }
    const char result = mSource[++mIndex];
    if (result == '\n') {
      mLineNumber++;
      mColumnNumber = 1;
    }
    return result;
  }
  char Lexer::peekChar() {
    if (mIndex + 1 >= mSource.size()) {
      return '\0';
    }
    return mSource[mIndex + 1];
  }

  Token Lexer::next() {
    // if (peekChar) {
    // 
    // }
    GAME_UNREACHABLE("");
  }

} // namespace Game::Utils