#include "CHTLToken.h"
#include <algorithm>
#include <regex>

namespace CHTL {

    // 静态成员初始化
    std::unordered_map<std::string, CHTLTokenType> CHTLTokenUtil::keywordMap;
    std::unordered_map<std::string, CHTLTokenType> CHTLTokenUtil::htmlElementMap;
    bool CHTLTokenUtil::initialized = false;

    void CHTLTokenUtil::InitializeMaps() {
        if (initialized) return;

        // 初始化关键字映射表
        // 严格按照CHTL语法文档定义
        
        // 基础关键字
        keywordMap["text"] = CHTLTokenType::TEXT;
        keywordMap["style"] = CHTLTokenType::STYLE;
        keywordMap["script"] = CHTLTokenType::SCRIPT;
        
        // 块关键字
        keywordMap["[Template]"] = CHTLTokenType::TEMPLATE;
        keywordMap["[Custom]"] = CHTLTokenType::CUSTOM;
        keywordMap["[Origin]"] = CHTLTokenType::ORIGIN;
        keywordMap["[Configuration]"] = CHTLTokenType::CONFIGURATION;
        keywordMap["[Namespace]"] = CHTLTokenType::NAMESPACE;
        keywordMap["[Import]"] = CHTLTokenType::IMPORT;
        keywordMap["[Info]"] = CHTLTokenType::INFO;
        keywordMap["[Export]"] = CHTLTokenType::EXPORT;
        keywordMap["[Name]"] = CHTLTokenType::NAME;
        keywordMap["[OriginType]"] = CHTLTokenType::ORIGIN_TYPE;
        
        // 类型关键字
        keywordMap["@Style"] = CHTLTokenType::AT_STYLE;
        keywordMap["@Element"] = CHTLTokenType::AT_ELEMENT;
        keywordMap["@Var"] = CHTLTokenType::AT_VAR;
        keywordMap["@Html"] = CHTLTokenType::AT_HTML;
        keywordMap["@JavaScript"] = CHTLTokenType::AT_JAVASCRIPT;
        keywordMap["@Chtl"] = CHTLTokenType::AT_CHTL;
        keywordMap["@CJmod"] = CHTLTokenType::AT_CJMOD;
        keywordMap["@Config"] = CHTLTokenType::AT_CONFIG;
        
        // 操作关键字
        keywordMap["inherit"] = CHTLTokenType::INHERIT;
        keywordMap["delete"] = CHTLTokenType::DELETE;
        keywordMap["insert"] = CHTLTokenType::INSERT;
        keywordMap["after"] = CHTLTokenType::AFTER;
        keywordMap["before"] = CHTLTokenType::BEFORE;
        keywordMap["replace"] = CHTLTokenType::REPLACE;
        keywordMap["at"] = CHTLTokenType::AT_TOP;  // 需要检查后续的"top"或"bottom"
        keywordMap["from"] = CHTLTokenType::FROM;
        keywordMap["as"] = CHTLTokenType::AS;
        keywordMap["except"] = CHTLTokenType::EXCEPT;
        keywordMap["use"] = CHTLTokenType::USE;
        keywordMap["html5"] = CHTLTokenType::HTML5;
        
        // 初始化HTML元素映射表
        // 常用HTML元素
        std::vector<std::string> htmlElements = {
            "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
            "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input",
            "button", "textarea", "select", "option", "nav", "header", "footer", "main",
            "section", "article", "aside", "figure", "figcaption", "video", "audio",
            "canvas", "svg", "iframe", "script", "style", "link", "meta", "title",
            "br", "hr", "strong", "em", "small", "mark", "del", "ins", "sub", "sup"
        };
        
        for (const auto& element : htmlElements) {
            htmlElementMap[element] = CHTLTokenType::HTML_ELEMENT;
        }

        initialized = true;
    }

    bool CHTLTokenUtil::IsKeyword(const std::string& text) {
        InitializeMaps();
        return keywordMap.find(text) != keywordMap.end();
    }

    bool CHTLTokenUtil::IsHTMLElement(const std::string& text) {
        InitializeMaps();
        return htmlElementMap.find(text) != htmlElementMap.end();
    }

    bool CHTLTokenUtil::IsOperator(const std::string& text) {
        // 检查是否为操作符
        return text == ":" || text == "=" || text == ";" || text == "," || 
               text == "." || text == "&" || text == "@" ||
               text == "{" || text == "}" || text == "[" || text == "]" ||
               text == "(" || text == ")";
    }

    bool CHTLTokenUtil::IsSelector(const std::string& text) {
        // 检查是否为CSS选择器
        if (text.empty()) return false;
        
        // 类选择器 .className
        if (text[0] == '.' && text.length() > 1) {
            return IsValidClassName(text.substr(1));
        }
        
        // ID选择器 #idName
        if (text[0] == '#' && text.length() > 1) {
            return IsValidIdName(text.substr(1));
        }
        
        return false;
    }

    CHTLTokenType CHTLTokenUtil::GetKeywordType(const std::string& text) {
        InitializeMaps();
        auto it = keywordMap.find(text);
        return (it != keywordMap.end()) ? it->second : CHTLTokenType::UNKNOWN;
    }

