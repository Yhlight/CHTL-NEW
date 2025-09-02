
// Generated from CSS.g4 by ANTLR 4.13.2


#include "CSSListener.h"

#include "CSSParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct CSSParserStaticData final {
  CSSParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CSSParserStaticData(const CSSParserStaticData&) = delete;
  CSSParserStaticData(CSSParserStaticData&&) = delete;
  CSSParserStaticData& operator=(const CSSParserStaticData&) = delete;
  CSSParserStaticData& operator=(CSSParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag cssParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<CSSParserStaticData> cssParserStaticData = nullptr;

void cssParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (cssParserStaticData != nullptr) {
    return;
  }
#else
  assert(cssParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CSSParserStaticData>(
    std::vector<std::string>{
      "stylesheet", "charset", "imports", "namespace", "mediaList", "medium", 
      "cssRule", "atRule", "block", "selector", "simpleSelector", "elementName", 
      "class_", "attrib", "pseudo", "combinator", "declaration", "property", 
      "prio", "expr", "term", "unaryOperator", "operator", "hexcolor", "any"
    },
    std::vector<std::string>{
      "", "';'", "','", "'{'", "'}'", "'*'", "'.'", "'['", "'='", "']'", 
      "':'", "')'", "'-'", "'/'", "'('", "'@charset'", "'@import'", "'@namespace'", 
      "", "'!important'", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "'~='", "'|='", "'^='", "'$='", "'*='", "", "", "'+'", "'>'", 
      "'~'", "", "", "'<!--'", "'-->'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "CHARSET_SYM", 
      "IMPORT_SYM", "NAMESPACE_SYM", "AT_KEYWORD", "IMPORTANT_SYM", "STRING", 
      "IDENT", "NUMBER", "PERCENTAGE", "LENGTH", "EMS", "EXS", "ANGLE", 
      "TIME", "FREQ", "DIMENSION", "URI", "HASH", "UNICODE_RANGE", "INCLUDES", 
      "DASHMATCH", "PREFIXMATCH", "SUFFIXMATCH", "SUBSTRINGMATCH", "FUNCTION", 
      "DELIM", "PLUS", "GREATER", "TILDE", "S", "COMMENT", "CDO", "CDC", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,48,278,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,1,0,1,0,1,0,5,0,55,8,0,10,0,12,0,
  	58,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,2,3,2,69,8,2,1,2,1,2,1,3,1,3,
  	3,3,75,8,3,1,3,1,3,1,4,1,4,1,4,5,4,82,8,4,10,4,12,4,85,9,4,1,5,1,5,1,
  	6,1,6,1,6,5,6,92,8,6,10,6,12,6,95,9,6,1,6,1,6,5,6,99,8,6,10,6,12,6,102,
  	9,6,1,6,1,6,1,6,3,6,107,8,6,1,7,1,7,5,7,111,8,7,10,7,12,7,114,9,7,1,7,
  	1,7,3,7,118,8,7,1,8,1,8,1,8,1,8,5,8,124,8,8,10,8,12,8,127,9,8,1,8,1,8,
  	1,9,1,9,1,9,1,9,5,9,135,8,9,10,9,12,9,138,9,9,1,10,1,10,1,10,1,10,1,10,
  	5,10,145,8,10,10,10,12,10,148,9,10,1,10,1,10,1,10,1,10,4,10,154,8,10,
  	11,10,12,10,155,3,10,158,8,10,1,11,1,11,1,12,1,12,1,12,1,13,1,13,1,13,
  	1,13,3,13,169,8,13,1,13,1,13,1,14,1,14,3,14,175,8,14,1,14,1,14,1,14,5,
  	14,180,8,14,10,14,12,14,183,9,14,1,14,3,14,186,8,14,1,15,1,15,1,16,1,
  	16,1,16,1,16,3,16,194,8,16,1,17,1,17,1,18,1,18,1,19,1,19,1,19,1,19,5,
  	19,204,8,19,10,19,12,19,207,9,19,1,20,3,20,210,8,20,1,20,1,20,1,20,1,
  	20,1,20,1,20,1,20,1,20,1,20,3,20,221,8,20,1,21,1,21,1,22,1,22,1,22,3,
  	22,228,8,22,1,23,1,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,
  	24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,5,24,254,
  	8,24,10,24,12,24,257,9,24,1,24,1,24,1,24,5,24,262,8,24,10,24,12,24,265,
  	9,24,1,24,1,24,1,24,5,24,270,8,24,10,24,12,24,273,9,24,1,24,3,24,276,
  	8,24,1,24,0,0,25,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,
  	38,40,42,44,46,48,0,8,2,0,20,20,31,31,2,0,20,21,31,31,2,0,5,5,21,21,2,
  	0,8,8,34,38,1,0,20,21,1,0,41,44,1,0,22,30,2,0,12,12,41,41,316,0,56,1,
  	0,0,0,2,61,1,0,0,0,4,65,1,0,0,0,6,72,1,0,0,0,8,78,1,0,0,0,10,86,1,0,0,
  	0,12,106,1,0,0,0,14,108,1,0,0,0,16,119,1,0,0,0,18,130,1,0,0,0,20,157,
  	1,0,0,0,22,159,1,0,0,0,24,161,1,0,0,0,26,164,1,0,0,0,28,172,1,0,0,0,30,
  	187,1,0,0,0,32,189,1,0,0,0,34,195,1,0,0,0,36,197,1,0,0,0,38,199,1,0,0,
  	0,40,220,1,0,0,0,42,222,1,0,0,0,44,227,1,0,0,0,46,229,1,0,0,0,48,275,
  	1,0,0,0,50,55,3,2,1,0,51,55,3,4,2,0,52,55,3,6,3,0,53,55,3,12,6,0,54,50,
  	1,0,0,0,54,51,1,0,0,0,54,52,1,0,0,0,54,53,1,0,0,0,55,58,1,0,0,0,56,54,
  	1,0,0,0,56,57,1,0,0,0,57,59,1,0,0,0,58,56,1,0,0,0,59,60,5,0,0,1,60,1,
  	1,0,0,0,61,62,5,15,0,0,62,63,5,20,0,0,63,64,5,1,0,0,64,3,1,0,0,0,65,66,
  	5,16,0,0,66,68,7,0,0,0,67,69,3,8,4,0,68,67,1,0,0,0,68,69,1,0,0,0,69,70,
  	1,0,0,0,70,71,5,1,0,0,71,5,1,0,0,0,72,74,5,17,0,0,73,75,7,1,0,0,74,73,
  	1,0,0,0,74,75,1,0,0,0,75,76,1,0,0,0,76,77,5,1,0,0,77,7,1,0,0,0,78,83,
  	3,10,5,0,79,80,5,2,0,0,80,82,3,10,5,0,81,79,1,0,0,0,82,85,1,0,0,0,83,
  	81,1,0,0,0,83,84,1,0,0,0,84,9,1,0,0,0,85,83,1,0,0,0,86,87,5,21,0,0,87,
  	11,1,0,0,0,88,93,3,18,9,0,89,90,5,2,0,0,90,92,3,18,9,0,91,89,1,0,0,0,
  	92,95,1,0,0,0,93,91,1,0,0,0,93,94,1,0,0,0,94,96,1,0,0,0,95,93,1,0,0,0,
  	96,100,5,3,0,0,97,99,3,32,16,0,98,97,1,0,0,0,99,102,1,0,0,0,100,98,1,
  	0,0,0,100,101,1,0,0,0,101,103,1,0,0,0,102,100,1,0,0,0,103,104,5,4,0,0,
  	104,107,1,0,0,0,105,107,3,14,7,0,106,88,1,0,0,0,106,105,1,0,0,0,107,13,
  	1,0,0,0,108,112,5,18,0,0,109,111,3,48,24,0,110,109,1,0,0,0,111,114,1,
  	0,0,0,112,110,1,0,0,0,112,113,1,0,0,0,113,117,1,0,0,0,114,112,1,0,0,0,
  	115,118,3,16,8,0,116,118,5,1,0,0,117,115,1,0,0,0,117,116,1,0,0,0,118,
  	15,1,0,0,0,119,125,5,3,0,0,120,124,3,48,24,0,121,124,3,16,8,0,122,124,
  	3,14,7,0,123,120,1,0,0,0,123,121,1,0,0,0,123,122,1,0,0,0,124,127,1,0,
  	0,0,125,123,1,0,0,0,125,126,1,0,0,0,126,128,1,0,0,0,127,125,1,0,0,0,128,
  	129,5,4,0,0,129,17,1,0,0,0,130,136,3,20,10,0,131,132,3,30,15,0,132,133,
  	3,20,10,0,133,135,1,0,0,0,134,131,1,0,0,0,135,138,1,0,0,0,136,134,1,0,
  	0,0,136,137,1,0,0,0,137,19,1,0,0,0,138,136,1,0,0,0,139,146,3,22,11,0,
  	140,145,5,32,0,0,141,145,3,24,12,0,142,145,3,26,13,0,143,145,3,28,14,
  	0,144,140,1,0,0,0,144,141,1,0,0,0,144,142,1,0,0,0,144,143,1,0,0,0,145,
  	148,1,0,0,0,146,144,1,0,0,0,146,147,1,0,0,0,147,158,1,0,0,0,148,146,1,
  	0,0,0,149,154,5,32,0,0,150,154,3,24,12,0,151,154,3,26,13,0,152,154,3,
  	28,14,0,153,149,1,0,0,0,153,150,1,0,0,0,153,151,1,0,0,0,153,152,1,0,0,
  	0,154,155,1,0,0,0,155,153,1,0,0,0,155,156,1,0,0,0,156,158,1,0,0,0,157,
  	139,1,0,0,0,157,153,1,0,0,0,158,21,1,0,0,0,159,160,7,2,0,0,160,23,1,0,
  	0,0,161,162,5,6,0,0,162,163,5,21,0,0,163,25,1,0,0,0,164,165,5,7,0,0,165,
  	168,5,21,0,0,166,167,7,3,0,0,167,169,7,4,0,0,168,166,1,0,0,0,168,169,
  	1,0,0,0,169,170,1,0,0,0,170,171,5,9,0,0,171,27,1,0,0,0,172,174,5,10,0,
  	0,173,175,5,10,0,0,174,173,1,0,0,0,174,175,1,0,0,0,175,185,1,0,0,0,176,
  	186,5,21,0,0,177,181,5,39,0,0,178,180,3,48,24,0,179,178,1,0,0,0,180,183,
  	1,0,0,0,181,179,1,0,0,0,181,182,1,0,0,0,182,184,1,0,0,0,183,181,1,0,0,
  	0,184,186,5,11,0,0,185,176,1,0,0,0,185,177,1,0,0,0,186,29,1,0,0,0,187,
  	188,7,5,0,0,188,31,1,0,0,0,189,190,3,34,17,0,190,191,5,10,0,0,191,193,
  	3,38,19,0,192,194,3,36,18,0,193,192,1,0,0,0,193,194,1,0,0,0,194,33,1,
  	0,0,0,195,196,5,21,0,0,196,35,1,0,0,0,197,198,5,19,0,0,198,37,1,0,0,0,
  	199,205,3,40,20,0,200,201,3,44,22,0,201,202,3,40,20,0,202,204,1,0,0,0,
  	203,200,1,0,0,0,204,207,1,0,0,0,205,203,1,0,0,0,205,206,1,0,0,0,206,39,
  	1,0,0,0,207,205,1,0,0,0,208,210,3,42,21,0,209,208,1,0,0,0,209,210,1,0,
  	0,0,210,211,1,0,0,0,211,221,7,6,0,0,212,221,5,20,0,0,213,221,5,21,0,0,
  	214,221,5,31,0,0,215,221,3,46,23,0,216,217,5,39,0,0,217,218,3,38,19,0,
  	218,219,5,11,0,0,219,221,1,0,0,0,220,209,1,0,0,0,220,212,1,0,0,0,220,
  	213,1,0,0,0,220,214,1,0,0,0,220,215,1,0,0,0,220,216,1,0,0,0,221,41,1,
  	0,0,0,222,223,7,7,0,0,223,43,1,0,0,0,224,228,5,13,0,0,225,228,5,2,0,0,
  	226,228,1,0,0,0,227,224,1,0,0,0,227,225,1,0,0,0,227,226,1,0,0,0,228,45,
  	1,0,0,0,229,230,5,32,0,0,230,47,1,0,0,0,231,276,5,21,0,0,232,276,5,22,
  	0,0,233,276,5,23,0,0,234,276,5,24,0,0,235,276,5,25,0,0,236,276,5,26,0,
  	0,237,276,5,27,0,0,238,276,5,28,0,0,239,276,5,29,0,0,240,276,5,30,0,0,
  	241,276,5,20,0,0,242,276,5,40,0,0,243,276,5,31,0,0,244,276,5,32,0,0,245,
  	276,5,33,0,0,246,276,5,34,0,0,247,276,5,35,0,0,248,276,5,36,0,0,249,276,
  	5,37,0,0,250,276,5,38,0,0,251,255,5,39,0,0,252,254,3,48,24,0,253,252,
  	1,0,0,0,254,257,1,0,0,0,255,253,1,0,0,0,255,256,1,0,0,0,256,258,1,0,0,
  	0,257,255,1,0,0,0,258,276,5,11,0,0,259,263,5,14,0,0,260,262,3,48,24,0,
  	261,260,1,0,0,0,262,265,1,0,0,0,263,261,1,0,0,0,263,264,1,0,0,0,264,266,
  	1,0,0,0,265,263,1,0,0,0,266,276,5,11,0,0,267,271,5,7,0,0,268,270,3,48,
  	24,0,269,268,1,0,0,0,270,273,1,0,0,0,271,269,1,0,0,0,271,272,1,0,0,0,
  	272,274,1,0,0,0,273,271,1,0,0,0,274,276,5,9,0,0,275,231,1,0,0,0,275,232,
  	1,0,0,0,275,233,1,0,0,0,275,234,1,0,0,0,275,235,1,0,0,0,275,236,1,0,0,
  	0,275,237,1,0,0,0,275,238,1,0,0,0,275,239,1,0,0,0,275,240,1,0,0,0,275,
  	241,1,0,0,0,275,242,1,0,0,0,275,243,1,0,0,0,275,244,1,0,0,0,275,245,1,
  	0,0,0,275,246,1,0,0,0,275,247,1,0,0,0,275,248,1,0,0,0,275,249,1,0,0,0,
  	275,250,1,0,0,0,275,251,1,0,0,0,275,259,1,0,0,0,275,267,1,0,0,0,276,49,
  	1,0,0,0,31,54,56,68,74,83,93,100,106,112,117,123,125,136,144,146,153,
  	155,157,168,174,181,185,193,205,209,220,227,255,263,271,275
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  cssParserStaticData = std::move(staticData);
}

}

CSSParser::CSSParser(TokenStream *input) : CSSParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

CSSParser::CSSParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  CSSParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *cssParserStaticData->atn, cssParserStaticData->decisionToDFA, cssParserStaticData->sharedContextCache, options);
}

