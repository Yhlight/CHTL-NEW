#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL模板类型枚举
 */
enum class CHTLTemplateType {
    STYLE_TEMPLATE,     // 样式组模板
    ELEMENT_TEMPLATE,   // 元素模板
    VAR_TEMPLATE        // 变量组模板
};

/**
 * CHTL自定义类型枚举
 */
enum class CHTLCustomType {
    STYLE_CUSTOM,       // 自定义样式组
    ELEMENT_CUSTOM,     // 自定义元素
    VAR_CUSTOM          // 自定义变量组
};

/**
 * CHTL原始嵌入类型枚举
 */
enum class CHTLOriginType {
    HTML_ORIGIN,        // @Html原始嵌入
    STYLE_ORIGIN,       // @Style原始嵌入
    JAVASCRIPT_ORIGIN,  // @JavaScript原始嵌入
    CUSTOM_ORIGIN       // 自定义类型原始嵌入
};

/**
 * 模板定义结构
 */
struct TemplateDefinition {
    CHTLTemplateType Type;              // 模板类型
    std::string Name;                   // 模板名称
    std::string Content;                // 模板内容
    std::vector<std::string> Inherits;  // 继承的模板列表
    std::unordered_map<std::string, std::string> Properties; // 属性映射
    std::string Namespace;              // 所属命名空间
    
    TemplateDefinition() = default;
    TemplateDefinition(CHTLTemplateType type, const std::string& name, 
                      const std::string& content, const std::string& ns = "")
        : Type(type), Name(name), Content(content), Namespace(ns) {}
};

/**
 * 自定义定义结构
 */
struct CustomDefinition {
    CHTLCustomType Type;                // 自定义类型
    std::string Name;                   // 自定义名称
    std::string Content;                // 自定义内容
    std::vector<std::string> Inherits;  // 继承的模板/自定义列表
    std::unordered_map<std::string, std::string> Properties; // 属性映射
    std::vector<std::string> DeletedProperties; // 删除的属性
    std::vector<std::string> DeletedInherits;   // 删除的继承
    std::string Namespace;              // 所属命名空间
    bool IsValueless;                   // 是否为无值样式组
    
    CustomDefinition() = default;
    CustomDefinition(CHTLCustomType type, const std::string& name, 
                    const std::string& content, const std::string& ns = "")
        : Type(type), Name(name), Content(content), Namespace(ns), IsValueless(false) {}
};

/**
 * 原始嵌入定义结构
 */
struct OriginDefinition {
    CHTLOriginType Type;                // 原始嵌入类型
    std::string Name;                   // 名称（可选）
    std::string Content;                // 原始内容
    std::string CustomTypeName;        // 自定义类型名称（如@Vue）
    std::string Namespace;              // 所属命名空间
    
    OriginDefinition() = default;
    OriginDefinition(CHTLOriginType type, const std::string& content, 
                    const std::string& name = "", const std::string& ns = "")
        : Type(type), Name(name), Content(content), Namespace(ns) {}
};

/**
 * 配置定义结构
 */
struct ConfigurationDefinition {
    std::string Name;                   // 配置组名称（可选）
    std::unordered_map<std::string, std::string> Settings; // 配置项
    std::unordered_map<std::string, std::vector<std::string>> NameMappings; // Name配置块
    std::unordered_map<std::string, std::string> OriginTypes; // OriginType配置块
    std::string Namespace;              // 所属命名空间
    
    ConfigurationDefinition() = default;
    ConfigurationDefinition(const std::string& name, const std::string& ns = "")
        : Name(name), Namespace(ns) {}
};

/**
 * 命名空间定义结构
 */
struct NamespaceDefinition {
    std::string Name;                   // 命名空间名称
    std::string ParentNamespace;       // 父命名空间
    std::vector<std::string> ChildNamespaces; // 子命名空间
    std::vector<std::string> Constraints; // 约束列表（except）
    
    NamespaceDefinition() = default;
    NamespaceDefinition(const std::string& name, const std::string& parent = "")
        : Name(name), ParentNamespace(parent) {}
};

