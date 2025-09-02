#include "CHTL/Lexer/Lexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

// 初始化关键字映射表
const std::unordered_map<std::string, TokenType> Lexer::keywordMap = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT},
    {"top", TokenType::TOP},
    {"bottom", TokenType::BOTTOM},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT}
};

const std::unordered_map<std::string, TokenType> Lexer::atKeywordMap = {
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
    {"@CJmod", TokenType::AT_CJMOD},
    {"@Config", TokenType::AT_CONFIG}
};

const std::unordered_map<std::string, TokenType> Lexer::bracketKeywordMap = {
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"[Origin]", TokenType::ORIGIN},
    {"[Import]", TokenType::IMPORT},
    {"[Namespace]", TokenType::NAMESPACE},
    {"[Configuration]", TokenType::CONFIGURATION},
    {"[Info]", TokenType::INFO},
    {"[Export]", TokenType::EXPORT}
};

Lexer::Lexer(const std::string& src, const std::string& fname)
    : source(src), filename(fname), position(0), line(1), column(1) {
}

void Lexer::SetSource(const std::string& src, const std::string& fname) {
    source = src;
    filename = fname;
    Reset();
}

void Lexer::Reset() {
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
}

void Lexer::SkipWhitespace() {
    while (position < source.length() && std::isspace(CurrentChar())) {
        Advance();
    }
}

Token Lexer::MakeToken(TokenType type, const std::string& value,
                       size_t startLine, size_t startColumn, size_t length) {
    return Token(type, value, startLine, startColumn, length);
}

void Lexer::ReportError(const std::string& message) {
    throw LexicalError(message, filename, line, column);
}

Token Lexer::ScanSingleLineComment() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 跳过 //
    Advance();
    Advance();
    
    // 读取到行尾
    while (CurrentChar() != '\n' && CurrentChar() != '\0') {
        value += CurrentChar();
        Advance();
    }
    
    return MakeToken(TokenType::SINGLE_COMMENT, value, startLine, startColumn, value.length() + 2);
}

Token Lexer::ScanMultiLineComment() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 跳过 /*
    Advance();
    Advance();
    
    // 读取到 */
    while (position < source.length()) {
        if (CurrentChar() == '*' && PeekChar() == '/') {
            Advance();
            Advance();
            break;
        }
        value += CurrentChar();
        Advance();
    }
    
    return MakeToken(TokenType::MULTI_COMMENT, value, startLine, startColumn, value.length() + 4);
}

Token Lexer::ScanGeneratorComment() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 跳过 --
    Advance();
    Advance();
    
    // 读取到行尾
    while (CurrentChar() != '\n' && CurrentChar() != '\0') {
        value += CurrentChar();
        Advance();
    }
    
    return MakeToken(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, value.length() + 2);
}

Token Lexer::ScanStringLiteral(char delimiter) {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 跳过开始引号
    Advance();
    
    while (CurrentChar() != delimiter && CurrentChar() != '\0') {
        if (CurrentChar() == '\\') {
            // 处理转义字符
            Advance();
            if (CurrentChar() != '\0') {
                switch (CurrentChar()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += CurrentChar(); break;
                }
                Advance();
            }
        } else {
            value += CurrentChar();
            Advance();
        }
    }
    
    if (CurrentChar() != delimiter) {
        ReportError("未结束的字符串字面量");
    }
    
    // 跳过结束引号
    Advance();
    
    return MakeToken(TokenType::STRING_LITERAL, value, startLine, startColumn, value.length() + 2);
}

Token Lexer::ScanNumber() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 整数部分
    while (std::isdigit(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    // 小数部分
    if (CurrentChar() == '.' && std::isdigit(PeekChar())) {
        value += CurrentChar();
        Advance();
        while (std::isdigit(CurrentChar())) {
            value += CurrentChar();
            Advance();
        }
    }
    
    // 科学计数法
    if (CurrentChar() == 'e' || CurrentChar() == 'E') {
        value += CurrentChar();
        Advance();
        if (CurrentChar() == '+' || CurrentChar() == '-') {
            value += CurrentChar();
            Advance();
        }
        while (std::isdigit(CurrentChar())) {
            value += CurrentChar();
            Advance();
        }
    }
    
    return MakeToken(TokenType::NUMBER, value, startLine, startColumn, value.length());
}

Token Lexer::ScanIdentifierOrKeyword() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 读取标识符
    while (IsIdentifierChar(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    // 检查是否为关键字
    auto it = keywordMap.find(value);
    TokenType type = (it != keywordMap.end()) ? it->second : TokenType::IDENTIFIER;
    
    return MakeToken(type, value, startLine, startColumn, value.length());
}

Token Lexer::ScanUnquotedLiteral() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value;
    
    // 跳过开头的空白
    while (std::isspace(CurrentChar()) && CurrentChar() != '\n') {
        Advance();
    }
    
    // 读取到分隔符
    while (IsUnquotedLiteralChar(CurrentChar()) && CurrentChar() != '\0') {
        value += CurrentChar();
        Advance();
    }
    
    // 去除尾部空白
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }
    
    return MakeToken(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn, value.length());
}

