#pragma once

#include "BaseNode.h"
#include <unordered_set>
#include <unordered_map>

namespace CHTL {

    // 原始嵌入类型枚举
    enum class CHTLOriginType {
        ORIGIN_HTML,            // @Html 类型
        ORIGIN_STYLE,           // @Style 类型
        ORIGIN_JAVASCRIPT,      // @JavaScript 类型
        ORIGIN_CUSTOM           // 自定义类型（如@Vue）
    };

    // 原始嵌入节点基类
    // 专门处理CHTL原始嵌入系统的AST节点
    // 严格按照CHTL语法文档第630-699行实现
    class CHTLOriginNode : public CHTLBaseNode {
    private:
        CHTLOriginType originType;                              // 原始嵌入类型
        std::string typeName;                                   // 类型名称（如Html、Style、Vue等）
        std::string originName;                                 // 原始嵌入名称（带名嵌入使用）
        std::string rawContent;                                 // 原始内容
        bool isNamed;                                          // 是否为带名原始嵌入
        bool isUsage;                                          // 是否为使用（而非定义）
        bool isCustomType;                                     // 是否为自定义类型
        bool willCreateImplicitConfig;                         // 是否会创建隐式配置块

    public:
        // 构造函数
        CHTLOriginNode(CHTLOriginType type, const std::string& typeName, const std::string& name = "");
        ~CHTLOriginNode() override = default;

        // 原始嵌入类型管理
        CHTLOriginType GetOriginType() const;                   // 获取原始嵌入类型
        void SetOriginType(CHTLOriginType type);                // 设置原始嵌入类型
        
        // 类型名称管理
        const std::string& GetTypeName() const;                 // 获取类型名称
        void SetTypeName(const std::string& typeName);          // 设置类型名称
        
        // 原始嵌入名称管理
        const std::string& GetOriginName() const;               // 获取原始嵌入名称
        void SetOriginName(const std::string& name);            // 设置原始嵌入名称
        
        // 原始内容管理
        const std::string& GetRawContent() const;               // 获取原始内容
        void SetRawContent(const std::string& content);         // 设置原始内容
        void AppendRawContent(const std::string& content);      // 追加原始内容
        void ClearRawContent();                                 // 清空原始内容
        
        // 带名嵌入管理
        bool IsNamed() const;                                   // 是否为带名嵌入
        void SetNamed(bool named);                              // 设置是否为带名嵌入
        
        // 使用/定义标记
        bool IsUsage() const;                                   // 是否为使用
        void SetIsUsage(bool usage);                           // 设置是否为使用
        
        // 自定义类型管理
        bool IsCustomType() const;                              // 是否为自定义类型
        void SetCustomType(bool custom);                       // 设置是否为自定义类型
        bool WillCreateImplicitConfig() const;                 // 是否会创建隐式配置
        void SetWillCreateImplicitConfig(bool create);          // 设置是否创建隐式配置
        
        // 核心特性：直接输出机制
        std::string GenerateDirectOutput() const;              // 生成直接输出（绝对不处理）
        bool IsEmpty() const;                                  // 是否为空内容
        size_t GetContentLength() const;                       // 获取内容长度
        
        // 验证方法
        bool IsValidOriginType() const;                        // 是否为有效的原始嵌入类型
        bool IsBasicType() const;                              // 是否为基本类型（Html/Style/JavaScript）
        bool RequiresAtPrefix() const;                         // 是否需要@前缀
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 静态工具方法
        static std::string OriginTypeToString(CHTLOriginType type);     // 原始嵌入类型转字符串
        static bool IsValidTypeName(const std::string& typeName);       // 验证类型名称是否有效
        static bool IsValidOriginName(const std::string& originName);   // 验证原始嵌入名称是否有效
    };

