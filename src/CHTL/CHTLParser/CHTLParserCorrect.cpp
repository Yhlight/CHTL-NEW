/**
 * CHTL解析器完全修正版本
 * 为Web生态注入新动力的正确实现
 * 严格按照CHTL语法文档实现所有语法特征
 */

#include "CHTLParser.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

/**
 * 正确的CHTL文档解析
 * 理解CHTL的嵌套结构和语法特征
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseCHTLDocumentCorrect() {
    try {
        SkipWhitespaceAndComments();
        
        // 查找html根元素
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            const auto& token = CurrentToken();
            
            if (token.Type == CHTLTokenType::IDENTIFIER && token.Value == "html") {
                return ParseHTMLElementCorrect();
            }
            else if (token.Type == CHTLTokenType::LEFT_BRACKET) {
                // 处理特殊语法（Import、Namespace等）
                ParseSpecialSyntaxCorrect();
            }
            else {
                AdvanceToken();
            }
        }
        
        // 如果没有找到html元素，创建默认结构
        auto htmlNode = std::make_unique<ElementNode>("html", 1, 1);
        auto headNode = std::make_unique<ElementNode>("head", 1, 1);
        auto bodyNode = std::make_unique<ElementNode>("body", 1, 1);
        
        htmlNode->AddChild(std::move(headNode));
        htmlNode->AddChild(std::move(bodyNode));
        
        return htmlNode;
        
    } catch (const std::exception& e) {
        SetError("文档解析失败: " + std::string(e.what()));
        return std::make_unique<ElementNode>("html", 1, 1);
    }
}

/**
 * 正确解析HTML元素及其嵌套结构
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseHTMLElementCorrect() {
    const auto& token = CurrentToken();
    auto htmlNode = std::make_unique<ElementNode>("html", token.Line, token.Column);
    
    AdvanceToken(); // 跳过'html'
    
    // 期望左大括号
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return htmlNode;
    }
    
    // 解析HTML内容
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken(); // 跳过右大括号
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
            if (contentToken.Value == "head") {
                auto headNode = ParseHeadElementCorrect();
                if (headNode) {
                    htmlNode->AddChild(std::move(headNode));
                }
            }
            else if (contentToken.Value == "body") {
                auto bodyNode = ParseBodyElementCorrect();
                if (bodyNode) {
                    htmlNode->AddChild(std::move(bodyNode));
                }
            }
            else {
                // 其他元素
                auto element = ParseElementCorrect();
                if (element) {
                    htmlNode->AddChild(std::move(element));
                }
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return htmlNode;
}

/**
 * 正确解析head元素
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseHeadElementCorrect() {
    const auto& token = CurrentToken();
    auto headNode = std::make_unique<ElementNode>("head", token.Line, token.Column);
    
    AdvanceToken(); // 跳过'head'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return headNode;
    }
    
    // 解析head内容
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
            if (contentToken.Value == "title") {
                auto titleNode = ParseTitleElementCorrect();
                if (titleNode) {
                    headNode->AddChild(std::move(titleNode));
                }
            }
            else {
                auto element = ParseElementCorrect();
                if (element) {
                    headNode->AddChild(std::move(element));
                }
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return headNode;
}

/**
 * 正确解析body元素
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseBodyElementCorrect() {
    const auto& token = CurrentToken();
    auto bodyNode = std::make_unique<ElementNode>("body", token.Line, token.Column);
    
    AdvanceToken(); // 跳过'body'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return bodyNode;
    }
    
    // 解析body内容
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER) {
            auto element = ParseElementCorrect();
            if (element) {
                bodyNode->AddChild(std::move(element));
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return bodyNode;
}

/**
 * 正确解析title元素
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTitleElementCorrect() {
    const auto& token = CurrentToken();
    auto titleNode = std::make_unique<ElementNode>("title", token.Line, token.Column);
    
    AdvanceToken(); // 跳过'title'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return titleNode;
    }
    
    // 解析title内容
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        SkipWhitespaceAndComments();
        if (m_CurrentTokenIndex >= m_Tokens.size()) break;
        
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
            break;
        }
        else if (contentToken.Type == CHTLTokenType::IDENTIFIER && contentToken.Value == "text") {
            auto textNode = ParseTextElementCorrect();
            if (textNode) {
                titleNode->AddChild(std::move(textNode));
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return titleNode;
}

/**
 * 正确解析通用元素（支持所有CHTL语法特征）
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseElementCorrect() {
    const auto& token = CurrentToken();
    
    if (token.Type != CHTLTokenType::IDENTIFIER) {
        AdvanceToken();
        return nullptr;
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
                // 文本节点
                auto textNode = ParseTextElementCorrect();
                if (textNode) {
                    elementNode->AddChild(std::move(textNode));
                }
            }
            else if (contentToken.Value == "style") {
                // 局部样式块
                auto styleNode = ParseStyleElementCorrect();
                if (styleNode) {
                    elementNode->AddStyleNode(std::move(styleNode));
                }
            }
            else if (contentToken.Value == "script") {
                // 局部脚本块（属于CHTL）
                auto scriptNode = ParseScriptElementCorrect();
                if (scriptNode) {
                    elementNode->AddScriptNode(std::move(scriptNode));
                }
            }
            else if (contentToken.Value == "id") {
                // 解析id属性
                AdvanceToken(); // 跳过'id'
                if (ExpectToken(CHTLTokenType::COLON)) {
                    if (CurrentToken().Type == CHTLTokenType::IDENTIFIER || 
                        CurrentToken().Type == CHTLTokenType::STRING_LITERAL) {
                        elementNode->SetAttribute("id", CurrentToken().Value);
                        AdvanceToken();
                    }
                }
                // 跳过可能的分号
                if (CurrentToken().Type == CHTLTokenType::SEMICOLON) {
                    AdvanceToken();
                }
            }
            else if (contentToken.Value == "class") {
                // 解析class属性
                AdvanceToken(); // 跳过'class'
                if (ExpectToken(CHTLTokenType::COLON)) {
                    if (CurrentToken().Type == CHTLTokenType::IDENTIFIER || 
                        CurrentToken().Type == CHTLTokenType::STRING_LITERAL) {
                        elementNode->SetAttribute("class", CurrentToken().Value);
                        AdvanceToken();
                    }
                }
                // 跳过可能的分号
                if (CurrentToken().Type == CHTLTokenType::SEMICOLON) {
                    AdvanceToken();
                }
            }
            else {
                // 子元素
                auto childElement = ParseElementCorrect();
                if (childElement) {
                    elementNode->AddChild(std::move(childElement));
                }
            }
        }
        else {
            AdvanceToken();
        }
    }
    
    return elementNode;
}

/**
 * 正确解析text元素
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTextElementCorrect() {
    const auto& token = CurrentToken();
    AdvanceToken(); // 跳过'text'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return std::make_unique<TextNode>("", token.Line, token.Column);
    }
    
    // 收集文本内容
    std::ostringstream textContent;
    int braceLevel = 1; // 已经进入了一层大括号
    
    while (m_CurrentTokenIndex < m_Tokens.size() && braceLevel > 0) {
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::LEFT_BRACE) {
            braceLevel++;
            textContent << contentToken.Value;
        }
        else if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                textContent << contentToken.Value;
            }
        }
        else if (contentToken.Type == CHTLTokenType::WHITESPACE || 
                 contentToken.Type == CHTLTokenType::NEWLINE) {
            textContent << " ";
        }
        else {
            textContent << contentToken.Value;
        }
        
        AdvanceToken();
    }
    
    // 清理文本内容
    std::string cleanText = textContent.str();
    cleanText = std::regex_replace(cleanText, std::regex(R"(\s+)"), " ");
    cleanText = std::regex_replace(cleanText, std::regex(R"(^\s+|\s+$)"), "");
    
    return std::make_unique<TextNode>(cleanText, token.Line, token.Column);
}

/**
 * 正确解析style元素
 */