CSSParser::~CSSParser() {
  delete _interpreter;
}

const atn::ATN& CSSParser::getATN() const {
  return *cssParserStaticData->atn;
}

std::string CSSParser::getGrammarFileName() const {
  return "CSS.g4";
}

const std::vector<std::string>& CSSParser::getRuleNames() const {
  return cssParserStaticData->ruleNames;
}

const dfa::Vocabulary& CSSParser::getVocabulary() const {
  return cssParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView CSSParser::getSerializedATN() const {
  return cssParserStaticData->serializedATN;
}


//----------------- StylesheetContext ------------------------------------------------------------------

CSSParser::StylesheetContext::StylesheetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::StylesheetContext::EOF() {
  return getToken(CSSParser::EOF, 0);
}

std::vector<CSSParser::CharsetContext *> CSSParser::StylesheetContext::charset() {
  return getRuleContexts<CSSParser::CharsetContext>();
}

CSSParser::CharsetContext* CSSParser::StylesheetContext::charset(size_t i) {
  return getRuleContext<CSSParser::CharsetContext>(i);
}

std::vector<CSSParser::ImportsContext *> CSSParser::StylesheetContext::imports() {
  return getRuleContexts<CSSParser::ImportsContext>();
}

CSSParser::ImportsContext* CSSParser::StylesheetContext::imports(size_t i) {
  return getRuleContext<CSSParser::ImportsContext>(i);
}

std::vector<CSSParser::NamespaceContext *> CSSParser::StylesheetContext::namespace_() {
  return getRuleContexts<CSSParser::NamespaceContext>();
}

CSSParser::NamespaceContext* CSSParser::StylesheetContext::namespace_(size_t i) {
  return getRuleContext<CSSParser::NamespaceContext>(i);
}

std::vector<CSSParser::CssRuleContext *> CSSParser::StylesheetContext::cssRule() {
  return getRuleContexts<CSSParser::CssRuleContext>();
}

CSSParser::CssRuleContext* CSSParser::StylesheetContext::cssRule(size_t i) {
  return getRuleContext<CSSParser::CssRuleContext>(i);
}


size_t CSSParser::StylesheetContext::getRuleIndex() const {
  return CSSParser::RuleStylesheet;
}

void CSSParser::StylesheetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStylesheet(this);
}

