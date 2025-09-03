#include "CHTLJSLexer.h"
#include <iostream>
#include <cctype>
#include <regex>

namespace CHTLJS {

    CHTLJSLexer::CHTLJSLexer(const std::string& source) 
        : sourceCode(source), currentPos(0), currentLine(1), currentColumn(1) {
        globalMap = CHTLJSGlobalMap::GetInstance();
    }

    std::vector<CHTLJSToken> CHTLJSLexer::Tokenize() {
        tokens.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;

        while (!IsAtEnd()) {
            ScanToken();
        }

        // 添加EOF Token
        AddToken(CHTLJSTokenType::EOF_TOKEN, "");
        
        return tokens;
    }

    void CHTLJSLexer::ScanToken() {
        char c = PeekChar();
        
        // 跳过空白字符
        if (IsWhitespace(c)) {
            SkipWhitespace();
            return;
        }
        
        // 处理换行
        if (IsNewline(c)) {
            AddToken(CHTLJSTokenType::NEWLINE, std::string(1, c));
            AdvancePosition();
            return;
        }
        
        // 处理CHTL JS选择器 {{}}
        if (c == '{' && PeekChar(1) == '{') {
            ScanSelector();
            return;
        }
        
        // 处理->操作符
        if (c == '-' && PeekChar(1) == '>') {
            ScanArrowOperator();
            return;
        }
        
        // 处理&->事件绑定操作符
        if (c == '&' && PeekChar(1) == '-' && PeekChar(2) == '>') {
            ScanEventBindingOperator();
            return;
        }
        
        // 处理注释
        if (c == '/') {
            char next = PeekChar(1);
            if (next == '/' || next == '*') {
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
        if (c == '"' || c == '\'') {
            ScanString();
            return;
        }
        
        // 处理单字符操作符
        switch (c) {
            case '{': AddToken(CHTLJSTokenType::LEFT_BRACE, "{"); break;
            case '}': AddToken(CHTLJSTokenType::RIGHT_BRACE, "}"); break;
            case '[': AddToken(CHTLJSTokenType::LEFT_BRACKET, "["); break;
            case ']': AddToken(CHTLJSTokenType::RIGHT_BRACKET, "]"); break;
            case '(': AddToken(CHTLJSTokenType::LEFT_PAREN, "("); break;
            case ')': AddToken(CHTLJSTokenType::RIGHT_PAREN, ")"); break;
            case ';': AddToken(CHTLJSTokenType::SEMICOLON, ";"); break;
            case ':': AddToken(CHTLJSTokenType::COLON, ":"); break;
            case '=': AddToken(CHTLJSTokenType::EQUALS, "="); break;
            case ',': AddToken(CHTLJSTokenType::COMMA, ","); break;
            case '.': AddToken(CHTLJSTokenType::DOT, "."); break;
            case '&': AddToken(CHTLJSTokenType::AMPERSAND, "&"); break;
            default:
                // 处理标识符或关键字
                if (IsValidIdentifierStart(c)) {
                    ScanIdentifierOrKeyword();
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

    void CHTLJSLexer::ScanIdentifierOrKeyword() {
        size_t start = currentPos;
        
        // 扫描标识符
        while (!IsAtEnd() && IsValidIdentifierChar(PeekChar())) {
            AdvancePosition();
        }
        
        std::string text = sourceCode.substr(start, currentPos - start);
        
        // 检查是否为CHTL JS关键字
        if (CHTLJSTokenUtil::IsKeyword(text)) {
            CHTLJSTokenType keywordType = CHTLJSTokenUtil::GetKeywordType(text);
            AddToken(keywordType, text);
        } else if (CHTLJSTokenUtil::IsEvent(text)) {
            CHTLJSTokenType eventType = CHTLJSTokenUtil::GetEventType(text);
            AddToken(eventType, text);
        } else {
            AddToken(CHTLJSTokenType::IDENTIFIER, text);
        }
    }

    void CHTLJSLexer::ScanString() {
        char quote = PeekChar();
        size_t start = currentPos;
        AdvancePosition(); // 跳过开始的引号
        
        while (!IsAtEnd() && PeekChar() != quote) {
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
        AddToken(CHTLJSTokenType::STRING_LITERAL, value);
    }

    void CHTLJSLexer::ScanNumber() {
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
        AddToken(CHTLJSTokenType::NUMBER, value);
    }

    void CHTLJSLexer::ScanComment() {
        size_t start = currentPos;
        
        if (PeekChar() == '/' && PeekChar(1) == '/') {
            // 单行注释
            AdvancePosition(2);
            while (!IsAtEnd() && !IsNewline(PeekChar())) {
                AdvancePosition();
            }
            std::string value = sourceCode.substr(start + 2, currentPos - start - 2);
            AddToken(CHTLJSTokenType::SINGLE_LINE_COMMENT, value);
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
            AddToken(CHTLJSTokenType::MULTI_LINE_COMMENT, value);
        } else if (PeekChar() == '-' && PeekChar(1) == '-') {
            // 生成器注释
            AdvancePosition(2);
            while (!IsAtEnd() && !IsNewline(PeekChar())) {
                AdvancePosition();
            }
            std::string value = sourceCode.substr(start + 2, currentPos - start - 2);
            AddToken(CHTLJSTokenType::GENERATOR_COMMENT, value);
        }
    }

    void CHTLJSLexer::ScanSelector() {
        size_t start = currentPos;
        AdvancePosition(2); // 跳过{{
        
        // 扫描选择器内容
        while (!IsAtEnd() && !(PeekChar() == '}' && PeekChar(1) == '}')) {
            AdvancePosition();
        }
        
        if (IsAtEnd()) {
            ReportError("未终止的选择器语法");
            return;
        }
        
        AdvancePosition(2); // 跳过}}
        
        std::string selectorText = sourceCode.substr(start, currentPos - start);
        
        // 解析选择器类型
        CHTLJSTokenType selectorType = CHTLJSTokenUtil::ParseSelectorType(selectorText);
        AddToken(selectorType, selectorText);
    }

    void CHTLJSLexer::ScanArrowOperator() {
        AddToken(CHTLJSTokenType::ARROW, "->");
        AdvancePosition(2);
    }

    void CHTLJSLexer::ScanEventBindingOperator() {
        AddToken(CHTLJSTokenType::AMPERSAND_ARROW, "&->");
        AdvancePosition(3);
    }

    // 辅助方法实现
    void CHTLJSLexer::AdvancePosition() {
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

    void CHTLJSLexer::AdvancePosition(int count) {
        for (int i = 0; i < count && !IsAtEnd(); i++) {
            AdvancePosition();
        }
    }

    char CHTLJSLexer::PeekChar(size_t offset) const {
        size_t pos = currentPos + offset;
        return (pos < sourceCode.length()) ? sourceCode[pos] : '\0';
    }

    std::string CHTLJSLexer::PeekString(size_t length) const {
        size_t end = std::min(currentPos + length, sourceCode.length());
        return sourceCode.substr(currentPos, end - currentPos);
    }

    bool CHTLJSLexer::IsAtEnd() const {
        return currentPos >= sourceCode.length();
    }

    bool CHTLJSLexer::IsAlpha(char c) const {
        return std::isalpha(c) || c == '_';
    }

    bool CHTLJSLexer::IsDigit(char c) const {
        return std::isdigit(c);
    }

    bool CHTLJSLexer::IsAlphaNumeric(char c) const {
        return IsAlpha(c) || IsDigit(c);
    }

    bool CHTLJSLexer::IsWhitespace(char c) const {
        return c == ' ' || c == '\t' || c == '\r';
    }

    bool CHTLJSLexer::IsNewline(char c) const {
        return c == '\n';
    }

    bool CHTLJSLexer::IsValidSelectorChar(char c) const {
        return IsAlphaNumeric(c) || c == '.' || c == '#' || c == '-' || c == '_' || c == '[' || c == ']' || c == ' ';
    }

    bool CHTLJSLexer::IsValidIdentifierStart(char c) const {
        return IsAlpha(c);
    }

    bool CHTLJSLexer::IsValidIdentifierChar(char c) const {
        return IsAlphaNumeric(c);
    }

    void CHTLJSLexer::AddToken(CHTLJSTokenType type, const std::string& value) {
        tokens.emplace_back(type, value, currentPos - value.length(), currentPos, currentLine, currentColumn);
    }

    void CHTLJSLexer::AddToken(CHTLJSTokenType type) {
        tokens.emplace_back(type, "", currentPos, currentPos, currentLine, currentColumn);
    }

    void CHTLJSLexer::ReportError(const std::string& message) {
        std::cerr << "CHTL JS词法分析错误 (行 " << currentLine << ", 列 " << currentColumn << "): " << message << std::endl;
    }

    void CHTLJSLexer::SkipWhitespace() {
        while (!IsAtEnd() && IsWhitespace(PeekChar())) {
            AdvancePosition();
        }
    }

    void CHTLJSLexer::Reset() {
        tokens.clear();
        currentPos = 0;
        currentLine = 1;
        currentColumn = 1;
    }

    void CHTLJSLexer::Reset(const std::string& newSource) {
        sourceCode = newSource;
        Reset();
    }

    const std::vector<CHTLJSToken>& CHTLJSLexer::GetTokens() const {
        return tokens;
    }

    void CHTLJSLexer::PrintTokens() const {
        std::cout << "=== CHTL JS词法分析结果 ===" << std::endl;
        for (size_t i = 0; i < tokens.size(); i++) {
            const auto& token = tokens[i];
            std::cout << i << ": " << CHTLJSTokenUtil::TokenTypeToString(token.type) 
                      << " [" << token.value << "] "
                      << "(行:" << token.lineNumber << ", 列:" << token.columnNumber << ")" << std::endl;
        }
        std::cout << "CHTL JS Token总数: " << tokens.size() << std::endl;
    }

    bool CHTLJSLexer::ValidateSelector(const std::string& selector) const {
        return CHTLJSTokenUtil::IsValidSelectorSyntax(selector);
    }

    bool CHTLJSLexer::ValidateArrowChain(const std::string& chain) const {
        return CHTLJSTokenUtil::IsValidArrowSyntax(chain);
    }

    bool CHTLJSLexer::ValidateEventBinding(const std::string& binding) const {
        return CHTLJSTokenUtil::IsValidEventBinding(binding);
    }

    bool CHTLJSLexer::ValidateCHTLJSFunction(const std::string& function) const {
        return CHTLJSTokenUtil::IsValidCHTLJSFunction(function);
    }

} // namespace CHTLJS