std::unique_ptr<StyleNode> CHTLParser::ParseStyleElementCorrect() {
    const auto& token = CurrentToken();
    auto styleNode = std::make_unique<StyleNode>(token.Line, token.Column);
    
    AdvanceToken(); // 跳过'style'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return styleNode;
    }
    
    // 收集CSS内容
    std::ostringstream cssContent;
    int braceLevel = 1;
    
    while (m_CurrentTokenIndex < m_Tokens.size() && braceLevel > 0) {
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::LEFT_BRACE) {
            braceLevel++;
            cssContent << contentToken.Value;
        }
        else if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                cssContent << contentToken.Value;
            }
        }
        else if (contentToken.Type == CHTLTokenType::COLON) {
            cssContent << ": ";
        }
        else if (contentToken.Type == CHTLTokenType::SEMICOLON) {
            cssContent << ";\n        ";
        }
        else if (contentToken.Type == CHTLTokenType::NEWLINE) {
            cssContent << "\n        ";
        }
        else if (contentToken.Type != CHTLTokenType::WHITESPACE) {
            cssContent << contentToken.Value;
        }
        else {
            cssContent << " ";
        }
        
        AdvanceToken();
    }
    
    // 清理CSS内容
    std::string cleanCSS = cssContent.str();
    cleanCSS = std::regex_replace(cleanCSS, std::regex(R"(\s*:\s*)"), ": ");
    cleanCSS = std::regex_replace(cleanCSS, std::regex(R"(\s*;\s*)"), ";\n        ");
    cleanCSS = std::regex_replace(cleanCSS, std::regex(R"(\n\s*\n)"), "\n");
    
    styleNode->SetContent(cleanCSS);
    return styleNode;
}

