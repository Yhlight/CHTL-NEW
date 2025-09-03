#include "CHTLLexer.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_set>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& sourceCode)
    : m_SourceCode(sourceCode), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLTokenManager>();
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_ContextManager = std::make_unique<CHTLContextManager>();
    InitializeKeywords();
}

CHTLLexer::CHTLLexer()
    : m_SourceCode(""), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLTokenManager>();
    m_GlobalMap = std::make_unique<CHTLGlobalMap>();
    m_StateMachine = std::make_unique<CHTLStateMachine>();
    m_ContextManager = std::make_unique<CHTLContextManager>();
    InitializeKeywords();
}

void CHTLLexer::InitializeKeywords() {
    // CHTL核心关键字
    m_Keywords["text"] = CHTLTokenType::TEXT;
    m_Keywords["style"] = CHTLTokenType::STYLE;
    m_Keywords["script"] = CHTLTokenType::SCRIPT;
    m_Keywords["inherit"] = CHTLTokenType::INHERIT;
    m_Keywords["delete"] = CHTLTokenType::DELETE;
    m_Keywords["insert"] = CHTLTokenType::INSERT;
    m_Keywords["after"] = CHTLTokenType::AFTER;
    m_Keywords["before"] = CHTLTokenType::BEFORE;
    m_Keywords["replace"] = CHTLTokenType::REPLACE;
    m_Keywords["except"] = CHTLTokenType::EXCEPT;
    m_Keywords["from"] = CHTLTokenType::FROM;
    m_Keywords["as"] = CHTLTokenType::AS;
    m_Keywords["use"] = CHTLTokenType::USE;
    m_Keywords["html5"] = CHTLTokenType::HTML5;
    
    // @类型关键字
    m_AtTypes["@Style"] = CHTLTokenType::AT_STYLE;
    m_AtTypes["@Element"] = CHTLTokenType::AT_ELEMENT;
    m_AtTypes["@Var"] = CHTLTokenType::AT_VAR;
    m_AtTypes["@Html"] = CHTLTokenType::AT_HTML;
    m_AtTypes["@JavaScript"] = CHTLTokenType::AT_JAVASCRIPT;
    m_AtTypes["@Chtl"] = CHTLTokenType::AT_CHTL;
    m_AtTypes["@CJmod"] = CHTLTokenType::AT_CJMOD;
    m_AtTypes["@Config"] = CHTLTokenType::AT_CONFIG;
    
    // 块结构关键字
    m_BlockKeywords["[Template]"] = CHTLTokenType::TEMPLATE_BLOCK;
    m_BlockKeywords["[Custom]"] = CHTLTokenType::CUSTOM_BLOCK;
    m_BlockKeywords["[Origin]"] = CHTLTokenType::ORIGIN_BLOCK;
    m_BlockKeywords["[Configuration]"] = CHTLTokenType::CONFIGURATION_BLOCK;
    m_BlockKeywords["[Import]"] = CHTLTokenType::IMPORT_BLOCK;
    m_BlockKeywords["[Namespace]"] = CHTLTokenType::NAMESPACE_BLOCK;
    m_BlockKeywords["[Info]"] = CHTLTokenType::INFO_BLOCK;
    m_BlockKeywords["[Export]"] = CHTLTokenType::EXPORT_BLOCK;
    m_BlockKeywords["[Name]"] = CHTLTokenType::NAME_BLOCK;
    m_BlockKeywords["[OriginType]"] = CHTLTokenType::ORIGIN_TYPE_BLOCK;
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
    
    try {
        // 根据字符类型进行扫描
        if (ch == '[') {
            return ScanBlockStructureToken();
        }
        else if (ch == '{') {
            return ScanSingleCharToken(CHTLTokenType::LEFT_BRACE, '{');
        }
        else if (ch == '}') {
            return ScanSingleCharToken(CHTLTokenType::RIGHT_BRACE, '}');
        }
        else if (ch == '(') {
            return ScanSingleCharToken(CHTLTokenType::LEFT_PAREN, '(');
        }
        else if (ch == ')') {
            return ScanSingleCharToken(CHTLTokenType::RIGHT_PAREN, ')');
        }
        else if (ch == ';') {
            return ScanSingleCharToken(CHTLTokenType::SEMICOLON, ';');
        }
        else if (ch == ':') {
            return ScanColonOrPseudo();
        }
        else if (ch == '=') {
            return ScanSingleCharToken(CHTLTokenType::EQUAL, '=');
        }
        else if (ch == ',') {
            return ScanSingleCharToken(CHTLTokenType::COMMA, ',');
        }
        else if (ch == '.') {
            return ScanDotOrClassSelector();
        }
        else if (ch == '#') {
            return ScanIdSelector();
        }
        else if (ch == '&') {
            return ScanContextReference();
        }
        else if (ch == '"') {
            return ScanStringLiteralToken('"', CHTLTokenType::STRING_LITERAL);
        }
        else if (ch == '\'') {
            return ScanStringLiteralToken('\'', CHTLTokenType::SINGLE_QUOTE_STRING);
        }
        else if (ch == '/' && PeekChar(2) == '/') {
            return ScanLineCommentToken();
        }
        else if (ch == '/' && PeekChar(2) == '*') {
            return ScanBlockCommentToken();
        }
        else if (ch == '-' && PeekChar(2) == '-') {
            return ScanGeneratorCommentToken();
        }
        else if (ch == '@') {
            return ScanAtTypeToken();
        }
        else if (std::isalpha(ch) || ch == '_' || (static_cast<unsigned char>(ch) > 127)) {
            return ScanIdentifierOrKeywordToken();
        }
        else if (std::isdigit(ch)) {
            return ScanNumberToken();
        }
        else if (ch == '\n') {
            return ScanNewlineToken();
        }
        else {
            // 跳过未知字符但不报错
            GetNextChar();
            return true;
        }
    }
    catch (const std::exception& e) {
        SetError("Token scanning error: " + std::string(e.what()));
        return false;
    }
}

