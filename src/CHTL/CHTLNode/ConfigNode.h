#pragma once

#include "BaseNode.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

    // 配置类型枚举
    enum class CHTLConfigType {
        CONFIG_BASIC,           // 基础配置
        CONFIG_NAMED,           // 命名配置组
        CONFIG_NAME_BLOCK,      // Name配置块
        CONFIG_ORIGIN_TYPE      // OriginType配置块
    };

    // 配置项类型枚举
    enum class CHTLConfigItemType {
        BOOLEAN_CONFIG,         // 布尔配置
        INTEGER_CONFIG,         // 整数配置
        STRING_CONFIG,          // 字符串配置
        GROUP_CONFIG            // 组选项配置
    };

    // 配置节点基类
    // 专门处理CHTL配置系统的AST节点
    // 严格按照CHTL语法文档第701-796行实现
    class CHTLConfigNode : public CHTLBaseNode {
    private:
        CHTLConfigType configType;                              // 配置类型
        std::string configName;                                 // 配置名称（命名配置组使用）
        bool isNamed;                                          // 是否为命名配置组
        bool isActive;                                         // 是否为活跃配置（无名配置组才启用）
        std::unordered_map<std::string, std::string> configItems; // 配置项映射
        std::unordered_map<std::string, CHTLConfigItemType> itemTypes; // 配置项类型映射

    public:
        // 构造函数
        CHTLConfigNode(CHTLConfigType type, const std::string& name = "");
        ~CHTLConfigNode() override = default;

        // 配置类型管理
        CHTLConfigType GetConfigType() const;                   // 获取配置类型
        void SetConfigType(CHTLConfigType type);                // 设置配置类型
        
        // 配置名称管理
        const std::string& GetConfigName() const;               // 获取配置名称
        void SetConfigName(const std::string& name);            // 设置配置名称
        bool IsNamed() const;                                   // 是否为命名配置
        void SetNamed(bool named);                              // 设置是否为命名配置
        
        // 活跃状态管理
        bool IsActive() const;                                  // 是否为活跃配置
        void SetActive(bool active);                           // 设置活跃状态
        
        // 配置项管理
        void AddConfigItem(const std::string& key, const std::string& value, CHTLConfigItemType type); // 添加配置项
        void RemoveConfigItem(const std::string& key);          // 移除配置项
        std::string GetConfigItem(const std::string& key) const; // 获取配置项
        bool HasConfigItem(const std::string& key) const;       // 是否有配置项
        CHTLConfigItemType GetConfigItemType(const std::string& key) const; // 获取配置项类型
        const std::unordered_map<std::string, std::string>& GetAllConfigItems() const; // 获取所有配置项
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string ConfigTypeToString(CHTLConfigType type);         // 配置类型转字符串
        static std::string ConfigItemTypeToString(CHTLConfigItemType type); // 配置项类型转字符串
    };

    // Name配置节点类
    // 处理 [Name] { 关键字重定义 } 语法
    class CHTLNameConfigNode : public CHTLBaseNode {
    private:
        std::unordered_map<std::string, std::string> keywordMappings;       // 关键字映射
        std::unordered_map<std::string, std::vector<std::string>> groupOptions; // 组选项
        int optionCountLimit;                                   // 组选项数量限制

    public:
        // 构造函数
        CHTLNameConfigNode();
        ~CHTLNameConfigNode() override = default;

        // 关键字映射管理
        void AddKeywordMapping(const std::string& keyword, const std::string& value); // 添加关键字映射
        void RemoveKeywordMapping(const std::string& keyword);  // 移除关键字映射
        std::string GetKeywordMapping(const std::string& keyword) const; // 获取关键字映射
        bool HasKeywordMapping(const std::string& keyword) const; // 是否有关键字映射
        
        // 组选项管理
        void AddGroupOption(const std::string& groupName, const std::vector<std::string>& options); // 添加组选项
        void AddOptionToGroup(const std::string& groupName, const std::string& option); // 添加选项到组
        void RemoveGroupOption(const std::string& groupName);   // 移除组选项
        std::vector<std::string> GetGroupOptions(const std::string& groupName) const; // 获取组选项
        bool HasGroupOption(const std::string& groupName) const; // 是否有组选项
        
        // 选项数量限制管理
        int GetOptionCountLimit() const;                        // 获取选项数量限制
        void SetOptionCountLimit(int limit);                   // 设置选项数量限制
        bool IsWithinOptionLimit(const std::string& groupName) const; // 是否在选项限制内
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // OriginType配置节点类
    // 处理 [OriginType] { 自定义原始嵌入类型定义 } 语法
    class CHTLOriginTypeConfigNode : public CHTLBaseNode {
    private:
        std::unordered_map<std::string, std::string> originTypeMappings;    // 原始嵌入类型映射

    public:
        // 构造函数
        CHTLOriginTypeConfigNode();
        ~CHTLOriginTypeConfigNode() override = default;

        // 原始嵌入类型映射管理
        void AddOriginTypeMapping(const std::string& typeName, const std::string& value); // 添加类型映射
        void RemoveOriginTypeMapping(const std::string& typeName); // 移除类型映射
        std::string GetOriginTypeMapping(const std::string& typeName) const; // 获取类型映射
        bool HasOriginTypeMapping(const std::string& typeName) const; // 是否有类型映射
        const std::unordered_map<std::string, std::string>& GetAllOriginTypeMappings() const; // 获取所有映射
        
        // 类型验证
        bool IsValidOriginTypeDefinition(const std::string& typeName, const std::string& value) const; // 验证类型定义
        bool FollowsNamingConvention(const std::string& typeName, const std::string& value) const; // 是否遵循命名约定
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 配置冲突检测器
    // 检测配置组冲突和验证配置规则
    class CHTLConfigConflictDetector {
    private:
        static std::vector<std::shared_ptr<CHTLConfigNode>> allConfigs;     // 所有配置
        static std::shared_ptr<CHTLConfigNode> activeConfig;               // 活跃配置

    public:
        // 配置注册
        static void RegisterConfig(std::shared_ptr<CHTLConfigNode> config);     // 注册配置
        static void UnregisterConfig(std::shared_ptr<CHTLConfigNode> config);   // 注销配置
        
        // 冲突检测
        static bool HasConfigConflict();                               // 是否有配置冲突
        static std::vector<std::shared_ptr<CHTLConfigNode>> GetConflictingConfigs(); // 获取冲突配置
        static bool HasMultipleUnnamedConfigs();                       // 是否有多个无名配置
        
        // 活跃配置管理
        static std::shared_ptr<CHTLConfigNode> GetActiveConfig();      // 获取活跃配置
        static void SetActiveConfig(std::shared_ptr<CHTLConfigNode> config); // 设置活跃配置
        static void DetermineActiveConfig();                           // 确定活跃配置
        
        // 工具方法
        static void ClearAllConfigs();                                 // 清空所有配置
        static std::vector<std::shared_ptr<CHTLConfigNode>> GetNamedConfigs(); // 获取命名配置
        static std::vector<std::shared_ptr<CHTLConfigNode>> GetUnnamedConfigs(); // 获取无名配置
        
        // 验证方法
        static bool ValidateConfigSetup();                             // 验证配置设置
    };

    // 配置节点工厂类
    class CHTLConfigNodeFactory {
    public:
        // 创建配置节点
        static std::shared_ptr<CHTLConfigNode> CreateBasicConfigNode();
        static std::shared_ptr<CHTLConfigNode> CreateNamedConfigNode(const std::string& configName);
        static std::shared_ptr<CHTLNameConfigNode> CreateNameConfigNode();
        static std::shared_ptr<CHTLOriginTypeConfigNode> CreateOriginTypeConfigNode();
        
        // 创建预定义配置
        static std::shared_ptr<CHTLConfigNode> CreateDefaultConfig();          // 创建默认配置
        static std::shared_ptr<CHTLNameConfigNode> CreateDefaultNameConfig();  // 创建默认Name配置
        
        // 解析方法
        static CHTLConfigType ParseConfigType(const std::string& configText);
        static CHTLConfigItemType ParseConfigItemType(const std::string& value);
        
        // 验证方法
        static bool IsValidConfigName(const std::string& name);
        static bool IsValidConfigItem(const std::string& key, const std::string& value);
        static bool IsValidGroupOption(const std::vector<std::string>& options);
    };

} // namespace CHTL