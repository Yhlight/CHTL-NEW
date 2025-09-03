#include "CHTLJSParser.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::string& sourceCode)
    : m_CurrentTokenIndex(0), m_HasError(false), m_StrictCHTLJSMode(true), 
      m_ValidateSelectors(true), m_ProcessVirtualObjects(true) {
    
    m_Lexer = std::make_unique<CHTLJSLexer>(sourceCode);
}

CHTLJSParser::CHTLJSParser()
    : m_CurrentTokenIndex(0), m_HasError(false), m_StrictCHTLJSMode(true), 
      m_ValidateSelectors(true), m_ProcessVirtualObjects(true) {
    
    m_Lexer = std::make_unique<CHTLJSLexer>("");
}

CHTLJSParseResult CHTLJSParser::Parse() {
    CHTLJSParseResult result;
    Reset();
    
    // 执行CHTL JS词法分析
    if (!m_Lexer->Tokenize()) {
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS lexical analysis failed: " + m_Lexer->GetErrorMessage();
        return result;
    }
    
    m_Tokens = m_Lexer->GetTokens();
    
    if (m_Tokens.empty()) {
        result.IsSuccess = false;
        result.ErrorMessage = "No CHTL JS tokens found in source code";
        return result;
    }
    
    try {
        // 解析CHTL JS文档
        m_RootNode = ParseCHTLJSDocument();
        
        if (m_HasError) {
            result.IsSuccess = false;
            result.ErrorMessage = m_ErrorMessage;
            result.Warnings = m_Warnings;
            return result;
        }
        
        // 构建完整的CHTL JS AST
        BuildCompleteCHTLJSAST();
        
        // 应用CHTL JS后处理规则
        ApplyCHTLJSPostProcessingRules();
        
        // 验证CHTL JS AST完整性
        if (!ValidateCHTLJSASTIntegrity()) {
            result.IsSuccess = false;
            result.ErrorMessage = "CHTL JS AST integrity validation failed";
            return result;
        }
        
        // 成功完成CHTL JS解析
        result.RootNode = std::move(m_RootNode);
        result.IsSuccess = true;
        result.TokensProcessed = m_CurrentTokenIndex;
        result.Warnings = m_Warnings;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS parse exception: " + std::string(e.what());
    }
    
    return result;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseCHTLJSDocument() {
    // 创建CHTL JS文档根节点
    auto documentNode = CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::MODULE_NODE, "chtljs_document", "", 0, 0);
    
    SkipCHTLJSWhitespaceAndComments();
    
    // 解析CHTL JS文档内容
    while (!IsAtCHTLJSEnd() && !m_HasError) {
        SkipCHTLJSWhitespaceAndComments();
        
        if (IsAtCHTLJSEnd()) {
            break;
        }
        
        const CHTLJSToken& token = CurrentCHTLJSToken();
        
        if (token.Type == CHTLJSTokenType::MODULE) {
            // 解析module{}块
            auto moduleNode = ParseModuleBlock();
            if (moduleNode) {
                documentNode->AddChild(std::move(moduleNode));
            }
        }
        else if (token.Type == CHTLJSTokenType::VIR) {
            // 解析vir虚对象声明
            auto virNode = ParseVirDeclaration();
            if (virNode) {
                documentNode->AddChild(std::move(virNode));
            }
        }
        else if (token.Type == CHTLJSTokenType::SELECTOR_START || 
                 (token.Value.length() >= 2 && token.Value.substr(0, 2) == "{{")) {
            // 解析增强选择器
            auto selectorNode = ParseEnhancedSelector();
            if (selectorNode) {
                documentNode->AddChild(std::move(selectorNode));
            }
        }
        else if (IsCHTLJSFunctionToken(token)) {
            // 解析CHTL JS函数
            std::unique_ptr<CHTLJSBaseNode> functionNode = nullptr;
            
            switch (token.Type) {
                case CHTLJSTokenType::LISTEN:
                    functionNode = ParseListenFunction();
                    break;
                case CHTLJSTokenType::DELEGATE:
                    functionNode = ParseDelegateFunction();
                    break;
                case CHTLJSTokenType::ANIMATE:
                    functionNode = ParseAnimateFunction();
                    break;
                default:
                    SetCHTLJSParseError("Unknown CHTL JS function", &token);
                    break;
            }
            
            if (functionNode) {
                documentNode->AddChild(std::move(functionNode));
            }
        }
        else {
            SetCHTLJSParseError("Unexpected token in CHTL JS document", &token);
            AdvanceCHTLJSToken();
        }
    }
    
    return documentNode;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseModuleBlock() {
    const CHTLJSToken& moduleToken = CurrentCHTLJSToken();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::MODULE, "Expected 'module' keyword")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::LEFT_BRACE, "Expected '{' after 'module'")) {
        return nullptr;
    }
    
    auto moduleNode = CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::MODULE_NODE, "module", "", 
                                                         moduleToken.Line, moduleToken.Column);
    
    // 解析module内容
    while (!IsAtCHTLJSEnd() && !MatchCHTLJSToken(CHTLJSTokenType::RIGHT_BRACE)) {
        SkipCHTLJSWhitespaceAndComments();
        
        if (MatchCHTLJSToken(CHTLJSTokenType::RIGHT_BRACE)) {
            break;
        }
        
        if (CurrentCHTLJSToken().Type == CHTLJSTokenType::LOAD) {
            auto loadNode = ParseModuleLoad();
            if (loadNode) {
                moduleNode->AddChild(std::move(loadNode));
            }
        }
        else {
            SetCHTLJSParseError("Expected 'load' in module block", &CurrentCHTLJSToken());
            AdvanceCHTLJSToken();
        }
    }
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::RIGHT_BRACE, "Expected '}' to close module block")) {
        return nullptr;
    }
    
    return moduleNode;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseModuleLoad() {
    const CHTLJSToken& loadToken = CurrentCHTLJSToken();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::LOAD, "Expected 'load' keyword")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::COLON, "Expected ':' after 'load'")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    // 解析模块路径
    std::string modulePath = ParseCHTLJSLiteral();
    
    if (modulePath.empty()) {
        SetCHTLJSParseError("Expected module path", &CurrentCHTLJSToken());
        return nullptr;
    }
    
    auto loadNode = CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::LOAD_NODE, "load", modulePath,
                                                       loadToken.Line, loadToken.Column);
    
    // 消费可选的逗号
    SkipCHTLJSWhitespaceAndComments();
    if (MatchCHTLJSToken(CHTLJSTokenType::COMMA)) {
        AdvanceCHTLJSToken();
    }
    
    return loadNode;
}

