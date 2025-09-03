#include "CHTLParser.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <unordered_set>
#include <codecvt>
#include <locale>

namespace CHTL {

CHTLParser::CHTLParser(const std::string& sourceCode)
    : m_CurrentTokenIndex(0), m_HasError(false), m_StrictMode(true), 
      m_ValidateConstraints(true), m_ProcessInheritance(true) {
    
    m_Lexer = std::make_unique<CHTLLexer>(sourceCode);
}

CHTLParser::CHTLParser()
    : m_CurrentTokenIndex(0), m_HasError(false), m_StrictMode(true), 
      m_ValidateConstraints(true), m_ProcessInheritance(true) {
    
    m_Lexer = std::make_unique<CHTLLexer>("");
}

ParseResult CHTLParser::Parse() {
    ParseResult result;
    Reset();
    
    // 执行词法分析
    if (!m_Lexer->Tokenize()) {
        result.IsSuccess = false;
        result.ErrorMessage = "Lexical analysis failed: " + m_Lexer->GetErrorMessage();
        return result;
    }
    
    m_Tokens = m_Lexer->GetTokens();
    
    if (m_Tokens.empty()) {
        result.IsSuccess = false;
        result.ErrorMessage = "No tokens found in source code";
        return result;
    }
    
    try {
        // 解析CHTL文档
        m_RootNode = ParseDocument();
        
        if (m_HasError) {
            result.IsSuccess = false;
            result.ErrorMessage = m_ErrorMessage;
            result.Warnings = m_Warnings;
            return result;
        }
        
        // 构建完整的AST
        BuildCompleteAST();
        
        // 应用后处理规则
        ApplyPostProcessingRules();
        
        // 验证AST完整性
        if (!ValidateASTIntegrity()) {
            result.IsSuccess = false;
            result.ErrorMessage = "AST integrity validation failed";
            return result;
        }
        
        // 成功完成解析
        result.RootNode = std::move(m_RootNode);
        result.IsSuccess = true;
        result.TokensProcessed = m_CurrentTokenIndex;
        result.Warnings = m_Warnings;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.ErrorMessage = "Parse exception: " + std::string(e.what());
    }
    
    return result;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseDocument() {
    // 创建文档根节点
    auto documentNode = CHTLNodeFactory::CreateNode(CHTLNodeType::ELEMENT_NODE, "document", "", 0, 0);
    
    SkipWhitespaceAndComments();
    
    // 解析文档级别的特殊语法块
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        const auto& currentToken = CurrentToken();
        
        if (currentToken.Type == CHTLTokenType::LEFT_BRACKET) {
            // 处理特殊语法块：[Import], [Namespace], [Configuration]等
            if (PeekToken().Value == "Import") {
                auto importNode = ParseImportStatement();
                if (importNode) {
                    documentNode->AddChild(std::move(importNode));
                }
            }
            else if (PeekToken().Value == "Namespace") {
                auto namespaceNode = ParseNamespaceDefinition();
                if (namespaceNode) {
                    documentNode->AddChild(std::move(namespaceNode));
                }
            }
            else if (PeekToken().Value == "Configuration") {
                auto configNode = ParseConfiguration();
                if (configNode) {
                    documentNode->AddChild(std::move(configNode));
                }
            }
            else if (PeekToken().Value == "Template") {
                auto templateNode = ParseTemplateDefinition();
                if (templateNode) {
                    documentNode->AddChild(std::move(templateNode));
                }
            }
            else if (PeekToken().Value == "Custom") {
                auto customNode = ParseCustomDefinition();
                if (customNode) {
                    documentNode->AddChild(std::move(customNode));
                }
            }
            else if (PeekToken().Value == "Origin") {
                auto originNode = ParseOriginDefinition();
                if (originNode) {
                    documentNode->AddChild(std::move(originNode));
                }
            }
            else {
                SetParseError("未知的特殊语法块: " + PeekToken().Value, &currentToken);
                return nullptr;
            }
        }
        else if (currentToken.Type == CHTLTokenType::IDENTIFIER) {
            // 处理HTML元素
            auto elementNode = ParseElement();
            if (elementNode) {
                documentNode->AddChild(std::move(elementNode));
            } else if (m_HasError) {
                return nullptr;
            }
        }
        else if (currentToken.Type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
        else {
            // 跳过不认识的令牌
            AdvanceToken();
        }
        
        SkipWhitespaceAndComments();
    }
    
    return documentNode;
    
    // 检查use语句（必须在文件开头）
    if (MatchToken(CHTLTokenType::USE)) {
        auto useNode = ParseUseStatement();
        if (useNode) {
            documentNode->AddChild(std::move(useNode));
        }
    }
    
    // 解析文档内容
    while (!IsAtEnd() && !m_HasError) {
        SkipWhitespaceAndComments();
        
        if (IsAtEnd()) {
            break;
        }
        
        const CHTLToken& token = CurrentToken();
        
        if (IsBlockStructureStart(token)) {
            // 解析块结构（模板、自定义、原始嵌入、配置、导入、命名空间）
            std::unique_ptr<CHTLBaseNode> blockNode = nullptr;
            
            switch (token.Type) {
                case CHTLTokenType::TEMPLATE_BLOCK:
                    blockNode = ParseTemplateDefinition();
                    break;
                case CHTLTokenType::CUSTOM_BLOCK:
                    blockNode = ParseCustomDefinition();
                    break;
                case CHTLTokenType::ORIGIN_BLOCK:
                    blockNode = ParseOriginDefinition();
                    break;
                case CHTLTokenType::CONFIGURATION_BLOCK:
                    blockNode = ParseConfiguration();
                    break;
                case CHTLTokenType::IMPORT_BLOCK:
                    blockNode = ParseImportStatement();
                    break;
                case CHTLTokenType::NAMESPACE_BLOCK:
                    blockNode = ParseNamespaceDefinition();
                    break;
                default:
                    SetParseError("Unexpected block structure", &token);
                    break;
            }
            
            if (blockNode) {
                documentNode->AddChild(std::move(blockNode));
            }
        }
        else if (IsHTMLElementToken(token)) {
            // 解析HTML元素
            auto elementNode = ParseElement();
            if (elementNode) {
                documentNode->AddChild(std::move(elementNode));
            }
        }
        else if (IsAtTypeToken(token)) {
            // 解析@类型使用（模板使用、自定义使用等）
            if (token.Type == CHTLTokenType::AT_STYLE || 
                token.Type == CHTLTokenType::AT_ELEMENT || 
                token.Type == CHTLTokenType::AT_VAR) {
                
                // 检查是否为模板使用或自定义使用
                auto usageNode = ParseTemplateUsage();
                if (!usageNode) {
                    usageNode = ParseCustomUsage();
                }
                
                if (usageNode) {
                    documentNode->AddChild(std::move(usageNode));
                }
            }
        }
        else {
            SetParseError("Unexpected token in document", &token);
            AdvanceToken();
        }
    }
    
    return documentNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseUseStatement() {
    if (!ConsumeToken(CHTLTokenType::USE, "Expected 'use' keyword")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    const CHTLToken& token = CurrentToken();
    std::string useTarget;
    
    if (token.Type == CHTLTokenType::HTML5) {
        // use html5;
        useTarget = "html5";
        AdvanceToken();
    }
    else if (token.Type == CHTLTokenType::AT_CONFIG) {
        // use @Config ConfigName;
        useTarget = "@Config";
        AdvanceToken();
        
        SkipWhitespaceAndComments();
        if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
            useTarget += " " + CurrentToken().Value;
            AdvanceToken();
        }
    }
    else if (token.Type == CHTLTokenType::CONFIGURATION_BLOCK) {
        // use [Configuration] @Config ConfigName;
        useTarget = "[Configuration]";
        AdvanceToken();
        
        SkipWhitespaceAndComments();
        if (CurrentToken().Type == CHTLTokenType::AT_CONFIG) {
            useTarget += " @Config";
            AdvanceToken();
            
            SkipWhitespaceAndComments();
            if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
                useTarget += " " + CurrentToken().Value;
                AdvanceToken();
            }
        }
    }
    else {
        SetParseError("Invalid use statement target", &token);
        return nullptr;
    }
    
    // 消费分号
    SkipWhitespaceAndComments();
    if (MatchToken(CHTLTokenType::SEMICOLON)) {
        AdvanceToken();
    }
    
    auto useNode = CHTLNodeFactory::CreateNode(CHTLNodeType::USE_NODE, "use", useTarget, 
                                              token.Line, token.Column);
    return useNode;
}

std::unique_ptr<ElementNode> CHTLParser::ParseElement() {
    const CHTLToken& elementToken = CurrentToken();
    
    if (!IsHTMLElementToken(elementToken)) {
        SetParseError("Expected HTML element", &elementToken);
        return nullptr;
    }
    
    std::string tagName = elementToken.Value;
    
    // 验证HTML标签名
    if (m_StrictMode && !IsValidHTMLTag(tagName)) {
        AddParseWarning("Unknown HTML tag: " + tagName, &elementToken);
    }
    
    auto elementNode = std::make_unique<ElementNode>(tagName, elementToken.Line, elementToken.Column);
    AdvanceToken();
    
    SkipWhitespaceAndComments();
    
    // 解析元素块
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after element name")) {
        return nullptr;
    }
    
    // 解析元素内容
    while (!IsAtEnd() && !MatchToken(CHTLTokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (MatchToken(CHTLTokenType::RIGHT_BRACE)) {
            break;
        }
        
        const CHTLToken& token = CurrentToken();
        
        if (IsHTMLElementToken(token)) {
            // 嵌套元素
            auto childElement = ParseElement();
            if (childElement) {
                elementNode->AddChild(std::move(childElement));
            }
        }
        else if (token.Type == CHTLTokenType::TEXT) {
            // 文本节点
            auto textNode = ParseText();
            if (textNode) {
                elementNode->AddChild(std::move(textNode));
            }
        }
        else if (token.Type == CHTLTokenType::STYLE) {
            // 局部样式块
            auto styleNode = ParseLocalStyleBlock();
            if (styleNode) {
                elementNode->AddStyleNode(std::move(styleNode));
                
                // 处理自动化选择器生成
                ProcessSelectorAutomation(elementNode.get(), static_cast<StyleNode*>(styleNode.get()));
            }
        }
        else if (token.Type == CHTLTokenType::SCRIPT) {
            // 局部脚本块（属于CHTL）
            auto scriptNode = ParseLocalScriptBlock();
            if (scriptNode) {
                elementNode->AddScriptNode(std::move(scriptNode));
            }
        }
        else if (token.Type == CHTLTokenType::EXCEPT) {
            // 约束语句
            auto constraintNode = ParseConstraintStatement();
            if (constraintNode) {
                elementNode->AddChild(std::move(constraintNode));
            }
        }
        else if (IsCEEquivalenceToken(token) || token.Type == CHTLTokenType::IDENTIFIER) {
            // 属性
            ParseAttribute(elementNode.get());
        }
        else if (IsAtTypeToken(token)) {
            // @类型使用（模板使用、自定义使用）
            auto usageNode = ParseTemplateUsage();
            if (!usageNode) {
                usageNode = ParseCustomUsage();
            }
            if (usageNode) {
                elementNode->AddChild(std::move(usageNode));
            }
        }
        else {
            SetParseError("Unexpected token in element block", &token);
            AdvanceToken();
        }
    }
    
    // 消费右大括号
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close element block")) {
        return nullptr;
    }
    
    return elementNode;
}

std::unique_ptr<TextNode> CHTLParser::ParseText() {
    if (!ConsumeToken(CHTLTokenType::TEXT, "Expected 'text' keyword")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after 'text'")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    // 解析文本内容
    std::string textContent;
    const CHTLToken& contentToken = CurrentToken();
    
    if (contentToken.Type == CHTLTokenType::STRING_LITERAL ||
        contentToken.Type == CHTLTokenType::SINGLE_QUOTE_STRING ||
        contentToken.Type == CHTLTokenType::UNQUOTED_LITERAL ||
        contentToken.Type == CHTLTokenType::IDENTIFIER) {
        
        textContent = contentToken.Value;
        
        // 验证UTF-8内容
        if (!ValidateUTF8Content(textContent)) {
            AddParseWarning("Invalid UTF-8 encoding in text content", &contentToken);
        }
        
        AdvanceToken();
    }
    else {
        SetParseError("Expected text content", &contentToken);
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close text block")) {
        return nullptr;
    }
    
    auto textNode = std::make_unique<TextNode>(textContent, contentToken.Line, contentToken.Column);
    return textNode;
}

void CHTLParser::ParseAttribute(ElementNode* element) {
    if (!element) {
        return;
    }
    
    const CHTLToken& nameToken = CurrentToken();
    
    if (nameToken.Type != CHTLTokenType::IDENTIFIER) {
        SetParseError("Expected attribute name", &nameToken);
        return;
    }
    
    std::string attributeName = nameToken.Value;
    AdvanceToken();
    
    SkipWhitespaceAndComments();
    
    // 检查CE对等式（:或=）
    const CHTLToken& operatorToken = CurrentToken();
    if (!IsCEEquivalenceToken(operatorToken)) {
        SetParseError("Expected ':' or '=' after attribute name", &operatorToken);
        return;
    }
    
    AdvanceToken();
    SkipWhitespaceAndComments();
    
    // 解析属性值
    std::string attributeValue = ParseLiteral();
    
    if (attributeValue.empty()) {
        SetParseError("Expected attribute value", &CurrentToken());
        return;
    }
    
    // 设置属性
    element->SetAttribute(attributeName, attributeValue);
    
    // 消费可选的分号
    SkipWhitespaceAndComments();
    if (MatchToken(CHTLTokenType::SEMICOLON)) {
        AdvanceToken();
    }
}

std::unique_ptr<StyleNode> CHTLParser::ParseLocalStyleBlock() {
    const CHTLToken& styleToken = CurrentToken();
    
    if (!ConsumeToken(CHTLTokenType::STYLE, "Expected 'style' keyword")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after 'style'")) {
        return nullptr;
    }
    
    auto styleNode = std::make_unique<StyleNode>(styleToken.Line, styleToken.Column);
    
    // 解析样式内容
    std::ostringstream styleContent;
    int braceLevel = 1;
    
    while (!IsAtEnd() && braceLevel > 0) {
        const CHTLToken& token = CurrentToken();
        
        if (token.Type == CHTLTokenType::LEFT_BRACE) {
            braceLevel++;
        }
        else if (token.Type == CHTLTokenType::RIGHT_BRACE) {
            braceLevel--;
            
            if (braceLevel == 0) {
                break; // 样式块结束
            }
        }
        
        styleContent << token.Value;
        
        // 处理特殊令牌
        if (token.Type == CHTLTokenType::CLASS_SELECTOR) {
            // 自动化类名生成
            std::string className = token.Value.substr(1); // 去掉.
            styleNode->AddAutoGeneratedSelector(token.Value);
        }
        else if (token.Type == CHTLTokenType::ID_SELECTOR) {
            // 自动化ID生成
            std::string idName = token.Value.substr(1); // 去掉#
            styleNode->AddAutoGeneratedSelector(token.Value);
        }
        else if (token.Type == CHTLTokenType::CONTEXT_REFERENCE) {
            // 上下文引用
            styleNode->AddContextReference(token.Value);
        }
        else if (IsAtTypeToken(token)) {
            // 模板或自定义使用
            if (token.Value.find("@Style") != std::string::npos) {
                styleNode->AddTemplateUsage(token.Value);
            }
        }
        
        AdvanceToken();
    }
    
    // 消费右大括号
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close style block")) {
        return nullptr;
    }
    
    // 解析样式内容
    styleNode->ParseStyleContent(styleContent.str());
    
    return styleNode;
}

std::unique_ptr<ScriptNode> CHTLParser::ParseLocalScriptBlock() {
    const CHTLToken& scriptToken = CurrentToken();
    
    if (!ConsumeToken(CHTLTokenType::SCRIPT, "Expected 'script' keyword")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after 'script'")) {
        return nullptr;
    }
    
    // 解析脚本内容
    std::ostringstream scriptContent;
    int braceLevel = 1;
    
    while (!IsAtEnd() && braceLevel > 0) {
        const CHTLToken& token = CurrentToken();
        
        if (token.Type == CHTLTokenType::LEFT_BRACE) {
            braceLevel++;
        }
        else if (token.Type == CHTLTokenType::RIGHT_BRACE) {
            braceLevel--;
            
            if (braceLevel == 0) {
                break; // 脚本块结束
            }
        }
        
        scriptContent << token.Value;
        AdvanceToken();
    }
    
    // 消费右大括号
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close script block")) {
        return nullptr;
    }
    
