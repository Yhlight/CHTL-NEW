#include "CHTLJSLexer.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& sourceCode)
    : m_SourceCode(sourceCode), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLJSTokenManager>();
    m_GlobalMap = std::make_unique<CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<CHTLJSStateMachine>();
    m_ContextManager = std::make_unique<CHTLJSContextManager>();
}

CHTLJSLexer::CHTLJSLexer()
    : m_SourceCode(""), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    
    m_TokenManager = std::make_unique<CHTLJSTokenManager>();
    m_GlobalMap = std::make_unique<CHTLJSGlobalMap>();
    m_StateMachine = std::make_unique<CHTLJSStateMachine>();
    m_ContextManager = std::make_unique<CHTLJSContextManager>();
}

bool CHTLJSLexer::Tokenize() {
    Reset();
    
    while (!IsAtEnd()) {
        if (!ScanNextCHTLJSToken()) {
            return false;
        }
    }
    
    // 添加EOF令牌
    m_Tokens.push_back(CreateToken(CHTLJSTokenType::EOF_TOKEN, "", 0));
    
    return !m_HasError;
}

bool CHTLJSLexer::ScanNextCHTLJSToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return true;
    }
    
    char ch = PeekChar();
    size_t tokenStart = m_CurrentPosition;
    
    CHTLJSToken token;
    token.Line = 1; // 简化实现
    token.Column = 1;
    token.Position = tokenStart;
    
    // 根据字符类型扫描不同的CHTL JS令牌
    if (ch == '{' && PeekChar(2) == '{') {
        token = ScanEnhancedSelector();
    }
    else if (ch == '-' && PeekChar(2) == '>') {
        token = ScanArrowOperator();
    }
    else if (ch == '&' && PeekChar(2) == '-' && PeekChar(3) == '>') {
        token = ScanEventBindOperator();
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
    else if (std::isalpha(ch) || ch == '_') {
        token = ScanCHTLJSIdentifierOrKeyword();
    }
    else {
        token = ScanOperator();
    }
    
    // 验证令牌的UTF-8编码
    if (!ValidateUTF8(token.Value)) {
        SetError("Invalid UTF-8 encoding in CHTL JS token: " + token.Value);
        return false;
    }
    
    // 检查当前上下文是否允许此CHTL JS语法
    if (!IsAllowedCHTLJSSyntaxInCurrentContext(token.GetTypeName())) {
        SetError("CHTL JS syntax not allowed in current context: " + token.Value);
        return false;
    }
    
    // 根据令牌类型更新CHTL JS状态机和上下文
    if (!UpdateCHTLJSStateAndContext(token)) {
        return false;
    }
    
    m_Tokens.push_back(token);
    return true;
}

CHTLJSToken CHTLJSLexer::ScanCHTLJSIdentifierOrKeyword() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取标识符
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        value += GetNextChar();
    }
    
    CHTLJSTokenType type = m_TokenManager->GetTokenType(value);
    return CreateToken(type, value, value.length());
}

CHTLJSToken CHTLJSLexer::ScanEnhancedSelector() {
    size_t start = m_CurrentPosition;
    std::string value;
    
    // 读取{{
    value += GetNextChar(); // {
    value += GetNextChar(); // {
    
    // 读取选择器内容
    std::string selectorContent;
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (ch == '}' && PeekChar(2) == '}') {
            break;
        }
        selectorContent += GetNextChar();
    }
    
    // 读取}}
    if (!IsAtEnd()) {
        value += selectorContent;
        value += GetNextChar(); // }
        value += GetNextChar(); // }
    }
    
    // 处理增强选择器
    std::string domCode = ProcessEnhancedSelector(value);
    m_ContextManager->RegisterSelectorMapping(value, domCode);
    
    return CreateToken(CHTLJSTokenType::SELECTOR_CONTENT, value, value.length());
}

CHTLJSToken CHTLJSLexer::ScanArrowOperator() {
    GetNextChar(); // -
    GetNextChar(); // >
    
    return CreateToken(CHTLJSTokenType::ARROW_OPERATOR, "->", 2);
}

CHTLJSToken CHTLJSLexer::ScanEventBindOperator() {
    GetNextChar(); // &
    GetNextChar(); // -
    GetNextChar(); // >
    
    return CreateToken(CHTLJSTokenType::EVENT_BIND_OPERATOR, "&->", 3);
}

