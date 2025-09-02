#include "NamespaceNode.h"
#include "Visitor.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& namespaceName, const std::string& parentNamespace,
                            size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::NAMESPACE_NODE, namespaceName, line, column),
      m_NamespaceName(namespaceName), m_ParentNamespace(parentNamespace),
      m_IsNestedNamespace(!parentNamespace.empty()), m_IsDefaultNamespace(false),
      m_AllowAutoMerge(true), m_HasConflicts(false) {
    
    InitializeNamespace();
    BuildFullNamespacePath();
}

void NamespaceNode::InitializeNamespace() {
    // 验证命名空间名称
    if (!ValidateNamespaceName(m_NamespaceName)) {
        // 处理无效名称
        m_NamespaceName = "InvalidNamespace";
    }
    
    // SetValue(m_NamespaceName); // 暂时注释，等待CHTLBaseNode方法确认
}

void NamespaceNode::BuildFullNamespacePath() {
    if (m_ParentNamespace.empty()) {
        m_FullNamespacePath = m_NamespaceName;
    }
    else {
        m_FullNamespacePath = m_ParentNamespace + "::" + m_NamespaceName;
    }
}

void NamespaceNode::SetNamespaceName(const std::string& name) {
    m_NamespaceName = name;
    BuildFullNamespacePath();
    // SetValue(name); // 暂时注释，等待CHTLBaseNode方法确认
}

void NamespaceNode::SetParentNamespace(const std::string& parent) {
    m_ParentNamespace = parent;
    m_IsNestedNamespace = !parent.empty();
    BuildFullNamespacePath();
}

void NamespaceNode::SetDefaultNamespace(bool isDefault) {
    m_IsDefaultNamespace = isDefault;
    
    if (isDefault) {
        // 默认命名空间的特殊处理
        SetAttribute("default-namespace", "true");
    }
}

void NamespaceNode::AddChildNamespace(std::unique_ptr<NamespaceNode> childNamespace) {
    if (childNamespace) {
        childNamespace->SetParentNamespace(m_FullNamespacePath);
        m_ChildNamespaces.push_back(std::move(childNamespace));
    }
}

NamespaceNode* NamespaceNode::GetChildNamespace(const std::string& namespaceName) {
    for (const auto& child : m_ChildNamespaces) {
        if (child->GetNamespaceName() == namespaceName) {
            return child.get();
        }
    }
    return nullptr;
}

NamespaceNode* NamespaceNode::FindNestedNamespace(const std::string& namespacePath) {
    // 解析命名空间路径：UI::Components::Buttons
    std::vector<std::string> pathParts;
    std::stringstream ss(namespacePath);
    std::string part;
    
    while (std::getline(ss, part, ':')) {
        if (!part.empty() && part != ":") {
            pathParts.push_back(part);
        }
    }
    
    if (pathParts.empty()) {
        return nullptr;
    }
    
    // 从当前命名空间开始查找
    NamespaceNode* current = this;
    
    for (const auto& pathPart : pathParts) {
        NamespaceNode* child = current->GetChildNamespace(pathPart);
        if (!child) {
            return nullptr; // 找不到路径
        }
        current = child;
    }
    
    return current;
}

void NamespaceNode::AddNamespaceItem(const std::string& name, const std::string& type, CHTLBaseNode* node) {
    std::string fullPath = m_FullNamespacePath + "::" + name;
    m_NamespaceItems[name] = NamespaceItem(name, type, fullPath, node);
}

NamespaceItem NamespaceNode::GetNamespaceItem(const std::string& name) const {
    auto it = m_NamespaceItems.find(name);
    return (it != m_NamespaceItems.end()) ? it->second : NamespaceItem();
}

bool NamespaceNode::HasNamespaceItem(const std::string& name) const {
    return m_NamespaceItems.find(name) != m_NamespaceItems.end();
}

bool NamespaceNode::MergeNamespace(NamespaceNode* otherNamespace) {
    if (!otherNamespace || !CanMergeWith(otherNamespace)) {
        return false;
    }
    
    // 合并命名空间项
    for (const auto& item : otherNamespace->GetNamespaceItems()) {
        if (!HasNamespaceItem(item.first)) {
            m_NamespaceItems[item.first] = item.second;
        }
        else {
            // 检测冲突
            NamespaceConflict conflict;
            conflict.Type = NamespaceConflictType::NAME_CONFLICT;
            conflict.ConflictName = item.first;
            conflict.ExistingNamespace = m_FullNamespacePath;
            conflict.NewNamespace = otherNamespace->GetFullNamespacePath();
            conflict.Description = "命名空间项名称冲突: " + item.first;
            
            AddConflict(conflict);
        }
    }
    
    // 合并子命名空间
    for (auto& childNamespace : otherNamespace->m_ChildNamespaces) {
        NamespaceNode* existingChild = GetChildNamespace(childNamespace->GetNamespaceName());
        if (existingChild) {
            // 递归合并子命名空间
            existingChild->MergeNamespace(childNamespace.get());
        }
        else {
            // 添加新的子命名空间
            AddChildNamespace(std::move(childNamespace));
        }
    }
    
    return true;
}

