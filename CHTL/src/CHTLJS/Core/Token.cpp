#include "CHTLJS/Core/Token.h"

namespace CHTL {
namespace JS {

std::string Token::GetTypeString() const {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::MODULE: return "MODULE";
        case TokenType::LOAD: return "LOAD";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::INEVERAWAY: return "INEVERAWAY";
        case TokenType::UTIL: return "UTIL";
        case TokenType::CHANGE: return "CHANGE";
        case TokenType::THEN: return "THEN";
        case TokenType::DOUBLE_LEFT_BRACE: return "DOUBLE_LEFT_BRACE";
        case TokenType::DOUBLE_RIGHT_BRACE: return "DOUBLE_RIGHT_BRACE";
        case TokenType::ARROW: return "ARROW";
        case TokenType::AMPERSAND_ARROW: return "AMPERSAND_ARROW";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.GetTypeString() << ", \"" << token.value 
       << "\", " << token.line << ":" << token.column << ")";
    return os;
}

} // namespace JS
} // namespace CHTL