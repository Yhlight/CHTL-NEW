#pragma once

#include "CHTLBaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * 命名空间冲突类型
 */
enum class NamespaceConflictType {
    NO_CONFLICT,        // 无冲突
    NAME_CONFLICT,      // 名称冲突
    TYPE_CONFLICT,      // 类型冲突
    SCOPE_CONFLICT      // 作用域冲突
};

/**
 * 命名空间冲突信息
 */
struct NamespaceConflict {
    NamespaceConflictType Type;         // 冲突类型
    std::string ConflictName;           // 冲突名称
    std::string ExistingNamespace;      // 已存在的命名空间
    std::string NewNamespace;           // 新命名空间
    std::string Description;            // 冲突描述
    
    NamespaceConflict() : Type(NamespaceConflictType::NO_CONFLICT) {}
};

/**
 * 命名空间项
 */
struct NamespaceItem {
    std::string Name;                   // 项名称
    std::string Type;                   // 项类型（Template/Custom/Origin等）
    std::string FullPath;               // 完整路径
    CHTLBaseNode* Node;                 // 关联节点
    
    NamespaceItem() : Node(nullptr) {}
    NamespaceItem(const std::string& name, const std::string& type, 
                 const std::string& fullPath, CHTLBaseNode* node = nullptr)
        : Name(name), Type(type), FullPath(fullPath), Node(node) {}
};

/**
 * CHTL命名空间节点
 * 表示命名空间定义（[Namespace]）
 * 支持嵌套、合并、冲突检测等高级功能
 */
class NamespaceNode : public CHTLBaseNode {
private:
    std::string m_NamespaceName;                        // 命名空间名称
    std::string m_ParentNamespace;                      // 父命名空间
    std::string m_FullNamespacePath;                    // 完整命名空间路径
    
    // 命名空间内容
    std::unordered_map<std::string, NamespaceItem> m_NamespaceItems; // 命名空间项
    std::vector<std::unique_ptr<NamespaceNode>> m_ChildNamespaces;   // 子命名空间
    
    // 命名空间属性
    bool m_IsNestedNamespace;                           // 是否为嵌套命名空间
    bool m_IsDefaultNamespace;                          // 是否为默认命名空间（来自文件名）
    bool m_AllowAutoMerge;                              // 是否允许自动合并
    
