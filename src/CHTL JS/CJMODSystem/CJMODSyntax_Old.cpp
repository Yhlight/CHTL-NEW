#include "CJMODSyntax.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CJMOD {

// Syntax类实现
Syntax::Syntax(const std::string& sourceCode) : m_SourceCode(sourceCode) {
    initializeDefaultTypeMappings();
}

SyntaxAnalysisResult Syntax::analyze(const std::string& code) {
    m_AnalysisResult = SyntaxAnalysisResult();
    m_AnalysisResult.Content = code;
    
    // 分析各种语法类型
    m_AnalysisResult.IsObject = analyzeObjectSyntax(code);
    m_AnalysisResult.IsFunction = analyzeFunctionSyntax(code);
    m_AnalysisResult.IsArray = analyzeArraySyntax(code);
    m_AnalysisResult.IsCHTLJSFunction = analyzeCHTLJSFunctionSyntax(code);
    
    // 确定主要类型
    if (m_AnalysisResult.IsCHTLJSFunction) {
        m_AnalysisResult.Type = "CHTLJSFunction";
    }
    else if (m_AnalysisResult.IsFunction) {
        m_AnalysisResult.Type = "Function";
    }
    else if (m_AnalysisResult.IsObject) {
        m_AnalysisResult.Type = "Object";
    }
    else if (m_AnalysisResult.IsArray) {
        m_AnalysisResult.Type = "Array";
    }
    else {
        m_AnalysisResult.Type = "Unknown";
    }
    
    return m_AnalysisResult;
}

bool Syntax::isObject(const std::string& code) {
    Syntax instance(code);
    return instance.analyzeObjectSyntax(code);
}

bool Syntax::isFunction(const std::string& code) {
    Syntax instance(code);
    return instance.analyzeFunctionSyntax(code);
}

bool Syntax::isArray(const std::string& code) {
    Syntax instance(code);
    return instance.analyzeArraySyntax(code);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    Syntax instance(code);
    return instance.analyzeCHTLJSFunctionSyntax(code);
}

Arg Syntax::analyze(const std::string& syntaxPattern) {
    Arg result(syntaxPattern);
    return result;
}

std::string Syntax::getType(const std::string& code) {
    analyze(code);
    return m_AnalysisResult.Type;
}

void Syntax::setTypeMapping(const std::string& pattern, const std::string& type) {
    m_TypeMappings[pattern] = type;
}

void Syntax::initializeDefaultTypeMappings() {
    // 初始化默认类型映射
    m_TypeMappings["listen"] = "CHTLJSFunction";
    m_TypeMappings["delegate"] = "CHTLJSFunction";
    m_TypeMappings["animate"] = "CHTLJSFunction";
    m_TypeMappings["vir"] = "VirtualObject";
    m_TypeMappings["{{.*}}"] = "EnhancedSelector";
    m_TypeMappings["module"] = "Module";
}

bool Syntax::analyzeObjectSyntax(const std::string& code) {
    // 检查对象语法：{ key: value, ... }
    std::regex objectPattern(R"(\s*\{[^}]*\}\s*)");
    return std::regex_match(code, objectPattern);
}

bool Syntax::analyzeFunctionSyntax(const std::string& code) {
    // 检查函数语法：function(...) { ... } 或 (...) => { ... }
    std::regex functionPattern1(R"(\s*function\s*\([^)]*\)\s*\{[^}]*\}\s*)");
    std::regex functionPattern2(R"(\s*\([^)]*\)\s*=>\s*\{[^}]*\}\s*)");
    std::regex functionPattern3(R"(\s*\w+\s*\([^)]*\)\s*\{[^}]*\}\s*)");
    
    return std::regex_match(code, functionPattern1) ||
           std::regex_match(code, functionPattern2) ||
           std::regex_match(code, functionPattern3);
}

bool Syntax::analyzeArraySyntax(const std::string& code) {
    // 检查数组语法：[ ... ]
    std::regex arrayPattern(R"(\s*\[[^\]]*\]\s*)");
    return std::regex_match(code, arrayPattern);
}

bool Syntax::analyzeCHTLJSFunctionSyntax(const std::string& code) {
    // 检查CHTL JS函数语法：listen { ... }、delegate { ... }、animate { ... }
    std::regex chtljsFunctionPattern(R"(\s*(listen|delegate|animate)\s*\{[^}]*\}\s*)");
    return std::regex_match(code, chtljsFunctionPattern);
}

