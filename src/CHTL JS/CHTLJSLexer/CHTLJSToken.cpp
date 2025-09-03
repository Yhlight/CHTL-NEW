#include "CHTLJSToken.h"
#include <algorithm>
#include <regex>

namespace CHTLJS {

    // 静态成员初始化
    std::unordered_map<std::string, CHTLJSTokenType> CHTLJSTokenUtil::keywordMap;
    std::unordered_map<std::string, CHTLJSTokenType> CHTLJSTokenUtil::eventMap;
    bool CHTLJSTokenUtil::initialized = false;

    void CHTLJSTokenUtil::InitializeMaps() {
        if (initialized) return;

        // 初始化CHTL JS关键字映射表
        // 严格按照CHTL语法文档中的CHTL JS部分定义
        
        // 模块相关关键字
        keywordMap["module"] = CHTLJSTokenType::MODULE;
        keywordMap["load"] = CHTLJSTokenType::LOAD;
        
        // 事件监听相关关键字
        keywordMap["listen"] = CHTLJSTokenType::LISTEN;
        keywordMap["delegate"] = CHTLJSTokenType::DELEGATE;
        
        // 动画相关关键字
        keywordMap["animate"] = CHTLJSTokenType::ANIMATE;
        keywordMap["target"] = CHTLJSTokenType::TARGET;
        keywordMap["duration"] = CHTLJSTokenType::DURATION;
        keywordMap["easing"] = CHTLJSTokenType::EASING;
        keywordMap["begin"] = CHTLJSTokenType::BEGIN;
        keywordMap["when"] = CHTLJSTokenType::WHEN;
        keywordMap["end"] = CHTLJSTokenType::END;
        keywordMap["at"] = CHTLJSTokenType::AT_KEYWORD;
        keywordMap["loop"] = CHTLJSTokenType::LOOP;
        keywordMap["direction"] = CHTLJSTokenType::DIRECTION;
        keywordMap["delay"] = CHTLJSTokenType::DELAY;
        keywordMap["callback"] = CHTLJSTokenType::CALLBACK;
        
        // 虚对象关键字
        keywordMap["vir"] = CHTLJSTokenType::VIR;
        
        // util...then表达式关键字
        keywordMap["util"] = CHTLJSTokenType::UTIL;
        keywordMap["change"] = CHTLJSTokenType::CHANGE;
        keywordMap["then"] = CHTLJSTokenType::THEN;
        
        // 初始化事件映射表
        eventMap["click"] = CHTLJSTokenType::CLICK;
        eventMap["mouseenter"] = CHTLJSTokenType::MOUSEENTER;
        eventMap["mouseleave"] = CHTLJSTokenType::MOUSELEAVE;
        eventMap["mousemove"] = CHTLJSTokenType::MOUSEMOVE;

        initialized = true;
    }

    bool CHTLJSTokenUtil::IsKeyword(const std::string& text) {
        InitializeMaps();
        return keywordMap.find(text) != keywordMap.end();
    }

    bool CHTLJSTokenUtil::IsEvent(const std::string& text) {
        InitializeMaps();
        return eventMap.find(text) != eventMap.end();
    }

    bool CHTLJSTokenUtil::IsSelector(const std::string& text) {
        // 检查是否为CHTL JS选择器语法 {{选择器}}
        if (text.length() < 4) return false;
        
        return text.substr(0, 2) == "{{" && text.substr(text.length() - 2) == "}}";
    }

    bool CHTLJSTokenUtil::IsCHTLJSOperator(const std::string& text) {
        // CHTL JS特有操作符
        return text == "->" || text == "&->" || text == "{{" || text == "}}" ||
               text == ":" || text == "=" || text == ";" || text == "," ||
               text == "{" || text == "}" || text == "[" || text == "]" ||
               text == "(" || text == ")";
    }

    CHTLJSTokenType CHTLJSTokenUtil::GetKeywordType(const std::string& text) {
        InitializeMaps();
        auto it = keywordMap.find(text);
        return (it != keywordMap.end()) ? it->second : CHTLJSTokenType::UNKNOWN;
    }

    CHTLJSTokenType CHTLJSTokenUtil::GetEventType(const std::string& text) {
        InitializeMaps();
        auto it = eventMap.find(text);
        return (it != eventMap.end()) ? it->second : CHTLJSTokenType::UNKNOWN;
    }

