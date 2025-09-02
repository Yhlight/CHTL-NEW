
// Generated from CSS.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by CSSParser.
 */
class  CSSListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStylesheet(CSSParser::StylesheetContext *ctx) = 0;
  virtual void exitStylesheet(CSSParser::StylesheetContext *ctx) = 0;

  virtual void enterCharset(CSSParser::CharsetContext *ctx) = 0;
  virtual void exitCharset(CSSParser::CharsetContext *ctx) = 0;

  virtual void enterImports(CSSParser::ImportsContext *ctx) = 0;
  virtual void exitImports(CSSParser::ImportsContext *ctx) = 0;

  virtual void enterNamespace(CSSParser::NamespaceContext *ctx) = 0;
  virtual void exitNamespace(CSSParser::NamespaceContext *ctx) = 0;

  virtual void enterMediaList(CSSParser::MediaListContext *ctx) = 0;
  virtual void exitMediaList(CSSParser::MediaListContext *ctx) = 0;

  virtual void enterMedium(CSSParser::MediumContext *ctx) = 0;
  virtual void exitMedium(CSSParser::MediumContext *ctx) = 0;

  virtual void enterCssRule(CSSParser::CssRuleContext *ctx) = 0;
  virtual void exitCssRule(CSSParser::CssRuleContext *ctx) = 0;

  virtual void enterAtRule(CSSParser::AtRuleContext *ctx) = 0;
  virtual void exitAtRule(CSSParser::AtRuleContext *ctx) = 0;

  virtual void enterBlock(CSSParser::BlockContext *ctx) = 0;
  virtual void exitBlock(CSSParser::BlockContext *ctx) = 0;

  virtual void enterSelector(CSSParser::SelectorContext *ctx) = 0;
  virtual void exitSelector(CSSParser::SelectorContext *ctx) = 0;

  virtual void enterSimpleSelector(CSSParser::SimpleSelectorContext *ctx) = 0;
  virtual void exitSimpleSelector(CSSParser::SimpleSelectorContext *ctx) = 0;

  virtual void enterElementName(CSSParser::ElementNameContext *ctx) = 0;
  virtual void exitElementName(CSSParser::ElementNameContext *ctx) = 0;

  virtual void enterClass_(CSSParser::Class_Context *ctx) = 0;
  virtual void exitClass_(CSSParser::Class_Context *ctx) = 0;

  virtual void enterAttrib(CSSParser::AttribContext *ctx) = 0;
  virtual void exitAttrib(CSSParser::AttribContext *ctx) = 0;

  virtual void enterPseudo(CSSParser::PseudoContext *ctx) = 0;
  virtual void exitPseudo(CSSParser::PseudoContext *ctx) = 0;

  virtual void enterCombinator(CSSParser::CombinatorContext *ctx) = 0;
  virtual void exitCombinator(CSSParser::CombinatorContext *ctx) = 0;

  virtual void enterDeclaration(CSSParser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(CSSParser::DeclarationContext *ctx) = 0;

  virtual void enterProperty(CSSParser::PropertyContext *ctx) = 0;
  virtual void exitProperty(CSSParser::PropertyContext *ctx) = 0;

  virtual void enterPrio(CSSParser::PrioContext *ctx) = 0;
  virtual void exitPrio(CSSParser::PrioContext *ctx) = 0;

  virtual void enterExpr(CSSParser::ExprContext *ctx) = 0;
  virtual void exitExpr(CSSParser::ExprContext *ctx) = 0;

  virtual void enterTerm(CSSParser::TermContext *ctx) = 0;
  virtual void exitTerm(CSSParser::TermContext *ctx) = 0;

  virtual void enterUnaryOperator(CSSParser::UnaryOperatorContext *ctx) = 0;
  virtual void exitUnaryOperator(CSSParser::UnaryOperatorContext *ctx) = 0;

  virtual void enterOperator(CSSParser::OperatorContext *ctx) = 0;
  virtual void exitOperator(CSSParser::OperatorContext *ctx) = 0;

  virtual void enterHexcolor(CSSParser::HexcolorContext *ctx) = 0;
  virtual void exitHexcolor(CSSParser::HexcolorContext *ctx) = 0;

  virtual void enterAny(CSSParser::AnyContext *ctx) = 0;
  virtual void exitAny(CSSParser::AnyContext *ctx) = 0;


};

