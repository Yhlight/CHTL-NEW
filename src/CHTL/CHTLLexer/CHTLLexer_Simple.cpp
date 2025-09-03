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
        m_Tokens.push_back(CreateToken(CHTLTokenType::EOF_TOKEN, "", 0));
        return true;
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
    
    try {
        CHTLToken token;
        bool success = false;
        
        // 根据字符类型扫描令牌
        if (ch == '[') {
            token = ScanBlockStructure();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '{') {
            token = CreateToken(CHTLTokenType::LEFT_BRACE, "{", 1);
            GetNextChar();
            success = true;
        }
        else if (ch == '}') {
            token = CreateToken(CHTLTokenType::RIGHT_BRACE, "}", 1);
            GetNextChar();
            success = true;
        }
        else if (ch == ';') {
            token = CreateToken(CHTLTokenType::SEMICOLON, ";", 1);
            GetNextChar();
            success = true;
        }
        else if (ch == ':') {
            token = CreateToken(CHTLTokenType::COLON, ":", 1);
            GetNextChar();
            success = true;
        }
        else if (ch == '=') {
            token = CreateToken(CHTLTokenType::EQUAL, "=", 1);
            GetNextChar();
            success = true;
        }
        else if (ch == '"') {
            token = ScanStringLiteral('"');
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '\'') {
            token = ScanStringLiteral('\'');
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '/' && PeekChar(2) == '/') {
            token = ScanComment();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '/' && PeekChar(2) == '*') {
            token = ScanComment();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '-' && PeekChar(2) == '-') {
            token = ScanComment();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (std::isalpha(ch) || ch == '_' || (ch & 0x80)) {
            token = ScanIdentifierOrKeyword();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (std::isdigit(ch)) {
            token = ScanNumber();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '.') {
            token = ScanSelector();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '#') {
            token = ScanSelector();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else if (ch == '@') {
            token = ScanAtType();
            success = (token.Type != CHTLTokenType::UNKNOWN);
        }
        else {
            // 跳过未知字符
            GetNextChar();
            return true;
        }
        
        if (success && token.Type != CHTLTokenType::UNKNOWN) {
            m_Tokens.push_back(token);
        }
        
        return success;
    }
    catch (const std::exception& e) {
        SetError("Token scanning error: " + std::string(e.what()));
        return false;
    }
}

CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取标识符
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isalnum(ch) || ch == '_' || ch == '-' || (ch & 0x80)) {
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    // 检查关键字
    if (value == "text") {
        token.Type = CHTLTokenType::TEXT;
    }
    else if (value == "style") {
        token.Type = CHTLTokenType::STYLE;
    }
    else if (value == "script") {
        token.Type = CHTLTokenType::SCRIPT;
    }
    else if (value == "except") {
        token.Type = CHTLTokenType::EXCEPT;
    }
    else if (value == "delete") {
        token.Type = CHTLTokenType::DELETE;
    }
    else if (value == "inherit") {
        token.Type = CHTLTokenType::INHERIT;
    }
    else if (value == "from") {
        token.Type = CHTLTokenType::FROM;
    }
    else if (value == "as") {
        token.Type = CHTLTokenType::AS;
    }
    else {
        token.Type = CHTLTokenType::IDENTIFIER;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanBlockStructure() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取完整的 [...]
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar(); // 消费 ']'
    }
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    // 识别块类型
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
    else if (value.find("Info") != std::string::npos) {
        token.Type = CHTLTokenType::INFO_BLOCK;
    }
    else if (value.find("Export") != std::string::npos) {
        token.Type = CHTLTokenType::EXPORT_BLOCK;
    }
    else {
        token.Type = CHTLTokenType::LEFT_BRACKET;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanStringLiteral(char quote) {
    size_t start = m_CurrentPosition;
    std::string value;
    
    GetNextChar(); // 消费开始引号
    
    while (!IsAtEnd() && PeekChar() != quote) {
        char ch = GetNextChar();
        if (ch == '\\' && !IsAtEnd()) {
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
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length() - 2;
    token.Position = start;
    token.Length = value.length() + 2;
    token.Type = (quote == '"') ? CHTLTokenType::STRING_LITERAL : CHTLTokenType::SINGLE_QUOTE_STRING;
    
    return token;
}

CHTLToken CHTLLexer::ScanNumber() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    while (!IsAtEnd() && (std::isdigit(PeekChar()) || PeekChar() == '.')) {
        value += GetNextChar();
    }
    
    CHTLToken token;
    token.Type = CHTLTokenType::NUMBER;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    return token;
}

CHTLToken CHTLLexer::ScanComment() {
    size_t start = m_CurrentPosition;
    std::string value;
    CHTLTokenType type;
    
    if (PeekChar() == '/' && PeekChar(2) == '/') {
        // 单行注释
        type = CHTLTokenType::SINGLE_LINE_COMMENT;
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    else if (PeekChar() == '/' && PeekChar(2) == '*') {
        // 多行注释
        type = CHTLTokenType::MULTI_LINE_COMMENT;
        GetNextChar(); // /
        GetNextChar(); // *
        value += "/*";
        
        while (!IsAtEnd()) {
            if (PeekChar() == '*' && PeekChar(2) == '/') {
                value += GetNextChar(); // *
                value += GetNextChar(); // /
                break;
            }
            value += GetNextChar();
        }
    }
    else if (PeekChar() == '-' && PeekChar(2) == '-') {
        // 生成器注释
        type = CHTLTokenType::GENERATOR_COMMENT;
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    else {
        CHTLToken token;
        token.Type = CHTLTokenType::UNKNOWN;
        return token;
    }
    
    CHTLToken token;
    token.Type = type;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    return token;
}

CHTLToken CHTLLexer::ScanSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    char first = GetNextChar(); // 消费 '.' 或 '#'
    value += first;
    
    // 读取选择器名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    if (first == '.') {
        token.Type = CHTLTokenType::CLASS_SELECTOR;
    } else if (first == '#') {
        token.Type = CHTLTokenType::ID_SELECTOR;
    } else {
        token.Type = CHTLTokenType::DOT;
    }
    
    return token;
}

CHTLToken CHTLLexer::ScanAtType() {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // 消费 '@'
    
    // 读取@后面的标识符
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    CHTLToken token;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
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

CHTLToken CHTLLexer::ScanUnquotedLiteral() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取无引号字面量
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch != ';' && ch != '{' && ch != '}' && ch != '\n' && ch != '\r') {
            value += GetNextChar();
        } else {
            break;
        }
    }
    
    // 去掉尾部空白
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }
    
    CHTLToken token;
    token.Type = CHTLTokenType::UNQUOTED_LITERAL;
    token.Value = value;
    token.Line = m_CurrentLine;
    token.Column = m_CurrentColumn - value.length();
    token.Position = start;
    token.Length = value.length();
    
    return token;
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
    token.Position = m_CurrentPosition;
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

// 占位实现
std::string CHTLLexer::ResolveVariableReference(const std::string& varRef) {
    return varRef;
}

void CHTLLexer::HandleConfigurationItem(const std::string& configKey, const std::string& configValue) {
    // 占位
}

CHTLGlobalMap* CHTLLexer::GetGlobalMap() {
    return m_GlobalMap.get();
}

} // namespace CHTL