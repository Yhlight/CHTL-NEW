/**
 * CHTL解析器简单修正版本
 * 为Web生态注入新动力 - 直接有效的实现
 */

#include "CHTLParser.h"
#include <iostream>

namespace CHTL {

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseCHTLDocumentSimpleCorrect() {
    std::cout << "🔍 开始正确解析CHTL文档..." << std::endl;
    
    // 重置到开始
    m_CurrentTokenIndex = 0;
    SkipWhitespaceAndComments();
    
    // 查找html元素
    while (m_CurrentTokenIndex < m_Tokens.size()) {
        const auto& token = CurrentToken();
        std::cout << "   检查token: " << token.Value << " (类型: " << static_cast<int>(token.Type) << ")" << std::endl;
        
        if (token.Type == CHTLTokenType::IDENTIFIER && token.Value == "html") {
            std::cout << "   ✅ 找到html元素，开始解析..." << std::endl;
            return ParseHTMLElementSimpleCorrect();
        }
        
        AdvanceToken();
    }
    
    std::cout << "   ⚠️ 没有找到html元素，创建默认结构" << std::endl;
    auto htmlNode = std::make_unique<ElementNode>("html", 1, 1);
    return htmlNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseHTMLElementSimpleCorrect() {
    auto htmlNode = std::make_unique<ElementNode>("html", CurrentToken().Line, CurrentToken().Column);
    std::cout << "   🏗️ 创建html节点" << std::endl;
    
    AdvanceToken(); // 跳过'html'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        std::cout << "   📖 进入html内容解析" << std::endl;
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& token = CurrentToken();
            std::cout << "     检查html内容token: " << token.Value << std::endl;
            
            if (token.Type == CHTLTokenType::RIGHT_BRACE) {
                std::cout << "   📖 html内容解析完成" << std::endl;
                AdvanceToken();
                break;
            }
            else if (token.Type == CHTLTokenType::IDENTIFIER) {
                if (token.Value == "head") {
                    std::cout << "     🏗️ 解析head元素" << std::endl;
                    auto headNode = ParseHeadElementSimpleCorrect();
                    if (headNode) {
                        htmlNode->AddChild(std::move(headNode));
                        std::cout << "     ✅ head元素添加成功" << std::endl;
                    }
                }
                else if (token.Value == "body") {
                    std::cout << "     🏗️ 解析body元素" << std::endl;
                    auto bodyNode = ParseBodyElementSimpleCorrect();
                    if (bodyNode) {
                        htmlNode->AddChild(std::move(bodyNode));
                        std::cout << "     ✅ body元素添加成功" << std::endl;
                    }
                }
                else {
                    std::cout << "     🏗️ 解析其他元素: " << token.Value << std::endl;
                    auto element = ParseElementSimpleCorrect();
                    if (element) {
                        htmlNode->AddChild(std::move(element));
                    }
                }
            }
            else {
                AdvanceToken();
            }
        }
    }
    
