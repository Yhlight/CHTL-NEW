#pragma once

#include "antlr4-runtime.h"

class CSS3Lexer : public antlr4::Lexer {
public:
    enum {
        WS = 1, COMMENT = 2, CDO = 3, CDC = 4, INCLUDES = 5, DASHMATCH = 6,
        STRING = 7, IDENT = 8, HASH = 9, IMPORT_SYM = 10, PAGE_SYM = 11,
        MEDIA_SYM = 12, CHARSET_SYM = 13, NAMESPACE_SYM = 14, SUPPORTS_SYM = 15,
        DOCUMENT_SYM = 16, UNKNOWN_SYM = 17, PLUS = 18, GREATER = 19, COMMA = 20,
        TILDE = 21, PIPE = 22, LBRACE = 23, RBRACE = 24, LPAREN = 25, RPAREN = 26,
        LBRACKET = 27, RBRACKET = 28, SEMICOLON = 29, COLON = 30, SOLIDUS = 31,
        MINUS = 32, DOT = 33, UNDERSCORE = 34, PERCENTAGE = 35, URI = 36,
        FUNCTION = 37, DIMENSION = 38, NUMBER = 39, UNICODERANGE = 40, VENDOR_PREFIX = 41
    };

    explicit CSS3Lexer(antlr4::CharStream *input);
    ~CSS3Lexer() override;

    std::string getGrammarFileName() const override;
    const std::vector<std::string>& getRuleNames() const override;
    const std::vector<std::string>& getChannelNames() const override;
    const std::vector<std::string>& getModeNames() const override;
    const std::vector<std::string>& getTokenNames() const override;
    antlr4::dfa::Vocabulary& getVocabulary() const override;
    antlr4::atn::SerializedATNView getSerializedATN() const override;
    const antlr4::atn::ATN& getATN() const override;

private:
    static std::vector<antlr4::dfa::DFA> _decisionToDFA;
    static antlr4::atn::PredictionContextCache _sharedContextCache;
    static std::vector<std::string> _ruleNames;
    static std::vector<std::string> _tokenNames;
    static std::vector<std::string> _channelNames;
    static std::vector<std::string> _modeNames;
    static antlr4::dfa::Vocabulary _vocabulary;
    static antlr4::atn::ATN _atn;
    static std::vector<uint16_t> _serializedATN;

    void InitializeStaticData();
};