void CSSParser::StylesheetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStylesheet(this);
}

CSSParser::StylesheetContext* CSSParser::stylesheet() {
  StylesheetContext *_localctx = _tracker.createInstance<StylesheetContext>(_ctx, getState());
  enterRule(_localctx, 0, CSSParser::RuleStylesheet);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 4297557216) != 0)) {
      setState(54);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSSParser::CHARSET_SYM: {
          setState(50);
          charset();
          break;
        }

        case CSSParser::IMPORT_SYM: {
          setState(51);
          imports();
          break;
        }

        case CSSParser::NAMESPACE_SYM: {
          setState(52);
          namespace_();
          break;
        }

        case CSSParser::T__4:
        case CSSParser::T__5:
        case CSSParser::T__6:
        case CSSParser::T__9:
        case CSSParser::AT_KEYWORD:
        case CSSParser::IDENT:
        case CSSParser::HASH: {
          setState(53);
          cssRule();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(58);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(59);
    match(CSSParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CharsetContext ------------------------------------------------------------------

CSSParser::CharsetContext::CharsetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::CharsetContext::CHARSET_SYM() {
  return getToken(CSSParser::CHARSET_SYM, 0);
}

tree::TerminalNode* CSSParser::CharsetContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}


size_t CSSParser::CharsetContext::getRuleIndex() const {
  return CSSParser::RuleCharset;
}

void CSSParser::CharsetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCharset(this);
}

void CSSParser::CharsetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCharset(this);
}

CSSParser::CharsetContext* CSSParser::charset() {
  CharsetContext *_localctx = _tracker.createInstance<CharsetContext>(_ctx, getState());
  enterRule(_localctx, 2, CSSParser::RuleCharset);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(61);
    match(CSSParser::CHARSET_SYM);
    setState(62);
    match(CSSParser::STRING);
    setState(63);
    match(CSSParser::T__0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportsContext ------------------------------------------------------------------

CSSParser::ImportsContext::ImportsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::ImportsContext::IMPORT_SYM() {
  return getToken(CSSParser::IMPORT_SYM, 0);
}

tree::TerminalNode* CSSParser::ImportsContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::ImportsContext::URI() {
  return getToken(CSSParser::URI, 0);
}

CSSParser::MediaListContext* CSSParser::ImportsContext::mediaList() {
  return getRuleContext<CSSParser::MediaListContext>(0);
}


size_t CSSParser::ImportsContext::getRuleIndex() const {
  return CSSParser::RuleImports;
}

void CSSParser::ImportsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImports(this);
}

void CSSParser::ImportsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImports(this);
}

CSSParser::ImportsContext* CSSParser::imports() {
  ImportsContext *_localctx = _tracker.createInstance<ImportsContext>(_ctx, getState());
  enterRule(_localctx, 4, CSSParser::RuleImports);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(65);
    match(CSSParser::IMPORT_SYM);
    setState(66);
    _la = _input->LA(1);
    if (!(_la == CSSParser::STRING

    || _la == CSSParser::URI)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(68);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::IDENT) {
      setState(67);
      mediaList();
    }
    setState(70);
    match(CSSParser::T__0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NamespaceContext ------------------------------------------------------------------

CSSParser::NamespaceContext::NamespaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::NamespaceContext::NAMESPACE_SYM() {
  return getToken(CSSParser::NAMESPACE_SYM, 0);
}

tree::TerminalNode* CSSParser::NamespaceContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::NamespaceContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::NamespaceContext::URI() {
  return getToken(CSSParser::URI, 0);
}


size_t CSSParser::NamespaceContext::getRuleIndex() const {
  return CSSParser::RuleNamespace;
}

void CSSParser::NamespaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNamespace(this);
}

void CSSParser::NamespaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNamespace(this);
}

