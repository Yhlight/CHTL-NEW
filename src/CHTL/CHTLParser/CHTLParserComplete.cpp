/**
 * CHTL解析器完整实现
 * 支持所有CHTL语法文档中的特征
 * 为Web生态注入新动力的完整编译器
 */

#include "CHTLParser.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/OriginNode.h"
#include <iostream>
#include <regex>

namespace CHTL {

/**
 * 完整的CHTL文档解析
 * 支持所有语法特征：模板、自定义、命名空间、原始嵌入、约束等
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseCHTLDocumentComplete() {
    std::cout << "🔥 开始完整CHTL解析，支持所有语法特征..." << std::endl;
    
    try {
        // 重置解析状态
        m_CurrentTokenIndex = 0;
        SkipWhitespaceAndComments();
        
        // 创建文档根节点
        auto documentRoot = std::make_unique<ElementNode>("document", 1, 1);
        
        // 解析文档内容
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& token = CurrentToken();
            
            if (token.Type == CHTLTokenType::LEFT_BRACKET) {
                // 解析特殊语法块
                auto specialNode = ParseSpecialSyntaxComplete();
                if (specialNode) {
                    documentRoot->AddChild(std::move(specialNode));
                }
            }
            else if (token.Type == CHTLTokenType::IDENTIFIER) {
                if (token.Value == "use") {
                    // 解析use语句
                    ParseUseStatementComplete();
                }
                else if (token.Value == "html") {
                    // 找到HTML根元素
                    auto htmlNode = ParseHTMLElementComplete();
                    if (htmlNode) {
                        return htmlNode; // HTML元素作为真正的根节点
                    }
                }
                else {
                    // 其他顶级元素
                    auto element = ParseElementComplete();
                    if (element) {
                        documentRoot->AddChild(std::move(element));
                    }
                }
            }
            else {
                AdvanceToken();
            }
        }
        
        // 如果没有找到HTML元素，返回文档根节点
        std::cout << "   ✅ 完整解析完成，根节点子数量: " << documentRoot->GetChildren().size() << std::endl;
        return documentRoot;
        
    } catch (const std::exception& e) {
        std::cout << "   ❌ 解析异常: " << e.what() << std::endl;
        m_HasError = true;
        m_ErrorMessage = "完整解析失败: " + std::string(e.what());
        return std::make_unique<ElementNode>("html", 1, 1);
    }
}

/**
 * 解析特殊语法块（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseSpecialSyntaxComplete() {
    if (CurrentToken().Type != CHTLTokenType::LEFT_BRACKET) {
        return nullptr;
    }
    
    AdvanceToken(); // 跳过'['
    
    if (m_CurrentTokenIndex >= m_Tokens.size()) {
        return nullptr;
    }
    
    const auto& keywordToken = CurrentToken();
    
    if (keywordToken.Type == CHTLTokenType::IDENTIFIER) {
        if (keywordToken.Value == "Template") {
            return ParseTemplateDefinitionComplete();
        }
        else if (keywordToken.Value == "Custom") {
            return ParseCustomDefinitionComplete();
        }
        else if (keywordToken.Value == "Namespace") {
            return ParseNamespaceDefinitionComplete();
        }
        else if (keywordToken.Value == "Origin") {
            return ParseOriginDefinitionComplete();
        }
        else if (keywordToken.Value == "Import") {
            return ParseImportStatementComplete();
        }
        else if (keywordToken.Value == "Configuration") {
            return ParseConfigurationBlockComplete();
        }
        else {
            // 跳过未知的特殊语法
            SkipToClosingBracket();
        }
    }
    
    return nullptr;
}

/**
 * 解析模板定义（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTemplateDefinitionComplete() {
    AdvanceToken(); // 跳过'Template'
    
    if (!ExpectToken(CHTLTokenType::RIGHT_BRACKET)) {
        return nullptr;
    }
    
    // 解析模板类型和名称
    if (CurrentToken().Type != CHTLTokenType::AT_SYMBOL) {
        return nullptr;
    }
    
    AdvanceToken(); // 跳过'@'
    
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string templateType = CurrentToken().Value;
    AdvanceToken();
    
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string templateName = CurrentToken().Value;
    AdvanceToken();
    
    // 确定模板类型
    TemplateType type;
    if (templateType == "Style") {
        type = TemplateType::STYLE_TEMPLATE;
    }
    else if (templateType == "Element") {
        type = TemplateType::ELEMENT_TEMPLATE;
    }
    else if (templateType == "Var") {
        type = TemplateType::VAR_TEMPLATE;
    }
    else {
        std::cout << "   ❌ 未知模板类型: " << templateType << std::endl;
        return nullptr;
    }
    
    auto templateNode = std::make_unique<TemplateNode>(templateName, type, 
                                                      CurrentToken().Line, CurrentToken().Column);
    
    std::cout << "   🏗️ 创建模板: " << templateName << " (类型: @" << templateType << ")" << std::endl;
    
    // 解析模板内容
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            if (type == TemplateType::STYLE_TEMPLATE || type == TemplateType::VAR_TEMPLATE) {
                // 解析属性定义
                ParseTemplatePropertiesComplete(templateNode.get());
            }
            else if (type == TemplateType::ELEMENT_TEMPLATE) {
                // 解析元素内容
                auto content = ParseElementComplete();
                if (content) {
                    templateNode->AddTemplateContent(std::move(content));
                }
            }
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken(); // 跳过'}'
        }
    }
    
    std::cout << "   ✅ 模板解析完成: " << templateName << std::endl;
    return templateNode;
}

/**
 * 解析自定义定义（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseCustomDefinitionComplete() {
    AdvanceToken(); // 跳过'Custom'
    
    if (!ExpectToken(CHTLTokenType::RIGHT_BRACKET)) {
        return nullptr;
    }
    
    // 解析自定义类型和名称
    if (CurrentToken().Type != CHTLTokenType::AT_SYMBOL) {
        return nullptr;
    }
    
    AdvanceToken(); // 跳过'@'
    
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string customType = CurrentToken().Value;
    AdvanceToken();
    
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string customName = CurrentToken().Value;
    AdvanceToken();
    
    // 确定自定义类型
    CustomType type;
    if (customType == "Style") {
        type = CustomType::STYLE_CUSTOM;
    }
    else if (customType == "Element") {
        type = CustomType::ELEMENT_CUSTOM;
    }
    else if (customType == "Var") {
        type = CustomType::VAR_CUSTOM;
    }
    else {
        std::cout << "   ❌ 未知自定义类型: " << customType << std::endl;
        return nullptr;
    }
    
    auto customNode = std::make_unique<CustomNode>(customName, type, 
                                                  CurrentToken().Line, CurrentToken().Column);
    
    std::cout << "   🏗️ 创建自定义: " << customName << " (类型: @" << customType << ")" << std::endl;
    
    // 解析自定义内容
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& contentToken = CurrentToken();
            
            if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
                // 检查是否为特例化操作
                if (contentToken.Value == "delete" || contentToken.Value == "insert" ||
                    contentToken.Value == "modify" || contentToken.Value == "add") {
                    ParseSpecializationOperationComplete(customNode.get());
                }
                else if (contentToken.Value == "inherit") {
                    // 解析显式继承
                    ParseInheritanceComplete(customNode.get());
                }
                else if (contentToken.Value.starts_with("@")) {
                    // 解析继承引用
                    ParseInheritanceReferenceComplete(customNode.get());
                }
                else {
                    // 普通内容或属性
                    if (type == CustomType::STYLE_CUSTOM || type == CustomType::VAR_CUSTOM) {
                        ParseCustomPropertiesComplete(customNode.get());
                    }
                    else if (type == CustomType::ELEMENT_CUSTOM) {
                        auto content = ParseElementComplete();
                        if (content) {
                            customNode->AddCustomContent(std::move(content));
                        }
                    }
                }
            }
            else {
                AdvanceToken();
            }
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken(); // 跳过'}'
        }
    }
    
    std::cout << "   ✅ 自定义解析完成: " << customName << std::endl;
    return customNode;
}

/**
 * 解析命名空间定义（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseNamespaceDefinitionComplete() {
    AdvanceToken(); // 跳过'Namespace'
    
    if (!ExpectToken(CHTLTokenType::RIGHT_BRACKET)) {
        return nullptr;
    }
    
    // 解析命名空间名称
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string namespaceName = CurrentToken().Value;
    AdvanceToken();
    
    auto namespaceNode = std::make_unique<NamespaceNode>(namespaceName, 
                                                        CurrentToken().Line, CurrentToken().Column);
    
    std::cout << "   🏗️ 创建命名空间: " << namespaceName << std::endl;
    
    // 解析命名空间内容（可选大括号）
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            // 解析命名空间内容
            auto content = ParseNamespaceContentComplete(namespaceNode.get());
            if (content) {
                namespaceNode->AddNamespaceContent(std::move(content));
            }
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken(); // 跳过'}'
        }
    }
    
    std::cout << "   ✅ 命名空间解析完成: " << namespaceName << std::endl;
    return namespaceNode;
}

/**
 * 解析原始嵌入定义（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseOriginDefinitionComplete() {
    AdvanceToken(); // 跳过'Origin'
    
    if (!ExpectToken(CHTLTokenType::RIGHT_BRACKET)) {
        return nullptr;
    }
    
    // 解析原始嵌入类型
    if (CurrentToken().Type != CHTLTokenType::AT_SYMBOL) {
        return nullptr;
    }
    
    AdvanceToken(); // 跳过'@'
    
    if (CurrentToken().Type != CHTLTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string originType = CurrentToken().Value;
    AdvanceToken();
    
    // 检查是否为带名原始嵌入
    std::string originName = "";
    if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
        originName = CurrentToken().Value;
        AdvanceToken();
    }
    
    // 确定原始嵌入类型
    std::unique_ptr<OriginNode> originNode;
    
    if (originType == "Html") {
        originNode = std::make_unique<OriginNode>(OriginType::HTML_ORIGIN, originName, "", 
                                                 CurrentToken().Line, CurrentToken().Column);
    }
    else if (originType == "Style") {
        originNode = std::make_unique<OriginNode>(OriginType::STYLE_ORIGIN, originName, "",
                                                 CurrentToken().Line, CurrentToken().Column);
    }
    else if (originType == "JavaScript") {
        originNode = std::make_unique<OriginNode>(OriginType::JAVASCRIPT_ORIGIN, originName, "",
                                                 CurrentToken().Line, CurrentToken().Column);
    }
    else {
        // 自定义类型原始嵌入
        originNode = std::make_unique<OriginNode>(originType, "",
                                                 CurrentToken().Line, CurrentToken().Column);
    }
    
    std::cout << "   🏗️ 创建原始嵌入: @" << originType 
              << (originName.empty() ? "" : " " + originName) << std::endl;
    
    // 解析原始内容
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        std::ostringstream rawContent;
        int braceLevel = 1;
        
        while (m_CurrentTokenIndex < m_Tokens.size() && braceLevel > 0) {
            const auto& contentToken = CurrentToken();
            
            if (contentToken.Type == CHTLTokenType::LEFT_BRACE) {
                braceLevel++;
                rawContent << contentToken.Value;
            }
            else if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
                braceLevel--;
                if (braceLevel > 0) {
                    rawContent << contentToken.Value;
                }
            }
            else {
                rawContent << contentToken.Value;
            }
            
            AdvanceToken();
        }
        
        originNode->SetRawContent(rawContent.str());
    }
    
    std::cout << "   ✅ 原始嵌入解析完成，内容长度: " << originNode->GetRawContent().length() << std::endl;
    return originNode;
}

/**
 * 解析HTML元素（完整版）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseHTMLElementComplete() {
    const auto& token = CurrentToken();
    auto htmlNode = std::make_unique<ElementNode>("html", token.Line, token.Column);
    
    std::cout << "   🏗️ 解析HTML根元素..." << std::endl;
    AdvanceToken(); // 跳过'html'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return htmlNode;
    }
    
    // 解析HTML内容
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
            if (contentToken.Value == "head") {
                auto headNode = ParseHeadElementComplete();
                if (headNode) {
                    htmlNode->AddChild(std::move(headNode));
                }
            }
            else if (contentToken.Value == "body") {
                auto bodyNode = ParseBodyElementComplete();
                if (bodyNode) {
                    htmlNode->AddChild(std::move(bodyNode));
                }
            }
            else {
                auto element = ParseElementComplete();
                if (element) {
                    htmlNode->AddChild(std::move(element));
                }
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    std::cout << "   ✅ HTML元素解析完成，子节点数量: " << htmlNode->GetChildren().size() << std::endl;
    return htmlNode;
}

/**
 * 解析元素（完整版，支持所有特征）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseElementComplete() {
    const auto& token = CurrentToken();
    
    if (token.Type != CHTLTokenType::IDENTIFIER) {
        AdvanceToken();
        return nullptr;
    }
    
    // 检查是否为模板或自定义引用
    if (token.Value.starts_with("@")) {
        return ParseTemplateOrCustomReferenceComplete();
    }
    
    auto elementNode = std::make_unique<ElementNode>(token.Value, token.Line, token.Column);
    AdvanceToken(); // 跳过元素名
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return elementNode;
    }
    
    // 解析元素内容 - 支持所有CHTL语法特征
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
            if (contentToken.Value == "text") {
                auto textNode = ParseTextElementComplete();
                if (textNode) {
                    elementNode->AddChild(std::move(textNode));
                }
            }
            else if (contentToken.Value == "style") {
                auto styleNode = ParseStyleElementComplete();
                if (styleNode) {
                    elementNode->AddStyleNode(std::move(styleNode));
                }
            }
            else if (contentToken.Value == "script") {
                auto scriptNode = ParseScriptElementComplete();
                if (scriptNode) {
                    elementNode->AddScriptNode(std::move(scriptNode));
                }
            }
            else if (contentToken.Value == "id" || contentToken.Value == "class") {
                // 解析属性
                ParseElementAttributesComplete(elementNode.get());
            }
            else if (contentToken.Value == "except") {
                // 解析约束
                ParseElementConstraintsComplete(elementNode.get());
            }
            else {
                // 子元素
                auto childElement = ParseElementComplete();
                if (childElement) {
                    elementNode->AddChild(std::move(childElement));
                }
            }
        }
        else if (contentToken.Type == CHTLTokenType::LEFT_BRACKET) {
            // 特殊语法块
            auto specialNode = ParseSpecialSyntaxComplete();
            if (specialNode) {
                elementNode->AddChild(std::move(specialNode));
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return elementNode;
}

} // namespace CHTL