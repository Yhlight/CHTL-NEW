#include "CHTLJSToken.h"
#include <regex>
#include <algorithm>

namespace CHTLJS {

std::string CHTLJSToken::GetTypeName() const {
    switch (Type) {
        case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLJSTokenType::SINGLE_QUOTE_STRING: return "SINGLE_QUOTE_STRING";
        case CHTLJSTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLJSTokenType::NUMBER: return "NUMBER";
        case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLJSTokenType::COLON: return "COLON";
        case CHTLJSTokenType::COMMA: return "COMMA";
        case CHTLJSTokenType::DOT: return "DOT";
        case CHTLJSTokenType::EQUAL: return "EQUAL";
        case CHTLJSTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLJSTokenType::ARROW_OPERATOR: return "ARROW_OPERATOR";
        case CHTLJSTokenType::EVENT_BIND_OPERATOR: return "EVENT_BIND_OPERATOR";
        case CHTLJSTokenType::MODULE: return "MODULE";
        case CHTLJSTokenType::LOAD: return "LOAD";
        case CHTLJSTokenType::LISTEN: return "LISTEN";
        case CHTLJSTokenType::DELEGATE: return "DELEGATE";
        case CHTLJSTokenType::ANIMATE: return "ANIMATE";
        case CHTLJSTokenType::VIR: return "VIR";
        case CHTLJSTokenType::TARGET: return "TARGET";
        case CHTLJSTokenType::DURATION: return "DURATION";
        case CHTLJSTokenType::EASING: return "EASING";
        case CHTLJSTokenType::BEGIN: return "BEGIN";
        case CHTLJSTokenType::WHEN: return "WHEN";
        case CHTLJSTokenType::END: return "END";
        case CHTLJSTokenType::LOOP: return "LOOP";
        case CHTLJSTokenType::DIRECTION: return "DIRECTION";
        case CHTLJSTokenType::DELAY: return "DELAY";
        case CHTLJSTokenType::CALLBACK: return "CALLBACK";
        case CHTLJSTokenType::AT: return "AT";
        case CHTLJSTokenType::SELECTOR_START: return "SELECTOR_START";
        case CHTLJSTokenType::SELECTOR_END: return "SELECTOR_END";
        case CHTLJSTokenType::SELECTOR_CONTENT: return "SELECTOR_CONTENT";
        case CHTLJSTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLJSTokenType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLJSTokenType::TAG_SELECTOR: return "TAG_SELECTOR";
        case CHTLJSTokenType::DESCENDANT_SELECTOR: return "DESCENDANT_SELECTOR";
        case CHTLJSTokenType::INDEX_SELECTOR: return "INDEX_SELECTOR";
        case CHTLJSTokenType::EOF_TOKEN: return "EOF_TOKEN";
        case CHTLJSTokenType::NEWLINE: return "NEWLINE";
        case CHTLJSTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLJSTokenType::COMMENT: return "COMMENT";
        case CHTLJSTokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

bool CHTLJSToken::IsKeyword() const {
    return Type >= CHTLJSTokenType::MODULE && Type <= CHTLJSTokenType::AT;
}

bool CHTLJSToken::IsEnhancedSelector() const {
    return Type >= CHTLJSTokenType::SELECTOR_START && Type <= CHTLJSTokenType::INDEX_SELECTOR;
}

bool CHTLJSToken::IsCHTLJSFunction() const {
    return Type == CHTLJSTokenType::LISTEN || 
           Type == CHTLJSTokenType::DELEGATE || 
           Type == CHTLJSTokenType::ANIMATE ||
           Type == CHTLJSTokenType::MODULE;
}

bool CHTLJSToken::IsAnimationToken() const {
    return Type >= CHTLJSTokenType::TARGET && Type <= CHTLJSTokenType::AT;
}

CHTLJSTokenManager::CHTLJSTokenManager() {
    InitializeKeywordMap();
    InitializeCHTLJSFunctions();
}

void CHTLJSTokenManager::InitializeKeywordMap() {
    // CHTL JS核心关键字（完全独立于CHTL）
    m_KeywordMap["module"] = CHTLJSTokenType::MODULE;
    m_KeywordMap["load"] = CHTLJSTokenType::LOAD;
    m_KeywordMap["listen"] = CHTLJSTokenType::LISTEN;
    m_KeywordMap["delegate"] = CHTLJSTokenType::DELEGATE;
    m_KeywordMap["animate"] = CHTLJSTokenType::ANIMATE;
    m_KeywordMap["vir"] = CHTLJSTokenType::VIR;
    
    // 动画相关关键字
    m_KeywordMap["target"] = CHTLJSTokenType::TARGET;
    m_KeywordMap["duration"] = CHTLJSTokenType::DURATION;
    m_KeywordMap["easing"] = CHTLJSTokenType::EASING;
    m_KeywordMap["begin"] = CHTLJSTokenType::BEGIN;
    m_KeywordMap["when"] = CHTLJSTokenType::WHEN;
    m_KeywordMap["end"] = CHTLJSTokenType::END;
    m_KeywordMap["loop"] = CHTLJSTokenType::LOOP;
    m_KeywordMap["direction"] = CHTLJSTokenType::DIRECTION;
    m_KeywordMap["delay"] = CHTLJSTokenType::DELAY;
    m_KeywordMap["callback"] = CHTLJSTokenType::CALLBACK;
    m_KeywordMap["at"] = CHTLJSTokenType::AT;
}

void CHTLJSTokenManager::InitializeCHTLJSFunctions() {
    // 注册CHTL JS内置函数
    m_CHTLJSFunctions["listen"] = true;
    m_CHTLJSFunctions["delegate"] = true;
    m_CHTLJSFunctions["animate"] = true;
    m_CHTLJSFunctions["module"] = true;
    
    // 注意：printMylove、iNeverAway、util...then属于CJMOD扩展，不是核心语法
}

CHTLJSTokenType CHTLJSTokenManager::GetTokenType(const std::string& value) {
    // 首先检查是否为关键字
    auto keywordIt = m_KeywordMap.find(value);
    if (keywordIt != m_KeywordMap.end()) {
        return keywordIt->second;
    }
    
    // 检查特殊模式
    if (value.empty()) {
        return CHTLJSTokenType::UNKNOWN;
    }
    
    // 检查增强选择器模式
    if (value == "{{") {
        return CHTLJSTokenType::SELECTOR_START;
    }
    
    if (value == "}}") {
        return CHTLJSTokenType::SELECTOR_END;
    }
    
    // 检查操作符
    if (value == "->") {
        return CHTLJSTokenType::ARROW_OPERATOR;
    }
    
    if (value == "&->") {
        return CHTLJSTokenType::EVENT_BIND_OPERATOR;
    }
    
    // 检查选择器内容（在{{}}内部）
    if (value.front() == '.' && value.length() > 1) {
        return CHTLJSTokenType::CLASS_SELECTOR;
    }
    
    if (value.front() == '#' && value.length() > 1) {
        return CHTLJSTokenType::ID_SELECTOR;
    }
    
    // 检查索引选择器
    std::regex indexPattern(R"(\[\d+\])");
    if (std::regex_match(value, indexPattern)) {
        return CHTLJSTokenType::INDEX_SELECTOR;
    }
    
    // 检查数字
    std::regex numberPattern(R"(\d+(?:\.\d+)?)");
    if (std::regex_match(value, numberPattern)) {
        return CHTLJSTokenType::NUMBER;
    }
    
    // 检查字符串字面量
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\'')) {
        return (value.front() == '"') ? CHTLJSTokenType::STRING_LITERAL : CHTLJSTokenType::SINGLE_QUOTE_STRING;
    }
    
    // 检查是否为有效标识符
    std::regex identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    if (std::regex_match(value, identifierPattern)) {
        return CHTLJSTokenType::IDENTIFIER;
    }
    
    // 检查无修饰字面量
    if (std::regex_match(value, std::regex(R"([a-zA-Z0-9\-_.]+)"))) {
        return CHTLJSTokenType::UNQUOTED_LITERAL;
    }
    
    return CHTLJSTokenType::UNKNOWN;
}

bool CHTLJSTokenManager::IsKeyword(const std::string& value) {
    return m_KeywordMap.find(value) != m_KeywordMap.end();
}

bool CHTLJSTokenManager::IsCHTLJSFunction(const std::string& value) {
    return m_CHTLJSFunctions.find(value) != m_CHTLJSFunctions.end();
}

void CHTLJSTokenManager::RegisterCHTLJSFunction(const std::string& functionName) {
    m_CHTLJSFunctions[functionName] = true;
}

bool CHTLJSTokenManager::IsValidEnhancedSelector(const std::string& selector) {
    // 验证增强选择器格式：{{...}}
    if (selector.length() < 4 || 
        selector.substr(0, 2) != "{{" || 
        selector.substr(selector.length() - 2) != "}}") {
        return false;
    }
    
    std::string content = selector.substr(2, selector.length() - 4);
    return !content.empty();
}

CHTLJSTokenType CHTLJSTokenManager::ParseSelectorContent(const std::string& content) {
    // 解析{{}}内的选择器内容
    
    // 检查类选择器
    if (content.front() == '.') {
        return CHTLJSTokenType::CLASS_SELECTOR;
    }
    
    // 检查ID选择器
    if (content.front() == '#') {
        return CHTLJSTokenType::ID_SELECTOR;
    }
    
    // 检查是否包含后代选择器（空格分隔）
    if (content.find(' ') != std::string::npos) {
        return CHTLJSTokenType::DESCENDANT_SELECTOR;
    }
    
    // 检查是否包含索引访问
    std::regex indexPattern(R"(\w+\[\d+\])");
    if (std::regex_search(content, indexPattern)) {
        return CHTLJSTokenType::INDEX_SELECTOR;
    }
    
    // 默认为标签选择器
    return CHTLJSTokenType::TAG_SELECTOR;
}

} // namespace CHTLJS