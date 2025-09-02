
// Generated from CSS.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "CSSListener.h"


/**
 * This class provides an empty implementation of CSSListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  CSSBaseListener : public CSSListener {
public:

  virtual void enterStylesheet(CSSParser::StylesheetContext * /*ctx*/) override { }
  virtual void exitStylesheet(CSSParser::StylesheetContext * /*ctx*/) override { }

  virtual void enterCharset(CSSParser::CharsetContext * /*ctx*/) override { }
  virtual void exitCharset(CSSParser::CharsetContext * /*ctx*/) override { }

  virtual void enterImports(CSSParser::ImportsContext * /*ctx*/) override { }
  virtual void exitImports(CSSParser::ImportsContext * /*ctx*/) override { }

  virtual void enterNamespace(CSSParser::NamespaceContext * /*ctx*/) override { }
  virtual void exitNamespace(CSSParser::NamespaceContext * /*ctx*/) override { }

  virtual void enterMediaList(CSSParser::MediaListContext * /*ctx*/) override { }
  virtual void exitMediaList(CSSParser::MediaListContext * /*ctx*/) override { }

  virtual void enterMedium(CSSParser::MediumContext * /*ctx*/) override { }
  virtual void exitMedium(CSSParser::MediumContext * /*ctx*/) override { }

  virtual void enterCssRule(CSSParser::CssRuleContext * /*ctx*/) override { }
  virtual void exitCssRule(CSSParser::CssRuleContext * /*ctx*/) override { }

  virtual void enterAtRule(CSSParser::AtRuleContext * /*ctx*/) override { }
  virtual void exitAtRule(CSSParser::AtRuleContext * /*ctx*/) override { }

  virtual void enterBlock(CSSParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(CSSParser::BlockContext * /*ctx*/) override { }

  virtual void enterSelector(CSSParser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(CSSParser::SelectorContext * /*ctx*/) override { }

  virtual void enterSimpleSelector(CSSParser::SimpleSelectorContext * /*ctx*/) override { }
  virtual void exitSimpleSelector(CSSParser::SimpleSelectorContext * /*ctx*/) override { }

  virtual void enterElementName(CSSParser::ElementNameContext * /*ctx*/) override { }
  virtual void exitElementName(CSSParser::ElementNameContext * /*ctx*/) override { }

  virtual void enterClass_(CSSParser::Class_Context * /*ctx*/) override { }
  virtual void exitClass_(CSSParser::Class_Context * /*ctx*/) override { }

  virtual void enterAttrib(CSSParser::AttribContext * /*ctx*/) override { }
  virtual void exitAttrib(CSSParser::AttribContext * /*ctx*/) override { }

  virtual void enterPseudo(CSSParser::PseudoContext * /*ctx*/) override { }
  virtual void exitPseudo(CSSParser::PseudoContext * /*ctx*/) override { }

  virtual void enterCombinator(CSSParser::CombinatorContext * /*ctx*/) override { }
  virtual void exitCombinator(CSSParser::CombinatorContext * /*ctx*/) override { }

  virtual void enterDeclaration(CSSParser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(CSSParser::DeclarationContext * /*ctx*/) override { }

  virtual void enterProperty(CSSParser::PropertyContext * /*ctx*/) override { }
  virtual void exitProperty(CSSParser::PropertyContext * /*ctx*/) override { }

  virtual void enterPrio(CSSParser::PrioContext * /*ctx*/) override { }
  virtual void exitPrio(CSSParser::PrioContext * /*ctx*/) override { }

  virtual void enterExpr(CSSParser::ExprContext * /*ctx*/) override { }
  virtual void exitExpr(CSSParser::ExprContext * /*ctx*/) override { }

  virtual void enterTerm(CSSParser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(CSSParser::TermContext * /*ctx*/) override { }

  virtual void enterUnaryOperator(CSSParser::UnaryOperatorContext * /*ctx*/) override { }
  virtual void exitUnaryOperator(CSSParser::UnaryOperatorContext * /*ctx*/) override { }

  virtual void enterOperator(CSSParser::OperatorContext * /*ctx*/) override { }
  virtual void exitOperator(CSSParser::OperatorContext * /*ctx*/) override { }

  virtual void enterHexcolor(CSSParser::HexcolorContext * /*ctx*/) override { }
  virtual void exitHexcolor(CSSParser::HexcolorContext * /*ctx*/) override { }

  virtual void enterAny(CSSParser::AnyContext * /*ctx*/) override { }
  virtual void exitAny(CSSParser::AnyContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