bool CHTLLexer::ScanSingleCharToken(CHTLTokenType type, char ch) {
    CHTLToken token = CreateToken(type, std::string(1, ch), 1);
    GetNextChar();
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanBlockStructureToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取完整的 [...]
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar(); // 消费 ']'
    }
    
    // 查找匹配的块关键字
    CHTLTokenType type = CHTLTokenType::LEFT_BRACKET;
    for (const auto& pair : m_BlockKeywords) {
        if (value == pair.first) {
            type = pair.second;
            break;
        }
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanIdentifierOrKeywordToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取标识符（支持UTF-8）
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isalnum(ch) || ch == '_' || ch == '-' || 
            (static_cast<unsigned char>(ch) > 127)) { // UTF-8字符
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    if (value.empty()) {
        return false;
    }
    
    // 查找关键字
    CHTLTokenType type = CHTLTokenType::IDENTIFIER;
    auto it = m_Keywords.find(value);
    if (it != m_Keywords.end()) {
        type = it->second;
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanStringLiteralToken(char quote, CHTLTokenType type) {
    size_t start = m_CurrentPosition;
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
    
    CHTLToken token = CreateToken(type, value, value.length() + 2);
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanNumberToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    bool hasDot = false;
    
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isdigit(ch)) {
            value += GetNextChar();
        }
        else if (ch == '.' && !hasDot) {
            hasDot = true;
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::NUMBER, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanLineCommentToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 消费 //
    value += GetNextChar(); // /
    value += GetNextChar(); // /
    
    // 读取到行尾
    while (!IsAtEnd() && PeekChar() != '\n') {
        value += GetNextChar();
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::SINGLE_LINE_COMMENT, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanBlockCommentToken() {
    size_t start = m_CurrentPosition;
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
    
    CHTLToken token = CreateToken(CHTLTokenType::MULTI_LINE_COMMENT, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanGeneratorCommentToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 消费 --
    value += GetNextChar(); // -
    value += GetNextChar(); // -
    
    // 读取到行尾
    while (!IsAtEnd() && PeekChar() != '\n') {
        value += GetNextChar();
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::GENERATOR_COMMENT, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanAtTypeToken() {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // 消费 '@'
    
    // 读取@后面的标识符
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    // 查找@类型
    CHTLTokenType type = CHTLTokenType::CUSTOM_AT_TYPE;
    auto it = m_AtTypes.find(value);
    if (it != m_AtTypes.end()) {
        type = it->second;
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanDotOrClassSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // 消费 '.'
    
    // 检查是否是类选择器
    if (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '_')) {
        // 读取类名
        while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
            value += GetNextChar();
        }
        
        CHTLToken token = CreateToken(CHTLTokenType::CLASS_SELECTOR, value, value.length());
        token.Position = start;
        m_Tokens.push_back(token);
    } else {
        // 单独的点
        CHTLToken token = CreateToken(CHTLTokenType::DOT, ".", 1);
        token.Position = start;
        m_Tokens.push_back(token);
    }
    
    return true;
}

bool CHTLLexer::ScanIdSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // 消费 '#'
    
    // 读取ID名
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::ID_SELECTOR, value, value.length());
    token.Position = start;
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanContextReference() {
    size_t start = m_CurrentPosition;
    
    CHTLToken token = CreateToken(CHTLTokenType::CONTEXT_REFERENCE, "&", 1);
    token.Position = start;
    GetNextChar(); // 消费 '&'
    m_Tokens.push_back(token);
    return true;
}

bool CHTLLexer::ScanColonOrPseudo() {
    size_t start = m_CurrentPosition;
    
    // 检查是否是伪类/伪元素
    if (PeekChar(2) == ':') {
        // 伪元素 ::
        std::string value = "::";
        GetNextChar(); // :
        GetNextChar(); // :
        
        // 读取伪元素名
        while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
            value += GetNextChar();
        }
        
        CHTLToken token = CreateToken(CHTLTokenType::PSEUDO_ELEMENT, value, value.length());
        token.Position = start;
        m_Tokens.push_back(token);
    }
    else if (std::isalpha(PeekChar(2))) {
        // 可能是伪类
        std::string value = ":";
        GetNextChar(); // :
        
        // 读取伪类名
        while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
            value += GetNextChar();
        }
        
        CHTLToken token = CreateToken(CHTLTokenType::PSEUDO_CLASS, value, value.length());
        token.Position = start;
        m_Tokens.push_back(token);
    }
    else {
        // 普通冒号
        CHTLToken token = CreateToken(CHTLTokenType::COLON, ":", 1);
        token.Position = start;
        GetNextChar();
        m_Tokens.push_back(token);
    }
    
    return true;
}

bool CHTLLexer::ScanNewlineToken() {
    CHTLToken token = CreateToken(CHTLTokenType::NEWLINE, "\n", 1);
    token.Position = m_CurrentPosition;
    GetNextChar();
    m_Tokens.push_back(token);
    return true;
}

// 实现头文件中声明的所有方法
CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isalnum(ch) || ch == '_' || ch == '-' || 
            (static_cast<unsigned char>(ch) > 127)) {
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    CHTLTokenType type = CHTLTokenType::IDENTIFIER;
    auto it = m_Keywords.find(value);
    if (it != m_Keywords.end()) {
        type = it->second;
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
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
    
    CHTLTokenType type = (quote == '"') ? CHTLTokenType::STRING_LITERAL : CHTLTokenType::SINGLE_QUOTE_STRING;
    CHTLToken token = CreateToken(type, value, value.length() + 2);
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanUnquotedLiteral() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取直到分隔符
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch != ';' && ch != '{' && ch != '}' && ch != '\n' && ch != '\r' && 
            ch != ':' && ch != '=' && ch != ',') {
            value += GetNextChar();
        } else {
            break;
        }
    }
    
    // 去掉尾部空白
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::UNQUOTED_LITERAL, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanNumber() {
    size_t start = m_CurrentPosition;
    std::string value;
    bool hasDot = false;
    
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isdigit(ch)) {
            value += GetNextChar();
        }
        else if (ch == '.' && !hasDot) {
            hasDot = true;
            value += GetNextChar();
        }
        else {
            break;
        }
    }
    
    CHTLToken token = CreateToken(CHTLTokenType::NUMBER, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanComment() {
    size_t start = m_CurrentPosition;
    std::string value;
    CHTLTokenType type;
    
    if (PeekChar() == '/' && PeekChar(2) == '/') {
        type = CHTLTokenType::SINGLE_LINE_COMMENT;
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    else if (PeekChar() == '/' && PeekChar(2) == '*') {
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
        type = CHTLTokenType::GENERATOR_COMMENT;
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
    }
    else {
        return CreateToken(CHTLTokenType::UNKNOWN, "", 0);
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanBlockStructure() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar();
    }
    
    CHTLTokenType type = CHTLTokenType::LEFT_BRACKET;
    auto it = m_BlockKeywords.find(value);
    if (it != m_BlockKeywords.end()) {
        type = it->second;
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    char first = GetNextChar();
    value += first;
    
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    CHTLTokenType type = (first == '.') ? CHTLTokenType::CLASS_SELECTOR : CHTLTokenType::ID_SELECTOR;
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanAtType() {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // @
    
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    CHTLTokenType type = CHTLTokenType::CUSTOM_AT_TYPE;
    auto it = m_AtTypes.find(value);
    if (it != m_AtTypes.end()) {
        type = it->second;
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Position = start;
    return token;
}

CHTLToken CHTLLexer::ScanContextReference() {
    CHTLToken token = CreateToken(CHTLTokenType::CONTEXT_REFERENCE, "&", 1);
    token.Position = m_CurrentPosition;
    GetNextChar();
    return token;
}

// 辅助方法实现
void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            GetNextChar();
        } else {
            break;
        }
    }
}

char CHTLLexer::PeekChar(size_t offset) {
    size_t pos = m_CurrentPosition + offset - 1;
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
    token.Column = m_CurrentColumn - length;
    token.Position = m_CurrentPosition - length;
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

CHTLGlobalMap* CHTLLexer::GetGlobalMap() {
    return m_GlobalMap.get();
}

std::string CHTLLexer::ResolveVariableReference(const std::string& varRef) {
    if (m_GlobalMap) {
        return m_GlobalMap->ResolveVariableReference(varRef);
    }
    return varRef;
}

void CHTLLexer::HandleConfigurationItem(const std::string& configKey, const std::string& configValue) {
    if (m_GlobalMap) {
        // 处理配置项，更新全局映射表
        // 这里应该调用GlobalMap的方法来存储配置
    }
}

} // namespace CHTL