    std::cout << "   ✅ html元素解析完成，子节点数量: " << htmlNode->GetChildren().size() << std::endl;
    return htmlNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseHeadElementSimpleCorrect() {
    auto headNode = std::make_unique<ElementNode>("head", CurrentToken().Line, CurrentToken().Column);
    
    AdvanceToken(); // 跳过'head'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& token = CurrentToken();
            
            if (token.Type == CHTLTokenType::RIGHT_BRACE) {
                AdvanceToken();
                break;
            }
            else if (token.Type == CHTLTokenType::IDENTIFIER) {
                if (token.Value == "title") {
                    auto titleNode = ParseTitleElementSimpleCorrect();
                    if (titleNode) {
                        headNode->AddChild(std::move(titleNode));
                    }
                }
                else {
                    auto element = ParseElementSimpleCorrect();
                    if (element) {
                        headNode->AddChild(std::move(element));
                    }
                }
            }
            else {
                AdvanceToken();
            }
        }
    }
    
    return headNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseBodyElementSimpleCorrect() {
    auto bodyNode = std::make_unique<ElementNode>("body", CurrentToken().Line, CurrentToken().Column);
    
    AdvanceToken(); // 跳过'body'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& token = CurrentToken();
            
            if (token.Type == CHTLTokenType::RIGHT_BRACE) {
                AdvanceToken();
                break;
            }
            else if (token.Type == CHTLTokenType::IDENTIFIER) {
                auto element = ParseElementSimpleCorrect();
                if (element) {
                    bodyNode->AddChild(std::move(element));
                }
            }
            else {
                AdvanceToken();
            }
        }
    }
    
    return bodyNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTitleElementSimpleCorrect() {
    auto titleNode = std::make_unique<ElementNode>("title", CurrentToken().Line, CurrentToken().Column);
    
    AdvanceToken(); // 跳过'title'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        while (m_CurrentTokenIndex < m_Tokens.size()) {
            SkipWhitespaceAndComments();
            if (m_CurrentTokenIndex >= m_Tokens.size()) break;
            
            const auto& token = CurrentToken();
            
            if (token.Type == CHTLTokenType::RIGHT_BRACE) {
                AdvanceToken();
                break;
            }
            else if (token.Type == CHTLTokenType::IDENTIFIER && token.Value == "text") {
                auto textNode = ParseTextElementSimpleCorrect();
                if (textNode) {
                    titleNode->AddChild(std::move(textNode));
                }
            }
            else {
                AdvanceToken();
            }
        }
    }
    
    return titleNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseElementSimpleCorrect() {
    const auto& token = CurrentToken();
    auto elementNode = std::make_unique<ElementNode>(token.Value, token.Line, token.Column);
    
    AdvanceToken(); // 跳过元素名
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
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
                    auto textNode = ParseTextElementSimpleCorrect();
                    if (textNode) {
                        elementNode->AddChild(std::move(textNode));
                    }
                }
                else if (contentToken.Value == "style") {
                    auto styleNode = ParseStyleElementSimpleCorrect();
                    if (styleNode) {
                        elementNode->AddStyleNode(std::move(styleNode));
                    }
                }
                else if (contentToken.Value == "script") {
                    auto scriptNode = ParseScriptElementSimpleCorrect();
                    if (scriptNode) {
                        elementNode->AddScriptNode(std::move(scriptNode));
                    }
                }
                else if (contentToken.Value == "id") {
                    AdvanceToken(); // 跳过'id'
                    if (CurrentToken().Type == CHTLTokenType::COLON) {
                        AdvanceToken(); // 跳过':'
                        if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
                            elementNode->SetAttribute("id", CurrentToken().Value);
                            AdvanceToken();
                        }
                    }
                }
                else if (contentToken.Value == "class") {
                    AdvanceToken(); // 跳过'class'
                    if (CurrentToken().Type == CHTLTokenType::COLON) {
                        AdvanceToken(); // 跳过':'
                        if (CurrentToken().Type == CHTLTokenType::IDENTIFIER) {
                            elementNode->SetAttribute("class", CurrentToken().Value);
                            AdvanceToken();
                        }
                    }
                }
                else {
                    // 子元素
                    auto childElement = ParseElementSimpleCorrect();
                    if (childElement) {
                        elementNode->AddChild(std::move(childElement));
                    }
                }
            }
            else {
                AdvanceToken();
            }
        }
    }
    
    return elementNode;
}

std::unique_ptr<CHTLBaseNode> CHTLParser::ParseTextElementSimpleCorrect() {
    AdvanceToken(); // 跳过'text'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        std::ostringstream textContent;
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            textContent << CurrentToken().Value;
            AdvanceToken();
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
        }
        
        std::string content = textContent.str();
        content = std::regex_replace(content, std::regex(R"(\s+)"), " ");
        content = std::regex_replace(content, std::regex(R"(^\s+|\s+$)"), "");
        
        return std::make_unique<TextNode>(content, CurrentToken().Line, CurrentToken().Column);
    }
    
    return nullptr;
}

std::unique_ptr<StyleNode> CHTLParser::ParseStyleElementSimpleCorrect() {
    auto styleNode = std::make_unique<StyleNode>(CurrentToken().Line, CurrentToken().Column);
    
    AdvanceToken(); // 跳过'style'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        std::ostringstream cssContent;
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            cssContent << CurrentToken().Value;
            AdvanceToken();
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
        }
        
        styleNode->SetContent(cssContent.str());
    }
    
    return styleNode;
}

std::unique_ptr<ScriptNode> CHTLParser::ParseScriptElementSimpleCorrect() {
    auto scriptNode = std::make_unique<ScriptNode>("", CurrentToken().Line, CurrentToken().Column);
    
    AdvanceToken(); // 跳过'script'
    SkipWhitespaceAndComments();
    
    if (CurrentToken().Type == CHTLTokenType::LEFT_BRACE) {
        AdvanceToken(); // 跳过'{'
        
        std::ostringstream jsContent;
        while (m_CurrentTokenIndex < m_Tokens.size() && CurrentToken().Type != CHTLTokenType::RIGHT_BRACE) {
            jsContent << CurrentToken().Value;
            AdvanceToken();
        }
        
        if (CurrentToken().Type == CHTLTokenType::RIGHT_BRACE) {
            AdvanceToken();
        }
        
        scriptNode->SetContent(jsContent.str());
    }
    
    return scriptNode;
}

} // namespace CHTL