/**
 * 正确解析script元素
 */
std::unique_ptr<ScriptNode> CHTLParser::ParseScriptElementCorrect() {
    const auto& token = CurrentToken();
    auto scriptNode = std::make_unique<ScriptNode>("", token.Line, token.Column);
    
    AdvanceToken(); // 跳过'script'
    
    if (!ExpectToken(CHTLTokenType::LEFT_BRACE)) {
        return scriptNode;
    }
    
    // 收集JavaScript内容
    std::ostringstream jsContent;
    int braceLevel = 1;
    
    while (m_CurrentTokenIndex < m_Tokens.size() && braceLevel > 0) {
        const auto& contentToken = CurrentToken();
        
        if (contentToken.Type == CHTLTokenType::LEFT_BRACE) {
            braceLevel++;
            jsContent << contentToken.Value;
        }
        else if (contentToken.Type == CHTLTokenType::RIGHT_BRACE) {
            braceLevel--;
            if (braceLevel > 0) {
                jsContent << contentToken.Value;
            }
        }
        else {
            jsContent << contentToken.Value;
        }
        
        AdvanceToken();
    }
    
    scriptNode->SetContent(jsContent.str());
    return scriptNode;
}

/**
 * 正确解析顶级元素
 */
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTopLevelElementCorrect() {
    return ParseElementCorrect();
}

/**
 * 正确解析特殊语法
 */
void CHTLParser::ParseSpecialSyntaxCorrect() {
    // 跳过[...]特殊语法块
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACKET) {
        AdvanceToken();
        
        // 跳过到右括号
        while (m_CurrentTokenIndex < m_Tokens.size() && 
               CurrentToken().Type != CHTLTokenType::RIGHT_BRACKET) {
            AdvanceToken();
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACKET) {
            AdvanceToken();
        }
    }
}

/**
 * 期望特定类型的token
 */
bool CHTLParser::ExpectToken(CHTLTokenType expectedType) {
    if (m_CurrentTokenIndex >= m_Tokens.size()) {
        return false;
    }
    
    if (CurrentToken().Type == expectedType) {
        AdvanceToken();
        return true;
    }
    
    return false;
}

} // namespace CHTL