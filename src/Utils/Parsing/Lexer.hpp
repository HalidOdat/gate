#pragma once

#include "Core/Base.hpp"

namespace Gate::Utils {

  class Token {
  public:
    enum class Type {
      Eof,
      Integer,
      Float,
      Identifier,
      String,
      OpenBracket,
      CloseBracket,
      OpenBrace,
      CloseBrace,
      Comma,
      Colon,
      Minus,
    };

  public:
    inline Type getType() const { return mType; }
    inline const StringView& getRaw() const { return mRaw; }
    inline u32 getLineNumber() const { return mLineNumber; }
    inline u32 getColumnNumber() const { return mColumnNumber; }

    inline bool is(Type type) const { return mType == type; }

    const String& getString() const { return mString; }
    f64 getFloat() const { return mFloat; }
    u64 getInteger() const { return mInteger; }

  private:
    Type       mType;
    StringView mRaw;
    u32        mLineNumber;
    u32        mColumnNumber;

    u64    mInteger;
    f64    mFloat;
    String mString;

  private:
    friend class Lexer;
  };

  class Lexer {
  public:
    using NextCharIfPredicate = auto(*)(char) -> bool;

  public:
    Lexer(const StringView& source);

    void enableIgnoreNewline(bool yes = true);

    const Token& current() const { return mToken; }
    bool next();

    inline bool hasError() const { return mLexerError; }

  private:
    char currentChar();
    char nextChar();
    char peekChar();

    template<typename F>
    inline bool nextCharIf(F& f) {
      if (f(mSource[mIndex])) {
        nextChar();
        return true;
      }
      return false;
    }

    bool lexNumber();
    bool lexInteger();
    bool lexFloat();
    bool lexIdentifier();
    bool lexString();

  private:
    StringView mSource;
    usize      mIndex = 0;

    u32 mLineNumber     = 1;
    u32 mColumnNumber   = 1;

    Token mToken;

    bool mIgnoreNewline = true;

    bool mFinished = false;
    bool mLexerError = false;
  };

} // namespace Gate::Utils