    std::string CHTLJSTokenUtil::TokenTypeToString(CHTLJSTokenType type) {
        switch (type) {
            case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
            case CHTLJSTokenType::STRING_LITERAL: return "STRING_LITERAL";
            case CHTLJSTokenType::NUMBER: return "NUMBER";
            case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
            case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
            case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
            case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
            case CHTLJSTokenType::COLON: return "COLON";
            case CHTLJSTokenType::EQUALS: return "EQUALS";
            case CHTLJSTokenType::COMMA: return "COMMA";
            case CHTLJSTokenType::DOT: return "DOT";
            case CHTLJSTokenType::ARROW: return "ARROW";
            case CHTLJSTokenType::AMPERSAND: return "AMPERSAND";
            case CHTLJSTokenType::AMPERSAND_ARROW: return "AMPERSAND_ARROW";
            case CHTLJSTokenType::DOUBLE_LEFT_BRACE: return "DOUBLE_LEFT_BRACE";
            case CHTLJSTokenType::DOUBLE_RIGHT_BRACE: return "DOUBLE_RIGHT_BRACE";
            case CHTLJSTokenType::MODULE: return "MODULE";
            case CHTLJSTokenType::LOAD: return "LOAD";
            case CHTLJSTokenType::LISTEN: return "LISTEN";
            case CHTLJSTokenType::DELEGATE: return "DELEGATE";
            case CHTLJSTokenType::ANIMATE: return "ANIMATE";
            case CHTLJSTokenType::VIR: return "VIR";
            case CHTLJSTokenType::UTIL: return "UTIL";
            case CHTLJSTokenType::CHANGE: return "CHANGE";
            case CHTLJSTokenType::THEN: return "THEN";
            case CHTLJSTokenType::TARGET: return "TARGET";
            case CHTLJSTokenType::DURATION: return "DURATION";
            case CHTLJSTokenType::EASING: return "EASING";
            case CHTLJSTokenType::BEGIN: return "BEGIN";
            case CHTLJSTokenType::WHEN: return "WHEN";
            case CHTLJSTokenType::END: return "END";
            case CHTLJSTokenType::AT_KEYWORD: return "AT_KEYWORD";
            case CHTLJSTokenType::LOOP: return "LOOP";
            case CHTLJSTokenType::DIRECTION: return "DIRECTION";
            case CHTLJSTokenType::DELAY: return "DELAY";
            case CHTLJSTokenType::CALLBACK: return "CALLBACK";
            case CHTLJSTokenType::CLICK: return "CLICK";
            case CHTLJSTokenType::MOUSEENTER: return "MOUSEENTER";
            case CHTLJSTokenType::MOUSELEAVE: return "MOUSELEAVE";
            case CHTLJSTokenType::MOUSEMOVE: return "MOUSEMOVE";
            case CHTLJSTokenType::CSS_SELECTOR: return "CSS_SELECTOR";
            case CHTLJSTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
            case CHTLJSTokenType::ID_SELECTOR: return "ID_SELECTOR";
            case CHTLJSTokenType::TAG_SELECTOR: return "TAG_SELECTOR";
            case CHTLJSTokenType::DESCENDANT_SELECTOR: return "DESCENDANT_SELECTOR";
            case CHTLJSTokenType::INDEX_SELECTOR: return "INDEX_SELECTOR";
            case CHTLJSTokenType::WHITESPACE: return "WHITESPACE";
            case CHTLJSTokenType::NEWLINE: return "NEWLINE";
            case CHTLJSTokenType::TAB: return "TAB";
            case CHTLJSTokenType::EOF_TOKEN: return "EOF_TOKEN";
            case CHTLJSTokenType::UNKNOWN: return "UNKNOWN";
            default: return "UNDEFINED";
        }
    }

    bool CHTLJSTokenUtil::IsWhitespace(CHTLJSTokenType type) {
        return type == CHTLJSTokenType::WHITESPACE || 
               type == CHTLJSTokenType::NEWLINE || 
               type == CHTLJSTokenType::TAB;
    }

    bool CHTLJSTokenUtil::IsComment(CHTLJSTokenType type) {
        return type == CHTLJSTokenType::SINGLE_LINE_COMMENT ||
               type == CHTLJSTokenType::MULTI_LINE_COMMENT ||
               type == CHTLJSTokenType::GENERATOR_COMMENT;
    }

    bool CHTLJSTokenUtil::IsValidSelectorSyntax(const std::string& text) {
        // 验证{{选择器}}语法
        if (text.length() < 4) return false;
        if (text.substr(0, 2) != "{{" || text.substr(text.length() - 2) != "}}") {
            return false;
        }
        
        // 提取选择器内容
        std::string selectorContent = text.substr(2, text.length() - 4);
        
        // 验证选择器内容是否有效
        if (selectorContent.empty()) return false;
        
        // 检查是否为有效的CSS选择器格式
        std::regex selectorRegex("^[.#]?[a-zA-Z][a-zA-Z0-9_-]*(?:\\[[0-9]+\\])?(?:\\s+[a-zA-Z][a-zA-Z0-9_-]*)*$");
        return std::regex_match(selectorContent, selectorRegex);
    }

    bool CHTLJSTokenUtil::IsValidArrowSyntax(const std::string& text) {
        // 验证->语法
        return text == "->";
    }

    bool CHTLJSTokenUtil::IsValidEventBinding(const std::string& text) {
        // 验证&->事件绑定语法
        return text == "&->";
    }

    bool CHTLJSTokenUtil::IsValidCHTLJSFunction(const std::string& text) {
        // 验证CHTL JS函数语法：functionName { key: value, ... }
        if (text.empty()) return false;
        
        // 基本模式匹配：标识符 + 空格 + {
        std::regex functionRegex("^[a-zA-Z_][a-zA-Z0-9_]*\\s*\\{[^}]*\\}$");
        return std::regex_match(text, functionRegex);
    }

    CHTLJSTokenType CHTLJSTokenUtil::ParseSelectorType(const std::string& selector) {
        if (selector.empty()) return CHTLJSTokenType::UNKNOWN;
        
        // 移除{{}}包装
        std::string content = selector;
        if (content.length() >= 4 && content.substr(0, 2) == "{{" && content.substr(content.length() - 2) == "}}") {
            content = content.substr(2, content.length() - 4);
        }
        
        if (content.empty()) return CHTLJSTokenType::UNKNOWN;
        
        // 类选择器
        if (content[0] == '.') {
            return CHTLJSTokenType::CLASS_SELECTOR;
        }
        
        // ID选择器
        if (content[0] == '#') {
            return CHTLJSTokenType::ID_SELECTOR;
        }
        
        // 检查是否包含索引 [index]
        if (content.find('[') != std::string::npos && content.find(']') != std::string::npos) {
            return CHTLJSTokenType::INDEX_SELECTOR;
        }
        
        // 检查是否为后代选择器（包含空格）
        if (content.find(' ') != std::string::npos) {
            return CHTLJSTokenType::DESCENDANT_SELECTOR;
        }
        
        // 标签选择器
        return CHTLJSTokenType::TAG_SELECTOR;
    }

} // namespace CHTLJS