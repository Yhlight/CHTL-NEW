#include "VirObjectNode.h"
#include "Visitor.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTLJS {

VirObjectNode::VirObjectNode(const std::string& virName, size_t line, size_t column)
    : CHTLJSBaseNode(CHTLJSNodeType::VIR_DECLARATION_NODE, virName, line, column),
      m_VirName(virName), m_IsActive(true), m_HasFunctionKeys(false), 
      m_HasObjectKeys(false), m_IsResolved(false) {
    
    SetName(virName);
    SetSupportsVirtualObject(true);
}

void VirObjectNode::SetVirName(const std::string& name) {
    if (IsValidVirName(name)) {
        m_VirName = name;
        SetName(name);
    }
}

void VirObjectNode::SetCHTLJSFunctionContent(const std::string& content) {
    m_CHTLJSFunctionContent = content;
    SetContent(content);
    
    // 重新解析函数内容
    m_VirKeys.clear();
    m_FunctionReferences.clear();
    m_ObjectReferences.clear();
    m_HasFunctionKeys = false;
    m_HasObjectKeys = false;
    m_IsResolved = false;
    
    ParseCHTLJSFunction(content);
}

void VirObjectNode::ParseCHTLJSFunction(const std::string& functionContent) {
    // 检测CHTL JS函数类型
    m_CHTLJSFunctionType = DetectCHTLJSFunctionType(functionContent);
    
    // 提取函数键值对
    ExtractFunctionKeyValuePairs(functionContent);
    
    m_IsResolved = true;
}

std::string VirObjectNode::DetectCHTLJSFunctionType(const std::string& content) {
    // 检测CHTL JS函数类型
    if (content.find("listen") != std::string::npos) {
        return "listen";
    }
    else if (content.find("delegate") != std::string::npos) {
        return "delegate";
    }
    else if (content.find("animate") != std::string::npos) {
        return "animate";
    }
    else if (content.find("module") != std::string::npos) {
        return "module";
    }
    
    return "unknown";
}

void VirObjectNode::ExtractFunctionKeyValuePairs(const std::string& content) {
    // 提取CHTL JS函数中的键值对
    // 格式：key: value, key2: value2
    
    std::regex keyValuePattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(content.begin(), content.end(), keyValuePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string keyName = match[1].str();
        std::string keyValue = match[2].str();
        
        // 清理键值
        keyValue.erase(0, keyValue.find_first_not_of(" \t\n\r"));
        keyValue.erase(keyValue.find_last_not_of(" \t\n\r") + 1);
        
        // 分析键类型
        VirKeyType keyType = AnalyzeKeyType(keyValue);
        
        VirKey virKey(keyName, keyType, keyValue);
        
        // 如果是函数，提取函数签名
        if (keyType == VirKeyType::FUNCTION_KEY) {
            virKey.FunctionSignature = ExtractFunctionSignature(keyValue);
            m_HasFunctionKeys = true;
        }
        else if (keyType == VirKeyType::OBJECT_KEY || keyType == VirKeyType::ARRAY_KEY) {
            m_HasObjectKeys = true;
        }
        
        AddVirKey(virKey);
    }
}

VirKeyType VirObjectNode::AnalyzeKeyType(const std::string& value) const {
    // 分析键值类型
    
    // 检查是否为函数
    if (value.find("function") != std::string::npos || 
        value.find("()") != std::string::npos ||
        value.find("=>") != std::string::npos) {
        return VirKeyType::FUNCTION_KEY;
    }
    
    // 检查是否为对象
    if (value.front() == '{' && value.back() == '}') {
        return VirKeyType::OBJECT_KEY;
    }
    
    // 检查是否为数组
    if (value.front() == '[' && value.back() == ']') {
        return VirKeyType::ARRAY_KEY;
    }
    
    // 默认为普通值
    return VirKeyType::VALUE_KEY;
}

void VirObjectNode::AddVirKey(const VirKey& key) {
    m_VirKeys[key.Name] = key;
    
    // 根据键类型添加到相应的引用映射
    switch (key.Type) {
        case VirKeyType::FUNCTION_KEY:
            m_FunctionReferences[key.Name] = GenerateUniqueFunctionName(key.Name);
            break;
        case VirKeyType::OBJECT_KEY:
        case VirKeyType::ARRAY_KEY:
            m_ObjectReferences[key.Name] = key.Value;
            break;
        default:
            break;
    }
}

void VirObjectNode::AddFunctionKey(const std::string& keyName, 
                                  const std::string& functionContent,
                                  const std::string& signature) {
    VirKey key(keyName, VirKeyType::FUNCTION_KEY, functionContent);
    key.FunctionSignature = signature.empty() ? ExtractFunctionSignature(functionContent) : signature;
    AddVirKey(key);
}