    auto scriptNode = std::make_unique<ScriptNode>(scriptContent.str(), scriptToken.Line, scriptToken.Column);
    
    // 验证局部script允许的CHTL语法
    if (m_ValidateConstraints) {
        // 局部script允许：模板变量、自定义变量组、变量组特例化、命名空间from、注释、原始嵌入、{{&}}特供语法
        const std::string& content = scriptContent.str();
        
        // 检查不允许的语法
        if (content.find("[Template] @Element") != std::string::npos ||
            content.find("[Template] @Style") != std::string::npos ||
            content.find("[Custom] @Element") != std::string::npos ||
            content.find("[Custom] @Style") != std::string::npos) {
            AddParseWarning("Template/Custom elements and styles not allowed in local script block", &scriptToken);
        }
    }
    
    return scriptNode;
}

std::unique_ptr<TemplateNode> CHTLParser::ParseTemplateDefinition() {
    if (!ConsumeToken(CHTLTokenType::TEMPLATE_BLOCK, "Expected '[Template]'")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    // 解析@类型
    const CHTLToken& typeToken = CurrentToken();
    if (!IsAtTypeToken(typeToken)) {
        SetParseError("Expected @Style, @Element, or @Var after [Template]", &typeToken);
        return nullptr;
    }
    
    std::string templateType = typeToken.Value;
    AdvanceToken();
    
    SkipWhitespaceAndComments();
    
    // 解析模板名称
    const CHTLToken& nameToken = CurrentToken();
    if (nameToken.Type != CHTLTokenType::IDENTIFIER) {
        SetParseError("Expected template name", &nameToken);
        return nullptr;
    }
    
    std::string templateName = nameToken.Value;
    AdvanceToken();
    
    // 根据模板类型解析
    if (templateType == "@Style") {
        return ParseStyleTemplate(templateName);
    }
    else if (templateType == "@Element") {
        return ParseElementTemplate(templateName);
    }
    else if (templateType == "@Var") {
        return ParseVarTemplate(templateName);
    }
    
    SetParseError("Unknown template type: " + templateType, &typeToken);
    return nullptr;
}

std::unique_ptr<TemplateNode> CHTLParser::ParseStyleTemplate(const std::string& templateName) {
    auto templateNode = std::make_unique<TemplateNode>(TemplateType::STYLE_TEMPLATE, templateName, 
                                                      CurrentToken().Line, CurrentToken().Column);
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after template name")) {
        return nullptr;
    }
    
    // 解析样式组模板内容
    std::ostringstream templateContent;
    
    while (!IsAtEnd() && !MatchToken(CHTLTokenType::RIGHT_BRACE)) {
        SkipWhitespaceAndComments();
        
        if (MatchToken(CHTLTokenType::RIGHT_BRACE)) {
            break;
        }
        
        // 解析CSS属性或继承语句
        const CHTLToken& token = CurrentToken();
        
        if (token.Type == CHTLTokenType::INHERIT) {
            // 显式继承：inherit @Style TemplateName;
            ParseInheritance(templateNode.get());
        }
        else if (IsAtTypeToken(token)) {
            // 组合式继承：@Style TemplateName;
            std::string inheritTarget = token.Value;
            AdvanceToken();
            
            SkipWhitespaceAndComments();
            if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
                inheritTarget += " " + CurrentToken().Value;
                templateNode->AddInheritedTemplate(inheritTarget);
                AdvanceToken();
            }
            
            // 消费分号
            SkipWhitespaceAndComments();
            if (MatchToken(CHTLTokenType::SEMICOLON)) {
                AdvanceToken();
            }
        }
        else if (token.Type == CHTLTokenType::IDENTIFIER) {
            // CSS属性
            auto property = ParseCSSProperty();
            if (!property.first.empty()) {
                templateNode->AddTemplateProperty(property.first, property.second);
                templateContent << property.first << ": " << property.second << ";\n";
            }
        }
        else {
            templateContent << token.Value;
            AdvanceToken();
        }
    }
    
    // 消费右大括号
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close template block")) {
        return nullptr;
    }
    
    templateNode->SetTemplateContent(templateContent.str());
    
    // 处理模板继承
    if (m_ProcessInheritance) {
        ProcessTemplateInheritance(templateNode.get());
    }
    
    return templateNode;
}

