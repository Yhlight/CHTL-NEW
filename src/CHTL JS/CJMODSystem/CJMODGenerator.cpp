#include "CJMODGenerator.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CJMOD {

CJMODGenerator::CJMODGenerator(const std::string& sourceCode) : m_SourceCode(sourceCode) {
    initializeDefaultExtensionMappings();
}

CJMODGenerationResult CJMODGenerator::exportResult(const CJMODScanResult& scanResult) {
    CJMODGenerationResult result;
    
    try {
        if (!scanResult.IsSuccess) {
            result.IsSuccess = false;
            result.ErrorMessage = "扫描结果无效: " + scanResult.ErrorMessage;
            return result;
        }
        
        // 生成基础代码
        result.GeneratedCode = scanResult.ProcessedCode;
        
        // 生成CHTL JS扩展
        std::ostringstream extensionCode;
        
        for (const auto& function : m_CHTLJSFunctions) {
            std::string wrapper = generateFunctionWrapper(function);
            extensionCode << wrapper << "\n";
        }
        
        result.ExtensionCode = extensionCode.str();
        
        // 收集CHTL JS扩展列表
        for (const auto& function : m_CHTLJSFunctions) {
            result.CHTLJSExtensions.push_back(function.FunctionName);
        }
        
        result.IsSuccess = true;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.ErrorMessage = "CJMOD生成异常: " + std::string(e.what());
    }
    
    return result;
}

std::string CJMODGenerator::generateCHTLJSExtension(const std::string& extensionName, const std::string& extensionCode) {
    std::ostringstream code;
    
    code << "// CJMOD Extension: " << extensionName << "\n";
    code << "(function() {\n";
    code << "  'use strict';\n";
    code << "  \n";
    code << "  // Extension Code\n";
    code << extensionCode;
    code << "\n";
    code << "  \n";
    code << "  // Register Extension\n";
    code << "  if (typeof window.CHTLJSExtensions === 'undefined') {\n";
    code << "    window.CHTLJSExtensions = {};\n";
    code << "  }\n";
    code << "  \n";
    code << "  window.CHTLJSExtensions['" << extensionName << "'] = {\n";
    code << "    name: '" << extensionName << "',\n";
    code << "    version: '1.0.0',\n";
    code << "    type: 'CJMOD',\n";
    code << "    loaded: true\n";
    code << "  };\n";
    code << "  \n";
    code << "  console.log('CJMOD Extension loaded:', '" << extensionName << "');\n";
    code << "})();\n";
    
    return code.str();
}

CHTLJSFunctionInfo CJMODGenerator::createCHTLJSFunction(const std::string& functionType, 
                                                       const std::string& functionName,
                                                       const std::string& functionBody) {
    CHTLJSFunctionInfo info;
    info.FunctionName = functionName;
    info.FunctionType = functionType;
    info.FunctionBody = functionBody;
    
    // 添加元数据
    info.Metadata["type"] = functionType;
    info.Metadata["name"] = functionName;
    info.Metadata["cjmod"] = "true";
    
    // 解析函数参数（简化实现）
    std::regex paramPattern(R"(\$[?!_]?|\.\.\.)");
    std::sregex_iterator iter(functionBody.begin(), functionBody.end(), paramPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        info.Parameters.push_back(iter->str());
    }
    
    m_CHTLJSFunctions.push_back(info);
    return info;
}

std::string CJMODGenerator::bindVirtualObject(const std::string& virName, const CHTLJSFunctionInfo& functionInfo) {
    return generateVirtualObjectBinding(virName, functionInfo);
}

std::string CJMODGenerator::generateExtensionSyntaxDefinition(const std::string& syntaxName,
                                                             const std::string& syntaxPattern,
                                                             const std::string& replacement) {
    std::ostringstream code;
    
    code << "// CJMOD Syntax Extension: " << syntaxName << "\n";
    code << "CHTLJSExtensions.registerSyntax({\n";
    code << "  name: '" << syntaxName << "',\n";
    code << "  pattern: /" << syntaxPattern << "/g,\n";
    code << "  replacement: '" << replacement << "',\n";
    code << "  processor: function(match, ...args) {\n";
    code << "    // 自定义处理逻辑\n";
    code << "    return match.replace(this.pattern, this.replacement);\n";
    code << "  }\n";
    code << "});\n";
    
    return code.str();
}

std::string CJMODGenerator::processPlaceholderBinding(const std::string& template_str,
                                                     const std::unordered_map<std::string, Arg>& args) {
    std::string result = template_str;
    
    // 处理所有参数绑定
    for (const auto& argPair : args) {
        const Arg& arg = argPair.second;
        Arg tempArg(result);
        arg.fillValue(tempArg);
        result = tempArg.toString();
    }
    
    return result;
}