CHTLJSToken CHTLJSLexer::ScanCHTLJSFunctionBlock(const std::string& functionType) {
    size_t start = m_CurrentPosition;
    std::string value = functionType;
    
    // 跳过空白字符
    SkipWhitespace();
    
    // 读取{
    if (!IsAtEnd() && PeekChar() == '{') {
        value += GetNextChar();
        
        // 读取函数内容
        int braceCount = 1;
        std::string content;
        
        while (!IsAtEnd() && braceCount > 0) {
            char ch = GetNextChar();
            content += ch;
            
            if (ch == '{') {
                braceCount++;
            }
            else if (ch == '}') {
                braceCount--;
            }
        }
        
        value += content;
        
        // 处理CHTL JS函数
        std::string jsCode = ProcessCHTLJSFunction(functionType, content);
        
        // 根据函数类型进入相应上下文
        if (functionType == "listen") {
            m_ContextManager->EnterContext(CHTLJSContextType::LISTEN_CONTEXT, 
                                         CHTLJSScopeType::FUNCTION_SCOPE, "listen");
        }
        else if (functionType == "delegate") {
            m_ContextManager->EnterContext(CHTLJSContextType::DELEGATE_CONTEXT, 
                                         CHTLJSScopeType::FUNCTION_SCOPE, "delegate");
        }
        else if (functionType == "animate") {
            m_ContextManager->EnterContext(CHTLJSContextType::ANIMATE_CONTEXT, 
                                         CHTLJSScopeType::FUNCTION_SCOPE, "animate");
        }
        else if (functionType == "module") {
            m_ContextManager->EnterContext(CHTLJSContextType::MODULE_CONTEXT, 
                                         CHTLJSScopeType::MODULE_SCOPE, "module");
        }
    }
    
    CHTLJSTokenType type = m_TokenManager->GetTokenType(functionType);
    return CreateToken(type, value, value.length());
}

CHTLJSToken CHTLJSLexer::ScanVirDeclaration() {
    size_t start = m_CurrentPosition;
    std::string value = "vir";
    
    // 跳过vir关键字
    m_CurrentPosition += 3;
    
    // 跳过空白字符
    SkipWhitespace();
    
    // 读取虚对象名称
    std::string virName;
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
        virName += GetNextChar();
    }
    
    value += " " + virName;
    
    // 跳过空白字符和=
    SkipWhitespace();
    if (!IsAtEnd() && PeekChar() == '=') {
        value += GetNextChar();
    }
    
    // 进入vir上下文
    m_ContextManager->EnterContext(CHTLJSContextType::VIR_CONTEXT, 
                                 CHTLJSScopeType::VIR_SCOPE, virName);
    
    return CreateToken(CHTLJSTokenType::VIR, value, value.length());
}

std::string CHTLJSLexer::ProcessEnhancedSelector(const std::string& selector) {
    return m_ContextManager->ResolveEnhancedSelector(selector);
}

std::string CHTLJSLexer::ProcessVirtualObjectKey(const std::string& virObject, const std::string& key) {
    auto resolution = m_ContextManager->ResolveVirtualObjectKey(virObject, key);
    
    if (resolution.IsResolved) {
        if (resolution.KeyType == "function") {
            return resolution.ResolvedValue + "()";
        }
        else {
            return resolution.ResolvedValue;
        }
    }
    
    return "/* Unresolved vir key: " + virObject + "->" + key + " */";
}

std::string CHTLJSLexer::ProcessCHTLJSFunction(const std::string& functionType, const std::string& content) {
    std::ostringstream jsCode;
    
    if (functionType == "listen") {
        // 解析listen函数：listen { click: handler, ... }
        auto events = ParseEventMappings(content);
        for (const auto& event : events) {
            jsCode << "element.addEventListener('" << event.first << "', " << event.second << ");\n";
        }
    }
    else if (functionType == "delegate") {
        // 解析delegate函数：delegate { target: selector, click: handler }
        auto delegateInfo = ParseDelegateInfo(content);
        jsCode << "/* Delegate: " << delegateInfo << " */\n";
    }
    else if (functionType == "animate") {
        // 解析animate函数：animate { target: selector, duration: 100, ... }
        auto animationInfo = ParseAnimationInfo(content);
        jsCode << "/* Animation: " << animationInfo << " */\n";
    }
    else if (functionType == "module") {
        // 解析module函数：module { load: path1, load: path2 }
        auto modulePaths = ParseModulePaths(content);
        jsCode << m_ContextManager->GenerateModuleLoadCode();
    }
    
    return jsCode.str();
}