// 令牌操作实现
const CHTLToken& CHTLParser::CurrentToken() {
    if (m_CurrentTokenIndex >= m_Tokens.size()) {
        static CHTLToken eofToken(CHTLTokenType::EOF_TOKEN, "", 0, 0, 0, 0);
        return eofToken;
    }
    
    return m_Tokens[m_CurrentTokenIndex];
}

const CHTLToken& CHTLParser::PeekToken(size_t offset) {
    size_t peekIndex = m_CurrentTokenIndex + offset;
    if (peekIndex >= m_Tokens.size()) {
        static CHTLToken eofToken(CHTLTokenType::EOF_TOKEN, "", 0, 0, 0, 0);
        return eofToken;
    }
    
    return m_Tokens[peekIndex];
}

bool CHTLParser::AdvanceToken() {
    if (m_CurrentTokenIndex < m_Tokens.size()) {
        m_CurrentTokenIndex++;
        return true;
    }
    
    return false;
}

bool CHTLParser::IsAtEnd() {
    return m_CurrentTokenIndex >= m_Tokens.size() || 
           CurrentToken().Type == CHTLTokenType::EOF_TOKEN;
}

bool CHTLParser::ConsumeToken(CHTLTokenType expectedType, const std::string& errorMessage) {
    if (CurrentToken().Type == expectedType) {
        AdvanceToken();
        return true;
    }
    
    if (!errorMessage.empty()) {
        SetParseError(errorMessage + " (got " + CurrentToken().GetTypeName() + ")", &CurrentToken());
    }
    
    return false;
}

