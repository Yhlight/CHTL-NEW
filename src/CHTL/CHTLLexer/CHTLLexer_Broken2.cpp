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
        // 空代码，添加EOF令牌
        m_Tokens.push_back(CreateToken(CHTLTokenType::EOF_TOKEN, "", 0));
        return true;
    }
    
    try {
        while (!IsAtEnd()) {
            if (!ScanNextToken()) {
                if (m_ErrorMessage.empty()) {
                    SetError("Unknown tokenization error at position " + std::to_string(m_CurrentPosition));
                }
                return false;
            }
        }
        
        // 添加EOF令牌
        m_Tokens.push_back(CreateToken(CHTLTokenType::EOF_TOKEN, "", 0));
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
    
    try {
        // 简化但正确的令牌识别
        if (ch == '[') {
            return ScanBlockKeyword();
        }
        else if (ch == '{') {
            m_Tokens.push_back(CreateToken(CHTLTokenType::LEFT_BRACE, "{", 1));
            GetNextChar();
            return true;
        }
        else if (ch == '}') {
            m_Tokens.push_back(CreateToken(CHTLTokenType::RIGHT_BRACE, "}", 1));
            GetNextChar();
            return true;
        }
        else if (ch == ';') {
            m_Tokens.push_back(CreateToken(CHTLTokenType::SEMICOLON, ";", 1));
            GetNextChar();
            return true;
        }
        else if (ch == ':') {
            m_Tokens.push_back(CreateToken(CHTLTokenType::COLON, ":", 1));
            GetNextChar();
            return true;
        }
        else if (ch == '=') {
            m_Tokens.push_back(CreateToken(CHTLTokenType::EQUAL, "=", 1));
            GetNextChar();
            return true;
        }
        else if (ch == '"') {
            return ScanStringLiteral('"');
        }
        else if (ch == '\'') {
            return ScanStringLiteral('\'');
        }
        else if (ch == '/' && PeekChar(2) == '/') {
            return ScanLineComment();
        }
        else if (ch == '/' && PeekChar(2) == '*') {
            return ScanBlockComment();
        }
        else if (ch == '-' && PeekChar(2) == '-') {
            return ScanGeneratorComment();
        }
        else if (std::isalpha(ch) || ch == '_' || (ch & 0x80)) { // 支持UTF-8
            return ScanIdentifierOrKeyword();
        }
        else if (std::isdigit(ch)) {
            return ScanNumber();
        }
        else if (ch == '.') {
            return ScanClassSelector();
        }
        else if (ch == '#') {
            return ScanIdSelector();
        }
        else if (ch == '@') {
            return ScanAtType();
        }
        else {
            // 跳过未知字符
            GetNextChar();
            return true;
        }
    }
    catch (const std::exception& e) {
        SetError("Token scanning error: " + std::string(e.what()));
        return false;
    }
}

bool CHTLLexer::ScanIdentifierOrKeyword() {
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
    
    if (value.empty()) {
        return false;
    }
    
    CHTLTokenType type;
    
    // 检查是否是CHTL关键字
    if (value == "text") {
        type = CHTLTokenType::TEXT;
    }
    else if (value == "style") {
        type = CHTLTokenType::STYLE;
    }
    else if (value == "script") {
        type = CHTLTokenType::SCRIPT;
    }
    else if (value == "except") {
        type = CHTLTokenType::EXCEPT;
    }
    else if (value == "delete") {
        type = CHTLTokenType::DELETE;
    }
    else if (value == "inherit") {
        type = CHTLTokenType::INHERIT;
    }
    else if (value == "insert") {
        type = CHTLTokenType::INSERT;
    }
    else if (value == "after") {
        type = CHTLTokenType::AFTER;
    }
    else if (value == "before") {
        type = CHTLTokenType::BEFORE;
    }
    else if (value == "replace") {
        type = CHTLTokenType::REPLACE;
    }
    else if (value == "from") {
        type = CHTLTokenType::FROM;
    }
    else if (value == "as") {
        type = CHTLTokenType::AS;
    }
    else if (value == "use") {
        type = CHTLTokenType::USE;
    }
    else {
        type = CHTLTokenType::IDENTIFIER;
    }
    
    m_Tokens.push_back(CreateToken(type, value, value.length()));
    return true;
}