bool CHTLJSLexer::UpdateCHTLJSStateAndContext(const CHTLJSToken& token) {
    // 根据CHTL JS令牌类型更新状态机和上下文
    switch (token.Type) {
        case CHTLJSTokenType::MODULE:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_MODULE_BLOCK, token.Value);
            
        case CHTLJSTokenType::LISTEN:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_LISTEN_BLOCK, token.Value);
            
        case CHTLJSTokenType::DELEGATE:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_DELEGATE_BLOCK, token.Value);
            
        case CHTLJSTokenType::ANIMATE:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_ANIMATE_BLOCK, token.Value);
            
        case CHTLJSTokenType::VIR:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_VIR_DECLARATION, token.Value);
            
        case CHTLJSTokenType::SELECTOR_CONTENT:
            m_ContextManager->EnterContext(CHTLJSContextType::SELECTOR_CONTEXT, 
                                         CHTLJSScopeType::FUNCTION_SCOPE, "selector");
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_ENHANCED_SELECTOR, token.Value);
            
        case CHTLJSTokenType::ARROW_OPERATOR:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_ARROW_OPERATION, token.Value);
            
        case CHTLJSTokenType::EVENT_BIND_OPERATOR:
            return m_StateMachine->TransitionTo(CHTLJSParseState::PARSING_EVENT_BIND, token.Value);
            
        case CHTLJSTokenType::LEFT_BRACE:
            m_StateMachine->EnterBrace();
            break;
            
        case CHTLJSTokenType::RIGHT_BRACE:
            m_StateMachine->ExitBrace();
            // 检查是否需要退出CHTL JS上下文
            if (m_StateMachine->GetCurrentContext().BraceLevel == 0) {
                m_ContextManager->ExitContext();
            }
            break;
            
        default:
            break;
    }
    
    return true;
}

CHTLJSToken CHTLJSLexer::ScanStringLiteral(char quote) {
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
    
    CHTLJSTokenType type = (quote == '"') ? CHTLJSTokenType::STRING_LITERAL : CHTLJSTokenType::SINGLE_QUOTE_STRING;
    return CreateToken(type, value, value.length());
}

CHTLJSToken CHTLJSLexer::ScanNumber() {
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
    
    return CreateToken(CHTLJSTokenType::NUMBER, value, value.length());
}

CHTLJSToken CHTLJSLexer::ScanOperator() {
    char ch = GetNextChar();
    std::string value(1, ch);
    
    switch (ch) {
        case '{': return CreateToken(CHTLJSTokenType::LEFT_BRACE, value, 1);
        case '}': return CreateToken(CHTLJSTokenType::RIGHT_BRACE, value, 1);
        case '[': return CreateToken(CHTLJSTokenType::LEFT_BRACKET, value, 1);
        case ']': return CreateToken(CHTLJSTokenType::RIGHT_BRACKET, value, 1);
        case '(': return CreateToken(CHTLJSTokenType::LEFT_PAREN, value, 1);
        case ')': return CreateToken(CHTLJSTokenType::RIGHT_PAREN, value, 1);
        case ';': return CreateToken(CHTLJSTokenType::SEMICOLON, value, 1);
        case ':': return CreateToken(CHTLJSTokenType::COLON, value, 1);
        case ',': return CreateToken(CHTLJSTokenType::COMMA, value, 1);
        case '.': return CreateToken(CHTLJSTokenType::DOT, value, 1);
        case '=': return CreateToken(CHTLJSTokenType::EQUAL, value, 1);
        case '&': return CreateToken(CHTLJSTokenType::AMPERSAND, value, 1);
        default:
            return CreateToken(CHTLJSTokenType::UNKNOWN, value, 1);
    }
}

void CHTLJSLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        if (std::isspace(ch)) {
            if (ch == '\n') {
                m_Tokens.push_back(CreateToken(CHTLJSTokenType::NEWLINE, "\n", 1));
            }
            GetNextChar();
        }
        else {
            break;
        }
    }
}

char CHTLJSLexer::GetNextChar() {
    if (IsAtEnd()) {
        return '\0';
    }
    
    char ch = m_SourceCode[m_CurrentPosition++];
    UpdateLineColumn(ch);
    return ch;
}

char CHTLJSLexer::PeekChar(size_t offset) {
    size_t peekPos = m_CurrentPosition + offset - 1;
    if (peekPos >= m_SourceCode.length()) {
        return '\0';
    }
    return m_SourceCode[peekPos];
}

bool CHTLJSLexer::IsAtEnd() const {
    return m_CurrentPosition >= m_SourceCode.length();
}

void CHTLJSLexer::UpdateLineColumn(char ch) {
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    }
    else {
        m_CurrentColumn++;
    }
}

CHTLJSToken CHTLJSLexer::CreateToken(CHTLJSTokenType type, const std::string& value, size_t length) {
    return CHTLJSToken(type, value, m_CurrentLine, m_CurrentColumn - length, 
                      m_CurrentPosition - length, length, true);
}