bool CHTLParser::MatchToken(CHTLTokenType type) {
    return CurrentToken().Type == type;
}

void CHTLParser::SkipWhitespaceAndComments() {
    while (!IsAtEnd()) {
        const CHTLToken& token = CurrentToken();
        
        if (token.Type == CHTLTokenType::WHITESPACE ||
            token.Type == CHTLTokenType::NEWLINE ||
            token.Type == CHTLTokenType::SINGLE_LINE_COMMENT ||
            token.Type == CHTLTokenType::MULTI_LINE_COMMENT ||
            token.Type == CHTLTokenType::GENERATOR_COMMENT) {
            AdvanceToken();
        }
        else {
            break;
        }
    }
}

std::pair<std::string, std::string> CHTLParser::ParseCSSProperty() {
    const CHTLToken& propertyToken = CurrentToken();
    
    if (propertyToken.Type != CHTLTokenType::IDENTIFIER) {
        SetParseError("Expected CSS property name", &propertyToken);
        return {"", ""};
    }
    
    std::string propertyName = propertyToken.Value;
    AdvanceToken();
    
    SkipWhitespaceAndComments();
    
    // 检查CE对等式
    if (!IsCEEquivalenceToken(CurrentToken())) {
        SetParseError("Expected ':' or '=' after CSS property name", &CurrentToken());
        return {"", ""};
    }
    
    AdvanceToken();
    SkipWhitespaceAndComments();
    
    // 解析属性值
    std::string propertyValue = ParseLiteral();
    
    if (propertyValue.empty()) {
        SetParseError("Expected CSS property value", &CurrentToken());
        return {"", ""};
    }
    
    // 验证CSS属性
    if (m_StrictMode && !IsValidCSSProperty(propertyName)) {
        AddParseWarning("Unknown CSS property: " + propertyName, &propertyToken);
    }
    
    // 消费分号
    SkipWhitespaceAndComments();
    if (MatchToken(CHTLTokenType::SEMICOLON)) {
        AdvanceToken();
    }
    
    return {propertyName, propertyValue};
}

