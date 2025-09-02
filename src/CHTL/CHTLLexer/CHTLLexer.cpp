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

bool CHTLLexer::Tokenize() {
    Reset();
    
    while (!IsAtEnd()) {
        if (!ScanNextToken()) {
            return false;
        }
    }
    
    // 添加EOF令牌
    m_Tokens.push_back(CreateToken(CHTLTokenType::EOF_TOKEN, "", 0));
    
    return !m_HasError;
}

bool CHTLLexer::ScanNextToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return true;
    }
    
    char ch = PeekChar();
    size_t tokenStart = m_CurrentPosition;
    
    CHTLToken token;
    token.Line = 1; // 简化实现
    token.Column = 1;
    token.Position = tokenStart;
    
    // 根据字符类型扫描不同的令牌
    if (ch == '[') {
        token = ScanBlockStructure();
    }
    else if (ch == '@') {
        token = ScanAtType();
    }
    else if (ch == '.') {
        token = ScanSelector();
    }
    else if (ch == '#') {
        token = ScanSelector();
    }
    else if (ch == '&') {
        token = ScanContextReference();
    }
    else if (ch == '"') {
        token = ScanStringLiteral('"');
    }
    else if (ch == '\'') {
        token = ScanStringLiteral('\'');
    }
    else if (std::isdigit(ch)) {
        token = ScanNumber();
    }
    else if (ch == '/' && (PeekChar(2) == '/' || PeekChar(2) == '*')) {
        token = ScanComment();
    }
    else if (ch == '-' && PeekChar(2) == '-') {
        token = ScanComment();
    }
    else if (std::isalpha(ch) || ch == '_') {
        token = ScanIdentifierOrKeyword();
    }
    else if (ch == ':' || ch == '=') {
        token = HandleCEEquivalence(ch);
    }
    else {
        token = ScanOperator();
    }
    
    // 验证令牌的UTF-8编码
    if (!ValidateUTF8(token.Value)) {
        SetError("Invalid UTF-8 encoding in token: " + token.Value);
        return false;
    }
    
    // 检查当前上下文是否允许此语法
    if (!IsAllowedInCurrentContext(token.GetTypeName())) {
        SetError("Syntax not allowed in current context: " + token.Value);
        return false;
    }
    
    // 根据令牌类型更新状态机和上下文
    if (!UpdateStateAndContext(token)) {
        return false;
    }
    
    m_Tokens.push_back(token);
    return true;
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
    
    // 检查复合关键字（如"at top", "at bottom"）
    if (value == "at") {
        SkipWhitespace();
        if (!IsAtEnd()) {
            std::string nextWord;
            size_t tempPos = m_CurrentPosition;
            
            while (!IsAtEnd() && std::isalpha(PeekChar())) {
                nextWord += GetNextChar();
            }
            
            if (nextWord == "top" || nextWord == "bottom") {
                value += " " + nextWord;
            }
            else {
                // 回退位置
                m_CurrentPosition = tempPos;
            }
        }
    }
    
    // 检查CHTL特殊关键字
    CHTLTokenType type = CHTLTokenType::IDENTIFIER;
    
    // 优先检查CHTL关键字
    if (value == "text") type = CHTLTokenType::TEXT;
    else if (value == "style") type = CHTLTokenType::STYLE;
    else if (value == "script") type = CHTLTokenType::SCRIPT;
    else if (value == "html") type = CHTLTokenType::IDENTIFIER; // HTML元素作为标识符
    else if (value == "inherit") type = CHTLTokenType::INHERIT;
    else if (value == "delete") type = CHTLTokenType::DELETE;
    else if (value == "insert") type = CHTLTokenType::INSERT;
    else if (value == "except") type = CHTLTokenType::EXCEPT;
    else if (value == "use") type = CHTLTokenType::USE;
    else if (value == "from") type = CHTLTokenType::FROM;
    else if (value == "as") type = CHTLTokenType::AS;
    else if (value == "after") type = CHTLTokenType::AFTER;
    else if (value == "before") type = CHTLTokenType::BEFORE;
    else if (value == "replace") type = CHTLTokenType::REPLACE;
    else if (value == "at top") type = CHTLTokenType::AT_TOP;
    else if (value == "at bottom") type = CHTLTokenType::AT_BOTTOM;
    else if (value == "html5") type = CHTLTokenType::HTML5;
    else {
        // 使用TokenManager进行更详细的类型判断
        type = m_TokenManager->GetTokenType(value);
    }
    
    CHTLToken token = CreateToken(type, value, value.length());
    token.Line = 1; // 简化实现
    token.Column = 1;
    token.Position = start;
    
    return token;
}