/**
 * 导入定义结构
 */
struct ImportDefinition {
    std::string Type;                   // 导入类型（@Html、@Chtl等）
    std::string Target;                 // 导入目标
    std::string FilePath;               // 文件路径
    std::string AsName;                 // as重命名
    std::string FromNamespace;          // from命名空间
    bool HasAs;                         // 是否有as语法
    
    ImportDefinition() = default;
    ImportDefinition(const std::string& type, const std::string& target, 
                    const std::string& path, const std::string& asName = "")
        : Type(type), Target(target), FilePath(path), AsName(asName), HasAs(!asName.empty()) {}
};

/**
 * CHTL全局映射表
 * 管理所有CHTL语法元素的定义和引用关系
 */
class CHTLGlobalMap {
private:
    // 模板映射表
    std::unordered_map<std::string, std::unique_ptr<TemplateDefinition>> m_Templates;
    
    // 自定义映射表
    std::unordered_map<std::string, std::unique_ptr<CustomDefinition>> m_Customs;
    
    // 原始嵌入映射表
    std::unordered_map<std::string, std::unique_ptr<OriginDefinition>> m_Origins;
    
    // 配置映射表
    std::unordered_map<std::string, std::unique_ptr<ConfigurationDefinition>> m_Configurations;
    
    // 命名空间映射表
    std::unordered_map<std::string, std::unique_ptr<NamespaceDefinition>> m_Namespaces;
    
    // 导入映射表
    std::vector<std::unique_ptr<ImportDefinition>> m_Imports;
    
    // 当前活动配置
    ConfigurationDefinition* m_ActiveConfiguration;
    
    // 当前命名空间栈
    std::vector<std::string> m_NamespaceStack;
    
    // 自动生成的类名/ID计数器
    std::unordered_map<std::string, int> m_AutoClassCounters;
    std::unordered_map<std::string, int> m_AutoIdCounters;

public:
    /**
     * 构造函数
     */
    CHTLGlobalMap();
    
    /**
     * 析构函数
     */
    ~CHTLGlobalMap() = default;
    
    // 模板管理
    /**
     * 注册模板定义
     * @param definition 模板定义
     * @return 是否注册成功
     */
    bool RegisterTemplate(std::unique_ptr<TemplateDefinition> definition);
    
    /**
     * 获取模板定义
     * @param name 模板名称
     * @param type 模板类型
     * @param ns 命名空间
     * @return 模板定义指针
     */
    TemplateDefinition* GetTemplate(const std::string& name, 
                                   CHTLTemplateType type, 
                                   const std::string& ns = "");
    
    /**
     * 检查模板是否存在
     * @param name 模板名称
     * @param type 模板类型
     * @param ns 命名空间
     * @return 是否存在
     */
    bool HasTemplate(const std::string& name, CHTLTemplateType type, const std::string& ns = "");
    
    // 自定义管理
    /**
     * 注册自定义定义
     * @param definition 自定义定义
     * @return 是否注册成功
     */
    bool RegisterCustom(std::unique_ptr<CustomDefinition> definition);
    
    /**
     * 获取自定义定义
     * @param name 自定义名称
     * @param type 自定义类型
     * @param ns 命名空间
     * @return 自定义定义指针
     */
    CustomDefinition* GetCustom(const std::string& name, 
                               CHTLCustomType type, 
                               const std::string& ns = "");
    
    // 原始嵌入管理
    /**
     * 注册原始嵌入定义
     * @param definition 原始嵌入定义
     * @return 是否注册成功
     */
    bool RegisterOrigin(std::unique_ptr<OriginDefinition> definition);
    
    /**
     * 获取原始嵌入定义
     * @param name 名称
     * @param type 类型
     * @param ns 命名空间
     * @return 原始嵌入定义指针
     */
    OriginDefinition* GetOrigin(const std::string& name, 
                               CHTLOriginType type, 
                               const std::string& ns = "");
    
    // 配置管理
    /**
     * 注册配置定义
     * @param definition 配置定义
     * @return 是否注册成功
     */
    bool RegisterConfiguration(std::unique_ptr<ConfigurationDefinition> definition);
    