std::string CHTLParser::ParseLiteral() {
    const CHTLToken& token = CurrentToken();
    
    switch (token.Type) {
        case CHTLTokenType::STRING_LITERAL:
        case CHTLTokenType::SINGLE_QUOTE_STRING:
            return ParseStringLiteral();
        case CHTLTokenType::UNQUOTED_LITERAL:
            return ParseUnquotedLiteral();
        case CHTLTokenType::IDENTIFIER:
        case CHTLTokenType::NUMBER:
            {
                std::string value = token.Value;
                AdvanceToken();
                return value;
            }
        default:
            return "";
    }
}

std::string CHTLParser::ParseStringLiteral() {
    const CHTLToken& token = CurrentToken();
    std::string value = token.Value;
    
    // 去除引号
    if (value.length() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.length() - 2);
    }
    
    AdvanceToken();
    return value;
}

std::string CHTLParser::ParseUnquotedLiteral() {
    const CHTLToken& token = CurrentToken();
    std::string value = token.Value;
    AdvanceToken();
    return value;
}

// 辅助方法实现
bool CHTLParser::IsHTMLElementToken(const CHTLToken& token) {
    return token.Type == CHTLTokenType::IDENTIFIER && IsValidHTMLTag(token.Value);
}

bool CHTLParser::IsBlockStructureStart(const CHTLToken& token) {
    return token.Type >= CHTLTokenType::TEMPLATE_BLOCK && 
           token.Type <= CHTLTokenType::ORIGIN_TYPE_BLOCK;
}