std::unique_ptr<EnhancedSelectorNode> CHTLJSParser::ParseEnhancedSelector() {
    const CHTLJSToken& selectorToken = CurrentCHTLJSToken();
    
    std::string selectorContent;
    
    if (selectorToken.Type == CHTLJSTokenType::SELECTOR_START || 
        selectorToken.Value == "{{") {
        // 标准格式：{{ ... }}
        AdvanceCHTLJSToken(); // 消费{{
        
        // 读取选择器内容
        while (!IsAtCHTLJSEnd() && 
               CurrentCHTLJSToken().Type != CHTLJSTokenType::SELECTOR_END &&
               CurrentCHTLJSToken().Value != "}}") {
            selectorContent += CurrentCHTLJSToken().Value;
            AdvanceCHTLJSToken();
        }
        
        if (!ConsumeCHTLJSToken(CHTLJSTokenType::SELECTOR_END, "Expected '}}'")) {
            return nullptr;
        }
        
        selectorContent = "{{" + selectorContent + "}}";
    }
    else if (selectorToken.Value.find("{{") != std::string::npos) {
        // 完整的选择器令牌
        selectorContent = selectorToken.Value;
        AdvanceCHTLJSToken();
    }
    else {
        SetCHTLJSParseError("Expected enhanced selector", &selectorToken);
        return nullptr;
    }
    
    // 验证选择器格式
    if (m_ValidateSelectors && !ValidateEnhancedSelector(selectorContent)) {
        AddCHTLJSParseWarning("Invalid enhanced selector format: " + selectorContent, &selectorToken);
    }
    
    auto selectorNode = std::make_unique<EnhancedSelectorNode>(selectorContent, 
                                                              selectorToken.Line, selectorToken.Column);
    return selectorNode;
}

