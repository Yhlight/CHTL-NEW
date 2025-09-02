#include "CJMODScanner.h"
#include <regex>
#include <algorithm>

namespace CJMOD {

CJMODScanner::CJMODScanner(const std::string& sourceCode) 
    : m_SourceCode(sourceCode), m_CurrentPosition(0) {
}

CJMODScanResult CJMODScanner::scan(const std::string& code) {
    CJMODScanResult result;
    
    try {
        m_SourceCode = code;
        m_CurrentPosition = 0;
        m_Tokens.clear();
        m_PlaceholderArgs.clear();
        
        // 扫描所有令牌
        while (!isAtEnd()) {
            skipWhitespace();
            
            if (isAtEnd()) {
                break;
            }
            
            std::string token = scanNextToken();
            if (!token.empty()) {
                m_Tokens.push_back(token);
            }
        }
        
        // 扫描占位符
        m_PlaceholderArgs = scanPlaceholders(code);
        
        // 处理占位符替换（示例）
        std::unordered_map<std::string, std::string> defaultArgs;
        defaultArgs["$"] = "defaultValue";
        defaultArgs["$?"] = "optionalValue";
        defaultArgs["$!"] = "true";
        defaultArgs["$_"] = "";
        
        result.ProcessedCode = processPlaceholderReplacement(code, defaultArgs);
        result.Tokens = m_Tokens;
        result.PlaceholderArgs = std::move(m_PlaceholderArgs);
        result.IsSuccess = true;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.ErrorMessage = "CJMOD扫描异常: " + std::string(e.what());
    }
    
    return result;
}

std::vector<std::unique_ptr<AtomArg>> CJMODScanner::scanPlaceholders(const std::string& code) {
    std::vector<std::unique_ptr<AtomArg>> placeholders;
    
    // 查找所有占位符
    std::regex placeholderPattern(R"(\$[?!_]?|\.\.\.)");
    std::sregex_iterator iter(code.begin(), code.end(), placeholderPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string placeholderStr = iter->str();
        auto atomArg = AtomArg::parsePlaceholder(placeholderStr);
        if (atomArg) {
            placeholders.push_back(std::move(atomArg));
        }
    }
    
    return placeholders;
}

std::string CJMODScanner::processPlaceholderReplacement(const std::string& code, 
                                                       const std::unordered_map<std::string, std::string>& args) {
    std::string result = code;
    
    // 替换所有占位符
    for (const auto& arg : args) {
        std::string placeholder = arg.first;
        std::string value = arg.second;
        
        // 转义正则表达式特殊字符
        std::string escapedPlaceholder = placeholder;
        
        // 使用字符串替换而不是std::replace
        size_t pos = 0;
        while ((pos = escapedPlaceholder.find('$', pos)) != std::string::npos) {
            escapedPlaceholder.replace(pos, 1, "\\$");
            pos += 2;
        }
        
        pos = 0;
        while ((pos = escapedPlaceholder.find('?', pos)) != std::string::npos) {
            escapedPlaceholder.replace(pos, 1, "\\?");
            pos += 2;
        }
        
        pos = 0;
        while ((pos = escapedPlaceholder.find('!', pos)) != std::string::npos) {
            escapedPlaceholder.replace(pos, 1, "\\!");
            pos += 2;
        }
        
        pos = 0;
        while ((pos = escapedPlaceholder.find('.', pos)) != std::string::npos) {
            escapedPlaceholder.replace(pos, 1, "\\.");
            pos += 2;
        }
        
        std::regex placeholderRegex(escapedPlaceholder);
        result = std::regex_replace(result, placeholderRegex, value);
    }
    
    return result;
}

std::vector<std::string> CJMODScanner::scanCHTLJSExtensions(const std::string& code) {
    std::vector<std::string> extensions;
    
    // 查找CHTL JS语法扩展
    std::vector<std::regex> extensionPatterns = {
        std::regex(R"(listen\s*\{[^}]*\})"),     // listen函数
        std::regex(R"(delegate\s*\{[^}]*\})"),   // delegate函数
        std::regex(R"(animate\s*\{[^}]*\})"),    // animate函数
        std::regex(R"(vir\s+\w+\s*=)"),         // vir声明
        std::regex(R"(\{\{[^}]+\}\})"),          // 增强选择器
        std::regex(R"(\w+\s*->\s*\w+)"),        // ->操作符
        std::regex(R"(&\s*->\s*\w+)")           // &->事件绑定操作符
    };
    
    for (const auto& pattern : extensionPatterns) {
        std::sregex_iterator iter(code.begin(), code.end(), pattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            extensions.push_back(iter->str());
        }
    }
    
    return extensions;
}

bool CJMODScanner::validateCJMODSyntax(const std::string& code) {
    // 基础CJMOD语法验证
    
    // 检查占位符格式
    std::regex invalidPlaceholder(R"(\$[^?!_\s])");
    if (std::regex_search(code, invalidPlaceholder)) {
        return false; // 无效的占位符格式
    }
    
    // 检查可变参数占位符
    std::regex variadicPattern(R"(\.\.\.)");
    std::sregex_iterator variadicIter(code.begin(), code.end(), variadicPattern);
    std::sregex_iterator end;
    
    int variadicCount = 0;
    for (; variadicIter != end; ++variadicIter) {
        variadicCount++;
    }
    
    if (variadicCount > 1) {
        return false; // 不能有多个可变参数占位符
    }
    
    return true;
}

void CJMODScanner::reset() {
    m_CurrentPosition = 0;
    m_Tokens.clear();
    m_PlaceholderArgs.clear();
}

std::string CJMODScanner::scanNextToken() {
    if (isAtEnd()) {
        return "";
    }
    
    char ch = currentChar();
    
    // 扫描占位符
    if (ch == '$') {
        return scanPlaceholderToken();
    }
    // 扫描可变参数
    else if (ch == '.' && peek() == '.' && peek(2) == '.') {
        advance(); advance(); advance();
        return "...";
    }
    // 扫描标识符
    else if (std::isalpha(ch) || ch == '_') {
        return scanIdentifierToken();
    }
    // 扫描字符串
    else if (ch == '"' || ch == '\'') {
        return scanStringLiteral(ch);
    }
    // 扫描操作符
    else {
        return scanOperatorToken();
    }
}

void CJMODScanner::skipWhitespace() {
    while (!isAtEnd() && std::isspace(currentChar())) {
        advance();
    }
}

bool CJMODScanner::isAtEnd() const {
    return m_CurrentPosition >= m_SourceCode.length();
}

char CJMODScanner::currentChar() const {
    if (isAtEnd()) {
        return '\0';
    }
    return m_SourceCode[m_CurrentPosition];
}

char CJMODScanner::advance() {
    if (!isAtEnd()) {
        m_CurrentPosition++;
    }
    return currentChar();
}

char CJMODScanner::peek(size_t offset) const {
    size_t peekPos = m_CurrentPosition + offset;
    if (peekPos >= m_SourceCode.length()) {
        return '\0';
    }
    return m_SourceCode[peekPos];
}

std::string CJMODScanner::scanPlaceholderToken() {
    std::string token = "$";
    advance(); // 跳过$
    
    // 检查占位符修饰符
    char modifier = currentChar();
    if (modifier == '?' || modifier == '!' || modifier == '_') {
        token += modifier;
        advance();
    }
    
    return token;
}

std::string CJMODScanner::scanIdentifierToken() {
    std::string token;
    
    while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_')) {
        token += currentChar();
        advance();
    }
    
    return token;
}

std::string CJMODScanner::scanStringLiteral(char quote) {
    std::string token;
    token += quote;
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            token += currentChar();
            advance();
            if (!isAtEnd()) {
                token += currentChar();
                advance();
            }
        }
        else {
            token += currentChar();
            advance();
        }
    }
    
    if (!isAtEnd()) {
        token += currentChar();
        advance(); // 跳过结束引号
    }
    
    return token;
}

std::string CJMODScanner::scanOperatorToken() {
    std::string token;
    char ch = currentChar();
    
    // 扫描单字符或双字符操作符
    if (ch == '-' && peek() == '>') {
        token = "->";
        advance(); advance();
    }
    else if (ch == '&' && peek() == '-' && peek(2) == '>') {
        token = "&->";
        advance(); advance(); advance();
    }
    else if (ch == '{' && peek() == '{') {
        // 扫描增强选择器开始
        token = "{{";
        advance(); advance();
    }
    else if (ch == '}' && peek() == '}') {
        // 扫描增强选择器结束
        token = "}}";
        advance(); advance();
    }
    else {
        token += ch;
        advance();
    }
    
    return token;
}

} // namespace CJMOD