#include "ConfigurationManager.h"
#include <regex>
#include <iostream>
#include <sstream>

namespace CHTL {

ConfigurationManager::ConfigurationManager() {
    InitializeDefaultConfigurations();
}

void ConfigurationManager::InitializeDefaultConfigurations() {
    // 初始化默认配置值
    m_DefaultValues = {
        {"INDEX_INITIAL_COUNT", "0"},
        {"DISABLE_NAME_GROUP", "true"},
        {"DISABLE_CUSTOM_ORIGIN_TYPE", "false"},
        {"DEBUG_MODE", "false"},
        {"DISABLE_STYLE_AUTO_ADD_CLASS", "false"},
        {"DISABLE_STYLE_AUTO_ADD_ID", "false"},
        {"DISABLE_SCRIPT_AUTO_ADD_CLASS", "true"},
        {"DISABLE_SCRIPT_AUTO_ADD_ID", "true"},
        {"DISABLE_DEFAULT_NAMESPACE", "false"},
        {"OPTION_COUNT", "3"}
    };
    
    std::cout << "📋 配置管理器初始化完成，加载默认配置" << std::endl;
}

void ConfigurationManager::RegisterConfiguration(const std::string& configContent,
                                                const std::string& configName,
                                                const std::string& namespaceName) {
    auto configBlock = std::make_unique<ConfigurationBlock>();
    configBlock->Name = configName;
    configBlock->Type = configName.empty() ? ConfigurationType::GLOBAL_CONFIG : ConfigurationType::NAMED_CONFIG;
    configBlock->Namespace = namespaceName.empty() ? m_CurrentNamespace : namespaceName;
    
    // 解析配置内容
    configBlock->Items = ParseConfigurationContent(configContent);
    
    std::string fullName = configBlock->Namespace.empty() ? configName : configBlock->Namespace + "::" + configName;
    
    if (configName.empty()) {
        // 全局配置
        m_GlobalConfigs["global"] = std::move(configBlock);
        std::cout << "📋 注册全局配置块" << std::endl;
    } else {
        // 命名配置
        m_NamedConfigs[fullName] = std::move(configBlock);
        std::cout << "📋 注册命名配置块: " << fullName << std::endl;
    }
}

void ConfigurationManager::ActivateConfiguration(const std::string& configName) {
    if (configName.empty()) {
        // 激活全局配置
        auto it = m_GlobalConfigs.find("global");
        if (it != m_GlobalConfigs.end()) {
            m_ActiveConfig = std::make_unique<ConfigurationBlock>(*it->second);
            m_ActiveConfig->IsActive = true;
            std::cout << "✅ 激活全局配置" << std::endl;
        }
    } else {
        // 激活命名配置
        auto it = m_NamedConfigs.find(configName);
        if (it != m_NamedConfigs.end()) {
            m_ActiveConfig = std::make_unique<ConfigurationBlock>(*it->second);
            m_ActiveConfig->IsActive = true;
            std::cout << "✅ 激活命名配置: " << configName << std::endl;
        }
    }
}

std::string ConfigurationManager::GetConfigValue(const std::string& key, const std::string& defaultValue) const {
    // 首先检查激活的配置
    if (m_ActiveConfig && m_ActiveConfig->Items.find(key) != m_ActiveConfig->Items.end()) {
        return m_ActiveConfig->Items.at(key).Value;
    }
    
    // 检查默认配置
    auto it = m_DefaultValues.find(key);
    if (it != m_DefaultValues.end()) {
        return it->second;
    }
    
    return defaultValue;
}

bool ConfigurationManager::GetConfigBool(const std::string& key, bool defaultValue) const {
    std::string value = GetConfigValue(key, defaultValue ? "true" : "false");
    return value == "true" || value == "1" || value == "yes";
}

int ConfigurationManager::GetConfigInt(const std::string& key, int defaultValue) const {
    std::string value = GetConfigValue(key, std::to_string(defaultValue));
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

std::vector<std::string> ConfigurationManager::GetGroupOptionValues(const std::string& key) const {
    if (m_ActiveConfig && m_ActiveConfig->Items.find(key) != m_ActiveConfig->Items.end()) {
        const auto& item = m_ActiveConfig->Items.at(key);
        if (item.IsGroupOption) {
            return item.GroupValues;
        }
    }
    
    return {};
}

std::vector<std::string> ConfigurationManager::GetNameConfigKeywords(const std::string& keyword) const {
    return GetGroupOptionValues(keyword);
}

std::string ConfigurationManager::GetOriginTypeConfig(const std::string& typeName) const {
    std::string key = "ORIGINTYPE_" + typeName;
    return GetConfigValue(key);
}

bool ConfigurationManager::IsFeatureDisabled(const std::string& featureName) const {
    return GetConfigBool("DISABLE_" + featureName, false);
}

std::unordered_map<std::string, ConfigurationItem> ConfigurationManager::ParseConfigurationContent(const std::string& configContent) {
    std::unordered_map<std::string, ConfigurationItem> items;
    
    std::istringstream stream(configContent);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 去除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || 
            (line.length() >= 2 && line.substr(0, 2) == "//") || 
            (line.length() >= 2 && line.substr(0, 2) == "/*")) {
            continue;
        }
        
        // 解析配置项: KEY = VALUE;
        std::regex configRegex(R"(([A-Z_][A-Z0-9_]*)\s*=\s*(.+);?)");
        std::smatch matches;
        
        if (std::regex_search(line, matches, configRegex)) {
            ConfigurationItem item;
            item.Key = matches[1].str();
            std::string value = matches[2].str();
            
            // 检查是否是组选项 [value1, value2, ...]
            if (value.length() >= 2 && value[0] == '[' && value[value.length() - 1] == ']') {
                item.IsGroupOption = true;
                item.GroupValues = ParseGroupOption(value);
                item.Value = value;
            } else {
                item.IsGroupOption = false;
                item.Value = value;
            }
            
            items[item.Key] = item;
        }
    }
    
    return items;
}

std::vector<std::string> ConfigurationManager::ParseGroupOption(const std::string& groupValue) {
    std::vector<std::string> values;
    
    // 移除 [ 和 ]
    std::string content = groupValue.substr(1, groupValue.length() - 2);
    
    // 按逗号分割
    std::istringstream stream(content);
    std::string value;
    
    while (std::getline(stream, value, ',')) {
        // 去除前后空白
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (!value.empty()) {
            values.push_back(value);
        }
    }
    
    return values;
}

bool ConfigurationManager::ValidateConfigurations() const {
    bool isValid = true;
    
    // 验证激活的配置
    if (m_ActiveConfig) {
        for (const auto& [key, item] : m_ActiveConfig->Items) {
            if (!ValidateConfigurationItem(item)) {
                std::cerr << "❌ 无效的配置项: " << key << std::endl;
                isValid = false;
            }
        }
    }
    
    return isValid;
}

bool ConfigurationManager::ValidateConfigurationItem(const ConfigurationItem& item) const {
    // 验证配置项的有效性
    if (item.Key.empty() || item.Value.empty()) {
        return false;
    }
    
    // 验证组选项
    if (item.IsGroupOption && item.GroupValues.empty()) {
        return false;
    }
    
    return true;
}

void ConfigurationManager::SetCurrentNamespace(const std::string& namespaceName) {
    m_CurrentNamespace = namespaceName;
}

void ConfigurationManager::Reset() {
    m_GlobalConfigs.clear();
    m_NamedConfigs.clear();
    m_ActiveConfig.reset();
    m_CurrentNamespace.clear();
    InitializeDefaultConfigurations();
}

} // namespace CHTL