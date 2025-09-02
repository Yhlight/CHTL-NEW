#pragma once

#include "CHTLBaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

// 前向声明
class CHTLGlobalMap;

/**
 * 配置类型枚举
 */
enum class ConfigType {
    BASIC_CONFIG,       // 基础配置（INDEX_INITIAL_COUNT、DEBUG_MODE等）
    NAME_CONFIG,        // Name配置块（关键字自定义）
    ORIGIN_TYPE_CONFIG, // OriginType配置块
    NAMED_CONFIG,       // 命名配置组
    SELECTOR_CONFIG     // 选择器自动化配置
};

/**
 * 配置项结构
 */
struct ConfigItem {
    std::string Key;                    // 配置键
    std::string Value;                  // 配置值
    std::string Type;                   // 值类型（string/int/bool）
    std::string Description;            // 描述
    bool IsRequired;                    // 是否必需
    
    ConfigItem() : IsRequired(false) {}
    ConfigItem(const std::string& key, const std::string& value, 
              const std::string& type = "string", bool required = false)
        : Key(key), Value(value), Type(type), IsRequired(required) {}
};

/**
 * Name配置项结构
 */
struct NameConfigItem {
    std::string OriginalKeyword;        // 原始关键字
    std::string CustomKeyword;          // 自定义关键字
    bool IsEnabled;                     // 是否启用
    
    NameConfigItem() : IsEnabled(true) {}
    NameConfigItem(const std::string& original, const std::string& custom, bool enabled = true)
        : OriginalKeyword(original), CustomKeyword(custom), IsEnabled(enabled) {}
};

/**
 * OriginType配置项结构
 */
struct OriginTypeConfigItem {
    std::string TypeName;               // 类型名称
    std::string FileExtension;          // 文件扩展名
    std::string MimeType;               // MIME类型
    std::string Processor;              // 处理器
    bool IsBuiltIn;                     // 是否为内置类型
    
    OriginTypeConfigItem() : IsBuiltIn(false) {}
    OriginTypeConfigItem(const std::string& name, const std::string& ext, 
                        const std::string& mime = "", const std::string& proc = "", 
                        bool builtin = false)
        : TypeName(name), FileExtension(ext), MimeType(mime), Processor(proc), IsBuiltIn(builtin) {}
};

/**
 * CHTL配置节点
 * 表示配置定义（[Configuration]）
 * 支持基础配置、Name配置块、OriginType配置块等
 */
class ConfigNode : public CHTLBaseNode {
private:
    ConfigType m_ConfigType;                            // 配置类型
    std::string m_ConfigName;                           // 配置名称
    std::unordered_map<std::string, ConfigItem> m_BasicConfigs; // 基础配置项
    std::unordered_map<std::string, NameConfigItem> m_NameConfigs; // Name配置项
    std::unordered_map<std::string, OriginTypeConfigItem> m_OriginTypeConfigs; // OriginType配置项
    
