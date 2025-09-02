
// Generated from CSS.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSSLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    CHARSET_SYM = 15, IMPORT_SYM = 16, NAMESPACE_SYM = 17, AT_KEYWORD = 18, 
    IMPORTANT_SYM = 19, STRING = 20, IDENT = 21, NUMBER = 22, PERCENTAGE = 23, 
    LENGTH = 24, EMS = 25, EXS = 26, ANGLE = 27, TIME = 28, FREQ = 29, DIMENSION = 30, 
    URI = 31, HASH = 32, UNICODE_RANGE = 33, INCLUDES = 34, DASHMATCH = 35, 
    PREFIXMATCH = 36, SUFFIXMATCH = 37, SUBSTRINGMATCH = 38, FUNCTION = 39, 
    DELIM = 40, PLUS = 41, GREATER = 42, TILDE = 43, S = 44, COMMENT = 45, 
    CDO = 46, CDC = 47, WS = 48
  };

  explicit CSSLexer(antlr4::CharStream *input);

  ~CSSLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

