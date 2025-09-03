#include "CJMODCore.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace CJMOD {

// AtomArg实现
AtomArg::AtomArg(const std::string& pattern) 
    : m_Pattern(pattern), m_Value(""), m_IsOptional(false), m_HasValue(false) {
    // 解析模式
    if (pattern == "$?") {
        m_IsOptional = true;
    } else if (pattern == "$!") {
        m_IsOptional = false;
    } else if (pattern == "$_") {
        m_IsOptional = true; // 忽略参数也是可选的
    } else if (pattern == "...") {
        m_IsOptional = true; // 可变参数
    }
}

AtomArg::AtomArg(const AtomArg& other) 
    : m_Pattern(other.m_Pattern), m_Value(other.m_Value), 
      m_IsOptional(other.m_IsOptional), m_HasValue(other.m_HasValue),
      m_BindFunction(other.m_BindFunction) {
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

void AtomArg::ParseFromString(const std::string& str) {
    m_Pattern = str;
    m_Value = str;
    m_IsOptional = (str.find("?") != std::string::npos);
    m_HasValue = true;
}

std::unique_ptr<AtomArg> AtomArg::parsePlaceholder(const std::string& placeholder) {
    return std::make_unique<AtomArg>(placeholder);
}

// Arg实现
Arg::Arg(const std::string& pattern) : m_Pattern(pattern) {
    parsePattern(pattern);
}

Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& value : values) {
        m_Args.push_back(std::make_unique<AtomArg>(value));
    }
}

Arg::Arg(const Arg& other) : m_Pattern(other.m_Pattern), m_TransformResult(other.m_TransformResult),
                             Type(other.Type), IsObject(other.IsObject), 
                             IsFunction(other.IsFunction), IsCHTLJSFunction(other.IsCHTLJSFunction) {
    // 深拷贝unique_ptr
    for (const auto& arg : other.m_Args) {
        m_Args.push_back(std::make_unique<AtomArg>(*arg));
    }
}

Arg::Arg(Arg&& other) noexcept : m_Pattern(std::move(other.m_Pattern)), 
                                 m_Args(std::move(other.m_Args)),
                                 m_TransformResult(std::move(other.m_TransformResult)),
                                 Type(std::move(other.Type)),
                                 IsObject(other.IsObject),
                                 IsFunction(other.IsFunction),
                                 IsCHTLJSFunction(other.IsCHTLJSFunction) {
}

Arg& Arg::operator=(const Arg& other) {
    if (this != &other) {
        m_Pattern = other.m_Pattern;
        m_TransformResult = other.m_TransformResult;
        Type = other.Type;
        IsObject = other.IsObject;
        IsFunction = other.IsFunction;
        IsCHTLJSFunction = other.IsCHTLJSFunction;
        
        // 深拷贝unique_ptr
        m_Args.clear();
        for (const auto& arg : other.m_Args) {
            m_Args.push_back(std::make_unique<AtomArg>(*arg));
        }
    }
    return *this;
}

Arg& Arg::operator=(Arg&& other) noexcept {
    if (this != &other) {
        m_Pattern = std::move(other.m_Pattern);
        m_Args = std::move(other.m_Args);
        m_TransformResult = std::move(other.m_TransformResult);
        Type = std::move(other.Type);
        IsObject = other.IsObject;
        IsFunction = other.IsFunction;
        IsCHTLJSFunction = other.IsCHTLJSFunction;
    }
    return *this;
}

Arg::Arg(const std::string& name, const std::string& value, const std::string& type) {
    m_Pattern = name;
    auto atomArg = std::make_unique<AtomArg>("$");
    atomArg->setValue(value);
    m_Args.push_back(std::move(atomArg));
    
    // 设置类型信息
    Type = type;
    IsObject = (type == "object");
    IsFunction = (type == "function");
    IsCHTLJSFunction = (type == "chtljs_function");
}

void Arg::fillValue(const Arg& result) const {
    // 这个方法现在不修改状态，只是兼容性方法
}

