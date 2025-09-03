#include "CHTLLexer.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& sourceCode)
    : m_SourceCode(sourceCode), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLTokenManager>();
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_ContextManager = std::make_unique<CHTLContextManager>();
}

CHTLLexer::CHTLLexer()
    : m_SourceCode(""), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLTokenManager>();
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_ContextManager = std::make_unique<CHTLContextManager>();
}

bool CHTLLexer::Tokenize() {
    Reset();
    
    if (m_SourceCode.empty()) {
        return true; // 空代码也是有效的
    }
    
    try {
        while (!IsAtEnd()) {
            if (!ScanNextToken()) {
                if (m_ErrorMessage.empty()) {
                    SetError("Unknown tokenization error");
                }
                return false;
            }
        }
        
        // 添加EOF令牌
        CHTLToken eofToken;
        eofToken.Type = CHTLTokenType::EOF_TOKEN;
        eofToken.Value = "";
        eofToken.Line = m_CurrentLine;
        eofToken.Column = m_CurrentColumn;
        eofToken.Position = m_CurrentPosition;
        eofToken.Length = 0;
        m_Tokens.push_back(eofToken);
        
        return true;
    }
    catch (const std::exception& e) {
        SetError("Tokenization exception: " + std::string(e.what()));
        return false;
    }
}

bool CHTLLexer::ScanNextToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return true;
    }
    
    char ch = PeekChar();
    size_t tokenStart = m_CurrentPosition;
    
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = tokenStart;
    
    try {
        // 简化的令牌识别
        if (ch == '[') {
            token = ScanBlockStructure();
        }
        else if (ch == '{') {
            token = CreateSimpleToken(CHTLTokenType::LEFT_BRACE, "{");
        }
        else if (ch == '}') {
            token = CreateSimpleToken(CHTLTokenType::RIGHT_BRACE, "}");
        }
        else if (ch == ';') {
            token = CreateSimpleToken(CHTLTokenType::SEMICOLON, ";");
        }
        else if (ch == ':') {
            token = CreateSimpleToken(CHTLTokenType::COLON, ":");
        }
        else if (ch == '=') {
            token = CreateSimpleToken(CHTLTokenType::EQUAL, "=");
        }
        else if (ch == '"') {
            token = ScanStringLiteral('"');
        }
        else if (ch == '\'') {
            token = ScanStringLiteral('\'');
        }
        else if (ch == '/' && PeekChar(1) == '/') {
            token = ScanComment();
        }
        else if (ch == '/' && PeekChar(1) == '*') {
            token = ScanComment();
        }
        else if (ch == '-' && PeekChar(1) == '-') {
            token = ScanComment();
        }
        else if (std::isalpha(ch) || ch == '_' || (ch & 0x80)) { // 支持UTF-8
            token = ScanIdentifierOrKeyword();
        }
        else if (std::isdigit(ch)) {
            token = ScanNumber();
        }
        else {
            // 跳过未知字符
            GetNextChar();
            return true;
        }
        
        if (token.Type != CHTLTokenType::UNKNOWN) {
            m_Tokens.push_back(token);
        }
        
        return true;
    }
    catch (const std::exception& e) {
        SetError("Token scanning error: " + std::string(e.what()));
        return false;
    }
}

