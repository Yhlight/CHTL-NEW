#include "CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace CHTL {

    CHTLLexer::CHTLLexer(const std::string& source) 
        : sourceCode(source), currentPos(0), currentLine(1), currentColumn(1) {
        globalMap = CHTLGlobalMap::GetInstance();
    }

    std::vector<CHTLToken> CHTLLexer::Tokenize() {
        tokens.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;

        while (!IsAtEnd()) {
            ScanToken();
        }

        // 添加EOF Token
        AddToken(CHTLTokenType::EOF_TOKEN, "");
        
        return tokens;
    }

    void CHTLLexer::ScanToken() {
        char c = PeekChar();
        
        // 跳过空白字符
        if (IsWhitespace(c)) {
            SkipWhitespace();
            return;
        }
        
        // 处理换行
        if (IsNewline(c)) {
            AddToken(CHTLTokenType::NEWLINE, std::string(1, c));
            AdvancePosition();
            return;
        }
        
        // 处理注释
        if (c == '/') {
            char next = PeekChar(1);
            if (next == '/') {
                ScanComment();
                return;
            } else if (next == '*') {
                ScanComment();
                return;
            }
        }
        
        // 处理生成器注释
        if (c == '-' && PeekChar(1) == '-') {
            ScanComment();
            return;
        }
        
        // 处理字符串字面量
        if (c == '"') {
            ScanString();
            return;
        }
        
        // 处理单引号字符串
        if (c == '\'') {
            ScanSingleQuoteString();
            return;
        }
        
        // 处理块关键字 [Template], [Custom]等
        if (c == '[') {
            ScanBlockKeyword();
            return;
        }
        
        // 处理类型关键字 @Style, @Element等
        if (c == '@') {
            ScanTypeKeyword();
            return;
        }
        
        // 处理CSS选择器
        if (c == '.' || c == '#') {
            ScanSelector();
            return;
        }
        
        // 处理单字符操作符
        switch (c) {
            case '{': AddToken(CHTLTokenType::LEFT_BRACE, "{"); break;
            case '}': AddToken(CHTLTokenType::RIGHT_BRACE, "}"); break;
            case '(': AddToken(CHTLTokenType::LEFT_PAREN, "("); break;
            case ')': AddToken(CHTLTokenType::RIGHT_PAREN, ")"); break;
            case ']': AddToken(CHTLTokenType::RIGHT_BRACKET, "]"); break;
            case ';': AddToken(CHTLTokenType::SEMICOLON, ";"); break;
            case ',': AddToken(CHTLTokenType::COMMA, ","); break;
            case '&': AddToken(CHTLTokenType::AMPERSAND, "&"); break;
            case ':':
            case '=':
                // CE对等式处理
                ScanCEEquivalent();
                return;
            default:
                // 处理标识符或关键字
                if (IsValidIdentifierStart(c)) {
                    ScanIdentifierOrKeyword();
                    return;
                }
                
                // 处理无修饰字面量
                if (IsValidUnquotedChar(c)) {
                    ScanUnquotedString();
                    return;
                }
                
                // 处理数字
                if (IsDigit(c)) {
                    ScanNumber();
                    return;
                }
                
                // 未知字符
                ReportError("未知字符: " + std::string(1, c));
                break;
        }
        
        AdvancePosition();
    }

    void CHTLLexer::ScanIdentifierOrKeyword() {
        size_t start = currentPos;
        
        // 扫描标识符
        while (!IsAtEnd() && IsValidIdentifierChar(PeekChar())) {
            AdvancePosition();
        }
        
        std::string text = sourceCode.substr(start, currentPos - start);
        
        // 检查是否为关键字
        if (CHTLTokenUtil::IsKeyword(text)) {
            CHTLTokenType keywordType = CHTLTokenUtil::GetKeywordType(text);
            AddToken(keywordType, text);
        } else if (CHTLTokenUtil::IsHTMLElement(text)) {
            AddToken(CHTLTokenType::HTML_ELEMENT, text);
        } else {
            AddToken(CHTLTokenType::IDENTIFIER, text);
        }
    }

    void CHTLLexer::ScanString() {
        size_t start = currentPos;
        AdvancePosition(); // 跳过开始的引号
        
        while (!IsAtEnd() && PeekChar() != '"') {
            if (PeekChar() == '\\') {
                AdvancePosition(); // 跳过转义字符
                if (!IsAtEnd()) {
                    AdvancePosition(); // 跳过被转义的字符
                }
            } else {
                AdvancePosition();
            }
        }
        
        if (IsAtEnd()) {
            ReportError("未终止的字符串字面量");
            return;
        }
        
        AdvancePosition(); // 跳过结束的引号
        
        std::string value = sourceCode.substr(start + 1, currentPos - start - 2);
        AddToken(CHTLTokenType::STRING_LITERAL, value);
    }

    void CHTLLexer::ScanSingleQuoteString() {
        size_t start = currentPos;
        AdvancePosition(); // 跳过开始的引号
        
        while (!IsAtEnd() && PeekChar() != '\'') {
            if (PeekChar() == '\\') {
                AdvancePosition(); // 跳过转义字符
                if (!IsAtEnd()) {
                    AdvancePosition(); // 跳过被转义的字符
                }
            } else {
                AdvancePosition();
            }
        }
        
        if (IsAtEnd()) {
            ReportError("未终止的单引号字符串");
            return;
        }
        
        AdvancePosition(); // 跳过结束的引号
        
        std::string value = sourceCode.substr(start + 1, currentPos - start - 2);
        AddToken(CHTLTokenType::SINGLE_QUOTE_STRING, value);
    }

    void CHTLLexer::ScanUnquotedString() {
        size_t start = currentPos;
        
        // 扫描无修饰字面量
        while (!IsAtEnd() && IsValidUnquotedChar(PeekChar())) {
            AdvancePosition();
        }
        
        std::string value = sourceCode.substr(start, currentPos - start);
        AddToken(CHTLTokenType::UNQUOTED_STRING, value);
    }

    void CHTLLexer::ScanNumber() {
        size_t start = currentPos;
        
        // 扫描整数部分
        while (!IsAtEnd() && IsDigit(PeekChar())) {
            AdvancePosition();
        }
        
        // 检查小数点
        if (!IsAtEnd() && PeekChar() == '.' && IsDigit(PeekChar(1))) {
            AdvancePosition(); // 跳过小数点
            while (!IsAtEnd() && IsDigit(PeekChar())) {
                AdvancePosition();
            }
        }
        
        std::string value = sourceCode.substr(start, currentPos - start);
        AddToken(CHTLTokenType::NUMBER, value);
    }

    void CHTLLexer::ScanComment() {
        size_t start = currentPos;
        
        if (PeekChar() == '/' && PeekChar(1) == '/') {
            // 单行注释
            AdvancePosition(2);
            while (!IsAtEnd() && !IsNewline(PeekChar())) {
                AdvancePosition();
            }
            std::string value = sourceCode.substr(start + 2, currentPos - start - 2);
            AddToken(CHTLTokenType::SINGLE_LINE_COMMENT, value);
        } else if (PeekChar() == '/' && PeekChar(1) == '*') {
            // 多行注释
            AdvancePosition(2);
            while (!IsAtEnd() && !(PeekChar() == '*' && PeekChar(1) == '/')) {
                AdvancePosition();
            }
            if (!IsAtEnd()) {
                AdvancePosition(2); // 跳过 */
            }
            std::string value = sourceCode.substr(start + 2, currentPos - start - 4);
            AddToken(CHTLTokenType::MULTI_LINE_COMMENT, value);
        } else if (PeekChar() == '-' && PeekChar(1) == '-') {
            // 生成器注释
            AdvancePosition(2);
            while (!IsAtEnd() && !IsNewline(PeekChar())) {
                AdvancePosition();
            }
            std::string value = sourceCode.substr(start + 2, currentPos - start - 2);
            AddToken(CHTLTokenType::GENERATOR_COMMENT, value);
        }
    }

    void CHTLLexer::ScanBlockKeyword() {
        size_t start = currentPos;
        
        // 扫描到]
        while (!IsAtEnd() && PeekChar() != ']') {
            AdvancePosition();
        }
        
        if (IsAtEnd()) {
            ReportError("未终止的块关键字");
            return;
        }
        
        AdvancePosition(); // 跳过]
        
        std::string text = sourceCode.substr(start, currentPos - start);
        
        // 检查是否为有效的块关键字
        if (CHTLTokenUtil::IsKeyword(text)) {
            CHTLTokenType keywordType = CHTLTokenUtil::GetKeywordType(text);
            AddToken(keywordType, text);
        } else {
            AddToken(CHTLTokenType::UNKNOWN, text);
        }
    }

    void CHTLLexer::ScanTypeKeyword() {
        size_t start = currentPos;
        AdvancePosition(); // 跳过@
        
        // 扫描类型名
        while (!IsAtEnd() && IsValidIdentifierChar(PeekChar())) {
            AdvancePosition();
        }
        
        std::string text = sourceCode.substr(start, currentPos - start);
        
        // 检查是否为有效的类型关键字
        if (CHTLTokenUtil::IsKeyword(text)) {
            CHTLTokenType keywordType = CHTLTokenUtil::GetKeywordType(text);
            AddToken(keywordType, text);
        } else {
            // 自定义类型（如@Vue）
            AddToken(CHTLTokenType::IDENTIFIER, text);
        }
    }

    void CHTLLexer::ScanSelector() {
        size_t start = currentPos;
        char firstChar = PeekChar();
        AdvancePosition(); // 跳过. 或 #
        
        // 扫描选择器名称
        while (!IsAtEnd() && (IsAlphaNumeric(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
            AdvancePosition();
        }
        
        std::string text = sourceCode.substr(start, currentPos - start);
        
        if (firstChar == '.') {
            AddToken(CHTLTokenType::CLASS_SELECTOR, text);
        } else if (firstChar == '#') {
            AddToken(CHTLTokenType::ID_SELECTOR, text);
        }
    }

    void CHTLLexer::ScanCEEquivalent() {
        char c = PeekChar();
        if (c == ':') {
            AddToken(CHTLTokenType::COLON, ":");
        } else if (c == '=') {
            AddToken(CHTLTokenType::EQUALS, "=");
        }
        AdvancePosition();
    }

    // 辅助方法实现
    void CHTLLexer::AdvancePosition() {
        if (!IsAtEnd()) {
            if (sourceCode[currentPos] == '\n') {
                currentLine++;
                currentColumn = 1;
            } else {
                currentColumn++;
            }
            currentPos++;
        }
    }

    void CHTLLexer::AdvancePosition(int count) {
        for (int i = 0; i < count && !IsAtEnd(); i++) {
            AdvancePosition();
        }
    }

    char CHTLLexer::PeekChar(size_t offset) const {
        size_t pos = currentPos + offset;
        return (pos < sourceCode.length()) ? sourceCode[pos] : '\0';
    }

    std::string CHTLLexer::PeekString(size_t length) const {
        size_t end = std::min(currentPos + length, sourceCode.length());
        return sourceCode.substr(currentPos, end - currentPos);
    }

    bool CHTLLexer::IsAtEnd() const {
        return currentPos >= sourceCode.length();
    }

    bool CHTLLexer::IsAlpha(char c) const {
        return std::isalpha(c) || c == '_';
    }

    bool CHTLLexer::IsDigit(char c) const {
        return std::isdigit(c);
    }

    bool CHTLLexer::IsAlphaNumeric(char c) const {
        return IsAlpha(c) || IsDigit(c);
    }

    bool CHTLLexer::IsWhitespace(char c) const {
        return c == ' ' || c == '\t' || c == '\r';
    }

    bool CHTLLexer::IsNewline(char c) const {
        return c == '\n';
    }

    bool CHTLLexer::IsValidUnquotedChar(char c) const {
        // 无修饰字面量允许的字符
        return IsAlphaNumeric(c) || c == '-' || c == '_';
    }

    bool CHTLLexer::IsValidIdentifierStart(char c) const {
        return IsAlpha(c);
    }

    bool CHTLLexer::IsValidIdentifierChar(char c) const {
        return IsAlphaNumeric(c);
    }

    void CHTLLexer::AddToken(CHTLTokenType type, const std::string& value) {
        tokens.emplace_back(type, value, currentPos - value.length(), currentPos, currentLine, currentColumn);
    }

    void CHTLLexer::AddToken(CHTLTokenType type) {
        tokens.emplace_back(type, "", currentPos, currentPos, currentLine, currentColumn);
    }

    void CHTLLexer::ReportError(const std::string& message) {
        std::cerr << "CHTL词法分析错误 (行 " << currentLine << ", 列 " << currentColumn << "): " << message << std::endl;
    }

    void CHTLLexer::SkipWhitespace() {
        while (!IsAtEnd() && IsWhitespace(PeekChar())) {
            AdvancePosition();
        }
    }

    void CHTLLexer::Reset() {
        tokens.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;
    }

    void CHTLLexer::Reset(const std::string& newSource) {
        sourceCode = newSource;
        Reset();
    }

    const std::vector<CHTLToken>& CHTLLexer::GetTokens() const {
        return tokens;
    }

    size_t CHTLLexer::GetCurrentPosition() const {
        return currentPos;
    }

    int CHTLLexer::GetCurrentLine() const {
        return currentLine;
    }

    int CHTLLexer::GetCurrentColumn() const {
        return currentColumn;
    }

    void CHTLLexer::SetGlobalMap(std::shared_ptr<CHTLGlobalMap> map) {
        globalMap = map;
    }

    void CHTLLexer::PrintTokens() const {
        std::cout << "=== CHTL词法分析结果 ===" << std::endl;
        for (size_t i = 0; i < tokens.size(); i++) {
            const auto& token = tokens[i];
            std::cout << i << ": " << CHTLTokenUtil::TokenTypeToString(token.type) 
                      << " [" << token.value << "] "
                      << "(行:" << token.lineNumber << ", 列:" << token.columnNumber << ")" << std::endl;
        }
        std::cout << "Token总数: " << tokens.size() << std::endl;
    }

    void CHTLLexer::PrintCurrentState() const {
        std::cout << "当前位置: " << currentPos << std::endl;
        std::cout << "当前行号: " << currentLine << std::endl;
        std::cout << "当前列号: " << currentColumn << std::endl;
        std::cout << "是否结束: " << (IsAtEnd() ? "是" : "否") << std::endl;
    }

} // namespace CHTL