void VirObjectNode::AddObjectKey(const std::string& keyName, const std::string& objectContent) {
    VirKey key(keyName, VirKeyType::OBJECT_KEY, objectContent);
    AddVirKey(key);
}

void VirObjectNode::AddArrayKey(const std::string& keyName, const std::string& arrayContent) {
    VirKey key(keyName, VirKeyType::ARRAY_KEY, arrayContent);
    AddVirKey(key);
}

void VirObjectNode::AddValueKey(const std::string& keyName, const std::string& value) {
    VirKey key(keyName, VirKeyType::VALUE_KEY, value);
    AddVirKey(key);
}

VirKey* VirObjectNode::GetVirKey(const std::string& keyName) {
    auto it = m_VirKeys.find(keyName);
    return (it != m_VirKeys.end()) ? &it->second : nullptr;
}

bool VirObjectNode::HasVirKey(const std::string& keyName) const {
    return m_VirKeys.find(keyName) != m_VirKeys.end();
}

std::string VirObjectNode::ResolveKeyReference(const std::string& keyName) const {
    auto it = m_VirKeys.find(keyName);
    if (it == m_VirKeys.end()) {
        return ""; // 键不存在
    }
    
    const VirKey& key = it->second;
    
    switch (key.Type) {
        case VirKeyType::FUNCTION_KEY:
            return ResolveFunctionReference(keyName);
        case VirKeyType::OBJECT_KEY:
        case VirKeyType::ARRAY_KEY:
            return ResolveObjectReference(keyName);
        case VirKeyType::VALUE_KEY:
            return key.Value;
        default:
            return "";
    }
}

std::string VirObjectNode::ResolveFunctionReference(const std::string& keyName) const {
    auto it = m_FunctionReferences.find(keyName);
    if (it != m_FunctionReferences.end()) {
        return it->second; // 返回函数引用
    }
    
    return "";
}

std::string VirObjectNode::ResolveObjectReference(const std::string& keyName) const {
    auto it = m_ObjectReferences.find(keyName);
    if (it != m_ObjectReferences.end()) {
        return it->second; // 返回对象值
    }
    
    return "";
}

std::string VirObjectNode::GenerateVirObjectCode() const {
    std::ostringstream code;
    
    // vir本身不存在，是编译期间的语法糖
    // 生成实际的JavaScript函数和对象
    
    code << "// Virtual Object: " << m_VirName << "\n";
    
    // 生成函数定义
    for (const auto& keyPair : m_VirKeys) {
        const VirKey& key = keyPair.second;
        
        if (key.Type == VirKeyType::FUNCTION_KEY) {
            std::string functionName = GenerateUniqueFunctionName(key.Name);
            code << "function " << functionName << key.FunctionSignature << " ";
            code << key.Value << "\n";
        }
    }
    
    // 生成对象定义
    for (const auto& keyPair : m_VirKeys) {
        const VirKey& key = keyPair.second;
        
        if (key.Type == VirKeyType::OBJECT_KEY || key.Type == VirKeyType::ARRAY_KEY) {
            code << "var " << m_VirName << "_" << key.Name << " = " << key.Value << ";\n";
        }
    }
    
    return code.str();
}

std::string VirObjectNode::GenerateKeyReferenceMap() const {
    std::ostringstream code;
    
    code << "// Key Reference Map for " << m_VirName << "\n";
    code << "var " << m_VirName << "_keyMap = {\n";
    
    bool first = true;
    for (const auto& keyPair : m_VirKeys) {
        const VirKey& key = keyPair.second;
        
        if (!first) {
            code << ",\n";
        }
        
        code << "  " << key.Name << ": ";
        
        switch (key.Type) {
            case VirKeyType::FUNCTION_KEY:
                code << GenerateUniqueFunctionName(key.Name);
                break;
            case VirKeyType::OBJECT_KEY:
            case VirKeyType::ARRAY_KEY:
                code << m_VirName << "_" << key.Name;
                break;
            case VirKeyType::VALUE_KEY:
                code << "'" << key.Value << "'";
                break;
        }
        
        first = false;
    }
    
    code << "\n};\n";
    
    return code.str();
}

std::string VirObjectNode::ExtractFunctionSignature(const std::string& functionContent) const {
    // 提取函数签名
    std::regex signaturePatterns[] = {
        std::regex(R"(function\s*(\([^)]*\)))"),        // function(params)
        std::regex(R"((\([^)]*\))\s*=>)"),              // (params) =>
        std::regex(R"((\(\))\s*=>)")                    // () =>
    };
    
    for (const auto& pattern : signaturePatterns) {
        std::smatch match;
        if (std::regex_search(functionContent, match, pattern)) {
            return match[1].str();
        }
    }
    
    return "()"; // 默认无参数
}