CSSParser::NamespaceContext* CSSParser::namespace_() {
  NamespaceContext *_localctx = _tracker.createInstance<NamespaceContext>(_ctx, getState());
  enterRule(_localctx, 6, CSSParser::RuleNamespace);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    match(CSSParser::NAMESPACE_SYM);
    setState(74);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2150629376) != 0)) {
      setState(73);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 2150629376) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(76);
    match(CSSParser::T__0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MediaListContext ------------------------------------------------------------------

CSSParser::MediaListContext::MediaListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::MediumContext *> CSSParser::MediaListContext::medium() {
  return getRuleContexts<CSSParser::MediumContext>();
}

CSSParser::MediumContext* CSSParser::MediaListContext::medium(size_t i) {
  return getRuleContext<CSSParser::MediumContext>(i);
}


size_t CSSParser::MediaListContext::getRuleIndex() const {
  return CSSParser::RuleMediaList;
}

void CSSParser::MediaListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMediaList(this);
}

void CSSParser::MediaListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMediaList(this);
}

CSSParser::MediaListContext* CSSParser::mediaList() {
  MediaListContext *_localctx = _tracker.createInstance<MediaListContext>(_ctx, getState());
  enterRule(_localctx, 8, CSSParser::RuleMediaList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    medium();
    setState(83);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == CSSParser::T__1) {
      setState(79);
      match(CSSParser::T__1);
      setState(80);
      medium();
      setState(85);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MediumContext ------------------------------------------------------------------

CSSParser::MediumContext::MediumContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::MediumContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::MediumContext::getRuleIndex() const {
  return CSSParser::RuleMedium;
}

void CSSParser::MediumContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedium(this);
}

void CSSParser::MediumContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedium(this);
}

CSSParser::MediumContext* CSSParser::medium() {
  MediumContext *_localctx = _tracker.createInstance<MediumContext>(_ctx, getState());
  enterRule(_localctx, 10, CSSParser::RuleMedium);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(86);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CssRuleContext ------------------------------------------------------------------

CSSParser::CssRuleContext::CssRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::SelectorContext *> CSSParser::CssRuleContext::selector() {
  return getRuleContexts<CSSParser::SelectorContext>();
}

CSSParser::SelectorContext* CSSParser::CssRuleContext::selector(size_t i) {
  return getRuleContext<CSSParser::SelectorContext>(i);
}

std::vector<CSSParser::DeclarationContext *> CSSParser::CssRuleContext::declaration() {
  return getRuleContexts<CSSParser::DeclarationContext>();
}

CSSParser::DeclarationContext* CSSParser::CssRuleContext::declaration(size_t i) {
  return getRuleContext<CSSParser::DeclarationContext>(i);
}

CSSParser::AtRuleContext* CSSParser::CssRuleContext::atRule() {
  return getRuleContext<CSSParser::AtRuleContext>(0);
}


size_t CSSParser::CssRuleContext::getRuleIndex() const {
  return CSSParser::RuleCssRule;
}

void CSSParser::CssRuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCssRule(this);
}

void CSSParser::CssRuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCssRule(this);
}

CSSParser::CssRuleContext* CSSParser::cssRule() {
  CssRuleContext *_localctx = _tracker.createInstance<CssRuleContext>(_ctx, getState());
  enterRule(_localctx, 12, CSSParser::RuleCssRule);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(106);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::T__4:
      case CSSParser::T__5:
      case CSSParser::T__6:
      case CSSParser::T__9:
      case CSSParser::IDENT:
      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 1);
        setState(88);
        selector();
        setState(93);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == CSSParser::T__1) {
          setState(89);
          match(CSSParser::T__1);
          setState(90);
          selector();
          setState(95);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(96);
        match(CSSParser::T__2);
        setState(100);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == CSSParser::IDENT) {
          setState(97);
          declaration();
          setState(102);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(103);
        match(CSSParser::T__3);
        break;
      }

      case CSSParser::AT_KEYWORD: {
        enterOuterAlt(_localctx, 2);
        setState(105);
        atRule();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AtRuleContext ------------------------------------------------------------------

CSSParser::AtRuleContext::AtRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::AtRuleContext::AT_KEYWORD() {
  return getToken(CSSParser::AT_KEYWORD, 0);
}

CSSParser::BlockContext* CSSParser::AtRuleContext::block() {
  return getRuleContext<CSSParser::BlockContext>(0);
}

std::vector<CSSParser::AnyContext *> CSSParser::AtRuleContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::AtRuleContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}


size_t CSSParser::AtRuleContext::getRuleIndex() const {
  return CSSParser::RuleAtRule;
}

void CSSParser::AtRuleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtRule(this);
}

void CSSParser::AtRuleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtRule(this);
}

CSSParser::AtRuleContext* CSSParser::atRule() {
  AtRuleContext *_localctx = _tracker.createInstance<AtRuleContext>(_ctx, getState());
  enterRule(_localctx, 14, CSSParser::RuleAtRule);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(108);
    match(CSSParser::AT_KEYWORD);
    setState(112);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2199022223488) != 0)) {
      setState(109);
      any();
      setState(114);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(117);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::T__2: {
        setState(115);
        block();
        break;
      }

      case CSSParser::T__0: {
        setState(116);
        match(CSSParser::T__0);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

CSSParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::AnyContext *> CSSParser::BlockContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::BlockContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}

std::vector<CSSParser::BlockContext *> CSSParser::BlockContext::block() {
  return getRuleContexts<CSSParser::BlockContext>();
}

CSSParser::BlockContext* CSSParser::BlockContext::block(size_t i) {
  return getRuleContext<CSSParser::BlockContext>(i);
}

std::vector<CSSParser::AtRuleContext *> CSSParser::BlockContext::atRule() {
  return getRuleContexts<CSSParser::AtRuleContext>();
}

CSSParser::AtRuleContext* CSSParser::BlockContext::atRule(size_t i) {
  return getRuleContext<CSSParser::AtRuleContext>(i);
}


size_t CSSParser::BlockContext::getRuleIndex() const {
  return CSSParser::RuleBlock;
}

void CSSParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void CSSParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}