void Arg::transform(const std::string& transformExpression) {
    m_TransformResult = transformExpression;
    for (const auto& arg : m_Args) {
        if (arg->hasValue()) {
            // 简单的占位符替换
            size_t pos = m_TransformResult.find("$");
            if (pos != std::string::npos) {
                m_TransformResult.replace(pos, 1, arg->getValue());
            }
        }
    }
}

AtomArg& Arg::operator[](size_t index) {
    if (index >= m_Args.size()) {
        m_Args.resize(index + 1);
        for (size_t i = m_Args.size() - 1; i >= index && !m_Args[i]; --i) {
            m_Args[i] = std::make_unique<AtomArg>();
        }
    }
    return *m_Args[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    static AtomArg defaultArg;
    if (index >= m_Args.size()) {
        return defaultArg;
    }
    return *m_Args[index];
}

std::string Arg::toString() const {
    return m_TransformResult.empty() ? m_Pattern : m_TransformResult;
}

bool Arg::bind(const std::string& value) {
    if (m_Args.empty()) {
        m_Args.push_back(std::make_unique<AtomArg>());
    }
    m_Args[0]->setValue(value);
    return true;
}

void Arg::parsePattern(const std::string& pattern) {
    // 简单的模式解析
    std::regex argPattern(R"(\$[?!_]?|\.\.\.)");
    std::sregex_iterator iter(pattern.begin(), pattern.end(), argPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        m_Args.push_back(std::make_unique<AtomArg>(iter->str()));
    }
}

// Syntax实现
Arg Syntax::analyzeCode(const std::string& code) {
    Arg result(code);
    
    // 分析代码类型
    if (isObject(code)) {
        result.Type = "object";
        result.IsObject = true;
    } else if (isFunction(code)) {
        result.Type = "function";
        result.IsFunction = true;
    } else if (isCHTLJSFunction(code)) {
        result.Type = "chtljs_function";
        result.IsCHTLJSFunction = true;
    } else if (isArray(code)) {
        result.Type = "array";
    } else {
        result.Type = "literal";
    }
    
    return result;
}

bool Syntax::isObject(const std::string& code) {
    Syntax instance;
    return instance.checkIsObject(code);
}

bool Syntax::isFunction(const std::string& code) {
    Syntax instance;
    return instance.checkIsFunction(code);
}

bool Syntax::isArray(const std::string& code) {
    Syntax instance;
    return instance.checkIsArray(code);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    Syntax instance;
    return instance.checkIsCHTLJSFunction(code);
}

bool Syntax::checkIsObject(const std::string& code) {
    return code.find("{") != std::string::npos && code.find("}") != std::string::npos;
}

bool Syntax::checkIsFunction(const std::string& code) {
    return code.find("function") != std::string::npos || code.find("=>") != std::string::npos;
}

bool Syntax::checkIsArray(const std::string& code) {
    return code.find("[") != std::string::npos && code.find("]") != std::string::npos;
}

bool Syntax::checkIsCHTLJSFunction(const std::string& code) {
    // 检查CHTL JS函数格式: name { key: value, ... }
    std::regex chtljsRegex(R"([a-zA-Z_][a-zA-Z0-9_]*\s*\{[^}]+\})");
    return std::regex_search(code, chtljsRegex);
}

// CJMODScanner实现
std::vector<std::unique_ptr<AtomArg>> CJMODScanner::scanPlaceholders(const std::string& code) {
    std::vector<std::unique_ptr<AtomArg>> placeholders;
    
    // 查找所有占位符
    std::regex placeholderPattern(R"(\$[?!_]?|\.\.\.)");
    std::sregex_iterator iter(code.begin(), code.end(), placeholderPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string placeholderStr = iter->str();
        auto atomArg = std::make_unique<AtomArg>(placeholderStr);
        placeholders.push_back(std::move(atomArg));
    }
    
    return placeholders;
}

Arg CJMODScanner::scan(const std::string& code, const std::string& pattern) {
    Arg result(pattern);
    
    // 扫描代码并填充结果
    auto placeholders = scanPlaceholders(code);
    // 通过公共接口访问
    for (size_t i = 0; i < placeholders.size(); ++i) {
        if (i < result.size()) {
            result[i].setValue(placeholders[i]->getValue());
        }
    }
    
    return result;
}

std::string CJMODScanner::processPlaceholderReplacement(const std::string& code, 
                                                      const std::unordered_map<std::string, std::string>& placeholders) {
    std::string result = code;
    
    // 替换所有占位符
    for (const auto& pair : placeholders) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

// CJMODGenerator实现
std::string CJMODGenerator::exportJavaScript() {
    return "// CJMOD Generated JavaScript\nconsole.log('CJMOD Export Success');";
}

std::string CJMODGenerator::exportResult(const std::string& result) {
    std::ostringstream output;
    output << "// CJMOD Export Result\n";
    output << result << "\n";
    output << "// Export Complete\n";
    return output.str();
}

std::string CJMODGenerator::processPlaceholderBinding(const std::string& template_str,
                                                    const std::unordered_map<std::string, Arg>& args) {
    std::string result = template_str;
    
    // 处理所有参数绑定
    for (const auto& argPair : args) {
        const Arg& arg = argPair.second;
        // 简单的占位符替换
        size_t pos = result.find("$");
        if (pos != std::string::npos) {
            result.replace(pos, 1, arg.toString());
        }
    }
    
    return result;
}

std::string CJMODGenerator::generateVirtualObjectBinding(const std::string& virName, const CJMODFunctionInfo& functionInfo) {
    std::ostringstream binding;
    
    binding << "// CJMOD Virtual Object Binding\n";
    binding << "vir " << virName << " = " << functionInfo.FunctionType << " {\n";
    binding << "    name: '" << functionInfo.FunctionName << "',\n";
    binding << "    body: function() {\n";
    binding << "        " << functionInfo.FunctionBody << "\n";
    binding << "    }\n";
    binding << "};\n";
    
    return binding.str();
}

// CJMODFunctionGenerator实现
CJMODFunctionInfo CJMODFunctionGenerator::CreateCHTLJSFunction(const std::string& functionType,
                                                              const std::string& functionName,
                                                              const std::string& functionBody) {
    CJMODFunctionInfo info;
    info.FunctionType = functionType;
    info.FunctionName = functionName;
    info.FunctionBody = functionBody;
    info.Metadata["type"] = functionType;
    
    return info;
}

std::string CJMODFunctionGenerator::bindVirtualObject(const std::string& virName, const CJMODFunctionInfo& functionInfo) {
    std::ostringstream binding;
    
    binding << "// CJMOD Virtual Object Binding\n";
    binding << "vir " << virName << " = " << functionInfo.FunctionType << " {\n";
    binding << "    name: '" << functionInfo.FunctionName << "',\n";
    binding << "    body: function() {\n";
    binding << "        " << functionInfo.FunctionBody << "\n";
    binding << "    }\n";
    binding << "};\n";
    
    return binding.str();
}

std::string CJMODFunctionGenerator::generateFunctionWrapper(const CJMODFunctionInfo& functionInfo) {
    std::ostringstream wrapper;
    
    wrapper << "// CHTL JS Function Wrapper (CJMOD Generated)\n";
    wrapper << functionInfo.FunctionType << " " << functionInfo.FunctionName << " {\n";
    wrapper << "    " << functionInfo.FunctionBody << "\n";
    wrapper << "}\n";
    
    return wrapper.str();
}

bool CJMODFunctionGenerator::validateCHTLJSFunction(const std::string& functionCode) {
    // 验证CHTL JS函数语法
    
    // 检查函数类型
    std::regex functionTypePattern(R"((listen|delegate|animate)\s*\{)");
    if (!std::regex_search(functionCode, functionTypePattern)) {
        return false;
    }
    
    // 检查基本语法结构
    if (functionCode.find("{") == std::string::npos || 
        functionCode.find("}") == std::string::npos) {
        return false;
    }
    
    return true;
}

std::string CJMODFunctionGenerator::detectValueType(const std::string& value) {
    if (Syntax::isFunction(value)) return "function";
    if (Syntax::isObject(value)) return "object";
    if (Syntax::isArray(value)) return "array";
    return "literal";
}

} // namespace CJMOD