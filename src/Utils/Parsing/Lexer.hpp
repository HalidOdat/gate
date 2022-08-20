#pragma once

#include "Core/Base.hpp"

namespace Game::Utils {

  class Token {
  public:
    enum class Type {
      Eof,
    };

  public:
    inline Type getType() const { return mType; }
    inline const StringView& getRaw() const { return mRaw; }
    inline u32 getLineNumber() const { return mLineNumber; }
    inline u32 getColumnNumber() const { return mColumnNumber; }

    inline bool is(Type type) const { return mType == type; }

  private:
    Type       mType;
    StringView mRaw;
    u32        mLineNumber;
    u32        mColumnNumber;

  private:
    friend class Lexer;
  };

  class Lexer {
  public:
    using NextCharIfPredicate = auto(*)(char) -> bool;

  public:
    Lexer(const StringView& source);

    void enableIgnoreNewline(bool yes = true);

    Token next();

  private:
    char nextChar();
    char peekChar();

    template<typename F>
    inline bool nextCharIf(F& f) {
      if (f(peekChar())) {
        nextChar();
        return true;
      }
      return false;
    }

  private:
    StringView mSource;
    u32 mIndex = 0;

    u32 mLineNumber     = 1;
    u32 mColumnNumber   = 1;

    bool mIgnoreNewline = true;
  };

} // namespace Game::Utils