std::unique_ptr<VirObjectNode> CHTLJSParser::ParseVirDeclaration() {
    const CHTLJSToken& virToken = CurrentCHTLJSToken();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::VIR, "Expected 'vir' keyword")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    // 解析虚对象名称
    const CHTLJSToken& nameToken = CurrentCHTLJSToken();
    if (nameToken.Type != CHTLJSTokenType::IDENTIFIER) {
        SetCHTLJSParseError("Expected virtual object name", &nameToken);
        return nullptr;
    }
    
    std::string virName = nameToken.Value;
    AdvanceCHTLJSToken();
    
    SkipCHTLJSWhitespaceAndComments();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::EQUAL, "Expected '=' after virtual object name")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    // 解析CHTL JS函数内容
    std::string functionContent;
    
    // 检查CHTL JS函数类型
    const CHTLJSToken& functionToken = CurrentCHTLJSToken();
    if (IsCHTLJSFunctionToken(functionToken)) {
        std::string functionType = functionToken.Value;
        AdvanceCHTLJSToken();
        
        SkipCHTLJSWhitespaceAndComments();
        
        if (!ConsumeCHTLJSToken(CHTLJSTokenType::LEFT_BRACE, "Expected '{' after CHTL JS function")) {
            return nullptr;
        }
        
        // 读取函数内容
        std::ostringstream content;
        int braceLevel = 1;
        
        while (!IsAtCHTLJSEnd() && braceLevel > 0) {
            const CHTLJSToken& token = CurrentCHTLJSToken();
            
            if (token.Type == CHTLJSTokenType::LEFT_BRACE) {
                braceLevel++;
            }
            else if (token.Type == CHTLJSTokenType::RIGHT_BRACE) {
                braceLevel--;
                
                if (braceLevel == 0) {
                    break;
                }
            }
            
            content << token.Value;
            AdvanceCHTLJSToken();
        }
        
        if (!ConsumeCHTLJSToken(CHTLJSTokenType::RIGHT_BRACE, "Expected '}' to close CHTL JS function")) {
            return nullptr;
        }
        
        functionContent = functionType + " { " + content.str() + " }";
    }
    
    auto virNode = std::make_unique<VirObjectNode>(virName, virToken.Line, virToken.Column);
    virNode->SetCHTLJSFunctionContent(functionContent);
    
    // 处理虚对象
    if (m_ProcessVirtualObjects) {
        virNode->ParseCHTLJSFunction(functionContent);
    }
    
    return virNode;
}

std::unique_ptr<AnimateNode> CHTLJSParser::ParseAnimateFunction() {
    const CHTLJSToken& animateToken = CurrentCHTLJSToken();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::ANIMATE, "Expected 'animate' keyword")) {
        return nullptr;
    }
    
    SkipCHTLJSWhitespaceAndComments();
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::LEFT_BRACE, "Expected '{' after 'animate'")) {
        return nullptr;
    }
    
    auto animateNode = std::make_unique<AnimateNode>("", animateToken.Line, animateToken.Column);
    
    // 解析animate内容
    std::ostringstream animateContent;
    int braceLevel = 1;
    
    while (!IsAtCHTLJSEnd() && braceLevel > 0) {
        const CHTLJSToken& token = CurrentCHTLJSToken();
        
        if (token.Type == CHTLJSTokenType::LEFT_BRACE) {
            braceLevel++;
        }
        else if (token.Type == CHTLJSTokenType::RIGHT_BRACE) {
            braceLevel--;
            
            if (braceLevel == 0) {
                break;
            }
        }
        
        animateContent << token.Value;
        AdvanceCHTLJSToken();
    }
    
    if (!ConsumeCHTLJSToken(CHTLJSTokenType::RIGHT_BRACE, "Expected '}' to close animate block")) {
        return nullptr;
    }
    
    // 解析动画内容
    animateNode->ParseAnimateContent(animateContent.str());
    
    return animateNode;
}

// CHTL JS令牌操作实现
const CHTLJSToken& CHTLJSParser::CurrentCHTLJSToken() {
    if (m_CurrentTokenIndex >= m_Tokens.size()) {
        static CHTLJSToken eofToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0, 0);
        return eofToken;
    }
    
    return m_Tokens[m_CurrentTokenIndex];
}

const CHTLJSToken& CHTLJSParser::PeekCHTLJSToken(size_t offset) {
    size_t peekIndex = m_CurrentTokenIndex + offset;
    if (peekIndex >= m_Tokens.size()) {
        static CHTLJSToken eofToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0, 0);
        return eofToken;
    }
    
    return m_Tokens[peekIndex];
}