std::string CJMODGenerator::exportJavaScript() {
    std::ostringstream js;
    
    js << "// CJMOD Generated JavaScript\n";
    js << "(function() {\n";
    js << "  'use strict';\n";
    js << "  \n";
    
    // 导出所有CHTL JS函数为JavaScript
    for (const auto& function : m_CHTLJSFunctions) {
        js << "  // CHTL JS Function: " << function.FunctionName << "\n";
        js << "  function " << function.FunctionName << "() {\n";
        js << function.FunctionBody;
        js << "  }\n";
        js << "  \n";
    }
    
    // 导出扩展映射
    js << "  // Extension Mappings\n";
    for (const auto& mapping : m_ExtensionMappings) {
        js << "  window['" << mapping.first << "'] = " << mapping.second << ";\n";
    }
    
    js << "})();\n";
    
    return js.str();
}

std::string CJMODGenerator::exportCHTLJS() {
    std::ostringstream chtljs;
    
    chtljs << "// CJMOD Generated CHTL JS\n";
    
    // 导出所有CHTL JS函数
    for (const auto& function : m_CHTLJSFunctions) {
        chtljs << function.FunctionType << " " << function.FunctionName << " {\n";
        chtljs << function.FunctionBody;
        chtljs << "}\n\n";
    }
    
    return chtljs.str();
}

void CJMODGenerator::addExtensionMapping(const std::string& pattern, const std::string& replacement) {
    m_ExtensionMappings[pattern] = replacement;
}

void CJMODGenerator::reset() {
    m_CHTLJSFunctions.clear();
    m_ExtensionMappings.clear();
}

std::string CJMODGenerator::generateFunctionWrapper(const CHTLJSFunctionInfo& functionInfo) {
    std::ostringstream wrapper;
    
    wrapper << "// CJMOD Function Wrapper: " << functionInfo.FunctionName << "\n";
    wrapper << "(function() {\n";
    wrapper << "  var " << functionInfo.FunctionName << " = {\n";
    wrapper << "    type: '" << functionInfo.FunctionType << "',\n";
    wrapper << "    name: '" << functionInfo.FunctionName << "',\n";
    wrapper << "    execute: function(";
    
    // 添加参数
    for (size_t i = 0; i < functionInfo.Parameters.size(); ++i) {
        if (i > 0) wrapper << ", ";
        wrapper << "arg" << i;
    }
    
    wrapper << ") {\n";
    wrapper << functionInfo.FunctionBody;
    wrapper << "    },\n";
    wrapper << "    metadata: {\n";
    
    bool first = true;
    for (const auto& meta : functionInfo.Metadata) {
        if (!first) wrapper << ",\n";
        wrapper << "      " << meta.first << ": '" << meta.second << "'";
        first = false;
    }
    
    wrapper << "\n    }\n";
    wrapper << "  };\n";
    wrapper << "  \n";
    wrapper << "  // Register to CHTL JS\n";
    wrapper << "  if (typeof window.CHTLJSFunctions === 'undefined') {\n";
    wrapper << "    window.CHTLJSFunctions = {};\n";
    wrapper << "  }\n";
    wrapper << "  window.CHTLJSFunctions['" << functionInfo.FunctionName << "'] = " << functionInfo.FunctionName << ";\n";
    wrapper << "})();\n";
    
    return wrapper.str();
}

std::string CJMODGenerator::generateExtensionRegistration() {
    std::ostringstream registration;
    
    registration << "// CJMOD Extension Registration\n";
    registration << "(function() {\n";
    registration << "  if (typeof window.CJMODRegistry === 'undefined') {\n";
    registration << "    window.CJMODRegistry = {\n";
    registration << "      extensions: {},\n";
    registration << "      functions: {},\n";
    registration << "      syntaxDefinitions: {}\n";
    registration << "    };\n";
    registration << "  }\n";
    registration << "  \n";
    
    // 注册所有扩展
    for (const auto& function : m_CHTLJSFunctions) {
        registration << "  CJMODRegistry.functions['" << function.FunctionName << "'] = {\n";
        registration << "    type: '" << function.FunctionType << "',\n";
        registration << "    name: '" << function.FunctionName << "',\n";
        registration << "    cjmod: true\n";
        registration << "  };\n";
    }
    
    registration << "})();\n";
    
    return registration.str();
}

std::string CJMODGenerator::processSyntaxTransformation(const std::string& originalSyntax) {
    std::string transformed = originalSyntax;
    
    // 应用所有扩展映射
    for (const auto& mapping : m_ExtensionMappings) {
        std::regex pattern(mapping.first);
        transformed = std::regex_replace(transformed, pattern, mapping.second);
    }
    
    return transformed;
}

void CJMODGenerator::initializeDefaultExtensionMappings() {
    // 初始化默认扩展映射
    addExtensionMapping("printMylove\\(([^)]*)\\)", "console.log('💖 Chtholly:', $1)");
    addExtensionMapping("iNeverAway\\(([^)]*)\\)", "setInterval($1, 1000)");
    addExtensionMapping("util\\.\\.\\.then\\(([^)]*)\\)", "Promise.resolve().then($1)");
}