bool NamespaceNode::CanMergeWith(NamespaceNode* otherNamespace) const {
    if (!otherNamespace) {
        return false;
    }
    
    // 只有同名命名空间才能合并
    return m_NamespaceName == otherNamespace->GetNamespaceName() &&
           m_ParentNamespace == otherNamespace->GetParentNamespace();
}

std::vector<std::unique_ptr<NamespaceNode>> NamespaceNode::AutoMergeNamespaces(
    std::vector<std::unique_ptr<NamespaceNode>>& namespaceList) {
    
    std::vector<std::unique_ptr<NamespaceNode>> mergedNamespaces;
    std::unordered_map<std::string, NamespaceNode*> namespaceMap;
    
    // 第一遍：收集所有命名空间
    for (auto& ns : namespaceList) {
        std::string fullPath = ns->GetFullNamespacePath();
        
        if (namespaceMap.find(fullPath) != namespaceMap.end()) {
            // 找到同名命名空间，尝试合并
            NamespaceNode* existing = namespaceMap[fullPath];
            if (existing->CanMergeWith(ns.get())) {
                existing->MergeNamespace(ns.get());
            }
        }
        else {
            // 新命名空间
            namespaceMap[fullPath] = ns.get();
            mergedNamespaces.push_back(std::move(ns));
        }
    }
    
    return mergedNamespaces;
}

std::vector<NamespaceConflict> NamespaceNode::DetectConflicts(
    const std::unordered_map<std::string, NamespaceNode*>& globalNamespaces) const {
    
    std::vector<NamespaceConflict> conflicts;
    
    // 检查当前命名空间的所有项
    for (const auto& item : m_NamespaceItems) {
        NamespaceConflict conflict = CheckNameConflict(item.first, item.second.Type, globalNamespaces);
        
        if (conflict.Type != NamespaceConflictType::NO_CONFLICT) {
            conflicts.push_back(conflict);
        }
    }
    
    // 递归检查子命名空间
    for (const auto& child : m_ChildNamespaces) {
        auto childConflicts = child->DetectConflicts(globalNamespaces);
        conflicts.insert(conflicts.end(), childConflicts.begin(), childConflicts.end());
    }
    
    return conflicts;
}

NamespaceConflict NamespaceNode::CheckNameConflict(const std::string& name, const std::string& type,
                                                  const std::unordered_map<std::string, NamespaceNode*>& globalNamespaces) const {
    NamespaceConflict conflict;
    
    // 在其他命名空间中查找同名项
    for (const auto& nsEntry : globalNamespaces) {
        NamespaceNode* otherNamespace = nsEntry.second;
        
        if (otherNamespace != this && otherNamespace->HasNamespaceItem(name)) {
            NamespaceItem otherItem = otherNamespace->GetNamespaceItem(name);
            
            if (otherItem.Type != type) {
                // 类型冲突
                conflict.Type = NamespaceConflictType::TYPE_CONFLICT;
                conflict.ConflictName = name;
                conflict.ExistingNamespace = otherNamespace->GetFullNamespacePath();
                conflict.NewNamespace = m_FullNamespacePath;
                conflict.Description = "类型冲突: " + name + " 在 " + otherNamespace->GetFullNamespacePath() + 
                                     " 中为 " + otherItem.Type + "，在 " + m_FullNamespacePath + " 中为 " + type;
                break;
            }
            else {
                // 名称冲突（同名同类型）
                conflict.Type = NamespaceConflictType::NAME_CONFLICT;
                conflict.ConflictName = name;
                conflict.ExistingNamespace = otherNamespace->GetFullNamespacePath();
                conflict.NewNamespace = m_FullNamespacePath;
                conflict.Description = "名称冲突: " + name + " 在多个命名空间中定义";
            }
        }
    }
    
    return conflict;
}

