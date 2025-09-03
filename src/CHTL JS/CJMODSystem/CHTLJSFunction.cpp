#include "CHTLJSFunction.h"
#include <regex>
#include <iostream>

namespace CJMOD {

// 静态成员初始化
std::unordered_map<std::string, bool> CHTLJSFunction::s_VirtualObjectBindings;

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, const std::string& pattern)
    : m_FunctionName(functionName), m_Pattern(pattern), m_SupportsVirtualObject(false) {
    parsePattern(pattern);
}

CHTLJSFunction CHTLJSFunction::CreateCHTLJSFunction(const std::string& pattern) {
    // 按照CJMOD.md示例：CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
    
    std::string functionName = extractFunctionName(pattern);
    std::string argsPattern = extractArgsPattern(pattern);
    
    CHTLJSFunction func(functionName, pattern);
    
    // 天然支持虚对象vir
    func.m_SupportsVirtualObject = true;
    s_VirtualObjectBindings[functionName] = true;
    
    // 解析参数模式
    func.m_Args = Syntax::analyze(argsPattern);
    
    std::cout << "✅ 创建CHTL JS函数: " << functionName << "（天然支持vir）" << std::endl;
    
    return func;
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    // 手动绑定虚对象vir支持
    s_VirtualObjectBindings[functionName] = true;
    
    std::cout << "✅ 绑定虚对象支持: " << functionName << std::endl;
}

bool CHTLJSFunction::supportsVirtualObject(const std::string& functionName) {
    auto it = s_VirtualObjectBindings.find(functionName);
    return it != s_VirtualObjectBindings.end() && it->second;
}

void CHTLJSFunction::parsePattern(const std::string& pattern) {
    // 解析函数模式
    m_FunctionName = extractFunctionName(pattern);
    std::string argsPattern = extractArgsPattern(pattern);
    
    if (!argsPattern.empty()) {
        m_Args = Syntax::analyze(argsPattern);
    }
}

std::string CHTLJSFunction::extractFunctionName(const std::string& pattern) {
    // 提取函数名，如从 "printMyLove {url: $!_, mode: $?_}" 提取 "printMyLove"
    std::regex namePattern(R"((\w+)\s*\{)");
    std::smatch match;
    
    if (std::regex_search(pattern, match, namePattern)) {
        return match[1].str();
    }
    
    return "";
}

std::string CHTLJSFunction::extractArgsPattern(const std::string& pattern) {
    // 提取参数模式，如从 "printMyLove {url: $!_, mode: $?_}" 提取 "url: $!_, mode: $?_"
    std::regex argsPattern(R"(\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(pattern, match, argsPattern)) {
        return match[1].str();
    }
    
    return "";
}

} // namespace CJMOD