bool CHTLJSParser::AdvanceCHTLJSToken() {
    if (m_CurrentTokenIndex < m_Tokens.size()) {
        m_CurrentTokenIndex++;
        return true;
    }
    
    return false;
}

bool CHTLJSParser::IsAtCHTLJSEnd() {
    return m_CurrentTokenIndex >= m_Tokens.size() || 
           CurrentCHTLJSToken().Type == CHTLJSTokenType::EOF_TOKEN;
}

bool CHTLJSParser::ConsumeCHTLJSToken(CHTLJSTokenType expectedType, const std::string& errorMessage) {
    if (CurrentCHTLJSToken().Type == expectedType) {
        AdvanceCHTLJSToken();
        return true;
    }
    
    if (!errorMessage.empty()) {
        SetCHTLJSParseError(errorMessage + " (got " + CurrentCHTLJSToken().GetTypeName() + ")", &CurrentCHTLJSToken());
    }
    
    return false;
}

bool CHTLJSParser::MatchCHTLJSToken(CHTLJSTokenType type) {
    return CurrentCHTLJSToken().Type == type;
}

void CHTLJSParser::SkipCHTLJSWhitespaceAndComments() {
    while (!IsAtCHTLJSEnd()) {
        const CHTLJSToken& token = CurrentCHTLJSToken();
        
        if (token.Type == CHTLJSTokenType::WHITESPACE ||
            token.Type == CHTLJSTokenType::NEWLINE ||
            token.Type == CHTLJSTokenType::COMMENT) {
            AdvanceCHTLJSToken();
        }
        else {
            break;
        }
    }
}

bool CHTLJSParser::IsCHTLJSFunctionToken(const CHTLJSToken& token) {
    return token.Type == CHTLJSTokenType::LISTEN ||
           token.Type == CHTLJSTokenType::DELEGATE ||
           token.Type == CHTLJSTokenType::ANIMATE ||
           token.Type == CHTLJSTokenType::MODULE;
}

bool CHTLJSParser::IsEnhancedSelectorToken(const CHTLJSToken& token) {
    return token.Type == CHTLJSTokenType::SELECTOR_START ||
           token.Type == CHTLJSTokenType::SELECTOR_CONTENT ||
           (token.Value.length() >= 2 && token.Value.substr(0, 2) == "{{");
}

std::string CHTLJSParser::ParseCHTLJSLiteral() {
    const CHTLJSToken& token = CurrentCHTLJSToken();
    
    switch (token.Type) {
        case CHTLJSTokenType::STRING_LITERAL:
        case CHTLJSTokenType::SINGLE_QUOTE_STRING:
            {
                std::string value = token.Value;
                
                // 去除引号
                if (value.length() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.length() - 2);
                }
                
                AdvanceCHTLJSToken();
                return value;
            }
        case CHTLJSTokenType::UNQUOTED_LITERAL:
        case CHTLJSTokenType::IDENTIFIER:
        case CHTLJSTokenType::NUMBER:
            {
                std::string value = token.Value;
                AdvanceCHTLJSToken();
                return value;
            }
        default:
            return "";
    }
}

bool CHTLJSParser::ValidateEnhancedSelector(const std::string& selector) {
    // 验证增强选择器格式：{{...}}
    if (selector.length() < 4 || 
        selector.substr(0, 2) != "{{" || 
        selector.substr(selector.length() - 2) != "}}") {
        return false;
    }
    
    std::string content = selector.substr(2, selector.length() - 4);
    
    // 验证选择器内容
    std::regex validSelectorPattern(R"([.#]?[a-zA-Z][a-zA-Z0-9\-_]*(?:\s+[a-zA-Z][a-zA-Z0-9\-_]*)*(?:\[\d+\])?)");
    return std::regex_match(content, validSelectorPattern);
}

void CHTLJSParser::SetCHTLJSParseError(const std::string& message, const CHTLJSToken* token) {
    m_HasError = true;
    
    if (token) {
        m_ErrorMessage = "CHTL JS parse error at " + FormatCHTLJSTokenPosition(*token) + ": " + message;
    }
    else {
        m_ErrorMessage = "CHTL JS parse error: " + message;
    }
}

