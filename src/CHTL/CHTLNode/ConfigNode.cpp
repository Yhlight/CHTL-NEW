#include "ConfigNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

    // 静态成员初始化
    std::vector<std::shared_ptr<CHTLConfigNode>> CHTLConfigConflictDetector::allConfigs;
    std::shared_ptr<CHTLConfigNode> CHTLConfigConflictDetector::activeConfig;

    // CHTLConfigNode实现
    CHTLConfigNode::CHTLConfigNode(CHTLConfigType type, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::CONFIGURATION, name), 
          configType(type), configName(name), isNamed(!name.empty()), 
          isActive(!isNamed) {  // 无名配置组才启用
        
        // 注册到冲突检测器的操作移到工厂方法中
    }

    // 配置类型管理实现
    CHTLConfigType CHTLConfigNode::GetConfigType() const {
        return configType;
    }

    void CHTLConfigNode::SetConfigType(CHTLConfigType type) {
        configType = type;
    }

    // 配置名称管理实现
    const std::string& CHTLConfigNode::GetConfigName() const {
        return configName;
    }

    void CHTLConfigNode::SetConfigName(const std::string& name) {
        configName = name;
        isNamed = !name.empty();
        isActive = !isNamed; // 命名配置组不被使用
        SetNodeName(name);
    }

    bool CHTLConfigNode::IsNamed() const {
        return isNamed;
    }

    void CHTLConfigNode::SetNamed(bool named) {
        isNamed = named;
        isActive = !named; // 命名配置组不被使用，无名配置组才启用
    }

    // 活跃状态管理实现
    bool CHTLConfigNode::IsActive() const {
        return isActive;
    }

    void CHTLConfigNode::SetActive(bool active) {
        isActive = active;
    }

    // 配置项管理实现
    void CHTLConfigNode::AddConfigItem(const std::string& key, const std::string& value, CHTLConfigItemType type) {
        configItems[key] = value;
        itemTypes[key] = type;
    }

    void CHTLConfigNode::RemoveConfigItem(const std::string& key) {
        configItems.erase(key);
        itemTypes.erase(key);
    }

    std::string CHTLConfigNode::GetConfigItem(const std::string& key) const {
        auto it = configItems.find(key);
        return (it != configItems.end()) ? it->second : "";
    }

    bool CHTLConfigNode::HasConfigItem(const std::string& key) const {
        return configItems.find(key) != configItems.end();
    }

    CHTLConfigItemType CHTLConfigNode::GetConfigItemType(const std::string& key) const {
        auto it = itemTypes.find(key);
        return (it != itemTypes.end()) ? it->second : CHTLConfigItemType::STRING_CONFIG;
    }

    const std::unordered_map<std::string, std::string>& CHTLConfigNode::GetAllConfigItems() const {
        return configItems;
    }

    // 重写虚方法实现
    std::string CHTLConfigNode::ToString() const {
        std::stringstream ss;
        ss << "Config[" << ConfigTypeToString(configType) << "]";
        if (isNamed) ss << " name:\"" << configName << "\"";
        ss << " items:" << configItems.size();
        ss << " active:" << (isActive ? "是" : "否");
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLConfigNode::Clone() const {
        auto cloned = std::make_shared<CHTLConfigNode>(configType, configName);
        
        cloned->isNamed = isNamed;
        cloned->isActive = isActive;
        cloned->configItems = configItems;
        cloned->itemTypes = itemTypes;
        
        return cloned;
    }

    bool CHTLConfigNode::Validate() const {
        // 验证配置节点
        if (isNamed && configName.empty()) return false;
        
        // 验证配置项
        for (const auto& item : configItems) {
            if (item.first.empty()) return false;
        }
        
        return CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLConfigNode::ConfigTypeToString(CHTLConfigType type) {
        switch (type) {
            case CHTLConfigType::CONFIG_BASIC: return "CONFIG_BASIC";
            case CHTLConfigType::CONFIG_NAMED: return "CONFIG_NAMED";
            case CHTLConfigType::CONFIG_NAME_BLOCK: return "CONFIG_NAME_BLOCK";
            case CHTLConfigType::CONFIG_ORIGIN_TYPE: return "CONFIG_ORIGIN_TYPE";
            default: return "UNKNOWN";
        }
    }

    std::string CHTLConfigNode::ConfigItemTypeToString(CHTLConfigItemType type) {
        switch (type) {
            case CHTLConfigItemType::BOOLEAN_CONFIG: return "BOOLEAN_CONFIG";
            case CHTLConfigItemType::INTEGER_CONFIG: return "INTEGER_CONFIG";
            case CHTLConfigItemType::STRING_CONFIG: return "STRING_CONFIG";
            case CHTLConfigItemType::GROUP_CONFIG: return "GROUP_CONFIG";
            default: return "UNKNOWN";
        }
    }

    // CHTLNameConfigNode实现
    CHTLNameConfigNode::CHTLNameConfigNode() 
        : CHTLBaseNode(CHTLNodeType::NAME, "name"), optionCountLimit(3) {
        
        // 初始化默认关键字映射
        keywordMappings["CUSTOM_STYLE"] = "@Style";
        keywordMappings["CUSTOM_ELEMENT"] = "@Element";
        keywordMappings["CUSTOM_VAR"] = "@Var";
        keywordMappings["TEMPLATE_STYLE"] = "@Style";
        keywordMappings["TEMPLATE_ELEMENT"] = "@Element";
        keywordMappings["TEMPLATE_VAR"] = "@Var";
        keywordMappings["ORIGIN_HTML"] = "@Html";
        keywordMappings["ORIGIN_STYLE"] = "@Style";
        keywordMappings["ORIGIN_JAVASCRIPT"] = "@JavaScript";
        keywordMappings["IMPORT_HTML"] = "@Html";
        keywordMappings["IMPORT_STYLE"] = "@Style";
        keywordMappings["IMPORT_JAVASCRIPT"] = "@JavaScript";
        keywordMappings["IMPORT_CHTL"] = "@Chtl";
        keywordMappings["IMPORT_CRMOD"] = "@CJmod";
        keywordMappings["KEYWORD_INHERIT"] = "inherit";
        keywordMappings["KEYWORD_DELETE"] = "delete";
        keywordMappings["KEYWORD_INSERT"] = "insert";
        keywordMappings["KEYWORD_AFTER"] = "after";
        keywordMappings["KEYWORD_BEFORE"] = "before";
        keywordMappings["KEYWORD_REPLACE"] = "replace";
        keywordMappings["KEYWORD_ATTOP"] = "at top";
        keywordMappings["KEYWORD_ATBOTTOM"] = "at bottom";
        keywordMappings["KEYWORD_FROM"] = "from";
        keywordMappings["KEYWORD_AS"] = "as";
        keywordMappings["KEYWORD_EXCEPT"] = "except";
        keywordMappings["KEYWORD_USE"] = "use";
        keywordMappings["KEYWORD_HTML5"] = "html5";
        keywordMappings["KEYWORD_TEXT"] = "text";
        keywordMappings["KEYWORD_STYLE"] = "style";
        keywordMappings["KEYWORD_SCRIPT"] = "script";
        keywordMappings["KEYWORD_CUSTOM"] = "[Custom]";
        keywordMappings["KEYWORD_TEMPLATE"] = "[Template]";
        keywordMappings["KEYWORD_ORIGIN"] = "[Origin]";
        keywordMappings["KEYWORD_IMPORT"] = "[Import]";
        keywordMappings["KEYWORD_NAMESPACE"] = "[Namespace]";
    }

    // 关键字映射管理实现
    void CHTLNameConfigNode::AddKeywordMapping(const std::string& keyword, const std::string& value) {
        keywordMappings[keyword] = value;
    }

    void CHTLNameConfigNode::RemoveKeywordMapping(const std::string& keyword) {
        keywordMappings.erase(keyword);
    }

    std::string CHTLNameConfigNode::GetKeywordMapping(const std::string& keyword) const {
        auto it = keywordMappings.find(keyword);
        return (it != keywordMappings.end()) ? it->second : "";
    }

    bool CHTLNameConfigNode::HasKeywordMapping(const std::string& keyword) const {
        return keywordMappings.find(keyword) != keywordMappings.end();
    }

    // 组选项管理实现
    void CHTLNameConfigNode::AddGroupOption(const std::string& groupName, const std::vector<std::string>& options) {
        // 检查选项数量限制
        if (options.size() <= static_cast<size_t>(optionCountLimit)) {
            groupOptions[groupName] = options;
        }
    }

    void CHTLNameConfigNode::AddOptionToGroup(const std::string& groupName, const std::string& option) {
        auto& options = groupOptions[groupName];
        if (options.size() < static_cast<size_t>(optionCountLimit)) {
            options.push_back(option);
        }
    }

    void CHTLNameConfigNode::RemoveGroupOption(const std::string& groupName) {
        groupOptions.erase(groupName);
    }

    std::vector<std::string> CHTLNameConfigNode::GetGroupOptions(const std::string& groupName) const {
        auto it = groupOptions.find(groupName);
        return (it != groupOptions.end()) ? it->second : std::vector<std::string>();
    }

    bool CHTLNameConfigNode::HasGroupOption(const std::string& groupName) const {
        return groupOptions.find(groupName) != groupOptions.end();
    }

    // 选项数量限制管理实现
    int CHTLNameConfigNode::GetOptionCountLimit() const {
        return optionCountLimit;
    }

    void CHTLNameConfigNode::SetOptionCountLimit(int limit) {
        optionCountLimit = limit;
    }

    bool CHTLNameConfigNode::IsWithinOptionLimit(const std::string& groupName) const {
        auto it = groupOptions.find(groupName);
        if (it != groupOptions.end()) {
            return it->second.size() <= static_cast<size_t>(optionCountLimit);
        }
        return true;
    }

    // 重写虚方法实现
    std::string CHTLNameConfigNode::ToString() const {
        std::stringstream ss;
        ss << "NameConfig";
        ss << " keywords:" << keywordMappings.size();
        ss << " groups:" << groupOptions.size();
        ss << " limit:" << optionCountLimit;
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLNameConfigNode::Clone() const {
        auto cloned = std::make_shared<CHTLNameConfigNode>();
        
        cloned->keywordMappings = keywordMappings;
        cloned->groupOptions = groupOptions;
        cloned->optionCountLimit = optionCountLimit;
        
        return cloned;
    }

    bool CHTLNameConfigNode::Validate() const {
        // 验证所有组选项是否在限制内
        for (const auto& group : groupOptions) {
            if (group.second.size() > static_cast<size_t>(optionCountLimit)) {
                return false;
            }
        }
        
        return CHTLBaseNode::Validate();
    }

    // CHTLOriginTypeConfigNode实现
    CHTLOriginTypeConfigNode::CHTLOriginTypeConfigNode() 
        : CHTLBaseNode(CHTLNodeType::ORIGIN_TYPE, "origintype") {
    }

    // 原始嵌入类型映射管理实现
    void CHTLOriginTypeConfigNode::AddOriginTypeMapping(const std::string& typeName, const std::string& value) {
        // 验证命名约定
        if (FollowsNamingConvention(typeName, value)) {
            originTypeMappings[typeName] = value;
        }
    }

    void CHTLOriginTypeConfigNode::RemoveOriginTypeMapping(const std::string& typeName) {
        originTypeMappings.erase(typeName);
    }

    std::string CHTLOriginTypeConfigNode::GetOriginTypeMapping(const std::string& typeName) const {
        auto it = originTypeMappings.find(typeName);
        return (it != originTypeMappings.end()) ? it->second : "";
    }

    bool CHTLOriginTypeConfigNode::HasOriginTypeMapping(const std::string& typeName) const {
        return originTypeMappings.find(typeName) != originTypeMappings.end();
    }

    const std::unordered_map<std::string, std::string>& CHTLOriginTypeConfigNode::GetAllOriginTypeMappings() const {
        return originTypeMappings;
    }

    // 类型验证实现
    bool CHTLOriginTypeConfigNode::IsValidOriginTypeDefinition(const std::string& typeName, const std::string& value) const {
        // 验证类型定义是否有效
        return !typeName.empty() && !value.empty() && 
               typeName.substr(0, 11) == "ORIGINTYPE_" && 
               value[0] == '@';
    }

    bool CHTLOriginTypeConfigNode::FollowsNamingConvention(const std::string& typeName, const std::string& value) const {
        // 检查是否遵循命名约定：ORIGINTYPE_全写的类型名称 = @全大写后 = 全写的类型名称的内容
        if (typeName.length() <= 11 || typeName.substr(0, 11) != "ORIGINTYPE_") {
            return false;
        }
        
        std::string typeNamePart = typeName.substr(11); // 移除ORIGINTYPE_前缀
        std::string valuePart = value.substr(1); // 移除@前缀
        
        // 类型名称应该是全大写，值应该是首字母大写
        std::string expectedValue = valuePart;
        std::transform(typeNamePart.begin(), typeNamePart.end(), typeNamePart.begin(), ::tolower);
        expectedValue[0] = std::toupper(expectedValue[0]);
        
        return typeNamePart == expectedValue || 
               std::equal(typeNamePart.begin(), typeNamePart.end(), valuePart.begin(), 
                         [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    }

    // 重写虚方法实现
    std::string CHTLOriginTypeConfigNode::ToString() const {
        std::stringstream ss;
        ss << "OriginTypeConfig";
        ss << " mappings:" << originTypeMappings.size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLOriginTypeConfigNode::Clone() const {
        auto cloned = std::make_shared<CHTLOriginTypeConfigNode>();
        cloned->originTypeMappings = originTypeMappings;
        return cloned;
    }

    bool CHTLOriginTypeConfigNode::Validate() const {
        // 验证所有类型映射是否遵循命名约定
        for (const auto& mapping : originTypeMappings) {
            if (!IsValidOriginTypeDefinition(mapping.first, mapping.second)) {
                return false;
            }
        }
        
        return CHTLBaseNode::Validate();
    }

    // CHTLConfigConflictDetector实现
    void CHTLConfigConflictDetector::RegisterConfig(std::shared_ptr<CHTLConfigNode> config) {
        if (config) {
            allConfigs.push_back(config);
            
            // 如果是无名配置且当前没有活跃配置，设为活跃
            if (!config->IsNamed() && !activeConfig) {
                activeConfig = config;
                config->SetActive(true);
            }
        }
    }

    void CHTLConfigConflictDetector::UnregisterConfig(std::shared_ptr<CHTLConfigNode> config) {
        auto it = std::find(allConfigs.begin(), allConfigs.end(), config);
        if (it != allConfigs.end()) {
            allConfigs.erase(it);
        }
        
        if (activeConfig == config) {
            activeConfig = nullptr;
            DetermineActiveConfig();
        }
    }

    bool CHTLConfigConflictDetector::HasConfigConflict() {
        return HasMultipleUnnamedConfigs();
    }

    std::vector<std::shared_ptr<CHTLConfigNode>> CHTLConfigConflictDetector::GetConflictingConfigs() {
        std::vector<std::shared_ptr<CHTLConfigNode>> conflicts;
        
        auto unnamedConfigs = GetUnnamedConfigs();
        if (unnamedConfigs.size() > 1) {
            conflicts = unnamedConfigs;
        }
        
        return conflicts;
    }

    bool CHTLConfigConflictDetector::HasMultipleUnnamedConfigs() {
        return GetUnnamedConfigs().size() > 1;
    }

    std::shared_ptr<CHTLConfigNode> CHTLConfigConflictDetector::GetActiveConfig() {
        return activeConfig;
    }

    void CHTLConfigConflictDetector::SetActiveConfig(std::shared_ptr<CHTLConfigNode> config) {
        // 取消之前的活跃配置
        if (activeConfig) {
            activeConfig->SetActive(false);
        }
        
        activeConfig = config;
        if (config) {
            config->SetActive(true);
        }
    }

    void CHTLConfigConflictDetector::DetermineActiveConfig() {
        // 确定活跃配置：无名配置组才启用
        auto unnamedConfigs = GetUnnamedConfigs();
        
        if (unnamedConfigs.size() == 1) {
            SetActiveConfig(unnamedConfigs[0]);
        } else if (unnamedConfigs.empty()) {
            // 如果没有无名配置，使用第一个有名配置
            auto namedConfigs = GetNamedConfigs();
            if (!namedConfigs.empty()) {
                SetActiveConfig(namedConfigs[0]);
            }
        } else {
            // 多个无名配置冲突
            activeConfig = nullptr;
        }
    }

    void CHTLConfigConflictDetector::ClearAllConfigs() {
        allConfigs.clear();
        activeConfig = nullptr;
    }

    std::vector<std::shared_ptr<CHTLConfigNode>> CHTLConfigConflictDetector::GetNamedConfigs() {
        std::vector<std::shared_ptr<CHTLConfigNode>> namedConfigs;
        
        for (const auto& config : allConfigs) {
            if (config->IsNamed()) {
                namedConfigs.push_back(config);
            }
        }
        
        return namedConfigs;
    }

    std::vector<std::shared_ptr<CHTLConfigNode>> CHTLConfigConflictDetector::GetUnnamedConfigs() {
        std::vector<std::shared_ptr<CHTLConfigNode>> unnamedConfigs;
        
        for (const auto& config : allConfigs) {
            if (!config->IsNamed()) {
                unnamedConfigs.push_back(config);
            }
        }
        
        return unnamedConfigs;
    }

    bool CHTLConfigConflictDetector::ValidateConfigSetup() {
        return !HasConfigConflict();
    }

    // CHTLConfigNodeFactory实现
    std::shared_ptr<CHTLConfigNode> CHTLConfigNodeFactory::CreateBasicConfigNode() {
        auto config = std::make_shared<CHTLConfigNode>(CHTLConfigType::CONFIG_BASIC);
        
        // 添加默认配置项
        config->AddConfigItem("INDEX_INITIAL_COUNT", "0", CHTLConfigItemType::INTEGER_CONFIG);
        config->AddConfigItem("DISABLE_NAME_GROUP", "true", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DISABLE_CUSTOM_ORIGIN_TYPE", "false", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DEBUG_MODE", "false", CHTLConfigItemType::BOOLEAN_CONFIG);
        
        // 注册到冲突检测器
        CHTLConfigConflictDetector::RegisterConfig(config);
        
        return config;
    }

    std::shared_ptr<CHTLConfigNode> CHTLConfigNodeFactory::CreateNamedConfigNode(const std::string& configName) {
        auto config = std::make_shared<CHTLConfigNode>(CHTLConfigType::CONFIG_NAMED, configName);
        CHTLConfigConflictDetector::RegisterConfig(config);
        return config;
    }

    std::shared_ptr<CHTLNameConfigNode> CHTLConfigNodeFactory::CreateNameConfigNode() {
        return std::make_shared<CHTLNameConfigNode>();
    }

    std::shared_ptr<CHTLOriginTypeConfigNode> CHTLConfigNodeFactory::CreateOriginTypeConfigNode() {
        return std::make_shared<CHTLOriginTypeConfigNode>();
    }

    std::shared_ptr<CHTLConfigNode> CHTLConfigNodeFactory::CreateDefaultConfig() {
        auto config = CreateBasicConfigNode();
        
        // 添加选择器自动化配置
        config->AddConfigItem("DISABLE_STYLE_AUTO_ADD_CLASS", "false", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DISABLE_STYLE_AUTO_ADD_ID", "false", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DISABLE_SCRIPT_AUTO_ADD_ID", "true", CHTLConfigItemType::BOOLEAN_CONFIG);
        config->AddConfigItem("DISABLE_DEFAULT_NAMESPACE", "false", CHTLConfigItemType::BOOLEAN_CONFIG);
        
        return config;
    }

    std::shared_ptr<CHTLNameConfigNode> CHTLConfigNodeFactory::CreateDefaultNameConfig() {
        auto nameConfig = CreateNameConfigNode();
        
        // 添加组选项示例：CUSTOM_STYLE具有多个值
        nameConfig->AddGroupOption("CUSTOM_STYLE", {"@Style", "@style", "@CSS", "@Css", "@css"});
        
        return nameConfig;
    }

    // 解析方法实现
    CHTLConfigType CHTLConfigNodeFactory::ParseConfigType(const std::string& configText) {
        if (configText.find("[Name]") != std::string::npos) {
            return CHTLConfigType::CONFIG_NAME_BLOCK;
        } else if (configText.find("[OriginType]") != std::string::npos) {
            return CHTLConfigType::CONFIG_ORIGIN_TYPE;
        } else if (configText.find("@Config") != std::string::npos) {
            return CHTLConfigType::CONFIG_NAMED;
        } else {
            return CHTLConfigType::CONFIG_BASIC;
        }
    }

    CHTLConfigItemType CHTLConfigNodeFactory::ParseConfigItemType(const std::string& value) {
        if (value == "true" || value == "false") {
            return CHTLConfigItemType::BOOLEAN_CONFIG;
        } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
            return CHTLConfigItemType::INTEGER_CONFIG;
        } else if (value.front() == '[' && value.back() == ']') {
            return CHTLConfigItemType::GROUP_CONFIG;
        } else {
            return CHTLConfigItemType::STRING_CONFIG;
        }
    }

    bool CHTLConfigNodeFactory::IsValidConfigName(const std::string& name) {
        if (name.empty()) return true; // 空名称表示无名配置，是有效的
        
        // 配置名验证：字母开头，字母数字下划线组成
        if (!std::isalpha(name[0])) return false;
        
        for (char c : name) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        
        return true;
    }

    bool CHTLConfigNodeFactory::IsValidConfigItem(const std::string& key, const std::string& value) {
        return !key.empty() && !value.empty();
    }

    bool CHTLConfigNodeFactory::IsValidGroupOption(const std::vector<std::string>& options) {
        // 验证组选项：不能为空，且每个选项都有效
        if (options.empty()) return false;
        
        for (const auto& option : options) {
            if (option.empty()) return false;
        }
        
        return true;
    }

} // namespace CHTL