CSSParser::BlockContext* CSSParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 16, CSSParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(119);
    match(CSSParser::T__2);
    setState(125);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 2199022485640) != 0)) {
      setState(123);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case CSSParser::T__6:
        case CSSParser::T__13:
        case CSSParser::STRING:
        case CSSParser::IDENT:
        case CSSParser::NUMBER:
        case CSSParser::PERCENTAGE:
        case CSSParser::LENGTH:
        case CSSParser::EMS:
        case CSSParser::EXS:
        case CSSParser::ANGLE:
        case CSSParser::TIME:
        case CSSParser::FREQ:
        case CSSParser::DIMENSION:
        case CSSParser::URI:
        case CSSParser::HASH:
        case CSSParser::UNICODE_RANGE:
        case CSSParser::INCLUDES:
        case CSSParser::DASHMATCH:
        case CSSParser::PREFIXMATCH:
        case CSSParser::SUFFIXMATCH:
        case CSSParser::SUBSTRINGMATCH:
        case CSSParser::FUNCTION:
        case CSSParser::DELIM: {
          setState(120);
          any();
          break;
        }

        case CSSParser::T__2: {
          setState(121);
          block();
          break;
        }

        case CSSParser::AT_KEYWORD: {
          setState(122);
          atRule();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(127);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(128);
    match(CSSParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectorContext ------------------------------------------------------------------

CSSParser::SelectorContext::SelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::SimpleSelectorContext *> CSSParser::SelectorContext::simpleSelector() {
  return getRuleContexts<CSSParser::SimpleSelectorContext>();
}

CSSParser::SimpleSelectorContext* CSSParser::SelectorContext::simpleSelector(size_t i) {
  return getRuleContext<CSSParser::SimpleSelectorContext>(i);
}

std::vector<CSSParser::CombinatorContext *> CSSParser::SelectorContext::combinator() {
  return getRuleContexts<CSSParser::CombinatorContext>();
}

CSSParser::CombinatorContext* CSSParser::SelectorContext::combinator(size_t i) {
  return getRuleContext<CSSParser::CombinatorContext>(i);
}


size_t CSSParser::SelectorContext::getRuleIndex() const {
  return CSSParser::RuleSelector;
}

void CSSParser::SelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSelector(this);
}

void CSSParser::SelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSelector(this);
}

CSSParser::SelectorContext* CSSParser::selector() {
  SelectorContext *_localctx = _tracker.createInstance<SelectorContext>(_ctx, getState());
  enterRule(_localctx, 18, CSSParser::RuleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(130);
    simpleSelector();
    setState(136);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32985348833280) != 0)) {
      setState(131);
      combinator();
      setState(132);
      simpleSelector();
      setState(138);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SimpleSelectorContext ------------------------------------------------------------------

CSSParser::SimpleSelectorContext::SimpleSelectorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::ElementNameContext* CSSParser::SimpleSelectorContext::elementName() {
  return getRuleContext<CSSParser::ElementNameContext>(0);
}

std::vector<tree::TerminalNode *> CSSParser::SimpleSelectorContext::HASH() {
  return getTokens(CSSParser::HASH);
}

tree::TerminalNode* CSSParser::SimpleSelectorContext::HASH(size_t i) {
  return getToken(CSSParser::HASH, i);
}

std::vector<CSSParser::Class_Context *> CSSParser::SimpleSelectorContext::class_() {
  return getRuleContexts<CSSParser::Class_Context>();
}

CSSParser::Class_Context* CSSParser::SimpleSelectorContext::class_(size_t i) {
  return getRuleContext<CSSParser::Class_Context>(i);
}

std::vector<CSSParser::AttribContext *> CSSParser::SimpleSelectorContext::attrib() {
  return getRuleContexts<CSSParser::AttribContext>();
}

CSSParser::AttribContext* CSSParser::SimpleSelectorContext::attrib(size_t i) {
  return getRuleContext<CSSParser::AttribContext>(i);
}

std::vector<CSSParser::PseudoContext *> CSSParser::SimpleSelectorContext::pseudo() {
  return getRuleContexts<CSSParser::PseudoContext>();
}

CSSParser::PseudoContext* CSSParser::SimpleSelectorContext::pseudo(size_t i) {
  return getRuleContext<CSSParser::PseudoContext>(i);
}


size_t CSSParser::SimpleSelectorContext::getRuleIndex() const {
  return CSSParser::RuleSimpleSelector;
}

void CSSParser::SimpleSelectorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleSelector(this);
}

void CSSParser::SimpleSelectorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleSelector(this);
}

CSSParser::SimpleSelectorContext* CSSParser::simpleSelector() {
  SimpleSelectorContext *_localctx = _tracker.createInstance<SimpleSelectorContext>(_ctx, getState());
  enterRule(_localctx, 20, CSSParser::RuleSimpleSelector);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(157);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::T__4:
      case CSSParser::IDENT: {
        enterOuterAlt(_localctx, 1);
        setState(139);
        elementName();
        setState(146);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4294968512) != 0)) {
          setState(144);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSSParser::HASH: {
              setState(140);
              match(CSSParser::HASH);
              break;
            }

            case CSSParser::T__5: {
              setState(141);
              class_();
              break;
            }

            case CSSParser::T__6: {
              setState(142);
              attrib();
              break;
            }

            case CSSParser::T__9: {
              setState(143);
              pseudo();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(148);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case CSSParser::T__5:
      case CSSParser::T__6:
      case CSSParser::T__9:
      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 2);
        setState(153); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(153);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case CSSParser::HASH: {
              setState(149);
              match(CSSParser::HASH);
              break;
            }

            case CSSParser::T__5: {
              setState(150);
              class_();
              break;
            }

            case CSSParser::T__6: {
              setState(151);
              attrib();
              break;
            }

            case CSSParser::T__9: {
              setState(152);
              pseudo();
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(155); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 4294968512) != 0));
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ElementNameContext ------------------------------------------------------------------

CSSParser::ElementNameContext::ElementNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::ElementNameContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::ElementNameContext::getRuleIndex() const {
  return CSSParser::RuleElementName;
}

void CSSParser::ElementNameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterElementName(this);
}

void CSSParser::ElementNameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitElementName(this);
}

