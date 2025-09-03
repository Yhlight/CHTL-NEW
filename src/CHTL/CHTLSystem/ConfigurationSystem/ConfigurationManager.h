#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 配置类型枚举
 */
enum class ConfigurationType {
    GLOBAL_CONFIG,          // 全局配置
    NAMED_CONFIG,           // 命名配置 @Config Name
    NAME_CONFIG,            // [Name] 配置块
    ORIGIN_TYPE_CONFIG      // [OriginType] 配置块
};

/**
 * 配置项信息
 */
struct ConfigurationItem {
    std::string Key;                        // 配置键
    std::string Value;                      // 配置值
    std::vector<std::string> GroupValues;  // 组选项值 (如: [@Style, @style, @CSS])
    bool IsGroupOption = false;             // 是否是组选项
    std::string Description;                // 配置描述
};

/**
 * 配置块信息
 */
struct ConfigurationBlock {
    std::string Name;                                               // 配置块名称
    ConfigurationType Type;                                         // 配置类型
    std::unordered_map<std::string, ConfigurationItem> Items;      // 配置项
    std::string Namespace;                                          // 所属命名空间
    bool IsActive = false;                                          // 是否激活
};

/**
 * 配置管理器
 * 实现完整的CHTL配置系统
 * 支持所有配置选项、Name配置块、OriginType配置等
 */
class ConfigurationManager {
private:
    // 配置存储
    std::unordered_map<std::string, std::unique_ptr<ConfigurationBlock>> m_GlobalConfigs;
    std::unordered_map<std::string, std::unique_ptr<ConfigurationBlock>> m_NamedConfigs;
    std::unique_ptr<ConfigurationBlock> m_ActiveConfig;
    
    // 默认配置值
    std::unordered_map<std::string, std::string> m_DefaultValues;
    
    // 当前命名空间
    std::string m_CurrentNamespace;

public:
    /**
     * 构造函数
     */
    ConfigurationManager();
    
    /**
     * 析构函数
     */
    ~ConfigurationManager() = default;
    
    /**
     * 注册配置块
     * @param configContent 配置内容
     * @param configName 配置名称 (空表示全局配置)
     * @param namespaceName 命名空间
     */
    void RegisterConfiguration(const std::string& configContent,
                              const std::string& configName = "",
                              const std::string& namespaceName = "");
    
    /**
     * 激活配置
     * @param configName 配置名称 (空表示激活全局配置)
     */
    void ActivateConfiguration(const std::string& configName = "");
    
    /**
     * 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string GetConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * 获取配置值 (布尔类型)
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    bool GetConfigBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * 获取配置值 (整数类型)
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    int GetConfigInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * 获取组选项值
     * @param key 配置键
     * @return 组选项值列表
     */
    std::vector<std::string> GetGroupOptionValues(const std::string& key) const;
    
    /**
     * 检查配置是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool HasConfiguration(const std::string& key) const;
    
    /**
     * 设置配置值
     * @param key 配置键
     * @param value 配置值
     */
    void SetConfigValue(const std::string& key, const std::string& value);
    
    /**
     * 获取Name配置块的关键字映射
     * @param keyword 关键字类型
     * @return 关键字值列表
     */
    std::vector<std::string> GetNameConfigKeywords(const std::string& keyword) const;
    
    /**
     * 获取OriginType配置
     * @param typeName 类型名称
     * @return 原始嵌入类型配置
     */
    std::string GetOriginTypeConfig(const std::string& typeName) const;
    
    /**
     * 检查是否禁用某个功能
     * @param featureName 功能名称
     * @return 是否禁用
     */
    bool IsFeatureDisabled(const std::string& featureName) const;
    
    /**
     * 获取所有配置统计信息
     * @return 配置统计
     */
    std::unordered_map<std::string, size_t> GetConfigurationStatistics() const;
    
    /**
     * 验证配置完整性
     * @return 验证结果
     */
    bool ValidateConfigurations() const;
    
    /**
     * 重置所有配置
     */
    void Reset();
    
    /**
     * 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);

private:
    /**
     * 初始化默认配置
     */
    void InitializeDefaultConfigurations();
    
    /**
     * 解析配置内容
     * @param configContent 配置内容
     * @return 配置项映射
     */
    std::unordered_map<std::string, ConfigurationItem> ParseConfigurationContent(const std::string& configContent);
    
    /**
     * 解析Name配置块
     * @param nameContent Name配置内容
     * @return 配置项映射
     */
    std::unordered_map<std::string, ConfigurationItem> ParseNameConfiguration(const std::string& nameContent);
    
    /**
     * 解析OriginType配置块
     * @param originTypeContent OriginType配置内容
     * @return 配置项映射
     */
    std::unordered_map<std::string, ConfigurationItem> ParseOriginTypeConfiguration(const std::string& originTypeContent);
    
    /**
     * 解析组选项
     * @param groupValue 组选项值 (如: [@Style, @style, @CSS])
     * @return 组选项列表
     */
    std::vector<std::string> ParseGroupOption(const std::string& groupValue);
    
    /**
     * 验证配置项
     * @param item 配置项
     * @return 是否有效
     */
    bool ValidateConfigurationItem(const ConfigurationItem& item) const;
    
    /**
     * 解析命名空间限定配置名
     * @param configName 配置名称
     * @return 解析后的命名空间和名称
     */
    std::pair<std::string, std::string> ParseNamespacedConfigName(const std::string& configName) const;
};

} // namespace CHTL