    /**
     * 设置活动配置
     * @param name 配置名称
     */
    void SetActiveConfiguration(const std::string& name);
    
    /**
     * 获取当前活动配置
     * @return 活动配置指针
     */
    ConfigurationDefinition* GetActiveConfiguration();
    
    /**
     * 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string GetConfigurationValue(const std::string& key, const std::string& defaultValue = "");
    
    // 命名空间管理
    /**
     * 注册命名空间
     * @param definition 命名空间定义
     * @return 是否注册成功
     */
    bool RegisterNamespace(std::unique_ptr<NamespaceDefinition> definition);
    
    /**
     * 进入命名空间
     * @param name 命名空间名称
     */
    void EnterNamespace(const std::string& name);
    
    /**
     * 退出当前命名空间
     */
    void ExitNamespace();
    
    /**
     * 获取当前命名空间
     * @return 当前命名空间名称
     */
    std::string GetCurrentNamespace();
    
    /**
     * 获取完整命名空间路径
     * @return 完整路径（用.分隔）
     */
    std::string GetFullNamespacePath() const;
    
    // 导入管理
    /**
     * 注册导入定义
     * @param definition 导入定义
     * @return 是否注册成功
     */
    bool RegisterImport(std::unique_ptr<ImportDefinition> definition);
    
    /**
     * 获取所有导入
     * @return 导入定义列表
     */
    const std::vector<std::unique_ptr<ImportDefinition>>& GetImports() const;
    
    // 自动化管理
    /**
     * 生成自动类名
     * @param baseName 基础名称
     * @return 生成的类名
     */
    std::string GenerateAutoClassName(const std::string& baseName);
    
    /**
     * 生成自动ID名
     * @param baseName 基础名称
     * @return 生成的ID名
     */
    std::string GenerateAutoIdName(const std::string& baseName);
    
    /**
     * 检查类名是否已自动生成
     * @param className 类名
     * @return 是否已生成
     */
    bool IsAutoGeneratedClass(const std::string& className);
    
    /**
     * 检查ID是否已自动生成
     * @param idName ID名
     * @return 是否已生成
     */
    bool IsAutoGeneratedId(const std::string& idName);
    
    // 查询和验证
    /**
     * 解析变量引用
     * @param varRef 变量引用（如ThemeColor(tableColor)）
     * @return 解析后的值
     */
    std::string ResolveVariableReference(const std::string& varRef);
    
    /**
     * 检查约束条件
     * @param elementType 元素类型
     * @param context 上下文
     * @return 是否通过约束检查
     */
    bool CheckConstraints(const std::string& elementType, const std::string& context);
    
    /**
     * 获取继承链
     * @param name 名称
     * @param type 类型（模板或自定义）
     * @return 继承链列表
     */
    std::vector<std::string> GetInheritanceChain(const std::string& name, const std::string& type);
    
    /**
     * 清空所有映射
     */
    void Clear();
    
    /**
     * 获取统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;

private:
    /**
     * 构建全限定名称
     * @param name 名称
     * @param ns 命名空间
     * @return 全限定名称
     */
    std::string BuildQualifiedName(const std::string& name, const std::string& ns);
    
    /**
     * 解析命名空间路径
     * @param path 路径（用.分隔）
     * @return 解析后的命名空间列表
     */
    std::vector<std::string> ParseNamespacePath(const std::string& path);
    
    /**
     * 合并同名命名空间
     * @param name 命名空间名称
     */
    void MergeNamespaces(const std::string& name);
    
    /**
     * 检测命名空间冲突
     * @param name 命名空间名称
     * @return 是否有冲突
     */
    bool DetectNamespaceConflict(const std::string& name);
    
    /**
     * 应用模板继承
     * @param definition 模板定义
     */
    void ApplyTemplateInheritance(TemplateDefinition* definition);
    
    /**
     * 应用自定义继承
     * @param definition 自定义定义
     */
    void ApplyCustomInheritance(CustomDefinition* definition);
};

} // namespace CHTL