CHTLToken CHTLLexer::ScanStringLiteral(char quote) {
    size_t start = m_CurrentPosition;
    std::string value;
    value += GetNextChar(); // 添加开始引号
    
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        value += ch;
        
        if (ch == quote) {
            break; // 找到结束引号
        }
        else if (ch == '\\') {
            // 处理转义字符
            if (!IsAtEnd()) {
                value += GetNextChar();
            }
        }
    }
    
    CHTLTokenType type = (quote == '"') ? CHTLTokenType::STRING_LITERAL : CHTLTokenType::SINGLE_QUOTE_STRING;
    return CreateToken(type, value, value.length());
}

CHTLToken CHTLLexer::ScanUnquotedLiteral() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取无修饰字面量（CSS样式的无引号字符串）
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isspace(ch) || ch == ';' || ch == '{' || ch == '}' || ch == ',') {
            break;
        }
        value += GetNextChar();
    }
    
    return CreateToken(CHTLTokenType::UNQUOTED_LITERAL, value, value.length());
}

CHTLToken CHTLLexer::ScanNumber() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取整数部分
    while (!IsAtEnd() && std::isdigit(PeekChar())) {
        value += GetNextChar();
    }
    
    // 检查小数点
    if (!IsAtEnd() && PeekChar() == '.') {
        value += GetNextChar(); // 添加小数点
        
        // 读取小数部分
        while (!IsAtEnd() && std::isdigit(PeekChar())) {
            value += GetNextChar();
        }
    }
    
    return CreateToken(CHTLTokenType::NUMBER, value, value.length());
}

CHTLToken CHTLLexer::ScanComment() {
    size_t start = m_CurrentPosition;
    std::string value;
    CHTLTokenType type;
    
    char first = GetNextChar();
    char second = PeekChar();
    
    if (first == '/' && second == '/') {
        // 单行注释
        value += first;
        value += GetNextChar(); // 第二个/
        
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
        
        type = CHTLTokenType::SINGLE_LINE_COMMENT;
    }
    else if (first == '/' && second == '*') {
        // 多行注释
        value += first;
        value += GetNextChar(); // *
        
        while (!IsAtEnd()) {
            char ch = GetNextChar();
            value += ch;
            
            if (ch == '*' && !IsAtEnd() && PeekChar() == '/') {
                value += GetNextChar(); // 添加结束/
                break;
            }
        }
        
        type = CHTLTokenType::MULTI_LINE_COMMENT;
    }
    else if (first == '-' && second == '-') {
        // 生成器注释
        value += first;
        value += GetNextChar(); // 第二个-
        
        while (!IsAtEnd() && PeekChar() != '\n') {
            value += GetNextChar();
        }
        
        type = CHTLTokenType::GENERATOR_COMMENT;
    }
    else {
        SetError("Invalid comment syntax");
        return CreateToken(CHTLTokenType::UNKNOWN, "", 0);
    }
    
    return CreateToken(type, value, value.length());
}

CHTLToken CHTLLexer::ScanBlockStructure() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    value += GetNextChar(); // [
    
    // 读取到]
    while (!IsAtEnd() && PeekChar() != ']') {
        value += GetNextChar();
    }
    
    if (!IsAtEnd()) {
        value += GetNextChar(); // ]
    }
    
    CHTLTokenType type = m_TokenManager->GetTokenType(value);
    return CreateToken(type, value, value.length());
}

CHTLToken CHTLLexer::ScanAtType() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    value += GetNextChar(); // @
    
    // 读取类型名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    CHTLTokenType type = m_TokenManager->GetTokenType(value);
    
    // 如果不是已知的@类型，检查是否为自定义类型
    if (type == CHTLTokenType::UNKNOWN) {
        m_TokenManager->RegisterCustomAtType(value);
        type = CHTLTokenType::CUSTOM_AT_TYPE;
    }
    
    return CreateToken(type, value, value.length());
}