bool CHTLParser::IsAtTypeToken(const CHTLToken& token) {
    return token.Type >= CHTLTokenType::AT_STYLE && 
           token.Type <= CHTLTokenType::AT_CONFIG;
}

bool CHTLParser::IsCEEquivalenceToken(const CHTLToken& token) {
    return token.Type == CHTLTokenType::COLON || token.Type == CHTLTokenType::EQUAL;
}

bool CHTLParser::IsValidHTMLTag(const std::string& tagName) {
    static const std::unordered_set<std::string> validTags = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer", "nav", "section",
        "article", "aside", "main", "figure", "figcaption", "button", "input", "form"
    };
    
    return validTags.find(tagName) != validTags.end();
}

bool CHTLParser::IsValidCSSProperty(const std::string& property) {
    static const std::unordered_set<std::string> validProperties = {
        "color", "background-color", "background", "font-size", "font-family", "font-weight",
        "width", "height", "margin", "padding", "border", "border-radius", "display",
        "position", "top", "left", "right", "bottom", "z-index", "opacity", "visibility",
        "overflow", "text-align", "line-height", "letter-spacing", "text-decoration",
        "transform", "transition", "animation", "box-shadow", "text-shadow", "cursor"
    };
    
    return validProperties.find(property) != validProperties.end() ||
           property.find("-webkit-") == 0 || property.find("-moz-") == 0 ||
           property.find("-ms-") == 0 || property.find("-o-") == 0 ||
           property.find("--") == 0;
}

bool CHTLParser::ValidateUTF8Content(const std::string& text) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(text);
        return true;
    }
    catch (const std::range_error&) {
        return false;
    }
}

void CHTLParser::ProcessSelectorAutomation(ElementNode* element, StyleNode* styleNode) {
    if (!element || !styleNode) {
        return;
    }
    
    // 获取配置
    auto config = m_Lexer->GetGlobalMap()->GetActiveConfiguration();
    bool disableClassAuto = false;
    bool disableIdAuto = false;
    
    if (config) {
        disableClassAuto = config->Settings["DISABLE_STYLE_AUTO_ADD_CLASS"] == "true";
        disableIdAuto = config->Settings["DISABLE_STYLE_AUTO_ADD_ID"] == "true";
    }
    
    // 应用自动化选择器规则
    element->ApplySelectorAutomation(disableClassAuto, disableIdAuto);
    
    // 处理自动生成的选择器
    const auto& autoSelectors = styleNode->GetAutoGeneratedSelectors();
    for (const auto& selector : autoSelectors) {
        if (selector.front() == '.' && !element->HasAttribute("class")) {
            // 自动添加第一个类选择器
            std::string className = selector.substr(1);
            element->SetAttribute("class", className);
            break;
        }
        else if (selector.front() == '#' && !element->HasAttribute("id")) {
            // 自动添加第一个ID选择器
            std::string idName = selector.substr(1);
            element->SetAttribute("id", idName);
            break;
        }
    }
}

void CHTLParser::SetParseError(const std::string& message, const CHTLToken* token) {
    m_HasError = true;
    
    if (token) {
        m_ErrorMessage = "Parse error at " + FormatTokenPosition(*token) + ": " + message;
    }
    else {
        m_ErrorMessage = "Parse error: " + message;
    }
}

void CHTLParser::AddParseWarning(const std::string& message, const CHTLToken* token) {
    std::string warning;
    
    if (token) {
        warning = "Parse warning at " + FormatTokenPosition(*token) + ": " + message;
    }
    else {
        warning = "Parse warning: " + message;
    }
    
    m_Warnings.push_back(warning);
}

