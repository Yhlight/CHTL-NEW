#include "CHTL/Parser/Parser.h"

namespace CHTL {

Parser::Parser(std::shared_ptr<CompileContext> ctx) 
    : context(ctx), currentIndex(0) {
}

void Parser::Reset() {
    currentIndex = 0;
    tokens.clear();
}

std::shared_ptr<ProgramNode> Parser::Parse(const std::vector<Token>& tokenList) {
    tokens = tokenList;
    currentIndex = 0;
    
    // TODO: 实现完整的解析逻辑
    auto program = std::make_shared<ProgramNode>(context->GetCurrentFile());
    
    return program;
}

bool Parser::Match(TokenType type) {
    if (Check(type)) {
        ConsumeToken();
        return true;
    }
    return false;
}

void Parser::Expect(TokenType type, const std::string& message) {
    if (!Check(type)) {
        ReportError(message);
    }
    ConsumeToken();
}

void Parser::ReportError(const std::string& message) {
    const Token& token = CurrentToken();
    throw SyntaxError(message, context->GetCurrentFile(), token.line, token.column);
}

} // namespace CHTL