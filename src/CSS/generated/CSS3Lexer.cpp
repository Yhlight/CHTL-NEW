#include "CSS3Lexer.h"

using namespace antlr4;

std::vector<dfa::DFA> CSS3Lexer::_decisionToDFA;
atn::PredictionContextCache CSS3Lexer::_sharedContextCache;
std::vector<std::string> CSS3Lexer::_ruleNames;
std::vector<std::string> CSS3Lexer::_tokenNames;
std::vector<std::string> CSS3Lexer::_channelNames;
std::vector<std::string> CSS3Lexer::_modeNames;
dfa::Vocabulary CSS3Lexer::_vocabulary;
atn::ATN CSS3Lexer::_atn;
std::vector<uint16_t> CSS3Lexer::_serializedATN;

void CSS3Lexer::InitializeStaticData() {
    static std::once_flag flag;
    std::call_once(flag, []() {
        _ruleNames = {
            "WS", "COMMENT", "CDO", "CDC", "INCLUDES", "DASHMATCH", "STRING", 
            "IDENT", "HASH", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM", "CHARSET_SYM",
            "NAMESPACE_SYM", "SUPPORTS_SYM", "DOCUMENT_SYM", "UNKNOWN_SYM", 
            "PLUS", "GREATER", "COMMA", "TILDE", "PIPE", "LBRACE", "RBRACE",
            "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "SEMICOLON", "COLON",
            "SOLIDUS", "MINUS", "DOT", "UNDERSCORE", "PERCENTAGE", "URI",
            "FUNCTION", "DIMENSION", "NUMBER", "UNICODERANGE", "VENDOR_PREFIX"
        };

        _tokenNames = {
            "<INVALID>", "WS", "COMMENT", "'<!--'", "'-->'", "'~='", "'|='",
            "STRING", "IDENT", "HASH", "'@import'", "'@page'", "'@media'",
            "'@charset'", "'@namespace'", "'@supports'", "'@document'", "UNKNOWN_SYM",
            "'+'", "'>'", "','", "'~'", "'|'", "'{'", "'}'", "'('", "')'",
            "'['", "']'", "';'", "':'", "'/'", "'-'", "'.'", "'_'", "'%'",
            "URI", "FUNCTION", "DIMENSION", "NUMBER", "UNICODERANGE", "VENDOR_PREFIX"
        };

        _channelNames = {
            "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
        };

        _modeNames = {
            "DEFAULT_MODE"
        };

        _vocabulary = dfa::Vocabulary(_tokenNames, {});
    });
}

CSS3Lexer::CSS3Lexer(CharStream *input) : Lexer(input) {
    InitializeStaticData();
    _interpreter = new atn::LexerATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

CSS3Lexer::~CSS3Lexer() {
    delete _interpreter;
}

std::string CSS3Lexer::getGrammarFileName() const {
    return "css3Lexer.g4";
}

const std::vector<std::string>& CSS3Lexer::getRuleNames() const {
    return _ruleNames;
}

const std::vector<std::string>& CSS3Lexer::getChannelNames() const {
    return _channelNames;
}

const std::vector<std::string>& CSS3Lexer::getModeNames() const {
    return _modeNames;
}

const std::vector<std::string>& CSS3Lexer::getTokenNames() const {
    return _tokenNames;
}

dfa::Vocabulary& CSS3Lexer::getVocabulary() const {
    return const_cast<dfa::Vocabulary&>(_vocabulary);
}

atn::SerializedATNView CSS3Lexer::getSerializedATN() const {
    return atn::SerializedATNView(_serializedATN);
}

const atn::ATN& CSS3Lexer::getATN() const {
    return _atn;
}