std::string CHTLParser::FormatTokenPosition(const CHTLToken& token) {
    return "line " + std::to_string(token.Line) + ", column " + std::to_string(token.Column);
}

CHTLBaseNode* CHTLParser::GetRootNode() {
    return m_RootNode.get();
}

void CHTLParser::Reset() {
    m_CurrentTokenIndex = 0;
    m_RootNode.reset();
    
    while (!m_NodeStack.empty()) {
        m_NodeStack.pop();
    }
    
    while (!m_ScopeStack.empty()) {
        m_ScopeStack.pop();
    }
    
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Warnings.clear();
}

void CHTLParser::SetSourceCode(const std::string& sourceCode) {
    m_Lexer->SetSourceCode(sourceCode);
    Reset();
}

void CHTLParser::SetParseOptions(bool strictMode, bool validateConstraints, bool processInheritance) {
    m_StrictMode = strictMode;
    m_ValidateConstraints = validateConstraints;
    m_ProcessInheritance = processInheritance;
}

// 占位实现（需要后续完善）
std::unique_ptr<TemplateNode> CHTLParser::ParseElementTemplate(const std::string& templateName) {
    // 暂时返回基础实现
    return std::make_unique<TemplateNode>(TemplateType::ELEMENT_TEMPLATE, templateName);
}

std::unique_ptr<TemplateNode> CHTLParser::ParseVarTemplate(const std::string& templateName) {
    // 暂时返回基础实现
    return std::make_unique<TemplateNode>(TemplateType::VAR_TEMPLATE, templateName);
}