    // 原始嵌入使用节点类
    // 处理原始嵌入的使用语法 [Origin] @Html box; 等
    class CHTLOriginUsageNode : public CHTLBaseNode {
    private:
        CHTLOriginType originType;                              // 使用的原始嵌入类型
        std::string typeName;                                   // 类型名称
        std::string originName;                                 // 原始嵌入名称
        bool isCustomType;                                     // 是否为自定义类型

    public:
        // 构造函数
        CHTLOriginUsageNode(CHTLOriginType type, const std::string& typeName, const std::string& name);
        ~CHTLOriginUsageNode() override = default;

        // 原始嵌入使用管理
        CHTLOriginType GetOriginType() const;                   // 获取原始嵌入类型
        const std::string& GetTypeName() const;                 // 获取类型名称
        const std::string& GetOriginName() const;               // 获取原始嵌入名称
        void SetOriginName(const std::string& name);            // 设置原始嵌入名称
        
        // 自定义类型管理
        bool IsCustomType() const;                              // 是否为自定义类型
        void SetCustomType(bool custom);                       // 设置是否为自定义类型
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
    };

    // 原始嵌入节点工厂类
    class CHTLOriginNodeFactory {
    public:
        // 创建原始嵌入定义节点
        static std::shared_ptr<CHTLOriginNode> CreateHtmlOriginNode(const std::string& name = "");
        static std::shared_ptr<CHTLOriginNode> CreateStyleOriginNode(const std::string& name = "");
        static std::shared_ptr<CHTLOriginNode> CreateJavaScriptOriginNode(const std::string& name = "");
        static std::shared_ptr<CHTLOriginNode> CreateCustomOriginNode(const std::string& typeName, const std::string& name = "");
        
        // 创建原始嵌入使用节点
        static std::shared_ptr<CHTLOriginUsageNode> CreateHtmlUsageNode(const std::string& name);
        static std::shared_ptr<CHTLOriginUsageNode> CreateStyleUsageNode(const std::string& name);
        static std::shared_ptr<CHTLOriginUsageNode> CreateJavaScriptUsageNode(const std::string& name);
        static std::shared_ptr<CHTLOriginUsageNode> CreateCustomUsageNode(const std::string& typeName, const std::string& name);
        
        // 解析方法
        static CHTLOriginType ParseOriginType(const std::string& typeText);
        static std::shared_ptr<CHTLOriginNode> ParseOrigin(const std::string& originText);
        
        // 验证方法
        static bool IsBasicOriginType(const std::string& typeName);      // 是否为基本原始嵌入类型
        static bool IsValidCustomTypeName(const std::string& typeName);  // 是否为有效的自定义类型名
        static bool RequiresAtPrefix(const std::string& typeName);       // 是否需要@前缀
    };

    // 隐式配置块管理类
    // 管理自定义原始嵌入类型创建的隐式配置块
    class CHTLImplicitConfigManager {
    private:
        static std::unordered_set<std::string> registeredCustomTypes;   // 已注册的自定义类型
        static std::unordered_map<std::string, std::string> typeConfigs; // 类型配置映射

    public:
        // 自定义类型注册
        static void RegisterCustomType(const std::string& typeName);    // 注册自定义类型
        static bool IsCustomTypeRegistered(const std::string& typeName); // 是否已注册自定义类型
        static const std::unordered_set<std::string>& GetRegisteredTypes(); // 获取所有注册类型
        
        // 隐式配置生成
        static std::string GenerateImplicitConfig(const std::string& typeName); // 生成隐式配置
        static void AddTypeConfig(const std::string& typeName, const std::string& config); // 添加类型配置
        static std::string GetTypeConfig(const std::string& typeName);   // 获取类型配置
        
        // 工具方法
        static void ClearRegistrations();                              // 清空注册
        static std::vector<std::string> GetAllImplicitConfigs();       // 获取所有隐式配置
        
        // 验证方法
        static bool ValidateTypeName(const std::string& typeName);     // 验证类型名称
        static bool RequiresImplicitConfig(const std::string& typeName); // 是否需要隐式配置
    };

} // namespace CHTL