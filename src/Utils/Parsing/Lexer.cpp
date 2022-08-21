#include "Utils/Parsing/Lexer.hpp"

namespace Game::Utils {

  Lexer::Lexer(const StringView& source)
    : mSource{source}
  {
    mLexerError = next();
  }

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

  static bool isSpace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
  }

  static bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  static bool isAplha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  static bool isAplhanumeric(char c) {
    return isAplha(c) || isDigit(c);
  }

  bool Lexer::lexNumber() {
    GAME_DEBUG_ASSERT(isDigit(currentChar()));
    usize start = mIndex;
    while (nextCharIf(isDigit)) {}
    if (currentChar() == '.') {
      mIndex = start;
      return lexFloat();
    }
    mIndex = start;
    return lexInteger();
  }

  bool Lexer::lexInteger() {
    GAME_DEBUG_ASSERT(isDigit(currentChar()));
    usize start = mIndex;

    u64 base = 10;
    u64 result = 0;
    while (isDigit(currentChar())) {
      result = result * base + (u64)(currentChar() - '0');
      nextChar();
    }

    usize end = mIndex;

    mToken.mType = Token::Type::Integer;
    mToken.mRaw = mSource.substr(start, end - start);
    mToken.mInteger = result;
    return true;
  }

  bool Lexer::lexFloat() {
    usize start = mIndex;
    while (nextCharIf(isDigit)) {}
    GAME_DEBUG_ASSERT(currentChar() == '.');
    nextChar();
    while (nextCharIf(isDigit)) {}
    usize end = mIndex;

    double result = strtod(&mSource[start], NULL);

    mToken.mType = Token::Type::Float;
    mToken.mRaw = mSource.substr(start, end - start);
    mToken.mFloat = result;
    return true;
  }

  bool Lexer::lexIdentifier() {
    usize start = mIndex;
    while (nextCharIf(isAplhanumeric)) {}
    usize end = mIndex;

    mToken.mType = Token::Type::Identifier;
    mToken.mRaw = mSource.substr(start, end - start);
    return true;
  }

  bool Lexer::lexString() {
    usize start = mIndex;
    String string;
    GAME_DEBUG_ASSERT(currentChar() == '"');
    nextChar();
    while (currentChar() != '"') {
      if (currentChar() == '\n' || currentChar() == '\r' || currentChar() == '\t') {
        return false;
      }
      if (currentChar() == '\0') {
        return false;
      }

      string.push_back(currentChar());

      nextChar();
    }
    GAME_DEBUG_ASSERT(currentChar() == '"');
    nextChar();
    usize end = mIndex;

    mToken.mType = Token::Type::String;
    mToken.mRaw = mSource.substr(start, end - start);
    mToken.mString = std::move(string);
    return true;
  }

  bool Lexer::next() {
    do {
      if (!mIgnoreNewline && currentChar() == '\n') {
        GAME_TODO("");
      }
    } while (nextCharIf(isSpace));

    mToken.mLineNumber = mLineNumber;
    mToken.mColumnNumber = mColumnNumber;

    switch (currentChar()) {
      case '\0':
        mToken.mType = Token::Type::Eof;
        mToken.mRaw = mSource.substr(mIndex, 0);
        return true;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
      case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
      case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
      case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
      case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
      case '_':
        return lexIdentifier();

      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        return lexNumber();

      case '"':
        return lexString();

      case '[':
        mToken.mType = Token::Type::OpenBracket;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;
      case ']':
        mToken.mType = Token::Type::CloseBracket;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;
      case '{':
        mToken.mType = Token::Type::OpenBrace;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;
      case '}':
        mToken.mType = Token::Type::CloseBrace;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;
      case ',':
        mToken.mType = Token::Type::Comma;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;
      case ':':
        mToken.mType = Token::Type::Colon;
        mToken.mRaw = mSource.substr(mIndex, 1);
        nextChar();
        return true;

      case ' ':
      case '!':
      case '#':
      case '$':
      case '%':
      case '&':
      case '\'':
      case '(':
      case ')':
      case '*':
      case '+':
      case '-':
      case '.':
      case '/':
      case ';':
      case '<':
      case '=':
      case '>':
      case '?':
      case '@':
      case '\\':
      case '^':
      case '`':
      case '|':
      case '~':
        return false;

      default:
        return false;
    }
  }

} // namespace Game::Utils