    // 冲突检测
    std::vector<NamespaceConflict> m_Conflicts;         // 冲突列表
    bool m_HasConflicts;                                // 是否有冲突

public:
    /**
     * 构造函数
     * @param namespaceName 命名空间名称
     * @param parentNamespace 父命名空间
     * @param line 行号
     * @param column 列号
     */
    NamespaceNode(const std::string& namespaceName, const std::string& parentNamespace = "",
                 size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~NamespaceNode() override = default;
    
    /**
     * 获取命名空间名称
     * @return 命名空间名称
     */
    const std::string& GetNamespaceName() const { return m_NamespaceName; }
    
    /**
     * 设置命名空间名称
     * @param name 新名称
     */
    void SetNamespaceName(const std::string& name);
    
    /**
     * 获取父命名空间
     * @return 父命名空间
     */
    const std::string& GetParentNamespace() const { return m_ParentNamespace; }
    
    /**
     * 设置父命名空间
     * @param parent 父命名空间
     */
    void SetParentNamespace(const std::string& parent);
    
    /**
     * 获取完整命名空间路径
     * @return 完整路径
     */
    const std::string& GetFullNamespacePath() const { return m_FullNamespacePath; }
    
    /**
     * 检查是否为嵌套命名空间
     * @return 是否为嵌套
     */
    bool IsNestedNamespace() const { return m_IsNestedNamespace; }
    
    /**
     * 检查是否为默认命名空间
     * @return 是否为默认
     */
    bool IsDefaultNamespace() const { return m_IsDefaultNamespace; }
    
    /**
     * 设置为默认命名空间
     * @param isDefault 是否为默认
     */
    void SetDefaultNamespace(bool isDefault);
    
    // 嵌套命名空间管理
    /**
     * 添加子命名空间
     * @param childNamespace 子命名空间
     */
    void AddChildNamespace(std::unique_ptr<NamespaceNode> childNamespace);
    
    /**
     * 获取子命名空间
     * @param namespaceName 命名空间名称
     * @return 子命名空间指针
     */
    NamespaceNode* GetChildNamespace(const std::string& namespaceName);
    
    /**
     * 获取所有子命名空间
     * @return 子命名空间列表
     */
    const std::vector<std::unique_ptr<NamespaceNode>>& GetChildNamespaces() const { return m_ChildNamespaces; }
    
    /**
     * 查找嵌套命名空间
     * @param namespacePath 命名空间路径（如UI::Components）
     * @return 找到的命名空间
     */
    NamespaceNode* FindNestedNamespace(const std::string& namespacePath);
    
    // 命名空间项管理
    /**
     * 添加命名空间项
     * @param name 项名称
     * @param type 项类型
     * @param node 关联节点
     */
    void AddNamespaceItem(const std::string& name, const std::string& type, CHTLBaseNode* node = nullptr);
    
    /**
     * 获取命名空间项
     * @param name 项名称
     * @return 命名空间项
     */
    NamespaceItem GetNamespaceItem(const std::string& name) const;
    
    /**
     * 检查是否有命名空间项
     * @param name 项名称
     * @return 是否存在
     */
    bool HasNamespaceItem(const std::string& name) const;
    
    /**
     * 获取所有命名空间项
     * @return 命名空间项映射
     */
    const std::unordered_map<std::string, NamespaceItem>& GetNamespaceItems() const { return m_NamespaceItems; }
    
    // 命名空间合并功能
    /**
     * 合并同名命名空间
     * @param otherNamespace 要合并的命名空间
     * @return 是否成功合并
     */
    bool MergeNamespace(NamespaceNode* otherNamespace);
    
    /**
     * 检查是否可以合并
     * @param otherNamespace 其他命名空间
     * @return 是否可以合并
     */
    bool CanMergeWith(NamespaceNode* otherNamespace) const;
    
    /**
     * 自动合并同名命名空间
     * @param namespaceList 命名空间列表
     * @return 合并后的命名空间列表
     */
    static std::vector<std::unique_ptr<NamespaceNode>> AutoMergeNamespaces(
        std::vector<std::unique_ptr<NamespaceNode>>& namespaceList);
    
    // 冲突检测功能
    /**
     * 检测命名空间冲突
     * @param globalNamespaces 全局命名空间映射
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectConflicts(
        const std::unordered_map<std::string, NamespaceNode*>& globalNamespaces) const;
    
    /**
     * 检查名称冲突
     * @param name 名称
     * @param type 类型
     * @param globalNamespaces 全局命名空间
     * @return 冲突信息
     */
    NamespaceConflict CheckNameConflict(const std::string& name, const std::string& type,
                                       const std::unordered_map<std::string, NamespaceNode*>& globalNamespaces) const;
    
    /**
     * 解决命名空间冲突
     * @param conflicts 冲突列表
     * @return 是否成功解决
     */
    bool ResolveConflicts(const std::vector<NamespaceConflict>& conflicts);
    
    /**
     * 检查是否有冲突
     * @return 是否有冲突
     */
    bool HasConflicts() const { return m_HasConflicts; }
    
    /**
     * 获取冲突列表
     * @return 冲突列表
     */
    const std::vector<NamespaceConflict>& GetConflicts() const { return m_Conflicts; }
    
    // from语法支持
    /**
     * 处理from语法
     * @param fromExpression from表达式
     * @param targetNamespace 目标命名空间
     * @return 是否成功
     */
    bool ProcessFromSyntax(const std::string& fromExpression, const std::string& targetNamespace);
    
    /**
     * 解析from表达式
     * @param fromExpression from表达式
     * @return 解析结果（命名空间路径和项名称）
     */
    std::pair<std::string, std::vector<std::string>> ParseFromExpression(const std::string& fromExpression);
    
    /**
     * 导入from指定的项
     * @param sourceNamespace 源命名空间
     * @param itemNames 项名称列表
     * @return 是否成功
     */
    bool ImportFromNamespace(const std::string& sourceNamespace, const std::vector<std::string>& itemNames);
    
    /**
     * 生成命名空间HTML表示
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
     * 构建完整命名空间路径
     */
    void BuildFullNamespacePath();
    
    /**
     * 验证命名空间名称
     * @param name 名称
     * @return 是否有效
     */
    bool ValidateNamespaceName(const std::string& name) const;
    
    /**
     * 初始化命名空间
     */
    void InitializeNamespace();
    
    /**
     * 添加冲突信息
     * @param conflict 冲突信息
     */
    void AddConflict(const NamespaceConflict& conflict);
};

/**
 * 命名空间管理器
 * 全局命名空间管理和协调
 */
class NamespaceManager {
private:
    std::unordered_map<std::string, NamespaceNode*> m_GlobalNamespaces; // 全局命名空间映射
    std::unordered_set<std::string> m_ReservedNames;                    // 保留名称
    std::string m_CurrentNamespace;                                     // 当前命名空间
    
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
     * 注册命名空间
     * @param namespaceNode 命名空间节点
     * @return 是否成功
     */
    bool RegisterNamespace(NamespaceNode* namespaceNode);
    
    /**
     * 查找命名空间
     * @param namespacePath 命名空间路径
     * @return 命名空间节点
     */
    NamespaceNode* FindNamespace(const std::string& namespacePath);
    
    /**
     * 自动合并同名命名空间
     * @return 合并数量
     */
    int AutoMergeNamespaces();
    
    /**
     * 检测全局冲突
     * @return 冲突列表
     */
    std::vector<NamespaceConflict> DetectGlobalConflicts();
    
    /**
     * 设置当前命名空间
     * @param namespacePath 命名空间路径
     */
    void SetCurrentNamespace(const std::string& namespacePath);
    
    /**
     * 获取当前命名空间
     * @return 当前命名空间路径
     */
    const std::string& GetCurrentNamespace() const { return m_CurrentNamespace; }
    
    /**
     * 创建默认文件名命名空间
     * @param fileName 文件名
     * @return 默认命名空间
     */
    std::unique_ptr<NamespaceNode> CreateDefaultFileNamespace(const std::string& fileName);
    
    /**
     * 重置命名空间管理器
     */
    void Reset();
};

} // namespace CHTL