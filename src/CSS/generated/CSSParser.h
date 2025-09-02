
// Generated from CSS.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  CSSParser : public antlr4::Parser {
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

  enum {
    RuleStylesheet = 0, RuleCharset = 1, RuleImports = 2, RuleNamespace = 3, 
    RuleMediaList = 4, RuleMedium = 5, RuleCssRule = 6, RuleAtRule = 7, 
    RuleBlock = 8, RuleSelector = 9, RuleSimpleSelector = 10, RuleElementName = 11, 
    RuleClass_ = 12, RuleAttrib = 13, RulePseudo = 14, RuleCombinator = 15, 
    RuleDeclaration = 16, RuleProperty = 17, RulePrio = 18, RuleExpr = 19, 
    RuleTerm = 20, RuleUnaryOperator = 21, RuleOperator = 22, RuleHexcolor = 23, 
    RuleAny = 24
  };

  explicit CSSParser(antlr4::TokenStream *input);

  CSSParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~CSSParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class StylesheetContext;
  class CharsetContext;
  class ImportsContext;
  class NamespaceContext;
  class MediaListContext;
  class MediumContext;
  class CssRuleContext;
  class AtRuleContext;
  class BlockContext;
  class SelectorContext;
  class SimpleSelectorContext;
  class ElementNameContext;
  class Class_Context;
  class AttribContext;
  class PseudoContext;
  class CombinatorContext;
  class DeclarationContext;
  class PropertyContext;
  class PrioContext;
  class ExprContext;
  class TermContext;
  class UnaryOperatorContext;
  class OperatorContext;
  class HexcolorContext;
  class AnyContext; 

  class  StylesheetContext : public antlr4::ParserRuleContext {
  public:
    StylesheetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<CharsetContext *> charset();
    CharsetContext* charset(size_t i);
    std::vector<ImportsContext *> imports();
    ImportsContext* imports(size_t i);
    std::vector<NamespaceContext *> namespace_();
    NamespaceContext* namespace_(size_t i);
    std::vector<CssRuleContext *> cssRule();
    CssRuleContext* cssRule(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StylesheetContext* stylesheet();

  class  CharsetContext : public antlr4::ParserRuleContext {
  public:
    CharsetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CHARSET_SYM();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CharsetContext* charset();

  class  ImportsContext : public antlr4::ParserRuleContext {
  public:
    ImportsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORT_SYM();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();
    MediaListContext *mediaList();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ImportsContext* imports();

  class  NamespaceContext : public antlr4::ParserRuleContext {
  public:
    NamespaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NAMESPACE_SYM();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *URI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NamespaceContext* namespace_();

  class  MediaListContext : public antlr4::ParserRuleContext {
  public:
    MediaListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MediumContext *> medium();
    MediumContext* medium(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MediaListContext* mediaList();

  class  MediumContext : public antlr4::ParserRuleContext {
  public:
    MediumContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MediumContext* medium();

  class  CssRuleContext : public antlr4::ParserRuleContext {
  public:
    CssRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SelectorContext *> selector();
    SelectorContext* selector(size_t i);
    std::vector<DeclarationContext *> declaration();
    DeclarationContext* declaration(size_t i);
    AtRuleContext *atRule();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CssRuleContext* cssRule();

  class  AtRuleContext : public antlr4::ParserRuleContext {
  public:
    AtRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT_KEYWORD();
    BlockContext *block();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AtRuleContext* atRule();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);
    std::vector<BlockContext *> block();
    BlockContext* block(size_t i);
    std::vector<AtRuleContext *> atRule();
    AtRuleContext* atRule(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BlockContext* block();

  class  SelectorContext : public antlr4::ParserRuleContext {
  public:
    SelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SimpleSelectorContext *> simpleSelector();
    SimpleSelectorContext* simpleSelector(size_t i);
    std::vector<CombinatorContext *> combinator();
    CombinatorContext* combinator(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SelectorContext* selector();

  class  SimpleSelectorContext : public antlr4::ParserRuleContext {
  public:
    SimpleSelectorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ElementNameContext *elementName();
    std::vector<antlr4::tree::TerminalNode *> HASH();
    antlr4::tree::TerminalNode* HASH(size_t i);
    std::vector<Class_Context *> class_();
    Class_Context* class_(size_t i);
    std::vector<AttribContext *> attrib();
    AttribContext* attrib(size_t i);
    std::vector<PseudoContext *> pseudo();
    PseudoContext* pseudo(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SimpleSelectorContext* simpleSelector();

  class  ElementNameContext : public antlr4::ParserRuleContext {
  public:
    ElementNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ElementNameContext* elementName();

  class  Class_Context : public antlr4::ParserRuleContext {
  public:
    Class_Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Class_Context* class_();

  class  AttribContext : public antlr4::ParserRuleContext {
  public:
    AttribContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENT();
    antlr4::tree::TerminalNode* IDENT(size_t i);
    antlr4::tree::TerminalNode *INCLUDES();
    antlr4::tree::TerminalNode *DASHMATCH();
    antlr4::tree::TerminalNode *PREFIXMATCH();
    antlr4::tree::TerminalNode *SUFFIXMATCH();
    antlr4::tree::TerminalNode *SUBSTRINGMATCH();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AttribContext* attrib();

  class  PseudoContext : public antlr4::ParserRuleContext {
  public:
    PseudoContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *FUNCTION();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PseudoContext* pseudo();

  class  CombinatorContext : public antlr4::ParserRuleContext {
  public:
    CombinatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *GREATER();
    antlr4::tree::TerminalNode *TILDE();
    antlr4::tree::TerminalNode *S();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CombinatorContext* combinator();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PropertyContext *property();
    ExprContext *expr();
    PrioContext *prio();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DeclarationContext* declaration();

  class  PropertyContext : public antlr4::ParserRuleContext {
  public:
    PropertyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PropertyContext* property();

  class  PrioContext : public antlr4::ParserRuleContext {
  public:
    PrioContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORTANT_SYM();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PrioContext* prio();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    std::vector<OperatorContext *> operator_();
    OperatorContext* operator_(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExprContext* expr();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *LENGTH();
    antlr4::tree::TerminalNode *EMS();
    antlr4::tree::TerminalNode *EXS();
    antlr4::tree::TerminalNode *ANGLE();
    antlr4::tree::TerminalNode *TIME();
    antlr4::tree::TerminalNode *FREQ();
    antlr4::tree::TerminalNode *DIMENSION();
    UnaryOperatorContext *unaryOperator();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *URI();
    HexcolorContext *hexcolor();
    antlr4::tree::TerminalNode *FUNCTION();
    ExprContext *expr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TermContext* term();

  class  UnaryOperatorContext : public antlr4::ParserRuleContext {
  public:
    UnaryOperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  UnaryOperatorContext* unaryOperator();

  class  OperatorContext : public antlr4::ParserRuleContext {
  public:
    OperatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OperatorContext* operator_();

  class  HexcolorContext : public antlr4::ParserRuleContext {
  public:
    HexcolorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  HexcolorContext* hexcolor();

  class  AnyContext : public antlr4::ParserRuleContext {
  public:
    AnyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENT();
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *LENGTH();
    antlr4::tree::TerminalNode *EMS();
    antlr4::tree::TerminalNode *EXS();
    antlr4::tree::TerminalNode *ANGLE();
    antlr4::tree::TerminalNode *TIME();
    antlr4::tree::TerminalNode *FREQ();
    antlr4::tree::TerminalNode *DIMENSION();
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *DELIM();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *HASH();
    antlr4::tree::TerminalNode *UNICODE_RANGE();
    antlr4::tree::TerminalNode *INCLUDES();
    antlr4::tree::TerminalNode *DASHMATCH();
    antlr4::tree::TerminalNode *PREFIXMATCH();
    antlr4::tree::TerminalNode *SUFFIXMATCH();
    antlr4::tree::TerminalNode *SUBSTRINGMATCH();
    antlr4::tree::TerminalNode *FUNCTION();
    std::vector<AnyContext *> any();
    AnyContext* any(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AnyContext* any();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