void CHTLJSLexer::SetError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = "CHTL JS Line " + std::to_string(m_CurrentLine) + 
                    ", Column " + std::to_string(m_CurrentColumn) + ": " + message;
}

bool CHTLJSLexer::ValidateUTF8(const std::string& str) {
    try {
        std::wstring wstr = m_UTF8Converter.from_bytes(str);
        return true;
    }
    catch (const std::range_error&) {
        return false;
    }
}

bool CHTLJSLexer::IsAllowedCHTLJSSyntaxInCurrentContext(const std::string& syntaxType) {
    return m_ContextManager->IsAllowedCHTLJSSyntax(syntaxType);
}

std::vector<std::string> CHTLJSLexer::ProcessAMDDependencies(const std::vector<std::string>& loadPaths) {
    return m_GlobalMap->AnalyzeDependencies(loadPaths);
}

std::string CHTLJSLexer::GenerateEventDelegateRegistry() {
    std::ostringstream code;
    
    code << "// CHTL JS Event Delegate Registry\n";
    code << "(function() {\n";
    code << "  var chtljs_delegates = {};\n";
    code << "  \n";
    code << "  window.CHTLJSEventDelegate = {\n";
    code << "    register: function(parent, target, event, handler) {\n";
    code << "      if (!chtljs_delegates[parent]) {\n";
    code << "        chtljs_delegates[parent] = {};\n";
    code << "      }\n";
    code << "      if (!chtljs_delegates[parent][event]) {\n";
    code << "        chtljs_delegates[parent][event] = [];\n";
    code << "      }\n";
    code << "      chtljs_delegates[parent][event].push({target: target, handler: handler});\n";
    code << "    }\n";
    code << "  };\n";
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSLexer::WrapWithRequestAnimationFrame(const std::string& animationCode) {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  function chtljs_animate() {\n";
    code << "    " << animationCode << "\n";
    code << "  }\n";
    code << "  \n";
    code << "  requestAnimationFrame(chtljs_animate);\n";
    code << "})();\n";
    
    return code.str();
}

// 辅助解析方法（需要实现）
std::unordered_map<std::string, std::string> CHTLJSLexer::ParseEventMappings(const std::string& content) {
    std::unordered_map<std::string, std::string> events;
    
    // 解析事件映射：click: handler, mouseenter: handler2
    std::regex eventPattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(content.begin(), content.end(), eventPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string eventType = match[1].str();
        std::string handler = match[2].str();
        
        // 清理处理器代码
        handler.erase(std::remove_if(handler.begin(), handler.end(), 
                     [](char c) { return c == '\n' || c == '\r'; }), handler.end());
        
        events[eventType] = handler;
    }
    
    return events;
}

std::string CHTLJSLexer::ParseDelegateInfo(const std::string& content) {
    // 简化的delegate信息解析
    return "delegate_info: " + content;
}

std::string CHTLJSLexer::ParseAnimationInfo(const std::string& content) {
    // 简化的animation信息解析
    return "animation_info: " + content;
}

std::vector<std::string> CHTLJSLexer::ParseModulePaths(const std::string& content) {
    std::vector<std::string> paths;
    
    // 解析模块路径：load: path1, load: path2
    std::regex loadPattern(R"(load:\s*([^,}]+))");
    std::sregex_iterator iter(content.begin(), content.end(), loadPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string path = match[1].str();
        
        // 清理路径
        path.erase(std::remove_if(path.begin(), path.end(), 
                  [](char c) { return std::isspace(c); }), path.end());
        
        if (!path.empty()) {
            paths.push_back(path);
        }
    }
    
    return paths;
}

const std::vector<CHTLJSToken>& CHTLJSLexer::GetTokens() const {
    return m_Tokens;
}

CHTLJSGlobalMap* CHTLJSLexer::GetGlobalMap() {
    return m_GlobalMap.get();
}

CHTLJSStateMachine* CHTLJSLexer::GetStateMachine() {
    return m_StateMachine.get();
}

CHTLJSContextManager* CHTLJSLexer::GetContextManager() {
    return m_ContextManager.get();
}

void CHTLJSLexer::Reset() {
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

void CHTLJSLexer::SetSourceCode(const std::string& sourceCode) {
    m_SourceCode = sourceCode;
    Reset();
}

bool CHTLJSLexer::HasError() const {
    return m_HasError;
}

std::string CHTLJSLexer::GetErrorMessage() const {
    return m_ErrorMessage;
}



} // namespace CHTLJS