std::string VirObjectNode::GenerateUniqueFunctionName(const std::string& keyName) const {
    return "chtljs_vir_" + m_VirName + "_" + keyName;
}

bool VirObjectNode::IsValidVirName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 虚对象名称必须以字母开始，后续可以是字母、数字或下划线
    std::regex namePattern(R"([a-zA-Z][a-zA-Z0-9_]*)");
    return std::regex_match(name, namePattern);
}

void VirObjectNode::Accept(CHTLJSNodeVisitor* visitor) {
    if (visitor) {
        // visitor->VisitVirObjectNode(this);
        // 暂时使用基础访问
    }
}

std::unique_ptr<CHTLJSBaseNode> VirObjectNode::Clone() const {
    auto cloned = std::make_unique<VirObjectNode>(m_VirName, GetLine(), GetColumn());
    
    // 复制所有属性
    cloned->m_CHTLJSFunctionContent = m_CHTLJSFunctionContent;
    cloned->m_CHTLJSFunctionType = m_CHTLJSFunctionType;
    cloned->m_VirKeys = m_VirKeys;
    cloned->m_FunctionReferences = m_FunctionReferences;
    cloned->m_ObjectReferences = m_ObjectReferences;
    cloned->m_IsActive = m_IsActive;
    cloned->m_HasFunctionKeys = m_HasFunctionKeys;
    cloned->m_HasObjectKeys = m_HasObjectKeys;
    cloned->m_IsResolved = m_IsResolved;
    
    return cloned;
}

std::string VirObjectNode::GenerateJavaScript() const {
    std::ostringstream js;
    
    // 生成虚对象代码
    js << GenerateVirObjectCode();
    
    // 生成键引用映射
    js << GenerateKeyReferenceMap();
    
    return js.str();
}

std::string VirObjectNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "VirObjectNode 'vir " << m_VirName << "'";
    
    if (!m_CHTLJSFunctionType.empty()) {
        oss << " [Function: " << m_CHTLJSFunctionType << "]";
    }
    
    if (m_HasFunctionKeys) {
        oss << " [Function Keys]";
    }
    
    if (m_HasObjectKeys) {
        oss << " [Object Keys]";
    }
    
    if (m_IsActive) {
        oss << " [Active]";
    }
    
    if (m_IsResolved) {
        oss << " [Resolved]";
    }
    
    oss << " [Keys: " << m_VirKeys.size() << "]";
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出虚对象键详情
    if (!m_VirKeys.empty()) {
        oss << indentStr << "  Virtual Object Keys:\n";
        for (const auto& keyPair : m_VirKeys) {
            const VirKey& key = keyPair.second;
            
            oss << indentStr << "    " << key.Name << ": ";
            
            switch (key.Type) {
                case VirKeyType::FUNCTION_KEY:
                    oss << "[Function] " << key.FunctionSignature;
                    break;
                case VirKeyType::OBJECT_KEY:
                    oss << "[Object] " << key.Value.substr(0, 30);
                    if (key.Value.length() > 30) oss << "...";
                    break;
                case VirKeyType::ARRAY_KEY:
                    oss << "[Array] " << key.Value.substr(0, 30);
                    if (key.Value.length() > 30) oss << "...";
                    break;
                case VirKeyType::VALUE_KEY:
                    oss << "[Value] " << key.Value;
                    break;
            }
            
            oss << "\n";
        }
    }
    
    // 输出函数引用映射
    if (!m_FunctionReferences.empty()) {
        oss << indentStr << "  Function References:\n";
        for (const auto& funcRef : m_FunctionReferences) {
            oss << indentStr << "    " << funcRef.first << " -> " << funcRef.second << "\n";
        }
    }
    
    // 输出对象引用映射
    if (!m_ObjectReferences.empty()) {
        oss << indentStr << "  Object References:\n";
        for (const auto& objRef : m_ObjectReferences) {
            oss << indentStr << "    " << objRef.first << " -> " << objRef.second.substr(0, 30);
            if (objRef.second.length() > 30) oss << "...";
            oss << "\n";
        }
    }
    
    // 输出CHTL JS函数内容预览
    if (!m_CHTLJSFunctionContent.empty()) {
        oss << indentStr << "  CHTL JS Function Content: ";
        std::string preview = m_CHTLJSFunctionContent.substr(0, 60);
        std::replace(preview.begin(), preview.end(), '\n', ' ');
        oss << preview;
        if (m_CHTLJSFunctionContent.length() > 60) oss << "...";
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace CHTLJS