void CHTLJSParser::AddCHTLJSParseWarning(const std::string& message, const CHTLJSToken* token) {
    std::string warning;
    
    if (token) {
        warning = "CHTL JS parse warning at " + FormatCHTLJSTokenPosition(*token) + ": " + message;
    }
    else {
        warning = "CHTL JS parse warning: " + message;
    }
    
    m_Warnings.push_back(warning);
}

std::string CHTLJSParser::FormatCHTLJSTokenPosition(const CHTLJSToken& token) {
    return "line " + std::to_string(token.Line) + ", column " + std::to_string(token.Column);
}

CHTLJSBaseNode* CHTLJSParser::GetRootNode() {
    return m_RootNode.get();
}

void CHTLJSParser::Reset() {
    m_CurrentTokenIndex = 0;
    m_RootNode.reset();
    
    while (!m_NodeStack.empty()) {
        m_NodeStack.pop();
    }
    
    while (!m_CHTLJSScopeStack.empty()) {
        m_CHTLJSScopeStack.pop();
    }
    
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Warnings.clear();
}

void CHTLJSParser::SetSourceCode(const std::string& sourceCode) {
    m_Lexer->SetSourceCode(sourceCode);
    Reset();
}

void CHTLJSParser::SetCHTLJSParseOptions(bool strictMode, bool validateSelectors, bool processVirtualObjects) {
    m_StrictCHTLJSMode = strictMode;
    m_ValidateSelectors = validateSelectors;
    m_ProcessVirtualObjects = processVirtualObjects;
}

// 占位实现（需要后续完善）
std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseListenFunction() {
    return CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::LISTEN_NODE, "listen");
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseDelegateFunction() {
    return CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::DELEGATE_NODE, "delegate");
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseArrowOperator() {
    return CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::ARROW_OPERATOR_NODE, "arrow");
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseEventBindOperator() {
    return CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::EVENT_BIND_OPERATOR_NODE, "event_bind");
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::ParseVirReference() {
    return CHTLJSNodeFactory::CreateCHTLJSNode(CHTLJSNodeType::VIR_REFERENCE_NODE, "vir_ref");
}

void CHTLJSParser::BuildCompleteCHTLJSAST() {
    // 构建完整的CHTL JS AST
}

void CHTLJSParser::ApplyCHTLJSPostProcessingRules() {
    // 应用CHTL JS后处理规则
}

bool CHTLJSParser::ValidateCHTLJSASTIntegrity() {
    return m_RootNode != nullptr;
}

SelectorPriority CHTLJSParser::AnalyzeSelectorPriority(const std::string& selector) {
    // 分析选择器优先级
    if (selector.find('#') != std::string::npos) {
        return SelectorPriority::ID_PRIORITY;
    }
    else if (selector.find('.') != std::string::npos) {
        return SelectorPriority::CLASS_PRIORITY;
    }
    else {
        return SelectorPriority::TAG_PRIORITY;
    }
}

std::unordered_map<std::string, std::string> CHTLJSParser::ParseCHTLJSFunctionContent(const std::string& functionType) {
    // 解析CHTL JS函数内容
    return {};
}

std::unordered_map<std::string, std::string> CHTLJSParser::ParseEventMapping() {
    // 解析事件映射
    return {};
}

std::string CHTLJSParser::ParseEventHandler() {
    // 解析事件处理器
    return "";
}

std::vector<std::string> CHTLJSParser::ParseDelegateTargets() {
    // 解析delegate目标
    return {};
}

void CHTLJSParser::ParseAnimationProperties(AnimateNode* animateNode) {
    // 解析动画属性
}

std::vector<std::string> CHTLJSParser::ParseAnimationTargets() {
    // 解析动画目标
    return {};
}

std::unordered_map<std::string, std::string> CHTLJSParser::ParseAnimationStateBlock() {
    // 解析动画状态块
    return {};
}

std::vector<AnimationKeyframe> CHTLJSParser::ParseWhenKeyframes() {
    // 解析when关键帧
    return {};
}

std::string CHTLJSParser::ParseSelectorContent(const std::string& selectorContent) {
    // 解析选择器内容
    return selectorContent;
}

std::vector<std::string> CHTLJSParser::ParseCHTLJSIdentifierList() {
    // 解析CHTL JS标识符列表
    return {};
}

bool CHTLJSParser::ValidateCHTLJSSyntax(const std::string& syntax) {
    // 验证CHTL JS语法
    return true;
}

} // namespace CHTLJS