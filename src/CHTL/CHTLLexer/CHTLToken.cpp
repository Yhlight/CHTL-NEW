#include "CHTLToken.h"
#include <regex>
#include <algorithm>

namespace CHTL {

std::string CHTLToken::GetTypeName() const {
    switch (Type) {
        case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLTokenType::SINGLE_QUOTE_STRING: return "SINGLE_QUOTE_STRING";
        case CHTLTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLTokenType::NUMBER: return "NUMBER";
        case CHTLTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLTokenType::COLON: return "COLON";
        case CHTLTokenType::COMMA: return "COMMA";
        case CHTLTokenType::DOT: return "DOT";
        case CHTLTokenType::EQUAL: return "EQUAL";
        case CHTLTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLTokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case CHTLTokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case CHTLTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLTokenType::TEXT: return "TEXT";
        case CHTLTokenType::STYLE: return "STYLE";
        case CHTLTokenType::SCRIPT: return "SCRIPT";
        case CHTLTokenType::TEMPLATE_BLOCK: return "TEMPLATE_BLOCK";
        case CHTLTokenType::CUSTOM_BLOCK: return "CUSTOM_BLOCK";
        case CHTLTokenType::ORIGIN_BLOCK: return "ORIGIN_BLOCK";
        case CHTLTokenType::CONFIGURATION_BLOCK: return "CONFIGURATION_BLOCK";
        case CHTLTokenType::IMPORT_BLOCK: return "IMPORT_BLOCK";
        case CHTLTokenType::NAMESPACE_BLOCK: return "NAMESPACE_BLOCK";
        case CHTLTokenType::INFO_BLOCK: return "INFO_BLOCK";
        case CHTLTokenType::EXPORT_BLOCK: return "EXPORT_BLOCK";
        case CHTLTokenType::NAME_BLOCK: return "NAME_BLOCK";
        case CHTLTokenType::ORIGIN_TYPE_BLOCK: return "ORIGIN_TYPE_BLOCK";
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
        case CHTLTokenType::AT_TOP: return "AT_TOP";
        case CHTLTokenType::AT_BOTTOM: return "AT_BOTTOM";
        case CHTLTokenType::EXCEPT: return "EXCEPT";
        case CHTLTokenType::FROM: return "FROM";
        case CHTLTokenType::AS: return "AS";
        case CHTLTokenType::USE: return "USE";
        case CHTLTokenType::HTML5: return "HTML5";
        case CHTLTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLTokenType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLTokenType::CONTEXT_REFERENCE: return "CONTEXT_REFERENCE";
        case CHTLTokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case CHTLTokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case CHTLTokenType::INDEX_ACCESS: return "INDEX_ACCESS";
        case CHTLTokenType::EOF_TOKEN: return "EOF_TOKEN";
        case CHTLTokenType::NEWLINE: return "NEWLINE";
        case CHTLTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLTokenType::CUSTOM_AT_TYPE: return "CUSTOM_AT_TYPE";
        case CHTLTokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

bool CHTLToken::IsKeyword() const {
    return Type >= CHTLTokenType::TEXT && Type <= CHTLTokenType::HTML5;
}

bool CHTLToken::IsBlockStructure() const {
    return Type >= CHTLTokenType::TEMPLATE_BLOCK && Type <= CHTLTokenType::ORIGIN_TYPE_BLOCK;
}

bool CHTLToken::IsAtType() const {
    return Type >= CHTLTokenType::AT_STYLE && Type <= CHTLTokenType::AT_CONFIG;
}

bool CHTLToken::IsSelector() const {
    return Type >= CHTLTokenType::CLASS_SELECTOR && Type <= CHTLTokenType::PSEUDO_ELEMENT;
}

CHTLTokenManager::CHTLTokenManager() {
    InitializeKeywordMap();
}

void CHTLTokenManager::InitializeKeywordMap() {
    // 基础关键字
    m_KeywordMap["text"] = CHTLTokenType::TEXT;
    m_KeywordMap["style"] = CHTLTokenType::STYLE;
    m_KeywordMap["script"] = CHTLTokenType::SCRIPT;  // 局部script属于CHTL
    
    // 块结构关键字
    m_KeywordMap["[Template]"] = CHTLTokenType::TEMPLATE_BLOCK;
    m_KeywordMap["[Custom]"] = CHTLTokenType::CUSTOM_BLOCK;
    m_KeywordMap["[Origin]"] = CHTLTokenType::ORIGIN_BLOCK;
    m_KeywordMap["[Configuration]"] = CHTLTokenType::CONFIGURATION_BLOCK;
    m_KeywordMap["[Import]"] = CHTLTokenType::IMPORT_BLOCK;
    m_KeywordMap["[Namespace]"] = CHTLTokenType::NAMESPACE_BLOCK;
    m_KeywordMap["[Info]"] = CHTLTokenType::INFO_BLOCK;
    m_KeywordMap["[Export]"] = CHTLTokenType::EXPORT_BLOCK;
    m_KeywordMap["[Name]"] = CHTLTokenType::NAME_BLOCK;
    m_KeywordMap["[OriginType]"] = CHTLTokenType::ORIGIN_TYPE_BLOCK;
    
    // @类型标识符
    m_KeywordMap["@Style"] = CHTLTokenType::AT_STYLE;
    m_KeywordMap["@Element"] = CHTLTokenType::AT_ELEMENT;
    m_KeywordMap["@Var"] = CHTLTokenType::AT_VAR;
    m_KeywordMap["@Html"] = CHTLTokenType::AT_HTML;
    m_KeywordMap["@JavaScript"] = CHTLTokenType::AT_JAVASCRIPT;
    m_KeywordMap["@Chtl"] = CHTLTokenType::AT_CHTL;
    m_KeywordMap["@CJmod"] = CHTLTokenType::AT_CJMOD;
    m_KeywordMap["@Config"] = CHTLTokenType::AT_CONFIG;
    
    // 操作关键字
    m_KeywordMap["inherit"] = CHTLTokenType::INHERIT;
    m_KeywordMap["delete"] = CHTLTokenType::DELETE;
    m_KeywordMap["insert"] = CHTLTokenType::INSERT;
    m_KeywordMap["after"] = CHTLTokenType::AFTER;
    m_KeywordMap["before"] = CHTLTokenType::BEFORE;
    m_KeywordMap["replace"] = CHTLTokenType::REPLACE;
    m_KeywordMap["at top"] = CHTLTokenType::AT_TOP;
    m_KeywordMap["at bottom"] = CHTLTokenType::AT_BOTTOM;
    m_KeywordMap["except"] = CHTLTokenType::EXCEPT;
    m_KeywordMap["from"] = CHTLTokenType::FROM;
    m_KeywordMap["as"] = CHTLTokenType::AS;
    m_KeywordMap["use"] = CHTLTokenType::USE;
    m_KeywordMap["html5"] = CHTLTokenType::HTML5;
}

CHTLTokenType CHTLTokenManager::GetTokenType(const std::string& value) {
    // 首先检查是否为关键字
    auto keywordIt = m_KeywordMap.find(value);
    if (keywordIt != m_KeywordMap.end()) {
        return keywordIt->second;
    }
    
    // 检查是否为自定义@类型
    auto customAtIt = m_CustomAtTypes.find(value);
    if (customAtIt != m_CustomAtTypes.end()) {
        return CHTLTokenType::CUSTOM_AT_TYPE;
    }
    
    // 检查特殊模式
    if (value.empty()) {
        return CHTLTokenType::UNKNOWN;
    }
    
    // 检查选择器
    if (value[0] == '.') {
        return IsValidClassSelector(value) ? CHTLTokenType::CLASS_SELECTOR : CHTLTokenType::UNKNOWN;
    }
    
    if (value[0] == '#') {
        return IsValidIdSelector(value) ? CHTLTokenType::ID_SELECTOR : CHTLTokenType::UNKNOWN;
    }
    
    // 检查上下文引用
    if (value == "&") {
        return CHTLTokenType::CONTEXT_REFERENCE;
    }
    
    // 检查伪类和伪元素
    if (value.find(':') != std::string::npos) {
        if (value.find("::") != std::string::npos) {
            return CHTLTokenType::PSEUDO_ELEMENT;
        }
        else {
            return CHTLTokenType::PSEUDO_CLASS;
        }
    }
    
    // 检查索引访问
    std::regex indexPattern(R"(\[\d+\])");
    if (std::regex_match(value, indexPattern)) {
        return CHTLTokenType::INDEX_ACCESS;
    }
    
    // 检查数字
    std::regex numberPattern(R"(\d+(?:\.\d+)?)");
    if (std::regex_match(value, numberPattern)) {
        return CHTLTokenType::NUMBER;
    }
    
    // 检查字符串字面量
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\'')) {
        return (value.front() == '"') ? CHTLTokenType::STRING_LITERAL : CHTLTokenType::SINGLE_QUOTE_STRING;
    }
    
    // 检查是否为有效标识符
    if (IsValidIdentifier(value)) {
        return CHTLTokenType::IDENTIFIER;
    }
    
    // 检查无修饰字面量（CSS样式的无引号字符串）
    if (std::regex_match(value, std::regex(R"([a-zA-Z0-9\-_]+)"))) {
        return CHTLTokenType::UNQUOTED_LITERAL;
    }
    
    return CHTLTokenType::UNKNOWN;
}

bool CHTLTokenManager::IsKeyword(const std::string& value) {
    return m_KeywordMap.find(value) != m_KeywordMap.end();
}

void CHTLTokenManager::RegisterCustomAtType(const std::string& typeName) {
    if (typeName.front() == '@') {
        m_CustomAtTypes[typeName] = CHTLTokenType::CUSTOM_AT_TYPE;
    }
}

std::vector<std::string> CHTLTokenManager::GetCustomAtTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : m_CustomAtTypes) {
        types.push_back(pair.first);
    }
    return types;
}

bool CHTLTokenManager::IsValidIdentifier(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    
    // 标识符必须以字母或下划线开始
    if (!std::isalpha(value[0]) && value[0] != '_') {
        return false;
    }
    
    // 后续字符必须是字母、数字或下划线
    for (size_t i = 1; i < value.length(); ++i) {
        if (!std::isalnum(value[i]) && value[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLTokenManager::IsValidClassSelector(const std::string& value) {
    if (value.length() < 2 || value[0] != '.') {
        return false;
    }
    
    // 类名部分必须是有效标识符
    std::string className = value.substr(1);
    return IsValidIdentifier(className);
}

bool CHTLTokenManager::IsValidIdSelector(const std::string& value) {
    if (value.length() < 2 || value[0] != '#') {
        return false;
    }
    
    // ID名部分必须是有效标识符
    std::string idName = value.substr(1);
    return IsValidIdentifier(idName);
}

} // namespace CHTL