// AtomArg类实现
AtomArg::AtomArg(PlaceholderType type, const std::string& name, const std::string& defaultValue)
    : m_Type(type), m_Name(name), m_DefaultValue(defaultValue), m_IsResolved(false) {
}

std::string AtomArg::getPlaceholderString() const {
    switch (m_Type) {
        case PlaceholderType::REQUIRED_PLACEHOLDER:
            return "$";
        case PlaceholderType::OPTIONAL_PLACEHOLDER:
            return "$?";
        case PlaceholderType::BOOLEAN_PLACEHOLDER:
            return "$!";
        case PlaceholderType::IGNORE_PLACEHOLDER:
            return "$_";
        case PlaceholderType::VARIADIC_PLACEHOLDER:
            return "...";
        default:
            return "$";
    }
}

std::unique_ptr<AtomArg> AtomArg::parsePlaceholder(const std::string& placeholderStr) {
    PlaceholderType type = PlaceholderType::REQUIRED_PLACEHOLDER;
    std::string name = "arg";
    
    if (placeholderStr == "$") {
        type = PlaceholderType::REQUIRED_PLACEHOLDER;
        name = "required_arg";
    }
    else if (placeholderStr == "$?") {
        type = PlaceholderType::OPTIONAL_PLACEHOLDER;
        name = "optional_arg";
    }
    else if (placeholderStr == "$!") {
        type = PlaceholderType::BOOLEAN_PLACEHOLDER;
        name = "boolean_arg";
    }
    else if (placeholderStr == "$_") {
        type = PlaceholderType::IGNORE_PLACEHOLDER;
        name = "ignore_arg";
    }
    else if (placeholderStr == "...") {
        type = PlaceholderType::VARIADIC_PLACEHOLDER;
        name = "variadic_args";
    }
    
    return std::make_unique<AtomArg>(type, name);
}

// Arg类实现
Arg::Arg(const std::string& name, const std::string& value, const std::string& type)
    : m_Name(name), m_Value(value), m_Type(type) {
}

bool Arg::bind(const std::string& value) {
    m_Value = value;
    
    // 验证值类型
    if (m_Type == "int") {
        try {
            std::stoi(value);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
    else if (m_Type == "bool") {
        return value == "true" || value == "false";
    }
    else if (m_Type == "string") {
        return true; // 字符串总是有效的
    }
    
    return true;
}

std::string Arg::fillValue(const std::string& template_str) const {
    std::string result = template_str;
    
    // 解析模板中的占位符
    auto placeholders = parsePlaceholders(template_str);
    
    for (const auto& placeholder : placeholders) {
        // 将占位符替换为实际值
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            std::string replacement = m_Value;
            
            // 根据占位符类型进行特殊处理
            if (placeholder == "$?") {
                // 可选占位符：如果值为空，使用默认值
                if (m_Value.empty()) {
                    replacement = "undefined"; // 默认值
                }
            }
            else if (placeholder == "$!") {
                // 布尔占位符：转换为布尔值
                replacement = (m_Value == "true" || m_Value == "1") ? "true" : "false";
            }
            else if (placeholder == "$_") {
                // 忽略占位符：替换为空
                replacement = "";
            }
            
            result.replace(pos, placeholder.length(), replacement);
        }
    }
    
    return result;
}

std::string Arg::transform(std::function<std::string(const std::string&)> transformer) {
    m_Transform = transformer;
    return applyTransform(m_Value);
}

void Arg::addAtomArg(std::unique_ptr<AtomArg> atomArg) {
    m_AtomArgs.push_back(std::move(atomArg));
}

std::vector<std::string> Arg::parsePlaceholders(const std::string& template_str) const {
    std::vector<std::string> placeholders;
    
    // 查找所有占位符
    std::regex placeholderPattern(R"(\$[?!_]?|\.\.\.)");
    std::sregex_iterator iter(template_str.begin(), template_str.end(), placeholderPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        placeholders.push_back(iter->str());
    }
    
    return placeholders;
}

std::string Arg::applyTransform(const std::string& value) {
    if (m_Transform) {
        return m_Transform(value);
    }
    return value;
}

} // namespace CJMOD