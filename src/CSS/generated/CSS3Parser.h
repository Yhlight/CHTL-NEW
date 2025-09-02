#pragma once

#include "antlr4-runtime.h"
#include "CSS3Lexer.h"

class CSS3Parser : public antlr4::Parser {
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

    enum {
        RuleStylesheet = 0, RuleRuleset = 1, RuleSelector = 2, RuleDeclaration = 3,
        RuleProperty = 4, RuleValue = 5, RuleAtRule = 6, RuleImportRule = 7,
        RuleMediaRule = 8, RulePageRule = 9, RuleCharsetRule = 10, RuleNamespaceRule = 11
    };

    explicit CSS3Parser(antlr4::TokenStream *input);
    ~CSS3Parser() override;

    std::string getGrammarFileName() const override;
    const std::vector<std::string>& getRuleNames() const override;
    const antlr4::dfa::Vocabulary& getVocabulary() const override;
    antlr4::atn::SerializedATNView getSerializedATN() const override;
    const antlr4::atn::ATN& getATN() const override;

    class StylesheetContext;
    class RulesetContext;
    class SelectorContext;
    class DeclarationContext;
    class PropertyContext;
    class ValueContext;
    class AtRuleContext;
    class ImportRuleContext;
    class MediaRuleContext;
    class PageRuleContext;
    class CharsetRuleContext;
    class NamespaceRuleContext;

    StylesheetContext* stylesheet();
    RulesetContext* ruleset();
    SelectorContext* selector();
    DeclarationContext* declaration();
    PropertyContext* property();
    ValueContext* value();
    AtRuleContext* atRule();

    class StylesheetContext : public antlr4::ParserRuleContext {
    public:
        StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        std::vector<RulesetContext *> ruleset();
        RulesetContext* ruleset(size_t i);
        std::vector<AtRuleContext *> atRule();
        AtRuleContext* atRule(size_t i);
    };

    class RulesetContext : public antlr4::ParserRuleContext {
    public:
        RulesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        SelectorContext *selector();
        antlr4::tree::TerminalNode *LBRACE();
        antlr4::tree::TerminalNode *RBRACE();
        std::vector<DeclarationContext *> declaration();
        DeclarationContext* declaration(size_t i);
    };

    class SelectorContext : public antlr4::ParserRuleContext {
    public:
        SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        antlr4::tree::TerminalNode *IDENT();
        antlr4::tree::TerminalNode *HASH();
        antlr4::tree::TerminalNode *DOT();
    };

    class DeclarationContext : public antlr4::ParserRuleContext {
    public:
        DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        PropertyContext *property();
        antlr4::tree::TerminalNode *COLON();
        ValueContext *value();
        antlr4::tree::TerminalNode *SEMICOLON();
    };

    class PropertyContext : public antlr4::ParserRuleContext {
    public:
        PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        antlr4::tree::TerminalNode *IDENT();
    };

    class ValueContext : public antlr4::ParserRuleContext {
    public:
        ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        antlr4::tree::TerminalNode *IDENT();
        antlr4::tree::TerminalNode *STRING();
        antlr4::tree::TerminalNode *NUMBER();
    };

    class AtRuleContext : public antlr4::ParserRuleContext {
    public:
        AtRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
        virtual size_t getRuleIndex() const override;
        ImportRuleContext *importRule();
        MediaRuleContext *mediaRule();
        PageRuleContext *pageRule();
        CharsetRuleContext *charsetRule();
        NamespaceRuleContext *namespaceRule();
    };

private:
    static std::vector<antlr4::dfa::DFA> _decisionToDFA;
    static antlr4::atn::PredictionContextCache _sharedContextCache;
    static std::vector<std::string> _ruleNames;
    static antlr4::dfa::Vocabulary _vocabulary;
    static antlr4::atn::ATN _atn;
    static std::vector<uint16_t> _serializedATN;

    void InitializeStaticData();
};