CHTLToken CHTLLexer::ScanSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    char selectorChar = GetNextChar(); // . 或 #
    value += selectorChar;
    
    // 读取选择器名称
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        value += GetNextChar();
    }
    
    // 检查是否有伪类或伪元素
    if (!IsAtEnd() && PeekChar() == ':') {
        value += GetNextChar(); // :
        
        // 检查是否为伪元素（::）
        if (!IsAtEnd() && PeekChar() == ':') {
            value += GetNextChar(); // 第二个:
            
            // 读取伪元素名称
            while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
                value += GetNextChar();
            }
            
            return CreateToken(CHTLTokenType::PSEUDO_ELEMENT, value, value.length());
        }
        else {
            // 读取伪类名称
            while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
                value += GetNextChar();
            }
            
            return CreateToken(CHTLTokenType::PSEUDO_CLASS, value, value.length());
        }
    }
    
    // 处理自动化选择器生成
    HandleAutoSelectorGeneration(value);
    
    CHTLTokenType type = (selectorChar == '.') ? CHTLTokenType::CLASS_SELECTOR : CHTLTokenType::ID_SELECTOR;
    return CreateToken(type, value, value.length());
}

CHTLToken CHTLLexer::ScanContextReference() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    value += GetNextChar(); // &
    
    // 检查是否有伪类或伪元素
    if (!IsAtEnd() && PeekChar() == ':') {
        value += GetNextChar(); // :
        
        if (!IsAtEnd() && PeekChar() == ':') {
            value += GetNextChar(); // 第二个:
            
            // 读取伪元素名称
            while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
                value += GetNextChar();
            }
            
            return CreateToken(CHTLTokenType::PSEUDO_ELEMENT, value, value.length());
        }
        else {
            // 读取伪类名称
            while (!IsAtEnd() && (std::isalpha(PeekChar()) || PeekChar() == '-')) {
                value += GetNextChar();
            }
            
            return CreateToken(CHTLTokenType::PSEUDO_CLASS, value, value.length());
        }
    }
    
    return CreateToken(CHTLTokenType::CONTEXT_REFERENCE, value, value.length());
}

CHTLToken CHTLLexer::ScanIndexAccess() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    value += GetNextChar(); // [
    
    // 读取索引数字
    while (!IsAtEnd() && std::isdigit(PeekChar())) {
        value += GetNextChar();
    }
    
    if (!IsAtEnd() && PeekChar() == ']') {
        value += GetNextChar(); // ]
    }
    
    return CreateToken(CHTLTokenType::INDEX_ACCESS, value, value.length());
}

CHTLToken CHTLLexer::ScanOperator() {
    char ch = GetNextChar();
    std::string value(1, ch);
    
    switch (ch) {
        case '{': return CreateToken(CHTLTokenType::LEFT_BRACE, value, 1);
        case '}': return CreateToken(CHTLTokenType::RIGHT_BRACE, value, 1);
        case '[': 
            // 检查是否为索引访问
            if (!IsAtEnd() && std::isdigit(PeekChar())) {
                m_CurrentPosition--; // 回退
                return ScanIndexAccess();
            }
            return CreateToken(CHTLTokenType::LEFT_BRACKET, value, 1);
        case ']': return CreateToken(CHTLTokenType::RIGHT_BRACKET, value, 1);
        case '(': return CreateToken(CHTLTokenType::LEFT_PAREN, value, 1);
        case ')': return CreateToken(CHTLTokenType::RIGHT_PAREN, value, 1);
        case ';': return CreateToken(CHTLTokenType::SEMICOLON, value, 1);
        case ',': return CreateToken(CHTLTokenType::COMMA, value, 1);
        case '.': return CreateToken(CHTLTokenType::DOT, value, 1);
        default:
            return CreateToken(CHTLTokenType::UNKNOWN, value, 1);
    }
}

CHTLToken CHTLLexer::HandleCEEquivalence(char ch) {
    GetNextChar(); // 消费字符
    
    // CE对等式：':'与'='完全等价
    if (ch == ':') {
        return CreateToken(CHTLTokenType::COLON, ":", 1);
    }
    else if (ch == '=') {
        return CreateToken(CHTLTokenType::EQUAL, "=", 1);
    }
    
    return CreateToken(CHTLTokenType::UNKNOWN, std::string(1, ch), 1);
}

void CHTLLexer::HandleAutoSelectorGeneration(const std::string& selector) {
    if (selector.empty() || (selector[0] != '.' && selector[0] != '#')) {
        return;
    }
    
    std::string selectorName = selector.substr(1); // 去掉. 或 #
    
    if (selector[0] == '.') {
        // 自动添加类名
        m_ContextManager->AddAutoClass(selectorName);
        m_ContextManager->SetProperty("class", selectorName);
    }
    else if (selector[0] == '#') {
        // 自动添加ID名
        m_ContextManager->AddAutoId(selectorName);
        m_ContextManager->SetProperty("id", selectorName);
    }
}