CHTLJSFunctionInfo CJMODGenerator::parseCHTLJSFunctionBody(const std::string& functionBody) {
    CHTLJSFunctionInfo info;
    
    // 简化的函数体解析
    info.FunctionBody = functionBody;
    
    // 提取函数名（如果有）
    std::regex namePattern(R"(function\s+(\w+))");
    std::smatch match;
    if (std::regex_search(functionBody, match, namePattern)) {
        info.FunctionName = match[1].str();
    }
    
    return info;
}

std::string CJMODGenerator::generateVirtualObjectBinding(const std::string& virName, const CHTLJSFunctionInfo& functionInfo) {
    std::ostringstream binding;
    
    binding << "// Virtual Object Binding: " << virName << "\n";
    binding << "var " << virName << " = {\n";
    binding << "  " << functionInfo.FunctionName << ": function() {\n";
    binding << functionInfo.FunctionBody;
    binding << "  },\n";
    binding << "  type: '" << functionInfo.FunctionType << "',\n";
    binding << "  virtual: true,\n";
    binding << "  cjmod: true\n";
    binding << "};\n";
    
    return binding.str();
}

// CHTLJSFunction类实现
CHTLJSFunctionInfo CHTLJSFunctionGenerator::CreateCHTLJSFunction(const std::string& functionType,
                                                       const std::string& functionName,
                                                       const std::string& functionBody) {
    CHTLJSFunctionInfo info;
    info.FunctionType = functionType;
    info.FunctionName = functionName;
    info.FunctionBody = functionBody;
    
    // 添加CJMOD特定元数据
    info.Metadata["cjmod"] = "true";
    info.Metadata["created_by"] = "CHTLJSFunction";
    info.Metadata["type"] = functionType;
    
    return info;
}

std::string CHTLJSFunctionGenerator::bindVirtualObject(const std::string& virName, const CHTLJSFunctionInfo& functionInfo) {
    std::ostringstream binding;
    
    binding << "// CJMOD Virtual Object Binding\n";
    binding << "vir " << virName << " = " << functionInfo.FunctionType << " {\n";
    
    // 处理函数体中的占位符
    std::string processedBody = functionInfo.FunctionBody;
    
    // 替换CJMOD特定语法
    std::regex cjmodPattern(R"(\$[?!_]?)");
    processedBody = std::regex_replace(processedBody, cjmodPattern, "arguments[0]");
    
    binding << processedBody;
    binding << "};\n";
    
    return binding.str();
}

std::string CHTLJSFunctionGenerator::generateFunctionWrapper(const CHTLJSFunctionInfo& functionInfo) {
    std::ostringstream wrapper;
    
    wrapper << "// CHTL JS Function Wrapper (CJMOD Generated)\n";
    wrapper << functionInfo.FunctionType << " " << functionInfo.FunctionName << " {\n";
    
    // 处理函数体
    std::string wrappedBody = functionInfo.FunctionBody;
    
    // 添加CJMOD特定的增强功能
    if (functionInfo.FunctionType == "listen") {
        wrapper << "  // CJMOD Enhanced Listen Function\n";
        wrapper << "  element: $,\n";
        wrapper << "  events: {\n";
        wrapper << wrappedBody;
        wrapper << "  },\n";
        wrapper << "  cjmod: true\n";
    }
    else if (functionInfo.FunctionType == "delegate") {
        wrapper << "  // CJMOD Enhanced Delegate Function\n";
        wrapper << "  parent: $,\n";
        wrapper << "  targets: $?,\n";
        wrapper << "  events: {\n";
        wrapper << wrappedBody;
        wrapper << "  },\n";
        wrapper << "  cjmod: true\n";
    }
    else if (functionInfo.FunctionType == "animate") {
        wrapper << "  // CJMOD Enhanced Animate Function\n";
        wrapper << "  target: $,\n";
        wrapper << "  duration: $?,\n";
        wrapper << "  easing: $?,\n";
        wrapper << "  keyframes: {\n";
        wrapper << wrappedBody;
        wrapper << "  },\n";
        wrapper << "  cjmod: true\n";
    }
    else {
        wrapper << wrappedBody;
    }
    
    wrapper << "}\n";
    
    return wrapper.str();
}

bool CHTLJSFunctionGenerator::validateCHTLJSFunction(const std::string& functionCode) {
    // 验证CHTL JS函数语法
    
    // 检查函数类型
    std::regex functionTypePattern(R"((listen|delegate|animate)\s*\{)");
    if (!std::regex_search(functionCode, functionTypePattern)) {
        return false;
    }
    
    // 检查大括号匹配
    int braceCount = 0;
    for (char ch : functionCode) {
        if (ch == '{') braceCount++;
        else if (ch == '}') braceCount--;
        
        if (braceCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0;
}

} // namespace CJMOD