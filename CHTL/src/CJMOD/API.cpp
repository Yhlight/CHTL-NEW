#include "CJMOD/API.h"
#include "Common/Logger.h"
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ==================== Arg 实现 ====================

void Arg::fillValue(const Arg& scannedArg) {
    if (atoms.size() != scannedArg.atoms.size()) {
        LOG_ERROR("参数数量不匹配");
        return;
    }
    
    for (size_t i = 0; i < atoms.size(); ++i) {
        if (atoms[i].type == AtomArgType::Placeholder) {
            // 如果有绑定的获取函数，使用它
            auto it = bindings.find(atoms[i].originalValue);
            if (it != bindings.end()) {
                atoms[i].value = it->second(scannedArg.atoms[i].value);
            } else {
                atoms[i].value = scannedArg.atoms[i].value;
            }
        }
    }
}

std::string Arg::transform(const std::string& pattern) const {
    std::string result = pattern;
    
    // 替换占位符
    for (size_t i = 0; i < atoms.size(); ++i) {
        std::string placeholder = "$" + std::to_string(i);
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), atoms[i].value);
            pos += atoms[i].value.length();
        }
    }
    
    return result;
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < atoms.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << atoms[i].value << "\"";
    }
    std::cout << "]" << std::endl;
}

// ==================== Syntax 实现 ====================

Arg Syntax::analyze(const std::string& pattern) {
    Arg result;
    std::istringstream stream(pattern);
    std::string token;
    
    while (stream >> token) {
        AtomArgType type;
        
        if (token == "$" || token == "$?" || token == "$!" || token == "$_") {
            type = AtomArgType::Placeholder;
        } else if (token == "...") {
            type = AtomArgType::Spread;
        } else if (token.find_first_of("+-*/%=<>!&|") != std::string::npos) {
            type = AtomArgType::Operator;
        } else {
            type = AtomArgType::Literal;
        }
        
        result.AddAtom(AtomArg(type, token));
    }
    
    return result;
}

bool Syntax::isObject(const std::string& code) {
    // 简单的对象检测：检查是否以{开始，以}结束
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool Syntax::isFunction(const std::string& code) {
    // 检查是否包含function关键字或箭头函数
    std::regex funcRegex(R"(^\s*(function\s+\w*\s*\(|(\w+|\(.*?\))\s*=>))");
    return std::regex_search(code, funcRegex);
}

bool Syntax::isArray(const std::string& code) {
    // 简单的数组检测：检查是否以[开始，以]结束
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // CHTL JS函数格式：name {prop: value, ...}
    std::regex cjtlFuncRegex(R"(^\s*\w+\s*\{[^}]*\}\s*;?\s*$)");
    return std::regex_match(code, cjtlFuncRegex);
}

// ==================== CJMODScanner 实现 ====================

Arg CJMODScanner::scan(const Arg& pattern, const std::string& code) {
    Arg result;
    
    // 简化实现：按空格分割代码，匹配模式
    std::istringstream stream(code);
    std::string token;
    size_t patternIndex = 0;
    
    while (stream >> token && patternIndex < pattern.size()) {
        const auto& patternAtom = pattern[patternIndex];
        
        if (patternAtom.type == AtomArgType::Placeholder) {
            // 占位符匹配任意值
            result.AddAtom(AtomArg(AtomArgType::Literal, token));
        } else if (patternAtom.value == token) {
            // 精确匹配
            result.AddAtom(patternAtom);
        } else {
            // 不匹配，跳过
            continue;
        }
        
        patternIndex++;
    }
    
    return result;
}

// ==================== CJMODGenerator 实现 ====================

void CJMODGenerator::exportResult(const Arg& args) {
    // TODO: 实现导出逻辑
    // 这里应该将转换后的代码输出到CHTL JS编译器
    LOG_DEBUG("导出CJMOD结果");
}

// ==================== CHTLJSFunction 实现 ====================

void CHTLJSFunction::bindVirtualObject(const std::string& virName, 
                                      std::function<std::string(const std::string&)> handler) {
    // TODO: 实现虚拟对象绑定
    LOG_DEBUG("绑定虚拟对象: " + virName);
}

// ==================== 辅助函数 ====================

std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& code) {
    // 解析CHTL JS函数语法
    std::regex funcRegex(R"(^\s*(\w+)\s*\{([^}]*)\}\s*;?\s*$)");
    std::smatch match;
    
    if (std::regex_match(code, match, funcRegex)) {
        auto func = std::make_shared<CHTLJSFunction>(match[1].str());
        
        // 解析属性
        std::string props = match[2].str();
        std::regex propRegex(R"((\w+)\s*:\s*([^,]+))");
        std::sregex_iterator it(props.begin(), props.end(), propRegex);
        std::sregex_iterator end;
        
        for (; it != end; ++it) {
            func->AddProperty((*it)[1].str(), (*it)[2].str());
        }
        
        return func;
    }
    
    return nullptr;
}

} // namespace CJMOD
} // namespace CHTL