bool CHTLLexer::UpdateStateAndContext(const CHTLToken& token) {
    // 根据令牌类型更新状态机和上下文
    switch (token.Type) {
        case CHTLTokenType::TEMPLATE_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_TEMPLATE_BLOCK, token.Value);
            
        case CHTLTokenType::CUSTOM_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_CUSTOM_BLOCK, token.Value);
            
        case CHTLTokenType::ORIGIN_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_ORIGIN_BLOCK, token.Value);
            
        case CHTLTokenType::CONFIGURATION_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_CONFIGURATION, token.Value);
            
        case CHTLTokenType::IMPORT_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_IMPORT_BLOCK, token.Value);
            
        case CHTLTokenType::NAMESPACE_BLOCK:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_NAMESPACE_BLOCK, token.Value);
            
        case CHTLTokenType::STYLE:
            m_ContextManager->EnterContext(CHTLContextType::STYLE_CONTEXT, 
                                         CHTLScopeType::STYLE_SCOPE, "style");
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_STYLE_BLOCK, token.Value);
            
        case CHTLTokenType::SCRIPT:
            // 局部script属于CHTL，支持CHTL语法元素
            m_ContextManager->EnterContext(CHTLContextType::SCRIPT_CONTEXT, 
                                         CHTLScopeType::SCRIPT_SCOPE, "script");
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_SCRIPT_BLOCK, token.Value);
            
        case CHTLTokenType::TEXT:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_TEXT, token.Value);
            
        case CHTLTokenType::CLASS_SELECTOR:
        case CHTLTokenType::ID_SELECTOR:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_CLASS_SELECTOR, token.Value);
            
        case CHTLTokenType::CONTEXT_REFERENCE:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_CONTEXT_REFERENCE, token.Value);
            
        case CHTLTokenType::INHERIT:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_INHERIT, token.Value);
            
        case CHTLTokenType::DELETE:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_DELETE, token.Value);
            
        case CHTLTokenType::INSERT:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_INSERT, token.Value);
            
        case CHTLTokenType::EXCEPT:
            return m_StateMachine->TransitionTo(CHTLParseState::PARSING_EXCEPT, token.Value);
            
        case CHTLTokenType::LEFT_BRACE:
            m_StateMachine->EnterBrace();
            break;
            
        case CHTLTokenType::RIGHT_BRACE:
            m_StateMachine->ExitBrace();
            // 检查是否需要退出上下文
            if (m_StateMachine->GetCurrentContext().BraceLevel == 0) {
                m_ContextManager->ExitContext();
            }
            break;
            
        default:
            break;
    }
    
    return true;
}

bool CHTLLexer::IsAllowedInCurrentContext(const std::string& syntaxType) {
    CHTLContextType currentContext = m_ContextManager->GetCurrentContextType();
    
    // 注释和原始嵌入在任意位置都允许使用
    if (syntaxType == "SINGLE_LINE_COMMENT" || 
        syntaxType == "MULTI_LINE_COMMENT" || 
        syntaxType == "GENERATOR_COMMENT" ||
        syntaxType == "ORIGIN_BLOCK") {
        return true;
    }
    
    switch (currentContext) {
        case CHTLContextType::GLOBAL_CONTEXT:
            // 全局上下文允许所有CHTL语法
            return true;
            
        case CHTLContextType::STYLE_CONTEXT:
            // 局部样式块允许：模板变量、自定义变量、自定义变量特例化、模板样式组、
            // 自定义样式组、无值样式组、自定义样式组特例化、delete属性、delete继承、
            // 样式组间继承、生成器注释、全缀名、任意类型原始嵌入、命名空间from
            return syntaxType == "AT_VAR" || syntaxType == "AT_STYLE" || 
                   syntaxType == "INHERIT" || syntaxType == "DELETE" ||
                   syntaxType == "CLASS_SELECTOR" || syntaxType == "ID_SELECTOR" ||
                   syntaxType == "CONTEXT_REFERENCE" || syntaxType == "PSEUDO_CLASS" ||
                   syntaxType == "PSEUDO_ELEMENT" || syntaxType == "FROM";
                   
        case CHTLContextType::SCRIPT_CONTEXT:
            // 局部script允许：模板变量、自定义变量组、变量组特例化、命名空间from、
            // 注释、任意类型原始嵌入。注意：{{&}}等特供语法属于CHTL本身功能
            return syntaxType == "AT_VAR" || syntaxType == "FROM" ||
                   syntaxType == "CONTEXT_REFERENCE"; // {{&}}属于CHTL功能
                   
        case CHTLContextType::NAMESPACE_CONTEXT:
            // 命名空间内根据约束限制
            return m_ContextManager->CheckConstraint(syntaxType);
            
        default:
            return true;
    }
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isspace(ch)) {
            if (ch == '\n') {
                m_Tokens.push_back(CreateToken(CHTLTokenType::NEWLINE, "\n", 1));
            }
            GetNextChar();
        }
        else {
            break;
        }
    }
}