bool NamespaceNode::ResolveConflicts(const std::vector<NamespaceConflict>& conflicts) {
    // 简化的冲突解决策略
    for (const auto& conflict : conflicts) {
        switch (conflict.Type) {
            case NamespaceConflictType::NAME_CONFLICT:
                // 名称冲突：可以通过全缀名访问解决
                break;
            case NamespaceConflictType::TYPE_CONFLICT:
                // 类型冲突：需要重命名或移除
                return false; // 严重冲突，无法自动解决
            case NamespaceConflictType::SCOPE_CONFLICT:
                // 作用域冲突：调整作用域
                break;
            default:
                break;
        }
    }
    
    return true;
}

bool NamespaceNode::ProcessFromSyntax(const std::string& fromExpression, const std::string& targetNamespace) {
    // 处理from语法：from UI::Components import Button, Card;
    auto parseResult = ParseFromExpression(fromExpression);
    
    std::string sourceNamespace = parseResult.first;
    std::vector<std::string> itemNames = parseResult.second;
    
    return ImportFromNamespace(sourceNamespace, itemNames);
}

std::pair<std::string, std::vector<std::string>> NamespaceNode::ParseFromExpression(const std::string& fromExpression) {
    // 解析from表达式
    // 格式：from UI::Components import Button, Card, *;
    
    std::regex fromPattern(R"(from\s+([a-zA-Z][a-zA-Z0-9:_]*)\s+import\s+([a-zA-Z0-9,\s*_]+))");
    std::smatch match;
    
    if (std::regex_match(fromExpression, match, fromPattern)) {
        std::string sourceNamespace = match[1].str();
        std::string importList = match[2].str();
        
        // 解析导入列表
        std::vector<std::string> itemNames;
        std::stringstream ss(importList);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            // 移除空白
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            
            if (!item.empty()) {
                itemNames.push_back(item);
            }
        }
        
        return {sourceNamespace, itemNames};
    }
    
    return {"", {}};
}

bool NamespaceNode::ImportFromNamespace(const std::string& sourceNamespace, const std::vector<std::string>& itemNames) {
    // 导入指定命名空间的项
    // 这里需要与全局命名空间管理器配合
    // 暂时返回true，实际实现需要全局上下文
    
    for (const auto& itemName : itemNames) {
        if (itemName == "*") {
            // 导入所有项
            // TODO: 实现通配符导入
        }
        else {
            // 导入指定项
            // TODO: 从源命名空间查找并导入项
        }
    }
    
    return true;
}

bool NamespaceNode::ValidateNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查命名空间名称格式
    std::regex namespacePattern(R"([a-zA-Z][a-zA-Z0-9_]*)");
    return std::regex_match(name, namespacePattern);
}

void NamespaceNode::AddConflict(const NamespaceConflict& conflict) {
    m_Conflicts.push_back(conflict);
    m_HasConflicts = true;
}

std::string NamespaceNode::GenerateHTML() const {
    std::ostringstream html;
    
    html << "<!-- CHTL Namespace: " << m_FullNamespacePath;
    if (m_IsDefaultNamespace) {
        html << " (Default)";
    }
    if (m_IsNestedNamespace) {
        html << " (Nested)";
    }
    html << " -->\n";
    
    html << "<div class=\"chtl-namespace\" data-namespace=\"" << m_FullNamespacePath << "\">\n";
    
    // 输出命名空间项
    for (const auto& item : m_NamespaceItems) {
        html << "  <!-- Namespace Item: " << item.second.Name << " (" << item.second.Type << ") -->\n";
    }
    
    // 输出子命名空间
    for (const auto& child : m_ChildNamespaces) {
        html << child->GenerateHTML();
    }
    
    html << "</div>\n";
    html << "<!-- End Namespace: " << m_FullNamespacePath << " -->";
    
    return html.str();
}

std::string NamespaceNode::GetTypeString() const {
    if (m_IsDefaultNamespace) {
        return "DefaultNamespace";
    }
    else if (m_IsNestedNamespace) {
        return "NestedNamespace";
    }
    else {
        return "Namespace";
    }
}

std::unique_ptr<CHTLBaseNode> NamespaceNode::Clone() const {
    auto cloned = std::make_unique<NamespaceNode>(m_NamespaceName, m_ParentNamespace, GetLine(), GetColumn());
    
    // 复制命名空间项
    cloned->m_NamespaceItems = m_NamespaceItems;
    cloned->m_IsDefaultNamespace = m_IsDefaultNamespace;
    cloned->m_AllowAutoMerge = m_AllowAutoMerge;
    
    // 复制子命名空间
    for (const auto& child : m_ChildNamespaces) {
        auto childClone = std::unique_ptr<NamespaceNode>(static_cast<NamespaceNode*>(child->Clone().release()));
        cloned->AddChildNamespace(std::move(childClone));
    }
    
    return std::move(cloned);
}

void NamespaceNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitNamespaceNode(this);
    }
}

// NamespaceManager实现
NamespaceManager::NamespaceManager() {
    // 初始化保留名称
    m_ReservedNames = {
        "CHTL", "CHTLCore", "System", "Global", "Default",
        "Html", "Style", "JavaScript", "Template", "Custom", "Origin"
    };
}

bool NamespaceManager::RegisterNamespace(NamespaceNode* namespaceNode) {
    if (!namespaceNode) {
        return false;
    }
    
    std::string fullPath = namespaceNode->GetFullNamespacePath();
    
    // 检查是否为保留名称
    if (m_ReservedNames.find(namespaceNode->GetNamespaceName()) != m_ReservedNames.end()) {
        return false; // 保留名称不能使用
    }
    
    // 检查是否已存在
    if (m_GlobalNamespaces.find(fullPath) != m_GlobalNamespaces.end()) {
        // 尝试合并
        NamespaceNode* existing = m_GlobalNamespaces[fullPath];
        return existing->MergeNamespace(namespaceNode);
    }
    else {
        // 注册新命名空间
        m_GlobalNamespaces[fullPath] = namespaceNode;
        return true;
    }
}

NamespaceNode* NamespaceManager::FindNamespace(const std::string& namespacePath) {
    auto it = m_GlobalNamespaces.find(namespacePath);
    return (it != m_GlobalNamespaces.end()) ? it->second : nullptr;
}

int NamespaceManager::AutoMergeNamespaces() {
    int mergeCount = 0;
    
    // 收集同名命名空间
    std::unordered_map<std::string, std::vector<NamespaceNode*>> sameNameNamespaces;
    
    for (const auto& nsEntry : m_GlobalNamespaces) {
        NamespaceNode* ns = nsEntry.second;
        std::string name = ns->GetNamespaceName();
        sameNameNamespaces[name].push_back(ns);
    }
    
    // 合并同名命名空间
    for (auto& sameNameGroup : sameNameNamespaces) {
        if (sameNameGroup.second.size() > 1) {
            // 有多个同名命名空间，尝试合并
            NamespaceNode* primary = sameNameGroup.second[0];
            
            for (size_t i = 1; i < sameNameGroup.second.size(); ++i) {
                if (primary->MergeNamespace(sameNameGroup.second[i])) {
                    mergeCount++;
                }
            }
        }
    }
    
    return mergeCount;
}

std::vector<NamespaceConflict> NamespaceManager::DetectGlobalConflicts() {
    std::vector<NamespaceConflict> allConflicts;
    
    for (const auto& nsEntry : m_GlobalNamespaces) {
        auto conflicts = nsEntry.second->DetectConflicts(m_GlobalNamespaces);
        allConflicts.insert(allConflicts.end(), conflicts.begin(), conflicts.end());
    }
    
    return allConflicts;
}

void NamespaceManager::SetCurrentNamespace(const std::string& namespacePath) {
    m_CurrentNamespace = namespacePath;
}

std::unique_ptr<NamespaceNode> NamespaceManager::CreateDefaultFileNamespace(const std::string& fileName) {
    // 从文件名创建默认命名空间
    std::string namespaceName = fileName;
    
    // 移除文件扩展名
    size_t dotPos = namespaceName.find_last_of('.');
    if (dotPos != std::string::npos) {
        namespaceName = namespaceName.substr(0, dotPos);
    }
    
    // 移除路径部分
    size_t slashPos = namespaceName.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        namespaceName = namespaceName.substr(slashPos + 1);
    }
    
    // 转换为有效的命名空间名称
    std::regex invalidChars(R"([^a-zA-Z0-9_])");
    namespaceName = std::regex_replace(namespaceName, invalidChars, "_");
    
    if (namespaceName.empty() || std::isdigit(namespaceName[0])) {
        namespaceName = "File_" + namespaceName;
    }
    
    auto defaultNamespace = std::make_unique<NamespaceNode>(namespaceName);
    defaultNamespace->SetDefaultNamespace(true);
    
    return defaultNamespace;
}

void NamespaceManager::Reset() {
    m_GlobalNamespaces.clear();
    m_CurrentNamespace.clear();
}

} // namespace CHTL