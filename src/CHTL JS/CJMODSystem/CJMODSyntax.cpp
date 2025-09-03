#include "CJMODSyntax.h"
#include <regex>
#include <iostream>

namespace CJMOD {

// AtomArg实现
AtomArg::AtomArg(const std::string& pattern) 
    : m_Pattern(pattern), m_IsOptional(pattern == "$?"), m_HasValue(false) {
}

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    m_BindFunction = func;
}

void AtomArg::setValue(const std::string& value) {
    m_Value = value;
    m_HasValue = true;
}

std::string AtomArg::process(const std::string& inputValue) {
    if (m_BindFunction) {
        return m_BindFunction(inputValue);
    }
    return inputValue;
}

// Arg实现
Arg::Arg(const std::string& pattern) : m_Pattern(pattern) {
    parsePattern(pattern);
}

Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& value : values) {
        auto atomArg = std::make_unique<AtomArg>("$");
        atomArg->setValue(value);
        m_Args.push_back(std::move(atomArg));
    }
}

void Arg::bind(const std::string& pattern, std::function<std::string(const std::string&)> func) {
    for (auto& arg : m_Args) {
        if (arg->getValue() == pattern || arg->getValue().empty()) {
            arg->bind(func);
            break;
        }
    }
}

void Arg::fillValue(const Arg& result) {
    size_t minSize = std::min(m_Args.size(), result.m_Args.size());
    for (size_t i = 0; i < minSize; ++i) {
        m_Args[i]->setValue(result.m_Args[i]->getValue());
    }
}

void Arg::transform(const std::string& transformExpression) {
    m_TransformResult = transformExpression;
    
    // 简单的变量替换
    for (size_t i = 0; i < m_Args.size(); ++i) {
        std::string placeholder = "arg[" + std::to_string(i) + "].value";
        size_t pos = m_TransformResult.find(placeholder);
        if (pos != std::string::npos) {
            m_TransformResult.replace(pos, placeholder.length(), m_Args[i]->getValue());
        }
    }
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < m_Args.size(); ++i) {
        std::cout << "\"" << m_Args[i]->getValue() << "\"";
        if (i < m_Args.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

AtomArg& Arg::operator[](size_t index) {
    if (index >= m_Args.size()) {
        throw std::out_of_range("Arg index out of range");
    }
    return *m_Args[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    if (index >= m_Args.size()) {
        throw std::out_of_range("Arg index out of range");
    }
    return *m_Args[index];
}

void Arg::parsePattern(const std::string& pattern) {
    // 简单解析: "$ ** $" -> ["$", "**", "$"]
    std::istringstream stream(pattern);
    std::string token;
    
    while (stream >> token) {
        auto atomArg = std::make_unique<AtomArg>(token);
        m_Args.push_back(std::move(atomArg));
    }
}

// Syntax实现
Syntax::Syntax(const std::string& sourceCode) : m_SourceCode(sourceCode) {
}

Arg Syntax::analyze(const std::string& syntaxPattern) {
    return Arg(syntaxPattern);
}

bool Syntax::isObject(const std::string& code) {
    return code.find("{") != std::string::npos && code.find("}") != std::string::npos;
}

bool Syntax::isFunction(const std::string& code) {
    return code.find("function") != std::string::npos || code.find("=>") != std::string::npos;
}

bool Syntax::isArray(const std::string& code) {
    return code.find("[") != std::string::npos && code.find("]") != std::string::npos;
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // 检查CHTL JS函数格式: name { key: value, ... }
    std::regex chtljsRegex(R"([a-zA-Z_][a-zA-Z0-9_]*\s*\{[^}]+\})");
    return std::regex_search(code, chtljsRegex);
}

SyntaxAnalysisResult Syntax::analyzeCode(const std::string& code) {
    SyntaxAnalysisResult result;
    result.Content = code;
    result.IsObject = isObject(code);
    result.IsFunction = isFunction(code);
    result.IsArray = isArray(code);
    result.IsCHTLJSFunction = isCHTLJSFunction(code);
    
    if (result.IsObject) result.Type = "object";
    else if (result.IsFunction) result.Type = "function";
    else if (result.IsArray) result.Type = "array";
    else if (result.IsCHTLJSFunction) result.Type = "chtljs_function";
    else result.Type = "literal";
    
    return result;
}

bool Syntax::analyzeObjectSyntax(const std::string& code) {
    return isObject(code);
}

bool Syntax::analyzeFunctionSyntax(const std::string& code) {
    return isFunction(code);
}

bool Syntax::analyzeArraySyntax(const std::string& code) {
    return isArray(code);
}

bool Syntax::analyzeCHTLJSFunctionSyntax(const std::string& code) {
    return isCHTLJSFunction(code);
}

// CHTLJSFunction实现
CHTLJSFunction::CHTLJSFunction(const std::string& functionContent) 
    : m_FunctionContent(functionContent), m_IsValid(false) {
    parseCHTLJSFunction(functionContent);
}

std::string CHTLJSFunction::getKeyValue(const std::string& key) const {
    auto it = m_KeyValues.find(key);
    return it != m_KeyValues.end() ? it->second : "";
}

bool CHTLJSFunction::hasKey(const std::string& key) const {
    return m_KeyValues.find(key) != m_KeyValues.end();
}

std::string CHTLJSFunction::getKeyType(const std::string& key) const {
    auto it = m_KeyValues.find(key);
    if (it != m_KeyValues.end()) {
        return detectValueType(it->second);
    }
    return "unknown";
}

void CHTLJSFunction::parseCHTLJSFunction(const std::string& content) {
    // 简单解析CHTL JS函数
    std::regex functionRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*\{([^}]+)\})");
    std::smatch matches;
    
    if (std::regex_search(content, matches, functionRegex)) {
        m_FunctionName = matches[1].str();
        std::string body = matches[2].str();
        
        // 解析键值对
        std::regex keyValueRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([^,}]+))");
        std::sregex_iterator iter(body.begin(), body.end(), keyValueRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            const std::smatch& match = *iter;
            std::string key = match[1].str();
            std::string value = match[2].str();
            
            m_KeyValues[key] = value;
            m_Keys.push_back(key);
        }
        
        m_IsValid = !m_KeyValues.empty();
    }
}

std::string CHTLJSFunction::detectValueType(const std::string& value) const {
    if (Syntax::isFunction(value)) return "function";
    if (Syntax::isObject(value)) return "object";
    if (Syntax::isArray(value)) return "array";
    return "literal";
}

} // namespace CJMOD