CSSParser::ElementNameContext* CSSParser::elementName() {
  ElementNameContext *_localctx = _tracker.createInstance<ElementNameContext>(_ctx, getState());
  enterRule(_localctx, 22, CSSParser::RuleElementName);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(159);
    _la = _input->LA(1);
    if (!(_la == CSSParser::T__4

    || _la == CSSParser::IDENT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Class_Context ------------------------------------------------------------------

CSSParser::Class_Context::Class_Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::Class_Context::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::Class_Context::getRuleIndex() const {
  return CSSParser::RuleClass_;
}

void CSSParser::Class_Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClass_(this);
}

void CSSParser::Class_Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClass_(this);
}

CSSParser::Class_Context* CSSParser::class_() {
  Class_Context *_localctx = _tracker.createInstance<Class_Context>(_ctx, getState());
  enterRule(_localctx, 24, CSSParser::RuleClass_);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(161);
    match(CSSParser::T__5);
    setState(162);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttribContext ------------------------------------------------------------------

CSSParser::AttribContext::AttribContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> CSSParser::AttribContext::IDENT() {
  return getTokens(CSSParser::IDENT);
}

tree::TerminalNode* CSSParser::AttribContext::IDENT(size_t i) {
  return getToken(CSSParser::IDENT, i);
}

tree::TerminalNode* CSSParser::AttribContext::INCLUDES() {
  return getToken(CSSParser::INCLUDES, 0);
}

tree::TerminalNode* CSSParser::AttribContext::DASHMATCH() {
  return getToken(CSSParser::DASHMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::PREFIXMATCH() {
  return getToken(CSSParser::PREFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::SUFFIXMATCH() {
  return getToken(CSSParser::SUFFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::SUBSTRINGMATCH() {
  return getToken(CSSParser::SUBSTRINGMATCH, 0);
}

tree::TerminalNode* CSSParser::AttribContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}


size_t CSSParser::AttribContext::getRuleIndex() const {
  return CSSParser::RuleAttrib;
}

void CSSParser::AttribContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAttrib(this);
}

void CSSParser::AttribContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAttrib(this);
}

CSSParser::AttribContext* CSSParser::attrib() {
  AttribContext *_localctx = _tracker.createInstance<AttribContext>(_ctx, getState());
  enterRule(_localctx, 26, CSSParser::RuleAttrib);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(164);
    match(CSSParser::T__6);
    setState(165);
    match(CSSParser::IDENT);
    setState(168);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 532575944960) != 0)) {
      setState(166);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 532575944960) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(167);
      _la = _input->LA(1);
      if (!(_la == CSSParser::STRING

      || _la == CSSParser::IDENT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(170);
    match(CSSParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PseudoContext ------------------------------------------------------------------

CSSParser::PseudoContext::PseudoContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::PseudoContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::PseudoContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

std::vector<CSSParser::AnyContext *> CSSParser::PseudoContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::PseudoContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}


size_t CSSParser::PseudoContext::getRuleIndex() const {
  return CSSParser::RulePseudo;
}

void CSSParser::PseudoContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPseudo(this);
}

void CSSParser::PseudoContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPseudo(this);
}

CSSParser::PseudoContext* CSSParser::pseudo() {
  PseudoContext *_localctx = _tracker.createInstance<PseudoContext>(_ctx, getState());
  enterRule(_localctx, 28, CSSParser::RulePseudo);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(172);
    match(CSSParser::T__9);
    setState(174);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::T__9) {
      setState(173);
      match(CSSParser::T__9);
    }
    setState(185);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::IDENT: {
        setState(176);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::FUNCTION: {
        setState(177);
        match(CSSParser::FUNCTION);
        setState(181);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2199022223488) != 0)) {
          setState(178);
          any();
          setState(183);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(184);
        match(CSSParser::T__10);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CombinatorContext ------------------------------------------------------------------

CSSParser::CombinatorContext::CombinatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::CombinatorContext::PLUS() {
  return getToken(CSSParser::PLUS, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::GREATER() {
  return getToken(CSSParser::GREATER, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::TILDE() {
  return getToken(CSSParser::TILDE, 0);
}

tree::TerminalNode* CSSParser::CombinatorContext::S() {
  return getToken(CSSParser::S, 0);
}


size_t CSSParser::CombinatorContext::getRuleIndex() const {
  return CSSParser::RuleCombinator;
}

void CSSParser::CombinatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCombinator(this);
}

void CSSParser::CombinatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCombinator(this);
}

CSSParser::CombinatorContext* CSSParser::combinator() {
  CombinatorContext *_localctx = _tracker.createInstance<CombinatorContext>(_ctx, getState());
  enterRule(_localctx, 30, CSSParser::RuleCombinator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(187);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32985348833280) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclarationContext ------------------------------------------------------------------

CSSParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

CSSParser::PropertyContext* CSSParser::DeclarationContext::property() {
  return getRuleContext<CSSParser::PropertyContext>(0);
}

CSSParser::ExprContext* CSSParser::DeclarationContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}

CSSParser::PrioContext* CSSParser::DeclarationContext::prio() {
  return getRuleContext<CSSParser::PrioContext>(0);
}


size_t CSSParser::DeclarationContext::getRuleIndex() const {
  return CSSParser::RuleDeclaration;
}

void CSSParser::DeclarationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDeclaration(this);
}

void CSSParser::DeclarationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDeclaration(this);
}

CSSParser::DeclarationContext* CSSParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 32, CSSParser::RuleDeclaration);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(189);
    property();
    setState(190);
    match(CSSParser::T__9);
    setState(191);
    expr();
    setState(193);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == CSSParser::IMPORTANT_SYM) {
      setState(192);
      prio();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertyContext ------------------------------------------------------------------

CSSParser::PropertyContext::PropertyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::PropertyContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}


size_t CSSParser::PropertyContext::getRuleIndex() const {
  return CSSParser::RuleProperty;
}

void CSSParser::PropertyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProperty(this);
}

void CSSParser::PropertyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProperty(this);
}

CSSParser::PropertyContext* CSSParser::property() {
  PropertyContext *_localctx = _tracker.createInstance<PropertyContext>(_ctx, getState());
  enterRule(_localctx, 34, CSSParser::RuleProperty);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(195);
    match(CSSParser::IDENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrioContext ------------------------------------------------------------------

CSSParser::PrioContext::PrioContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::PrioContext::IMPORTANT_SYM() {
  return getToken(CSSParser::IMPORTANT_SYM, 0);
}


size_t CSSParser::PrioContext::getRuleIndex() const {
  return CSSParser::RulePrio;
}

void CSSParser::PrioContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrio(this);
}

void CSSParser::PrioContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrio(this);
}

CSSParser::PrioContext* CSSParser::prio() {
  PrioContext *_localctx = _tracker.createInstance<PrioContext>(_ctx, getState());
  enterRule(_localctx, 36, CSSParser::RulePrio);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(197);
    match(CSSParser::IMPORTANT_SYM);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

CSSParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<CSSParser::TermContext *> CSSParser::ExprContext::term() {
  return getRuleContexts<CSSParser::TermContext>();
}

CSSParser::TermContext* CSSParser::ExprContext::term(size_t i) {
  return getRuleContext<CSSParser::TermContext>(i);
}

std::vector<CSSParser::OperatorContext *> CSSParser::ExprContext::operator_() {
  return getRuleContexts<CSSParser::OperatorContext>();
}

CSSParser::OperatorContext* CSSParser::ExprContext::operator_(size_t i) {
  return getRuleContext<CSSParser::OperatorContext>(i);
}


size_t CSSParser::ExprContext::getRuleIndex() const {
  return CSSParser::RuleExpr;
}

void CSSParser::ExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr(this);
}

void CSSParser::ExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr(this);
}

CSSParser::ExprContext* CSSParser::expr() {
  ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, getState());
  enterRule(_localctx, 38, CSSParser::RuleExpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(199);
    term();
    setState(205);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(200);
        operator_();
        setState(201);
        term(); 
      }
      setState(207);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TermContext ------------------------------------------------------------------