bool CHTLLexer::ScanBlockKeyword() {
    std::string value;
    
    // 读取完整的块关键字 [...]
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar(); // 消费 ']'
    }
    
    CHTLTokenType type;
    
    // 识别块结构类型
    if (value.find("Template") != std::string::npos) {
        type = CHTLTokenType::TEMPLATE_BLOCK;
    }
    else if (value.find("Custom") != std::string::npos) {
        type = CHTLTokenType::CUSTOM_BLOCK;
    }
    else if (value.find("Origin") != std::string::npos) {
        type = CHTLTokenType::ORIGIN_BLOCK;
    }
    else if (value.find("Configuration") != std::string::npos) {
        type = CHTLTokenType::CONFIGURATION_BLOCK;
    }
    else if (value.find("Import") != std::string::npos) {
        type = CHTLTokenType::IMPORT_BLOCK;
    }
    else if (value.find("Namespace") != std::string::npos) {
        type = CHTLTokenType::NAMESPACE_BLOCK;
    }
    else if (value.find("Info") != std::string::npos) {
        type = CHTLTokenType::INFO_BLOCK;
    }
    else if (value.find("Export") != std::string::npos) {
        type = CHTLTokenType::EXPORT_BLOCK;
    }
    else {
        type = CHTLTokenType::LEFT_BRACKET;
    }
    
    m_Tokens.push_back(CreateToken(type, value, value.length()));
    return true;
}

bool CHTLLexer::ScanStringLiteral(char quote) {
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
    
    CHTLTokenType type = (quote == '"') ? CHTLTokenType::STRING_LITERAL : CHTLTokenType::SINGLE_QUOTE_STRING;
    m_Tokens.push_back(CreateToken(type, value, value.length() + 2)); // +2 for quotes
    return true;
}

bool CHTLLexer::ScanNumber() {
    std::string value;
    
    while (!IsAtEnd() && (std::isdigit(PeekChar()) || PeekChar() == '.')) {
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::NUMBER, value, value.length()));
    return true;
}

bool CHTLLexer::ScanLineComment() {
    std::string value;
    
    // 消费 //
    value += GetNextChar(); // /
    value += GetNextChar(); // /
    
    // 读取到行尾
    while (!IsAtEnd() && PeekChar() != '\n') {
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::SINGLE_LINE_COMMENT, value, value.length()));
    return true;
}

bool CHTLLexer::ScanBlockComment() {
    std::string value;
    
    // 消费 /*
    value += GetNextChar(); // /
    value += GetNextChar(); // *
    
    // 读取到 */
    while (!IsAtEnd()) {
        if (PeekChar() == '*' && PeekChar(2) == '/') {
            value += GetNextChar(); // *
            value += GetNextChar(); // /
            break;
        }
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::MULTI_LINE_COMMENT, value, value.length()));
    return true;
}

bool CHTLLexer::ScanGeneratorComment() {
    std::string value;
    
    // 消费 --
    value += GetNextChar(); // -
    value += GetNextChar(); // -
    
    // 读取到行尾
    while (!IsAtEnd() && PeekChar() != '\n') {
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::GENERATOR_COMMENT, value, value.length()));
    return true;
}

bool CHTLLexer::ScanClassSelector() {
    std::string value;
    value += GetNextChar(); // 消费 '.'
    
    // 读取选择器名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::CLASS_SELECTOR, value, value.length()));
    return true;
}

bool CHTLLexer::ScanIdSelector() {
    std::string value;
    value += GetNextChar(); // 消费 '#'
    
    // 读取选择器名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    m_Tokens.push_back(CreateToken(CHTLTokenType::ID_SELECTOR, value, value.length()));
    return true;
}

bool CHTLLexer::ScanAtType() {
    std::string value;
    value += GetNextChar(); // 消费 '@'
    
    // 读取@后面的标识符
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    CHTLTokenType type;
    
    // 识别@类型
    if (value == "@Style") {
        type = CHTLTokenType::AT_STYLE;
    }
    else if (value == "@Element") {
        type = CHTLTokenType::AT_ELEMENT;
    }
    else if (value == "@Var") {
        type = CHTLTokenType::AT_VAR;
    }
    else if (value == "@Html") {
        type = CHTLTokenType::AT_HTML;
    }
    else if (value == "@JavaScript") {
        type = CHTLTokenType::AT_JAVASCRIPT;
    }
    else if (value == "@Chtl") {
        type = CHTLTokenType::AT_CHTL;
    }
    else if (value == "@CJmod") {
        type = CHTLTokenType::AT_CJMOD;
    }
    else {
        type = CHTLTokenType::CUSTOM_AT_TYPE;
    }
    
    m_Tokens.push_back(CreateToken(type, value, value.length()));
    return true;
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            GetNextChar();
        } else {
            break;
        }
    }
}

char CHTLLexer::PeekChar(size_t offset) {
    size_t pos = m_CurrentPosition + offset - 1; // offset从1开始
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

void CHTLLexer::UpdateLineColumn(char ch) {
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    } else {
        m_CurrentColumn++;
    }
}

CHTLToken CHTLLexer::CreateToken(CHTLTokenType type, const std::string& value, size_t length) {
    CHTLToken token;
    token.Type = type;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn;
    token.Position = m_CurrentPosition - length; // 调整位置
    token.Length = length;
    return token;
}

void CHTLLexer::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = "Line " + std::to_string(m_CurrentLine) + 
                    ", Column " + std::to_string(m_CurrentColumn) + ": " + message;
}

void CHTLLexer::Reset() {
    m_CurrentPosition = 0;
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Tokens.clear();
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

// 简化的占位实现
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