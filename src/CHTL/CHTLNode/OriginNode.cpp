#include "OriginNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

    // 静态成员初始化
    std::unordered_set<std::string> CHTLImplicitConfigManager::registeredCustomTypes;
    std::unordered_map<std::string, std::string> CHTLImplicitConfigManager::typeConfigs;

    // CHTLOriginNode实现
    CHTLOriginNode::CHTLOriginNode(CHTLOriginType type, const std::string& typeName, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::ORIGIN, name), 
          originType(type), typeName(typeName), originName(name), 
          isNamed(!name.empty()), isUsage(false), 
          isCustomType(type == CHTLOriginType::ORIGIN_CUSTOM), 
          willCreateImplicitConfig(type == CHTLOriginType::ORIGIN_CUSTOM) {
        
        // 如果是自定义类型，注册到隐式配置管理器
        if (isCustomType) {
            CHTLImplicitConfigManager::RegisterCustomType(typeName);
        }
    }

    // 原始嵌入类型管理实现
    CHTLOriginType CHTLOriginNode::GetOriginType() const {
        return originType;
    }

    void CHTLOriginNode::SetOriginType(CHTLOriginType type) {
        originType = type;
        isCustomType = (type == CHTLOriginType::ORIGIN_CUSTOM);
        willCreateImplicitConfig = isCustomType;
    }

    // 类型名称管理实现
    const std::string& CHTLOriginNode::GetTypeName() const {
        return typeName;
    }

    void CHTLOriginNode::SetTypeName(const std::string& typeNameValue) {
        typeName = typeNameValue;
        
        // 如果是自定义类型，注册到隐式配置管理器
        if (isCustomType) {
            CHTLImplicitConfigManager::RegisterCustomType(typeName);
        }
    }

    // 原始嵌入名称管理实现
    const std::string& CHTLOriginNode::GetOriginName() const {
        return originName;
    }

    void CHTLOriginNode::SetOriginName(const std::string& name) {
        originName = name;
        isNamed = !name.empty();
        SetNodeName(name);
    }

    // 原始内容管理实现
    const std::string& CHTLOriginNode::GetRawContent() const {
        return rawContent;
    }

    void CHTLOriginNode::SetRawContent(const std::string& content) {
        rawContent = content;
        SetNodeValue(content);
    }

    void CHTLOriginNode::AppendRawContent(const std::string& content) {
        rawContent += content;
        SetNodeValue(rawContent);
    }

    void CHTLOriginNode::ClearRawContent() {
        rawContent.clear();
        SetNodeValue("");
    }

    // 带名嵌入管理实现
    bool CHTLOriginNode::IsNamed() const {
        return isNamed;
    }

    void CHTLOriginNode::SetNamed(bool named) {
        isNamed = named;
        if (!named) {
            originName.clear();
            SetNodeName("");
        }
    }

    // 使用/定义标记实现
    bool CHTLOriginNode::IsUsage() const {
        return isUsage;
    }

    void CHTLOriginNode::SetIsUsage(bool usage) {
        isUsage = usage;
    }

    // 自定义类型管理实现
    bool CHTLOriginNode::IsCustomType() const {
        return isCustomType;
    }

    void CHTLOriginNode::SetCustomType(bool custom) {
        isCustomType = custom;
        willCreateImplicitConfig = custom;
        
        if (custom && !typeName.empty()) {
            CHTLImplicitConfigManager::RegisterCustomType(typeName);
        }
    }

    bool CHTLOriginNode::WillCreateImplicitConfig() const {
        return willCreateImplicitConfig;
    }

    void CHTLOriginNode::SetWillCreateImplicitConfig(bool create) {
        willCreateImplicitConfig = create;
    }

    // 核心特性：直接输出机制实现
    std::string CHTLOriginNode::GenerateDirectOutput() const {
        // 原始嵌入的核心特性：直接输出，绝对不会进行处理
        return rawContent;
    }

    bool CHTLOriginNode::IsEmpty() const {
        return rawContent.empty();
    }

    size_t CHTLOriginNode::GetContentLength() const {
        return rawContent.length();
    }

    // 验证方法实现
    bool CHTLOriginNode::IsValidOriginType() const {
        if (isCustomType) {
            // 自定义类型验证：必须以@为前缀
            return !typeName.empty() && typeName[0] == '@';
        } else {
            // 基本类型验证
            return IsBasicType();
        }
    }

    bool CHTLOriginNode::IsBasicType() const {
        return originType == CHTLOriginType::ORIGIN_HTML ||
               originType == CHTLOriginType::ORIGIN_STYLE ||
               originType == CHTLOriginType::ORIGIN_JAVASCRIPT;
    }

    bool CHTLOriginNode::RequiresAtPrefix() const {
        return true; // 所有原始嵌入类型都需要@前缀
    }

    // 重写虚方法实现
    std::string CHTLOriginNode::ToString() const {
        std::stringstream ss;
        ss << "Origin[" << OriginTypeToString(originType) << "]";
        ss << " type:\"" << typeName << "\"";
        if (isNamed) ss << " name:\"" << originName << "\"";
        if (isUsage) ss << " (usage)";
        if (isCustomType) ss << " (custom)";
        if (willCreateImplicitConfig) ss << " (implicit-config)";
        ss << " content:" << rawContent.length() << "chars";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLOriginNode::Clone() const {
        auto cloned = std::make_shared<CHTLOriginNode>(originType, typeName, originName);
        
        cloned->rawContent = rawContent;
        cloned->isNamed = isNamed;
        cloned->isUsage = isUsage;
        cloned->isCustomType = isCustomType;
        cloned->willCreateImplicitConfig = willCreateImplicitConfig;
        
        return cloned;
    }

    bool CHTLOriginNode::Validate() const {
        // 验证原始嵌入节点
        if (!IsValidOriginType()) return false;
        if (isNamed && originName.empty()) return false;
        if (isCustomType && !RequiresAtPrefix()) return false;
        
        return CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLOriginNode::OriginTypeToString(CHTLOriginType type) {
        switch (type) {
            case CHTLOriginType::ORIGIN_HTML: return "ORIGIN_HTML";
            case CHTLOriginType::ORIGIN_STYLE: return "ORIGIN_STYLE";
            case CHTLOriginType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
            case CHTLOriginType::ORIGIN_CUSTOM: return "ORIGIN_CUSTOM";
            default: return "UNKNOWN";
        }
    }

    bool CHTLOriginNode::IsValidTypeName(const std::string& typeNameValue) {
        if (typeNameValue.empty()) return false;
        
        // 类型名必须以@开头
        if (typeNameValue[0] != '@') return false;
        
        // 验证@后的内容
        std::string actualName = typeNameValue.substr(1);
        if (actualName.empty()) return false;
        
        // 字母开头，字母数字组成
        if (!std::isalpha(actualName[0])) return false;
        
        for (char c : actualName) {
            if (!std::isalnum(c)) {
                return false;
            }
        }
        
        return true;
    }

    bool CHTLOriginNode::IsValidOriginName(const std::string& originNameValue) {
        if (originNameValue.empty()) return true; // 空名称是有效的（匿名原始嵌入）
        
        // 原始嵌入名称验证：字母开头，字母数字下划线组成
        if (!std::isalpha(originNameValue[0])) return false;
        
        for (char c : originNameValue) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        
        return true;
    }

    // CHTLOriginUsageNode实现
    CHTLOriginUsageNode::CHTLOriginUsageNode(CHTLOriginType type, const std::string& typeNameValue, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::ORIGIN, name), 
          originType(type), typeName(typeNameValue), originName(name), 
          isCustomType(type == CHTLOriginType::ORIGIN_CUSTOM) {
    }

    // 原始嵌入使用管理实现
    CHTLOriginType CHTLOriginUsageNode::GetOriginType() const {
        return originType;
    }

    const std::string& CHTLOriginUsageNode::GetTypeName() const {
        return typeName;
    }

    const std::string& CHTLOriginUsageNode::GetOriginName() const {
        return originName;
    }

    void CHTLOriginUsageNode::SetOriginName(const std::string& name) {
        originName = name;
        SetNodeName(name);
    }

    // 自定义类型管理实现
    bool CHTLOriginUsageNode::IsCustomType() const {
        return isCustomType;
    }

    void CHTLOriginUsageNode::SetCustomType(bool custom) {
        isCustomType = custom;
    }

    // 重写虚方法实现
    std::string CHTLOriginUsageNode::ToString() const {
        std::stringstream ss;
        ss << "OriginUsage[" << CHTLOriginNode::OriginTypeToString(originType) << "]";
        ss << " type:\"" << typeName << "\"";
        ss << " name:\"" << originName << "\"";
        if (isCustomType) ss << " (custom)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLOriginUsageNode::Clone() const {
        auto cloned = std::make_shared<CHTLOriginUsageNode>(originType, typeName, originName);
        cloned->isCustomType = isCustomType;
        return cloned;
    }

    bool CHTLOriginUsageNode::Validate() const {
        return CHTLOriginNode::IsValidTypeName(typeName) && 
               CHTLOriginNode::IsValidOriginName(originName) && 
               CHTLBaseNode::Validate();
    }

    // CHTLOriginNodeFactory实现
    std::shared_ptr<CHTLOriginNode> CHTLOriginNodeFactory::CreateHtmlOriginNode(const std::string& name) {
        return std::make_shared<CHTLOriginNode>(CHTLOriginType::ORIGIN_HTML, "@Html", name);
    }

    std::shared_ptr<CHTLOriginNode> CHTLOriginNodeFactory::CreateStyleOriginNode(const std::string& name) {
        return std::make_shared<CHTLOriginNode>(CHTLOriginType::ORIGIN_STYLE, "@Style", name);
    }

    std::shared_ptr<CHTLOriginNode> CHTLOriginNodeFactory::CreateJavaScriptOriginNode(const std::string& name) {
        return std::make_shared<CHTLOriginNode>(CHTLOriginType::ORIGIN_JAVASCRIPT, "@JavaScript", name);
    }

    std::shared_ptr<CHTLOriginNode> CHTLOriginNodeFactory::CreateCustomOriginNode(const std::string& typeNameValue, const std::string& name) {
        // 确保类型名有@前缀
        std::string fullTypeName = (typeNameValue[0] == '@') ? typeNameValue : "@" + typeNameValue;
        return std::make_shared<CHTLOriginNode>(CHTLOriginType::ORIGIN_CUSTOM, fullTypeName, name);
    }

    std::shared_ptr<CHTLOriginUsageNode> CHTLOriginNodeFactory::CreateHtmlUsageNode(const std::string& name) {
        return std::make_shared<CHTLOriginUsageNode>(CHTLOriginType::ORIGIN_HTML, "@Html", name);
    }

    std::shared_ptr<CHTLOriginUsageNode> CHTLOriginNodeFactory::CreateStyleUsageNode(const std::string& name) {
        return std::make_shared<CHTLOriginUsageNode>(CHTLOriginType::ORIGIN_STYLE, "@Style", name);
    }

    std::shared_ptr<CHTLOriginUsageNode> CHTLOriginNodeFactory::CreateJavaScriptUsageNode(const std::string& name) {
        return std::make_shared<CHTLOriginUsageNode>(CHTLOriginType::ORIGIN_JAVASCRIPT, "@JavaScript", name);
    }

    std::shared_ptr<CHTLOriginUsageNode> CHTLOriginNodeFactory::CreateCustomUsageNode(const std::string& typeNameValue, const std::string& name) {
        std::string fullTypeName = (typeNameValue[0] == '@') ? typeNameValue : "@" + typeNameValue;
        return std::make_shared<CHTLOriginUsageNode>(CHTLOriginType::ORIGIN_CUSTOM, fullTypeName, name);
    }

    // 解析方法实现
    CHTLOriginType CHTLOriginNodeFactory::ParseOriginType(const std::string& typeText) {
        if (typeText == "@Html") return CHTLOriginType::ORIGIN_HTML;
        if (typeText == "@Style") return CHTLOriginType::ORIGIN_STYLE;
        if (typeText == "@JavaScript") return CHTLOriginType::ORIGIN_JAVASCRIPT;
        return CHTLOriginType::ORIGIN_CUSTOM; // 其他都是自定义类型
    }

    std::shared_ptr<CHTLOriginNode> CHTLOriginNodeFactory::ParseOrigin(const std::string& originText) {
        // 简化解析实现
        if (originText.find("@Html") != std::string::npos) {
            return CreateHtmlOriginNode();
        } else if (originText.find("@Style") != std::string::npos) {
            return CreateStyleOriginNode();
        } else if (originText.find("@JavaScript") != std::string::npos) {
            return CreateJavaScriptOriginNode();
        } else {
            return CreateCustomOriginNode("@Vue"); // 示例自定义类型
        }
    }

    // 验证方法实现
    bool CHTLOriginNodeFactory::IsBasicOriginType(const std::string& typeNameValue) {
        return typeNameValue == "@Html" || typeNameValue == "@Style" || typeNameValue == "@JavaScript";
    }

    bool CHTLOriginNodeFactory::IsValidCustomTypeName(const std::string& typeNameValue) {
        return CHTLOriginNode::IsValidTypeName(typeNameValue) && !IsBasicOriginType(typeNameValue);
    }

    bool CHTLOriginNodeFactory::RequiresAtPrefix(const std::string& typeNameValue) {
        return typeNameValue.empty() || typeNameValue[0] != '@';
    }

    // CHTLImplicitConfigManager实现
    void CHTLImplicitConfigManager::RegisterCustomType(const std::string& typeNameValue) {
        if (!typeNameValue.empty()) {
            registeredCustomTypes.insert(typeNameValue);
            
            // 生成隐式配置
            std::string config = GenerateImplicitConfig(typeNameValue);
            if (!config.empty()) {
                typeConfigs[typeNameValue] = config;
            }
        }
    }

    bool CHTLImplicitConfigManager::IsCustomTypeRegistered(const std::string& typeNameValue) {
        return registeredCustomTypes.find(typeNameValue) != registeredCustomTypes.end();
    }

    const std::unordered_set<std::string>& CHTLImplicitConfigManager::GetRegisteredTypes() {
        return registeredCustomTypes;
    }

    std::string CHTLImplicitConfigManager::GenerateImplicitConfig(const std::string& typeNameValue) {
        if (typeNameValue.empty() || typeNameValue[0] != '@') return "";
        
        // 生成隐式配置块
        // 按照语法文档：ORIGINTYPE_全写的类型名称 = @全大写后 = 全写的类型名称的内容
        std::string typeName = typeNameValue.substr(1); // 移除@前缀
        std::string upperTypeName = typeName;
        std::transform(upperTypeName.begin(), upperTypeName.end(), upperTypeName.begin(), ::toupper);
        
        std::stringstream ss;
        ss << "[OriginType]\n";
        ss << "{\n";
        ss << "    ORIGINTYPE_" << upperTypeName << " = " << typeNameValue << ";\n";
        ss << "}\n";
        
        return ss.str();
    }

    void CHTLImplicitConfigManager::AddTypeConfig(const std::string& typeNameValue, const std::string& config) {
        typeConfigs[typeNameValue] = config;
    }

    std::string CHTLImplicitConfigManager::GetTypeConfig(const std::string& typeNameValue) {
        auto it = typeConfigs.find(typeNameValue);
        return (it != typeConfigs.end()) ? it->second : "";
    }

    void CHTLImplicitConfigManager::ClearRegistrations() {
        registeredCustomTypes.clear();
        typeConfigs.clear();
    }

    std::vector<std::string> CHTLImplicitConfigManager::GetAllImplicitConfigs() {
        std::vector<std::string> configs;
        for (const auto& pair : typeConfigs) {
            configs.push_back(pair.second);
        }
        return configs;
    }

    bool CHTLImplicitConfigManager::ValidateTypeName(const std::string& typeNameValue) {
        return CHTLOriginNode::IsValidTypeName(typeNameValue);
    }

    bool CHTLImplicitConfigManager::RequiresImplicitConfig(const std::string& typeNameValue) {
        return !CHTLOriginNodeFactory::IsBasicOriginType(typeNameValue);
    }

} // namespace CHTL