#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 命名空间信息
 */
struct NamespaceInfo {
    std::string Name;                           // 命名空间名称
    std::string FullName;                       // 完整名称 (包含父命名空间)
    std::string ParentNamespace;                // 父命名空间
    std::vector<std::string> ChildNamespaces;   // 子命名空间列表
    std::unordered_set<std::string> Members;    // 成员列表 (模板、自定义等)
    bool IsActive = false;                      // 是否激活
    std::string SourceFile;                     // 来源文件
};

/**
 * 命名空间冲突信息
 */
struct NamespaceConflict {
    std::string ConflictName;                   // 冲突名称
    std::vector<std::string> ConflictSources;   // 冲突来源
    std::string Resolution;                     // 解决方案
};

/**
 * 命名空间管理器
 * 实现完整的CHTL命名空间系统
 * 支持嵌套、自动合并、冲突检测、默认命名空间等特性
 */
class NamespaceManager {
private:
    // 命名空间存储
    std::unordered_map<std::string, std::unique_ptr<NamespaceInfo>> m_Namespaces;
    
    // 当前状态
    std::string m_CurrentNamespace;             // 当前命名空间
    std::string m_DefaultNamespace;             // 默认命名空间 (文件名)
    
    // 别名映射
    std::unordered_map<std::string, std::string> m_NamespaceAliases;
    
    // 冲突检测
    std::vector<NamespaceConflict> m_Conflicts;
    
    // 配置选项
    bool m_DisableDefaultNamespace = false;     // 是否禁用默认命名空间

public:
    /**
     * 构造函数
     */
    NamespaceManager();
    
    /**
     * 析构函数
     */
    ~NamespaceManager() = default;
    
    /**
     * 创建命名空间
     * @param namespaceName 命名空间名称
     * @param parentNamespace 父命名空间 (空表示顶级)
     * @param sourceFile 来源文件
     */
    void CreateNamespace(const std::string& namespaceName,
                        const std::string& parentNamespace = "",
                        const std::string& sourceFile = "");
    
    /**
     * 设置当前命名空间
     * @param namespaceName 命名空间名称
     */
    void SetCurrentNamespace(const std::string& namespaceName);
    
    /**
     * 获取当前命名空间
     * @return 当前命名空间名称
     */
    std::string GetCurrentNamespace() const;
    
    /**
     * 设置默认命名空间 (通常是文件名)
     * @param defaultNamespace 默认命名空间
     */
    void SetDefaultNamespace(const std::string& defaultNamespace);
    
    /**
     * 添加成员到命名空间
     * @param namespaceName 命名空间名称
     * @param memberName 成员名称
     * @param memberType 成员类型 (template, custom, etc.)
     */
    void AddMemberToNamespace(const std::string& namespaceName,
                             const std::string& memberName,
                             const std::string& memberType);
    
    /**
     * 解析命名空间限定名
     * @param qualifiedName 限定名 (如: space::room::Box)
     * @return 解析后的命名空间和成员名
     */
    std::pair<std::string, std::string> ParseQualifiedName(const std::string& qualifiedName) const;
    
    /**
     * 构建完整限定名
     * @param namespaceName 命名空间名称
     * @param memberName 成员名称
     * @return 完整限定名
     */
    std::string BuildQualifiedName(const std::string& namespaceName, const std::string& memberName) const;
    
    /**
     * 查找成员
     * @param memberName 成员名称 (可能包含命名空间)
     * @param memberType 成员类型
     * @return 找到的完整限定名
     */
    std::string FindMember(const std::string& memberName, const std::string& memberType) const;
    
    /**
     * 合并同名命名空间
     * @param namespaceName 命名空间名称
     */
    void MergeSameNameNamespaces(const std::string& namespaceName);
    
    /**
     * 检测命名空间冲突
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectNamespaceConflicts();
    
    /**
     * 解决命名空间冲突
     * @param conflict 冲突信息
     * @param resolution 解决方案
     */
    void ResolveNamespaceConflict(const NamespaceConflict& conflict, const std::string& resolution);
    
    /**
     * 添加命名空间别名
     * @param alias 别名
     * @param namespaceName 实际命名空间
     */
    void AddNamespaceAlias(const std::string& alias, const std::string& namespaceName);
    
    /**
     * 解析命名空间别名
     * @param aliasName 别名
     * @return 实际命名空间名称
     */
    std::string ResolveNamespaceAlias(const std::string& aliasName) const;
    
    /**
     * 检查命名空间是否存在
     * @param namespaceName 命名空间名称
     * @return 是否存在
     */
    bool NamespaceExists(const std::string& namespaceName) const;
    
    /**
     * 获取命名空间信息
     * @param namespaceName 命名空间名称
     * @return 命名空间信息
     */
    const NamespaceInfo* GetNamespaceInfo(const std::string& namespaceName) const;
    
    /**
     * 获取所有命名空间列表
     * @return 命名空间名称列表
     */
    std::vector<std::string> GetAllNamespaces() const;
    
    /**
     * 获取子命名空间列表
     * @param parentNamespace 父命名空间
     * @return 子命名空间列表
     */
    std::vector<std::string> GetChildNamespaces(const std::string& parentNamespace) const;
    
    /**
     * 设置配置选项
     * @param disableDefaultNamespace 是否禁用默认命名空间
     */
    void SetConfiguration(bool disableDefaultNamespace);
    
    /**
     * 导入文件时的命名空间处理
     * @param fileName 文件名
     * @param hasExplicitNamespace 是否有显式命名空间
     * @return 使用的命名空间
     */
    std::string HandleFileImportNamespace(const std::string& fileName, bool hasExplicitNamespace);
    
    /**
     * 验证命名空间系统完整性
     * @return 验证结果
     */
    bool ValidateNamespaceSystem();
    
    /**
     * 获取命名空间统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetNamespaceStatistics() const;
    
    /**
     * 清理所有命名空间
     */
    void Clear();

private:
    /**
     * 解析嵌套命名空间
     * @param namespaceContent 命名空间内容
     * @param parentNamespace 父命名空间
     */
    void ParseNestedNamespaces(const std::string& namespaceContent, const std::string& parentNamespace);
    
    /**
     * 构建命名空间层次结构
     * @param namespaceName 命名空间名称
     * @param parentNamespace 父命名空间
     * @return 完整的命名空间名称
     */
    std::string BuildNamespaceHierarchy(const std::string& namespaceName, const std::string& parentNamespace);
    
    /**
     * 检查命名空间名称是否有效
     * @param namespaceName 命名空间名称
     * @return 是否有效
     */
    bool IsValidNamespaceName(const std::string& namespaceName) const;
    
    /**
     * 检查是否存在循环依赖
     * @param namespaceName 命名空间名称
     * @param parentNamespace 父命名空间
     * @return 是否存在循环依赖
     */
    bool HasCircularDependency(const std::string& namespaceName, const std::string& parentNamespace) const;
};

} // namespace CHTL