std::unique_ptr<CustomNode> CHTLParser::ParseCustomDefinition() {
    // 暂时返回基础实现
    return std::make_unique<CustomNode>(CustomType::STYLE_CUSTOM, "custom");
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTemplateUsage() {
    // 暂时返回基础实现
    return CHTLNodeFactory::CreateNode(CHTLNodeType::TEMPLATE_USAGE_NODE, "template_usage");
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseCustomUsage() {
    // 暂时返回基础实现
    return CHTLNodeFactory::CreateNode(CHTLNodeType::CUSTOM_USAGE_NODE, "custom_usage");
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseOriginDefinition() {
    // 暂时返回基础实现
    return CHTLNodeFactory::CreateNode(CHTLNodeType::ORIGIN_HTML_NODE, "origin");
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseConfiguration() {
    // 暂时返回基础实现
    return CHTLNodeFactory::CreateNode(CHTLNodeType::CONFIGURATION_NODE, "configuration");
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseImportStatement() {
    // 消费 [Import] 关键字
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACKET, "Expected '[' for import")) {
        return nullptr;
    }
    
    if (!ConsumeToken(CHTLTokenType::IDENTIFIER, "Expected 'Import' keyword")) {
        return nullptr;
    }
    
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACKET, "Expected ']' after 'Import'")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    auto importNode = CHTLNodeFactory::CreateNode(CHTLNodeType::IMPORT_NODE, "import", "", 0, 0);
    
    // 解析导入类型 - 简化处理
    const CHTLToken& typeToken = CurrentToken();
    std::string importType;
    
    if (typeToken.Type == CHTLTokenType::AT_CHTL || 
        typeToken.Type == CHTLTokenType::AT_CJMOD ||
        typeToken.Type == CHTLTokenType::AT_HTML ||
        typeToken.Type == CHTLTokenType::AT_JAVASCRIPT ||
        typeToken.Type == CHTLTokenType::AT_STYLE) {
        importType = typeToken.Value;
        AdvanceToken();
    }
    else if (typeToken.Type == CHTLTokenType::LEFT_BRACKET && PeekToken().Value == "Origin") {
        // [Origin] 原始嵌入导入
        AdvanceToken();
        importType = "[Origin]";
        AdvanceToken();
        if (!ConsumeToken(CHTLTokenType::RIGHT_BRACKET, "Expected ']' after 'Origin'")) {
            return nullptr;
        }
        
        // 继续解析@类型
        SkipWhitespaceAndComments();
        const CHTLToken& nextToken = CurrentToken();
        if (nextToken.Type == CHTLTokenType::AT_CHTL || 
            nextToken.Type == CHTLTokenType::AT_CJMOD ||
            nextToken.Type == CHTLTokenType::AT_HTML ||
            nextToken.Type == CHTLTokenType::AT_JAVASCRIPT ||
            nextToken.Type == CHTLTokenType::AT_STYLE) {
            importType += " " + nextToken.Value;
            AdvanceToken();
        }
    }
    else {
        SetParseError("Expected import type (@Chtl, @CJmod, @Html, etc.)", &typeToken);
        return nullptr;
    }
    
    importNode->SetProperty("type", importType);
    
    SkipWhitespaceAndComments();
    
    // 解析 from 关键字
    if (!ConsumeToken(CHTLTokenType::IDENTIFIER, "Expected 'from' keyword")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    // 解析导入路径
    std::string importPath;
    if (CurrentToken().Type == CHTLTokenType::STRING_LITERAL) {
        importPath = ParseStringLiteral();
    }
    else if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
        importPath = ParseUnquotedLiteral();
    }
    else {
        SetParseError("Expected import path (string or identifier)");
        return nullptr;
    }
    
    importNode->SetProperty("path", importPath);
    
    SkipWhitespaceAndComments();
    
    // 解析可选的 as 关键字
    if (MatchToken(CHTLTokenType::IDENTIFIER) && CurrentToken().Value == "as") {
        AdvanceToken();
        SkipWhitespaceAndComments();
        
        if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
            std::string aliasName = CurrentToken().Value;
            importNode->SetProperty("alias", aliasName);
            AdvanceToken();
        } else {
            SetParseError("Expected alias name after 'as'");
            return nullptr;
        }
    }
    
    return importNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseNamespaceDefinition() {
    // 消费 [Namespace] 关键字
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACKET, "Expected '[' for namespace")) {
        return nullptr;
    }
    
    if (!ConsumeToken(CHTLTokenType::IDENTIFIER, "Expected 'Namespace' keyword")) {
        return nullptr;
    }
    
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACKET, "Expected ']' after 'Namespace'")) {
        return nullptr;
    }
    
    SkipWhitespaceAndComments();
    
    if (!ConsumeToken(CHTLTokenType::LEFT_BRACE, "Expected '{' after namespace declaration")) {
        return nullptr;
    }
    
    auto namespaceNode = CHTLNodeFactory::CreateNode(CHTLNodeType::NAMESPACE_NODE, "namespace", "", 0, 0);
    
    // 解析命名空间名称
    SkipWhitespaceAndComments();
    if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
        std::string namespaceName = CurrentToken().Value;
        AdvanceToken();
        
        // 暂时简化，不支持嵌套命名空间 (后续可扩展)
        // TODO: 支持嵌套命名空间 A::B::C 语法
        
        namespaceNode->SetProperty("name", namespaceName);
        
        // 消费分号
        SkipWhitespaceAndComments();
        if (MatchToken(CHTLTokenType::SEMICOLON)) {
            AdvanceToken();
        }
    }
    
    if (!ConsumeToken(CHTLTokenType::RIGHT_BRACE, "Expected '}' to close namespace block")) {
        return nullptr;
    }
    
    return namespaceNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseConstraintStatement() {
    // 消费 [Constraint] 或 except 关键字
    const CHTLToken& startToken = CurrentToken();
    
    std::string constraintType;
    if (startToken.Type == CHTLTokenType::LEFT_BRACKET) {
        // [Constraint] 语法
        AdvanceToken();
        if (CurrentToken().Value == "Constraint") {
            constraintType = "Constraint";
            AdvanceToken();
            if (!ConsumeToken(CHTLTokenType::RIGHT_BRACKET, "Expected ']' after 'Constraint'")) {
                return nullptr;
            }
        }
    }
    else if (startToken.Value == "except") {
        // except 语法
        constraintType = "except";
        AdvanceToken();
    }
    else {
        SetParseError("Expected constraint keyword", &startToken);
        return nullptr;
    }
    
    auto constraintNode = CHTLNodeFactory::CreateNode(CHTLNodeType::EXCEPT_NODE, constraintType, "", 
                                                     startToken.Line, startToken.Column);
    
    SkipWhitespaceAndComments();
    
    // 解析约束表达式
    if (ConsumeToken(CHTLTokenType::LEFT_PAREN, "Expected '(' after constraint keyword")) {
        std::string constraintExpression;
        
        // 解析约束表达式内容
        while (!IsAtEnd() && !MatchToken(CHTLTokenType::RIGHT_PAREN)) {
            constraintExpression += CurrentToken().Value;
            AdvanceToken();
        }
        
        if (ConsumeToken(CHTLTokenType::RIGHT_PAREN, "Expected ')' to close constraint expression")) {
            constraintNode->SetProperty("expression", constraintExpression);
        }
    }
    
    return constraintNode;
}

void CHTLParser::ParseInheritance(CHTLBaseNode* parentNode) {
    // 暂时实现
    AdvanceToken();
}

void CHTLParser::BuildCompleteAST() {
    // 构建完整的AST
}

void CHTLParser::ApplyPostProcessingRules() {
    // 应用后处理规则
}

void CHTLParser::OptimizeASTStructure() {
    // 优化AST结构
}

bool CHTLParser::ValidateASTIntegrity() {
    return m_RootNode != nullptr;
}

void CHTLParser::ProcessTemplateInheritance(TemplateNode* templateNode) {
    // 处理模板继承
}

void CHTLParser::ProcessCustomInheritance(CustomNode* customNode) {
    // 处理自定义继承
}

void CHTLParser::ProcessSpecializationOperations(CustomNode* customNode) {
    // 处理特例化操作
}

} // namespace CHTL