CHTLToken CHTLLexer::CreateSimpleToken(CHTLTokenType type, const std::string& value) {
    CHTLToken token;
    token.Type = type;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    token.Length = value.length();
    
    // 消费字符
    for (size_t i = 0; i < value.length() && !IsAtEnd(); ++i) {
        GetNextChar();
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取标识符 (支持UTF-8字符)
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isalnum(ch) || ch == '_' || ch == '-' || 
            (ch & 0x80)) { // UTF-8多字节字符
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = start;
    token.Length = value.length();
    
    // 检查是否是关键字
    if (value == "text") {
        token.Type = CHTLTokenType::TEXT;
    }
    else if (value == "style") {
        token.Type = CHTLTokenType::STYLE;
    }
    else if (value == "script") {
        token.Type = CHTLTokenType::SCRIPT;
    }
    else if (value == "html" || value == "head" || value == "body" || 
             value == "div" || value == "span" || value == "title" || 
             value == "meta" || value == "h1" || value == "h2" || value == "h3" ||
             value == "p" || value == "a" || value == "img" || value == "ul" ||
             value == "li" || value == "button" || value == "input" || value == "form") {
        token.Type = CHTLTokenType::IDENTIFIER; // HTML标签作为标识符处理
    }
    else if (value == "except") {
        token.Type = CHTLTokenType::EXCEPT;
    }
    else if (value == "delete") {
        token.Type = CHTLTokenType::DELETE;
    }
    else {
        token.Type = CHTLTokenType::IDENTIFIER;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanBlockStructure() {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    
    if (PeekChar() != '[') {
        token.Type = CHTLTokenType::UNKNOWN;
        return token;
    }
    
    std::string value;
    value += GetNextChar(); // 消费 '['
    
    // 读取到 ']'
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar(); // 消费 ']'
    }
    
    token.Value = value;
    
    // 识别块结构类型
    if (value.find("Template") != std::string::npos) {
        token.Type = CHTLTokenType::TEMPLATE_BLOCK;
    }
    else if (value.find("Custom") != std::string::npos) {
        token.Type = CHTLTokenType::CUSTOM_BLOCK;
    }
    else if (value.find("Origin") != std::string::npos) {
        token.Type = CHTLTokenType::ORIGIN_BLOCK;
    }
    else if (value.find("Configuration") != std::string::npos) {
        token.Type = CHTLTokenType::CONFIGURATION_BLOCK;
    }
    else if (value.find("Import") != std::string::npos) {
        token.Type = CHTLTokenType::IMPORT_BLOCK;
    }
    else if (value.find("Namespace") != std::string::npos) {
        token.Type = CHTLTokenType::NAMESPACE_BLOCK;
    }
    else {
        token.Type = CHTLTokenType::LEFT_BRACKET; // 简化为左方括号
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanStringLiteral(char quote) {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    token.Type = CHTLTokenType::STRING_LITERAL;
    
    std::string value;
    GetNextChar(); // 消费开始引号
    
    while (!IsAtEnd() && PeekChar() != quote) {
        char ch = GetNextChar();
        if (ch == '\\' && !IsAtEnd()) {
            // 处理转义字符
            char escaped = GetNextChar();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += escaped; break;
            }
        } else {
            value += ch;
        }
    }
    
    if (!IsAtEnd() && PeekChar() == quote) {
        GetNextChar(); // 消费结束引号
    }
    
    token.Value = value;
    return token;
}

CHTLToken CHTLLexer::ScanNumber() {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    token.Type = CHTLTokenType::NUMBER;
    
    std::string value;
    
    while (!IsAtEnd() && (std::isdigit(PeekChar()) || PeekChar() == '.')) {
        value += GetNextChar();
    }
    
    token.Value = value;
    return token;
}

CHTLToken CHTLLexer::ScanComment() {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    token.Type = CHTLTokenType::COMMENT;
    
    std::string value;
    
    if (PeekChar() == '/' && PeekChar(1) == '/') {
        // 单行注释
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    else if (PeekChar() == '/' && PeekChar(1) == '*') {
        // 多行注释
        GetNextChar(); // /
        GetNextChar(); // *
        value += "/*";
        
        while (!IsAtEnd()) {
            if (PeekChar() == '*' && PeekChar(1) == '/') {
                value += GetNextChar(); // *
                value += GetNextChar(); // /
                break;
            }
            value += GetNextChar();
        }
    }
    else if (PeekChar() == '-' && PeekChar(1) == '-') {
        // CHTL特殊注释
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    
    token.Value = value;
    return token;
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            if (ch == '\n') {
                m_CurrentLine++;
                m_CurrentColumn = 1;
            } else {
                m_CurrentColumn++;
            }
            m_CurrentPosition++;
        } else {
            break;
        }
    }
}

char CHTLLexer::PeekChar(size_t offset) const {
    size_t pos = m_CurrentPosition + offset;
    if (pos >= m_SourceCode.length()) {
        return '\0';
    }
    return m_SourceCode[pos];
}

char CHTLLexer::GetNextChar() {
    if (IsAtEnd()) {
        return '\0';
    }
    
    char ch = m_SourceCode[m_CurrentPosition++];
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    } else {
        m_CurrentColumn++;
    }
    
    return ch;
}

bool CHTLLexer::IsAtEnd() const {
    return m_CurrentPosition >= m_SourceCode.length();
}

void CHTLLexer::Reset() {
    m_CurrentPosition = 0;
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Tokens.clear();
}

void CHTLLexer::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = "Line " + std::to_string(m_CurrentLine) + 
                    ", Column " + std::to_string(m_CurrentColumn) + ": " + message;
}

std::string CHTLLexer::GetErrorMessage() const {
    return m_ErrorMessage;
}

bool CHTLLexer::HasError() const {
    return m_HasError;
}

const std::vector<CHTLToken>& CHTLLexer::GetTokens() const {
    return m_Tokens;
}

void CHTLLexer::SetSourceCode(const std::string& sourceCode) {
    m_SourceCode = sourceCode;
    Reset();
}

// 简化的辅助方法
bool CHTLLexer::ValidateUTF8(const std::string& str) {
    // 简化的UTF-8验证 - 总是返回true
    return true;
}

bool CHTLLexer::IsAllowedInCurrentContext(const std::string& tokenType) {
    // 简化的上下文检查 - 总是返回true
    return true;
}

bool CHTLLexer::UpdateStateAndContext(const CHTLToken& token) {
    // 简化的状态更新 - 总是返回true
    return true;
}

// 占位实现的其他方法
CHTLToken CHTLLexer::ScanAtType() {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    
    std::string value;
    value += GetNextChar(); // 消费 '@'
    
    // 读取@后面的标识符
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    token.Value = value;
    token.Length = value.length();
    
    // 识别@类型
    if (value == "@Style") {
        token.Type = CHTLTokenType::AT_STYLE;
    }
    else if (value == "@Element") {
        token.Type = CHTLTokenType::AT_ELEMENT;
    }
    else if (value == "@Var") {
        token.Type = CHTLTokenType::AT_VAR;
    }
    else if (value == "@Html") {
        token.Type = CHTLTokenType::AT_HTML;
    }
    else if (value == "@JavaScript") {
        token.Type = CHTLTokenType::AT_JAVASCRIPT;
    }
    else if (value == "@Chtl") {
        token.Type = CHTLTokenType::AT_CHTL;
    }
    else if (value == "@CJmod") {
        token.Type = CHTLTokenType::AT_CJMOD;
    }
    else {
        token.Type = CHTLTokenType::CUSTOM_AT_TYPE;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanSelector() {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    
    std::string value;
    char first = GetNextChar(); // 消费 '.' 或 '#'
    value += first;
    
    // 读取选择器名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    token.Value = value;
    token.Length = value.length();
    
    // 设置正确的类型
    if (first == '.') {
        token.Type = CHTLTokenType::CLASS_SELECTOR;
    } else if (first == '#') {
        token.Type = CHTLTokenType::ID_SELECTOR;
    } else {
        token.Type = CHTLTokenType::DOT;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanContextReference() {
    CHTLToken token;
    token.Type = CHTLTokenType::CONTEXT_REFERENCE;
    token.Value = "&";
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = 1;
    
    GetNextChar(); // 消费 '&'
    return token;
}

CHTLToken CHTLLexer::HandleCEEquivalence(char ch) {
    CHTLToken token;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    
    if (ch == ':') {
        token.Type = CHTLTokenType::COLON;
        token.Value = ":";
        token.Length = 1;
    } else {
        token.Type = CHTLTokenType::EQUAL;
        token.Value = "=";
        token.Length = 1;
    }
    
    GetNextChar(); // 消费字符
    return token;
}

CHTLToken CHTLLexer::ScanOperator() {
    CHTLToken token;
    token.Type = CHTLTokenType::UNKNOWN;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition;
    token.Length = value.length();
    
    std::string value;
    value += GetNextChar();
    token.Value = value;
    
    return token;
}

CHTLToken CHTLLexer::CreateToken(CHTLTokenType type, const std::string& value, size_t position) {
    CHTLToken token;
    token.Type = type;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = position;
    return token;
}

// 其他占位实现
std::string CHTLLexer::ResolveVariableReference(const std::string& varRef) {
    return varRef;
}

void CHTLLexer::HandleConfigurationItem(const std::string& configKey, const std::string& configValue) {
    // 占位实现
}

bool CHTLLexer::ProcessCHTLKeyword(const std::string& keyword) {
    return true;
}

bool CHTLLexer::ProcessCHTLJSKeyword(const std::string& keyword) {
    return true;
}

void CHTLLexer::UpdateStateMachine(const CHTLToken& token) {
    // 占位实现
}

void CHTLLexer::UpdateContext(const CHTLToken& token) {
    // 占位实现
}

bool CHTLLexer::ValidateTokenInContext(const CHTLToken& token) {
    return true;
}

} // namespace CHTL