    CHTLTokenType CHTLTokenUtil::GetHTMLElementType(const std::string& text) {
        InitializeMaps();
        auto it = htmlElementMap.find(text);
        return (it != htmlElementMap.end()) ? it->second : CHTLTokenType::UNKNOWN;
    }

    std::string CHTLTokenUtil::TokenTypeToString(CHTLTokenType type) {
        switch (type) {
            case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
            case CHTLTokenType::STRING_LITERAL: return "STRING_LITERAL";
            case CHTLTokenType::SINGLE_QUOTE_STRING: return "SINGLE_QUOTE_STRING";
            case CHTLTokenType::UNQUOTED_STRING: return "UNQUOTED_STRING";
            case CHTLTokenType::NUMBER: return "NUMBER";
            case CHTLTokenType::LEFT_BRACE: return "LEFT_BRACE";
            case CHTLTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case CHTLTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case CHTLTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
            case CHTLTokenType::LEFT_PAREN: return "LEFT_PAREN";
            case CHTLTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case CHTLTokenType::SEMICOLON: return "SEMICOLON";
            case CHTLTokenType::COLON: return "COLON";
            case CHTLTokenType::EQUALS: return "EQUALS";
            case CHTLTokenType::COMMA: return "COMMA";
            case CHTLTokenType::DOT: return "DOT";
            case CHTLTokenType::AMPERSAND: return "AMPERSAND";
            case CHTLTokenType::AT: return "AT";
            case CHTLTokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
            case CHTLTokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
            case CHTLTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
            case CHTLTokenType::TEXT: return "TEXT";
            case CHTLTokenType::STYLE: return "STYLE";
            case CHTLTokenType::SCRIPT: return "SCRIPT";
            case CHTLTokenType::TEMPLATE: return "TEMPLATE";
            case CHTLTokenType::CUSTOM: return "CUSTOM";
            case CHTLTokenType::ORIGIN: return "ORIGIN";
            case CHTLTokenType::CONFIGURATION: return "CONFIGURATION";
            case CHTLTokenType::NAMESPACE: return "NAMESPACE";
            case CHTLTokenType::IMPORT: return "IMPORT";
            case CHTLTokenType::AT_STYLE: return "AT_STYLE";
            case CHTLTokenType::AT_ELEMENT: return "AT_ELEMENT";
            case CHTLTokenType::AT_VAR: return "AT_VAR";
            case CHTLTokenType::AT_HTML: return "AT_HTML";
            case CHTLTokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
            case CHTLTokenType::AT_CHTL: return "AT_CHTL";
            case CHTLTokenType::AT_CJMOD: return "AT_CJMOD";
            case CHTLTokenType::AT_CONFIG: return "AT_CONFIG";
            case CHTLTokenType::INHERIT: return "INHERIT";
            case CHTLTokenType::DELETE: return "DELETE";
            case CHTLTokenType::INSERT: return "INSERT";
            case CHTLTokenType::AFTER: return "AFTER";
            case CHTLTokenType::BEFORE: return "BEFORE";
            case CHTLTokenType::REPLACE: return "REPLACE";
            case CHTLTokenType::FROM: return "FROM";
            case CHTLTokenType::AS: return "AS";
            case CHTLTokenType::EXCEPT: return "EXCEPT";
            case CHTLTokenType::USE: return "USE";
            case CHTLTokenType::HTML5: return "HTML5";
            case CHTLTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
            case CHTLTokenType::ID_SELECTOR: return "ID_SELECTOR";
            case CHTLTokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
            case CHTLTokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
            case CHTLTokenType::HTML_ELEMENT: return "HTML_ELEMENT";
            case CHTLTokenType::WHITESPACE: return "WHITESPACE";
            case CHTLTokenType::NEWLINE: return "NEWLINE";
            case CHTLTokenType::TAB: return "TAB";
            case CHTLTokenType::EOF_TOKEN: return "EOF_TOKEN";
            case CHTLTokenType::UNKNOWN: return "UNKNOWN";
            default: return "UNDEFINED";
        }
    }

    bool CHTLTokenUtil::IsWhitespace(CHTLTokenType type) {
        return type == CHTLTokenType::WHITESPACE || 
               type == CHTLTokenType::NEWLINE || 
               type == CHTLTokenType::TAB;
    }

    bool CHTLTokenUtil::IsComment(CHTLTokenType type) {
        return type == CHTLTokenType::SINGLE_LINE_COMMENT ||
               type == CHTLTokenType::MULTI_LINE_COMMENT ||
               type == CHTLTokenType::GENERATOR_COMMENT;
    }

    bool CHTLTokenUtil::IsValidIdentifier(const std::string& text) {
        if (text.empty()) return false;
        
        // 标识符规则：字母或下划线开头，后跟字母、数字或下划线
        std::regex identifierRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
        return std::regex_match(text, identifierRegex);
    }

    bool CHTLTokenUtil::IsValidClassName(const std::string& text) {
        if (text.empty()) return false;
        
        // CSS类名规则：字母、数字、连字符、下划线，不能以数字开头
        std::regex classNameRegex("^[a-zA-Z_-][a-zA-Z0-9_-]*$");
        return std::regex_match(text, classNameRegex);
    }

    bool CHTLTokenUtil::IsValidIdName(const std::string& text) {
        if (text.empty()) return false;
        
        // CSS ID名规则：与类名相同
        return IsValidClassName(text);
    }

} // namespace CHTL