CSSParser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::TermContext::NUMBER() {
  return getToken(CSSParser::NUMBER, 0);
}

tree::TerminalNode* CSSParser::TermContext::PERCENTAGE() {
  return getToken(CSSParser::PERCENTAGE, 0);
}

tree::TerminalNode* CSSParser::TermContext::LENGTH() {
  return getToken(CSSParser::LENGTH, 0);
}

tree::TerminalNode* CSSParser::TermContext::EMS() {
  return getToken(CSSParser::EMS, 0);
}

tree::TerminalNode* CSSParser::TermContext::EXS() {
  return getToken(CSSParser::EXS, 0);
}

tree::TerminalNode* CSSParser::TermContext::ANGLE() {
  return getToken(CSSParser::ANGLE, 0);
}

tree::TerminalNode* CSSParser::TermContext::TIME() {
  return getToken(CSSParser::TIME, 0);
}

tree::TerminalNode* CSSParser::TermContext::FREQ() {
  return getToken(CSSParser::FREQ, 0);
}

tree::TerminalNode* CSSParser::TermContext::DIMENSION() {
  return getToken(CSSParser::DIMENSION, 0);
}

CSSParser::UnaryOperatorContext* CSSParser::TermContext::unaryOperator() {
  return getRuleContext<CSSParser::UnaryOperatorContext>(0);
}

tree::TerminalNode* CSSParser::TermContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::TermContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::TermContext::URI() {
  return getToken(CSSParser::URI, 0);
}

CSSParser::HexcolorContext* CSSParser::TermContext::hexcolor() {
  return getRuleContext<CSSParser::HexcolorContext>(0);
}

tree::TerminalNode* CSSParser::TermContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

CSSParser::ExprContext* CSSParser::TermContext::expr() {
  return getRuleContext<CSSParser::ExprContext>(0);
}


size_t CSSParser::TermContext::getRuleIndex() const {
  return CSSParser::RuleTerm;
}

void CSSParser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void CSSParser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

CSSParser::TermContext* CSSParser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 40, CSSParser::RuleTerm);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(220);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::T__11:
      case CSSParser::NUMBER:
      case CSSParser::PERCENTAGE:
      case CSSParser::LENGTH:
      case CSSParser::EMS:
      case CSSParser::EXS:
      case CSSParser::ANGLE:
      case CSSParser::TIME:
      case CSSParser::FREQ:
      case CSSParser::DIMENSION:
      case CSSParser::PLUS: {
        enterOuterAlt(_localctx, 1);
        setState(209);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == CSSParser::T__11

        || _la == CSSParser::PLUS) {
          setState(208);
          unaryOperator();
        }
        setState(211);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2143289344) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        break;
      }

      case CSSParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(212);
        match(CSSParser::STRING);
        break;
      }

      case CSSParser::IDENT: {
        enterOuterAlt(_localctx, 3);
        setState(213);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::URI: {
        enterOuterAlt(_localctx, 4);
        setState(214);
        match(CSSParser::URI);
        break;
      }

      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 5);
        setState(215);
        hexcolor();
        break;
      }

      case CSSParser::FUNCTION: {
        enterOuterAlt(_localctx, 6);
        setState(216);
        match(CSSParser::FUNCTION);
        setState(217);
        expr();
        setState(218);
        match(CSSParser::T__10);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOperatorContext ------------------------------------------------------------------

CSSParser::UnaryOperatorContext::UnaryOperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::UnaryOperatorContext::PLUS() {
  return getToken(CSSParser::PLUS, 0);
}


size_t CSSParser::UnaryOperatorContext::getRuleIndex() const {
  return CSSParser::RuleUnaryOperator;
}

void CSSParser::UnaryOperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryOperator(this);
}

void CSSParser::UnaryOperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryOperator(this);
}

CSSParser::UnaryOperatorContext* CSSParser::unaryOperator() {
  UnaryOperatorContext *_localctx = _tracker.createInstance<UnaryOperatorContext>(_ctx, getState());
  enterRule(_localctx, 42, CSSParser::RuleUnaryOperator);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(222);
    _la = _input->LA(1);
    if (!(_la == CSSParser::T__11

    || _la == CSSParser::PLUS)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OperatorContext ------------------------------------------------------------------

CSSParser::OperatorContext::OperatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t CSSParser::OperatorContext::getRuleIndex() const {
  return CSSParser::RuleOperator;
}

void CSSParser::OperatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOperator(this);
}

void CSSParser::OperatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOperator(this);
}

CSSParser::OperatorContext* CSSParser::operator_() {
  OperatorContext *_localctx = _tracker.createInstance<OperatorContext>(_ctx, getState());
  enterRule(_localctx, 44, CSSParser::RuleOperator);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(227);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::T__12: {
        enterOuterAlt(_localctx, 1);
        setState(224);
        match(CSSParser::T__12);
        break;
      }

      case CSSParser::T__1: {
        enterOuterAlt(_localctx, 2);
        setState(225);
        match(CSSParser::T__1);
        break;
      }

      case CSSParser::T__11:
      case CSSParser::STRING:
      case CSSParser::IDENT:
      case CSSParser::NUMBER:
      case CSSParser::PERCENTAGE:
      case CSSParser::LENGTH:
      case CSSParser::EMS:
      case CSSParser::EXS:
      case CSSParser::ANGLE:
      case CSSParser::TIME:
      case CSSParser::FREQ:
      case CSSParser::DIMENSION:
      case CSSParser::URI:
      case CSSParser::HASH:
      case CSSParser::FUNCTION:
      case CSSParser::PLUS: {
        enterOuterAlt(_localctx, 3);

        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HexcolorContext ------------------------------------------------------------------

CSSParser::HexcolorContext::HexcolorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::HexcolorContext::HASH() {
  return getToken(CSSParser::HASH, 0);
}


size_t CSSParser::HexcolorContext::getRuleIndex() const {
  return CSSParser::RuleHexcolor;
}

void CSSParser::HexcolorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHexcolor(this);
}

void CSSParser::HexcolorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHexcolor(this);
}