char CHTLLexer::GetNextChar() {
    if (IsAtEnd()) {
        return '\0';
    }
    
    char ch = m_SourceCode[m_CurrentPosition++];
    UpdateLineColumn(ch);
    return ch;
}

char CHTLLexer::PeekChar(size_t offset) {
    size_t peekPos = m_CurrentPosition + offset - 1;
    if (peekPos >= m_SourceCode.length()) {
        return '\0';
    }
    return m_SourceCode[peekPos];
}

bool CHTLLexer::IsAtEnd() const {
    return m_CurrentPosition >= m_SourceCode.length();
}

void CHTLLexer::UpdateLineColumn(char ch) {
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    }
    else {
        m_CurrentColumn++;
    }
}

CHTLToken CHTLLexer::CreateToken(CHTLTokenType type, const std::string& value, size_t length) {
    return CHTLToken(type, value, m_CurrentLine, m_CurrentColumn - length, 
                    m_CurrentPosition - length, length);
}

void CHTLLexer::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = "Line " + std::to_string(m_CurrentLine) + 
                    ", Column " + std::to_string(m_CurrentColumn) + ": " + message;
}

bool CHTLLexer::ValidateUTF8(const std::string& str) {
    try {
        std::wstring wstr = m_UTF8Converter.from_bytes(str);
        return true;
    }
    catch (const std::range_error&) {
        return false;
    }
}

const std::vector<CHTLToken>& CHTLLexer::GetTokens() const {
    return m_Tokens;
}

CHTLGlobalMap* CHTLLexer::GetGlobalMap() {
    return m_GlobalMap.get();
}

CHTLStateMachine* CHTLLexer::GetStateMachine() {
    return m_StateMachine.get();
}

CHTLContextManager* CHTLLexer::GetContextManager() {
    return m_ContextManager.get();
}

void CHTLLexer::Reset() {
    m_CurrentPosition = 0;
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_Tokens.clear();
    m_HasError = false;
    m_ErrorMessage.clear();
    
    if (m_StateMachine) {
        m_StateMachine->Reset();
    }
    
    if (m_ContextManager) {
        m_ContextManager->Reset();
    }
    
    if (m_GlobalMap) {
        m_GlobalMap->Clear();
    }
}

void CHTLLexer::SetSourceCode(const std::string& sourceCode) {
    m_SourceCode = sourceCode;
    Reset();
}

bool CHTLLexer::HasError() const {
    return m_HasError;
}

std::string CHTLLexer::GetErrorMessage() const {
    return m_ErrorMessage;
}

// 辅助方法实现
std::string CHTLLexer::ResolveVariableReference(const std::string& varRef) {
    return m_GlobalMap->ResolveVariableReference(varRef);
}

void CHTLLexer::HandleConfigurationItem(const std::string& configKey, const std::string& configValue) {
    // 处理配置项，更新全局映射表
    auto config = m_GlobalMap->GetActiveConfiguration();
    if (config) {
        config->Settings[configKey] = configValue;
    }
}

void CHTLLexer::HandleNameConfiguration(const std::string& nameKey, const std::vector<std::string>& nameValues) {
    // 处理Name配置块
    auto config = m_GlobalMap->GetActiveConfiguration();
    if (config) {
        config->NameMappings[nameKey] = nameValues;
    }
}

void CHTLLexer::HandleOriginTypeConfiguration(const std::string& typeKey, const std::string& typeName) {
    // 处理OriginType配置块
    auto config = m_GlobalMap->GetActiveConfiguration();
    if (config) {
        config->OriginTypes[typeKey] = typeName;
        
        // 自动注册自定义@类型
        if (typeName.front() == '@') {
            m_TokenManager->RegisterCustomAtType(typeName);
        }
    }
}



} // namespace CHTL