    // 配置状态
    bool m_IsGlobalConfig;                              // 是否为全局配置
    bool m_IsNamedConfig;                               // 是否为命名配置组

public:
    /**
     * 构造函数
     * @param configType 配置类型
     * @param configName 配置名称
     * @param line 行号
     * @param column 列号
     */
    ConfigNode(ConfigType configType, const std::string& configName, 
              size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~ConfigNode() override = default;
    
    /**
     * 获取配置类型
     * @return 配置类型
     */
    ConfigType GetConfigType() const { return m_ConfigType; }
    
    /**
     * 获取配置名称
     * @return 配置名称
     */
    const std::string& GetConfigName() const { return m_ConfigName; }
    
    /**
     * 设置配置名称
     * @param name 新配置名称
     */
    void SetConfigName(const std::string& name);
    
    // 基础配置管理
    /**
     * 添加基础配置项
     * @param key 配置键
     * @param value 配置值
     * @param type 值类型
     * @param required 是否必需
     */
    void AddBasicConfig(const std::string& key, const std::string& value, 
                       const std::string& type = "string", bool required = false);
    
    /**
     * 获取基础配置项
     * @param key 配置键
     * @return 配置值
     */
    std::string GetBasicConfig(const std::string& key) const;
    
    /**
     * 检查是否有基础配置项
     * @param key 配置键
     * @return 是否存在
     */
    bool HasBasicConfig(const std::string& key) const;
    
    // Name配置块管理
    /**
     * 添加Name配置项（关键字自定义）
     * @param originalKeyword 原始关键字
     * @param customKeyword 自定义关键字
     * @param enabled 是否启用
     */
    void AddNameConfig(const std::string& originalKeyword, const std::string& customKeyword, bool enabled = true);
    
    /**
     * 获取自定义关键字
     * @param originalKeyword 原始关键字
     * @return 自定义关键字
     */
    std::string GetCustomKeyword(const std::string& originalKeyword) const;
    
    /**
     * 检查关键字是否被自定义
     * @param originalKeyword 原始关键字
     * @return 是否被自定义
     */
    bool IsKeywordCustomized(const std::string& originalKeyword) const;
    
    /**
     * 禁用Name配置组
     * @param disable 是否禁用
     */
    void DisableNameConfig(bool disable);
    
    // OriginType配置块管理
    /**
     * 添加OriginType配置项
     * @param typeName 类型名称
     * @param fileExtension 文件扩展名
     * @param mimeType MIME类型
     * @param processor 处理器
     */
    void AddOriginTypeConfig(const std::string& typeName, const std::string& fileExtension,
                            const std::string& mimeType = "", const std::string& processor = "");
    
    /**
     * 获取OriginType配置
     * @param typeName 类型名称
     * @return 配置项
     */
    OriginTypeConfigItem GetOriginTypeConfig(const std::string& typeName) const;
    
    /**
     * 检查是否有OriginType配置
     * @param typeName 类型名称
     * @return 是否存在
     */
    bool HasOriginTypeConfig(const std::string& typeName) const;
    
    /**
     * 注册自定义原始嵌入类型
     * @param typeName 类型名称
     * @param fileExtension 文件扩展名
     * @param processor 处理器
     */
    void RegisterCustomOriginType(const std::string& typeName, const std::string& fileExtension, 
                                 const std::string& processor = "");
    
    // 配置应用和验证
    /**
     * 应用配置到全局
     * @param globalMap 全局映射表
     */
    void ApplyConfigToGlobal(CHTLGlobalMap* globalMap);
    
    /**
     * 验证配置有效性
     * @return 是否有效
     */
    bool ValidateConfig() const;
    
    /**
     * 生成配置HTML表示
     * @return HTML代码
     */
    std::string GenerateHTML() const;
    
    /**
     * 获取类型字符串表示
     * @return 类型字符串
     */
    std::string GetTypeString() const;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<CHTLBaseNode> Clone() const override;
    
    /**
     * 接受访问者
     * @param visitor 访问者
     */
    void Accept(CHTLNodeVisitor* visitor) override;

private:
    /**
     * 初始化默认配置
     */
    void InitializeDefaultConfigs();
    
    /**
     * 初始化Name配置块
     */
    void InitializeNameConfig();
    
    /**
     * 初始化OriginType配置块
     */
    void InitializeOriginTypeConfig();
    
    /**
     * 验证Name配置项
     * @param item Name配置项
     * @return 是否有效
     */
    bool ValidateNameConfigItem(const NameConfigItem& item) const;
    
    /**
     * 验证OriginType配置项
     * @param item OriginType配置项
     * @return 是否有效
     */
    bool ValidateOriginTypeConfigItem(const OriginTypeConfigItem& item) const;
};

/**
 * 配置节点工厂
 */
class ConfigNodeFactory {
public:
    /**
     * 创建基础配置节点
     * @param configName 配置名称
     * @param line 行号
     * @param column 列号
     * @return 配置节点
     */
    static std::unique_ptr<ConfigNode> CreateBasicConfig(const std::string& configName,
                                                        size_t line = 0, size_t column = 0);
    
    /**
     * 创建Name配置节点
     * @param line 行号
     * @param column 列号
     * @return 配置节点
     */
    static std::unique_ptr<ConfigNode> CreateNameConfig(size_t line = 0, size_t column = 0);
    
    /**
     * 创建OriginType配置节点
     * @param line 行号
     * @param column 列号
     * @return 配置节点
     */
    static std::unique_ptr<ConfigNode> CreateOriginTypeConfig(size_t line = 0, size_t column = 0);
    
    /**
     * 创建命名配置组
     * @param configName 配置组名称
     * @param line 行号
     * @param column 列号
     * @return 配置节点
     */
    static std::unique_ptr<ConfigNode> CreateNamedConfig(const std::string& configName,
                                                        size_t line = 0, size_t column = 0);
};

} // namespace CHTL