CSSParser::HexcolorContext* CSSParser::hexcolor() {
  HexcolorContext *_localctx = _tracker.createInstance<HexcolorContext>(_ctx, getState());
  enterRule(_localctx, 46, CSSParser::RuleHexcolor);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    match(CSSParser::HASH);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AnyContext ------------------------------------------------------------------

CSSParser::AnyContext::AnyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* CSSParser::AnyContext::IDENT() {
  return getToken(CSSParser::IDENT, 0);
}

tree::TerminalNode* CSSParser::AnyContext::NUMBER() {
  return getToken(CSSParser::NUMBER, 0);
}

tree::TerminalNode* CSSParser::AnyContext::PERCENTAGE() {
  return getToken(CSSParser::PERCENTAGE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::LENGTH() {
  return getToken(CSSParser::LENGTH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::EMS() {
  return getToken(CSSParser::EMS, 0);
}

tree::TerminalNode* CSSParser::AnyContext::EXS() {
  return getToken(CSSParser::EXS, 0);
}

tree::TerminalNode* CSSParser::AnyContext::ANGLE() {
  return getToken(CSSParser::ANGLE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::TIME() {
  return getToken(CSSParser::TIME, 0);
}

tree::TerminalNode* CSSParser::AnyContext::FREQ() {
  return getToken(CSSParser::FREQ, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DIMENSION() {
  return getToken(CSSParser::DIMENSION, 0);
}

tree::TerminalNode* CSSParser::AnyContext::STRING() {
  return getToken(CSSParser::STRING, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DELIM() {
  return getToken(CSSParser::DELIM, 0);
}

tree::TerminalNode* CSSParser::AnyContext::URI() {
  return getToken(CSSParser::URI, 0);
}

tree::TerminalNode* CSSParser::AnyContext::HASH() {
  return getToken(CSSParser::HASH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::UNICODE_RANGE() {
  return getToken(CSSParser::UNICODE_RANGE, 0);
}

tree::TerminalNode* CSSParser::AnyContext::INCLUDES() {
  return getToken(CSSParser::INCLUDES, 0);
}

tree::TerminalNode* CSSParser::AnyContext::DASHMATCH() {
  return getToken(CSSParser::DASHMATCH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::PREFIXMATCH() {
  return getToken(CSSParser::PREFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::SUFFIXMATCH() {
  return getToken(CSSParser::SUFFIXMATCH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::SUBSTRINGMATCH() {
  return getToken(CSSParser::SUBSTRINGMATCH, 0);
}

tree::TerminalNode* CSSParser::AnyContext::FUNCTION() {
  return getToken(CSSParser::FUNCTION, 0);
}

std::vector<CSSParser::AnyContext *> CSSParser::AnyContext::any() {
  return getRuleContexts<CSSParser::AnyContext>();
}

CSSParser::AnyContext* CSSParser::AnyContext::any(size_t i) {
  return getRuleContext<CSSParser::AnyContext>(i);
}


size_t CSSParser::AnyContext::getRuleIndex() const {
  return CSSParser::RuleAny;
}

void CSSParser::AnyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAny(this);
}

void CSSParser::AnyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<CSSListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAny(this);
}

CSSParser::AnyContext* CSSParser::any() {
  AnyContext *_localctx = _tracker.createInstance<AnyContext>(_ctx, getState());
  enterRule(_localctx, 48, CSSParser::RuleAny);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(275);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case CSSParser::IDENT: {
        enterOuterAlt(_localctx, 1);
        setState(231);
        match(CSSParser::IDENT);
        break;
      }

      case CSSParser::NUMBER: {
        enterOuterAlt(_localctx, 2);
        setState(232);
        match(CSSParser::NUMBER);
        break;
      }

      case CSSParser::PERCENTAGE: {
        enterOuterAlt(_localctx, 3);
        setState(233);
        match(CSSParser::PERCENTAGE);
        break;
      }

      case CSSParser::LENGTH: {
        enterOuterAlt(_localctx, 4);
        setState(234);
        match(CSSParser::LENGTH);
        break;
      }

      case CSSParser::EMS: {
        enterOuterAlt(_localctx, 5);
        setState(235);
        match(CSSParser::EMS);
        break;
      }

      case CSSParser::EXS: {
        enterOuterAlt(_localctx, 6);
        setState(236);
        match(CSSParser::EXS);
        break;
      }

      case CSSParser::ANGLE: {
        enterOuterAlt(_localctx, 7);
        setState(237);
        match(CSSParser::ANGLE);
        break;
      }

      case CSSParser::TIME: {
        enterOuterAlt(_localctx, 8);
        setState(238);
        match(CSSParser::TIME);
        break;
      }

      case CSSParser::FREQ: {
        enterOuterAlt(_localctx, 9);
        setState(239);
        match(CSSParser::FREQ);
        break;
      }

      case CSSParser::DIMENSION: {
        enterOuterAlt(_localctx, 10);
        setState(240);
        match(CSSParser::DIMENSION);
        break;
      }

      case CSSParser::STRING: {
        enterOuterAlt(_localctx, 11);
        setState(241);
        match(CSSParser::STRING);
        break;
      }

      case CSSParser::DELIM: {
        enterOuterAlt(_localctx, 12);
        setState(242);
        match(CSSParser::DELIM);
        break;
      }

      case CSSParser::URI: {
        enterOuterAlt(_localctx, 13);
        setState(243);
        match(CSSParser::URI);
        break;
      }

      case CSSParser::HASH: {
        enterOuterAlt(_localctx, 14);
        setState(244);
        match(CSSParser::HASH);
        break;
      }

      case CSSParser::UNICODE_RANGE: {
        enterOuterAlt(_localctx, 15);
        setState(245);
        match(CSSParser::UNICODE_RANGE);
        break;
      }

      case CSSParser::INCLUDES: {
        enterOuterAlt(_localctx, 16);
        setState(246);
        match(CSSParser::INCLUDES);
        break;
      }

      case CSSParser::DASHMATCH: {
        enterOuterAlt(_localctx, 17);
        setState(247);
        match(CSSParser::DASHMATCH);
        break;
      }

      case CSSParser::PREFIXMATCH: {
        enterOuterAlt(_localctx, 18);
        setState(248);
        match(CSSParser::PREFIXMATCH);
        break;
      }

      case CSSParser::SUFFIXMATCH: {
        enterOuterAlt(_localctx, 19);
        setState(249);
        match(CSSParser::SUFFIXMATCH);
        break;
      }

      case CSSParser::SUBSTRINGMATCH: {
        enterOuterAlt(_localctx, 20);
        setState(250);
        match(CSSParser::SUBSTRINGMATCH);
        break;
      }

      case CSSParser::FUNCTION: {
        enterOuterAlt(_localctx, 21);
        setState(251);
        match(CSSParser::FUNCTION);
        setState(255);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2199022223488) != 0)) {
          setState(252);
          any();
          setState(257);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(258);
        match(CSSParser::T__10);
        break;
      }

      case CSSParser::T__13: {
        enterOuterAlt(_localctx, 22);
        setState(259);
        match(CSSParser::T__13);
        setState(263);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2199022223488) != 0)) {
          setState(260);
          any();
          setState(265);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(266);
        match(CSSParser::T__10);
        break;
      }

      case CSSParser::T__6: {
        enterOuterAlt(_localctx, 23);
        setState(267);
        match(CSSParser::T__6);
        setState(271);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 2199022223488) != 0)) {
          setState(268);
          any();
          setState(273);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(274);
        match(CSSParser::T__8);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void CSSParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  cssParserInitialize();
#else
  ::antlr4::internal::call_once(cssParserOnceFlag, cssParserInitialize);
#endif
}