Token Lexer::ScanAtKeyword() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value = "@";
    
    // 跳过 @
    Advance();
    
    // 读取关键字部分
    while (std::isalpha(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    // 检查是否为已知的@关键字
    auto it = atKeywordMap.find(value);
    if (it != atKeywordMap.end()) {
        return MakeToken(it->second, value, startLine, startColumn, value.length());
    }
    
    // 如果不是已知的@关键字，作为标识符处理
    return MakeToken(TokenType::IDENTIFIER, value, startLine, startColumn, value.length());
}

Token Lexer::ScanBracketKeyword() {
    size_t startLine = line;
    size_t startColumn = column;
    std::string value = "[";
    
    // 跳过 [
    Advance();
    
    // 读取到 ]
    while (CurrentChar() != ']' && CurrentChar() != '\0') {
        value += CurrentChar();
        Advance();
    }
    
    if (CurrentChar() == ']') {
        value += ']';
        Advance();
    } else {
        ReportError("未结束的方括号");
    }
    
    // 检查是否为已知的[]关键字
    auto it = bracketKeywordMap.find(value);
    if (it != bracketKeywordMap.end()) {
        return MakeToken(it->second, value, startLine, startColumn, value.length());
    }
    
    // 如果不是已知的[]关键字，作为标识符处理
    return MakeToken(TokenType::IDENTIFIER, value, startLine, startColumn, value.length());
}

Token Lexer::NextToken() {
    SkipWhitespace();
    
    if (position >= source.length()) {
        return MakeToken(TokenType::EOF_TOKEN, "", line, column, 0);
    }
    
    size_t startLine = line;
    size_t startColumn = column;
    char ch = CurrentChar();
    
    // 注释
    if (ch == '/' && PeekChar() == '/') {
        return ScanSingleLineComment();
    }
    if (ch == '/' && PeekChar() == '*') {
        return ScanMultiLineComment();
    }
    if (ch == '-' && PeekChar() == '-') {
        return ScanGeneratorComment();
    }
    
    // 字符串字面量
    if (ch == '"' || ch == '\'') {
        return ScanStringLiteral(ch);
    }
    
    // 数字
    if (std::isdigit(ch)) {
        return ScanNumber();
    }
    
    // 标识符或关键字
    if (IsIdentifierStart(ch)) {
        return ScanIdentifierOrKeyword();
    }
    
    // @关键字
    if (ch == '@') {
        return ScanAtKeyword();
    }
    
    // []关键字
    if (ch == '[') {
        // 检查是否可能是[]关键字
        size_t savePos = position;
        size_t saveLine = line;
        size_t saveColumn = column;
        
        std::string preview = "[";
        Advance();
        while (std::isalpha(CurrentChar()) && position < source.length()) {
            preview += CurrentChar();
            Advance();
        }
        if (CurrentChar() == ']') {
            preview += ']';
        }
        
        // 恢复位置
        position = savePos;
        line = saveLine;
        column = saveColumn;
        
        // 如果匹配已知的[]关键字模式，则扫描为[]关键字
        if (bracketKeywordMap.find(preview) != bracketKeywordMap.end()) {
            return ScanBracketKeyword();
        }
        // 否则作为单独的左方括号
        Advance();
        return MakeToken(TokenType::LEFT_BRACKET, "[", startLine, startColumn, 1);
    }
    
    // 单字符操作符
    std::string opValue(1, ch);
    TokenType opType = TokenType::UNKNOWN;
    
    switch (ch) {
        case ':':
            opType = TokenType::COLON;
            break;
        case ';':
            opType = TokenType::SEMICOLON;
            break;
        case ',':
            opType = TokenType::COMMA;
            break;
        case '.':
            opType = TokenType::DOT;
            break;
        case '=':
            opType = TokenType::EQUALS;
            break;
        case '{':
            opType = TokenType::LEFT_BRACE;
            break;
        case '}':
            opType = TokenType::RIGHT_BRACE;
            break;
        case ']':
            opType = TokenType::RIGHT_BRACKET;
            break;
        case '(':
            opType = TokenType::LEFT_PAREN;
            break;
        case ')':
            opType = TokenType::RIGHT_PAREN;
            break;
        case '&':
            opType = TokenType::AMPERSAND;
            break;
        case '-':
            // 检查是否为 ->
            if (PeekChar() == '>') {
                Advance();
                Advance();
                return MakeToken(TokenType::ARROW, "->", startLine, startColumn, 2);
            }
            // 否则可能是负号或减号，这里暂时作为未知处理
            break;
    }
    
    if (opType != TokenType::UNKNOWN) {
        Advance();
        return MakeToken(opType, opValue, startLine, startColumn, 1);
    }
    
    // 未知字符
    Advance();
    return MakeToken(TokenType::UNKNOWN, opValue, startLine, startColumn, 1);
}

void Lexer::Tokenize() {
    tokens.clear();
    position = 0;
    line = 1;
    column = 1;
    
    Token token;
    do {
        token = NextToken();
        if (token.type != TokenType::UNKNOWN || token.type == TokenType::EOF_TOKEN) {
            tokens.push_back(token);
        }
    } while (token.type != TokenType::EOF_TOKEN);
}

} // namespace CHTL