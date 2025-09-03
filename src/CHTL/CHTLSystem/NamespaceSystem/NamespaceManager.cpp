#include "NamespaceManager.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {

NamespaceManager::NamespaceManager() {
}

void NamespaceManager::CreateNamespace(const std::string& namespaceName,
                                      const std::string& parentNamespace,
                                      const std::string& sourceFile) {
    if (namespaceName.empty() || !IsValidNamespaceName(namespaceName)) {
        std::cerr << "❌ 无效的命名空间名称: " << namespaceName << std::endl;
        return;
    }
    
    // 检查循环依赖
    if (HasCircularDependency(namespaceName, parentNamespace)) {
        std::cerr << "❌ 检测到命名空间循环依赖: " << namespaceName << " → " << parentNamespace << std::endl;
        return;
    }
    
    // 构建完整的命名空间名称
    std::string fullName = BuildNamespaceHierarchy(namespaceName, parentNamespace);
    
    // 检查是否已存在 (用于自动合并)
    auto it = m_Namespaces.find(fullName);
    if (it != m_Namespaces.end()) {
        // 同名命名空间自动合并
        std::cout << "🔄 自动合并同名命名空间: " << fullName << std::endl;
        if (!sourceFile.empty() && it->second->SourceFile != sourceFile) {
            it->second->SourceFile += ", " + sourceFile;
        }
        return;
    }
    
    // 创建新的命名空间
    auto namespaceInfo = std::make_unique<NamespaceInfo>();
    namespaceInfo->Name = namespaceName;
    namespaceInfo->FullName = fullName;
    namespaceInfo->ParentNamespace = parentNamespace;
    namespaceInfo->SourceFile = sourceFile;
    
    // 添加到父命名空间的子列表
    if (!parentNamespace.empty()) {
        auto parentIt = m_Namespaces.find(parentNamespace);
        if (parentIt != m_Namespaces.end()) {
            parentIt->second->ChildNamespaces.push_back(fullName);
        }
    }
    
    m_Namespaces[fullName] = std::move(namespaceInfo);
    
    std::cout << "📁 创建命名空间: " << fullName;
    if (!parentNamespace.empty()) {
        std::cout << " (父: " << parentNamespace << ")";
    }
    if (!sourceFile.empty()) {
        std::cout << " (来源: " << sourceFile << ")";
    }
    std::cout << std::endl;
}

void NamespaceManager::SetCurrentNamespace(const std::string& namespaceName) {
    m_CurrentNamespace = namespaceName;
    std::cout << "📁 设置当前命名空间: " << namespaceName << std::endl;
}

std::string NamespaceManager::GetCurrentNamespace() const {
    if (!m_CurrentNamespace.empty()) {
        return m_CurrentNamespace;
    }
    
    // 如果没有设置当前命名空间且未禁用默认命名空间，返回默认命名空间
    if (!m_DisableDefaultNamespace && !m_DefaultNamespace.empty()) {
        return m_DefaultNamespace;
    }
    
    return "";
}

void NamespaceManager::SetDefaultNamespace(const std::string& defaultNamespace) {
    m_DefaultNamespace = defaultNamespace;
    
    if (!m_DisableDefaultNamespace) {
        std::cout << "📁 设置默认命名空间: " << defaultNamespace << " (来自文件名)" << std::endl;
    }
}

void NamespaceManager::AddMemberToNamespace(const std::string& namespaceName,
                                           const std::string& memberName,
                                           const std::string& memberType) {
    std::string fullNamespaceName = namespaceName.empty() ? GetCurrentNamespace() : namespaceName;
    
    if (fullNamespaceName.empty()) {
        // 没有命名空间，直接添加到全局
        return;
    }
    
    auto it = m_Namespaces.find(fullNamespaceName);
    if (it != m_Namespaces.end()) {
        std::string memberKey = memberType + "::" + memberName;
        it->second->Members.insert(memberKey);
        std::cout << "📝 添加成员到命名空间 " << fullNamespaceName << ": " << memberKey << std::endl;
    }
}

std::pair<std::string, std::string> NamespaceManager::ParseQualifiedName(const std::string& qualifiedName) const {
    size_t pos = qualifiedName.find_last_of("::");
    if (pos != std::string::npos && pos > 0) {
        return {qualifiedName.substr(0, pos - 1), qualifiedName.substr(pos + 1)};
    }
    return {"", qualifiedName};
}

std::string NamespaceManager::BuildQualifiedName(const std::string& namespaceName, const std::string& memberName) const {
    if (namespaceName.empty()) {
        return memberName;
    }
    return namespaceName + "::" + memberName;
}

std::string NamespaceManager::FindMember(const std::string& memberName, const std::string& memberType) const {
    auto [namespaceName, simpleName] = ParseQualifiedName(memberName);
    
    if (!namespaceName.empty()) {
        // 有明确的命名空间，直接查找
        auto it = m_Namespaces.find(namespaceName);
        if (it != m_Namespaces.end()) {
            std::string memberKey = memberType + "::" + simpleName;
            if (it->second->Members.find(memberKey) != it->second->Members.end()) {
                return BuildQualifiedName(namespaceName, simpleName);
            }
        }
        return "";
    }
    
    // 没有明确命名空间，在当前命名空间中查找
    std::string currentNs = GetCurrentNamespace();
    if (!currentNs.empty()) {
        auto it = m_Namespaces.find(currentNs);
        if (it != m_Namespaces.end()) {
            std::string memberKey = memberType + "::" + simpleName;
            if (it->second->Members.find(memberKey) != it->second->Members.end()) {
                return BuildQualifiedName(currentNs, simpleName);
            }
        }
    }
    
    // 在所有命名空间中查找
    for (const auto& [nsName, nsInfo] : m_Namespaces) {
        std::string memberKey = memberType + "::" + simpleName;
        if (nsInfo->Members.find(memberKey) != nsInfo->Members.end()) {
            return BuildQualifiedName(nsName, simpleName);
        }
    }
    
    return simpleName; // 未找到，返回原始名称
}

void NamespaceManager::MergeSameNameNamespaces(const std::string& namespaceName) {
    std::vector<std::string> sameNameNamespaces;
    
    // 查找所有同名的命名空间
    for (const auto& [fullName, nsInfo] : m_Namespaces) {
        if (nsInfo->Name == namespaceName) {
            sameNameNamespaces.push_back(fullName);
        }
    }
    
    if (sameNameNamespaces.size() > 1) {
        std::cout << "🔄 合并同名命名空间: " << namespaceName << " (数量: " << sameNameNamespaces.size() << ")" << std::endl;
        
        // 合并到第一个命名空间
        auto& primaryNamespace = m_Namespaces[sameNameNamespaces[0]];
        
        for (size_t i = 1; i < sameNameNamespaces.size(); ++i) {
            const auto& mergeNamespace = m_Namespaces[sameNameNamespaces[i]];
            
            // 合并成员
            for (const auto& member : mergeNamespace->Members) {
                primaryNamespace->Members.insert(member);
            }
            
            // 合并子命名空间
            for (const auto& child : mergeNamespace->ChildNamespaces) {
                if (std::find(primaryNamespace->ChildNamespaces.begin(), 
                             primaryNamespace->ChildNamespaces.end(), child) == primaryNamespace->ChildNamespaces.end()) {
                    primaryNamespace->ChildNamespaces.push_back(child);
                }
            }
            
            // 合并源文件信息
            if (!mergeNamespace->SourceFile.empty()) {
                if (primaryNamespace->SourceFile.find(mergeNamespace->SourceFile) == std::string::npos) {
                    primaryNamespace->SourceFile += ", " + mergeNamespace->SourceFile;
                }
            }
            
            // 删除被合并的命名空间
            m_Namespaces.erase(sameNameNamespaces[i]);
        }
    }
}

std::vector<NamespaceConflict> NamespaceManager::DetectNamespaceConflicts() {
    std::vector<NamespaceConflict> conflicts;
    
    // 检测成员名称冲突
    std::unordered_map<std::string, std::vector<std::string>> memberSources;
    
    for (const auto& [nsName, nsInfo] : m_Namespaces) {
        for (const auto& member : nsInfo->Members) {
            memberSources[member].push_back(nsName);
        }
    }
    
    // 查找冲突
    for (const auto& [member, sources] : memberSources) {
        if (sources.size() > 1) {
            NamespaceConflict conflict;
            conflict.ConflictName = member;
            conflict.ConflictSources = sources;
            conflict.Resolution = "使用完整限定名访问";
            conflicts.push_back(conflict);
        }
    }
    
    return conflicts;
}

std::string NamespaceManager::HandleFileImportNamespace(const std::string& fileName, bool hasExplicitNamespace) {
    if (hasExplicitNamespace || m_DisableDefaultNamespace) {
        return "";
    }
    
    // 从文件名生成默认命名空间
    std::string namespaceName = fileName;
    
    // 移除文件扩展名
    size_t dotPos = namespaceName.find_last_of('.');
    if (dotPos != std::string::npos) {
        namespaceName = namespaceName.substr(0, dotPos);
    }
    
    // 移除路径
    size_t slashPos = namespaceName.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        namespaceName = namespaceName.substr(slashPos + 1);
    }
    
    CreateNamespace(namespaceName, "", fileName);
    return namespaceName;
}

bool NamespaceManager::IsValidNamespaceName(const std::string& namespaceName) const {
    if (namespaceName.empty()) {
        return false;
    }
    
    // 检查是否符合标识符规则
    std::regex namespaceRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(namespaceName, namespaceRegex);
}

bool NamespaceManager::HasCircularDependency(const std::string& namespaceName, const std::string& parentNamespace) const {
    if (parentNamespace.empty()) {
        return false;
    }
    
    // 检查父命名空间是否包含当前命名空间
    return parentNamespace.find(namespaceName) != std::string::npos;
}

std::string NamespaceManager::BuildNamespaceHierarchy(const std::string& namespaceName, const std::string& parentNamespace) {
    if (parentNamespace.empty()) {
        return namespaceName;
    }
    return parentNamespace + "::" + namespaceName;
}

bool NamespaceManager::ValidateNamespaceSystem() {
    // 检测冲突
    m_Conflicts = DetectNamespaceConflicts();
    
    if (!m_Conflicts.empty()) {
        std::cout << "⚠️ 检测到 " << m_Conflicts.size() << " 个命名空间冲突" << std::endl;
        for (const auto& conflict : m_Conflicts) {
            std::cout << "  冲突: " << conflict.ConflictName << " (来源: ";
            for (size_t i = 0; i < conflict.ConflictSources.size(); ++i) {
                std::cout << conflict.ConflictSources[i];
                if (i < conflict.ConflictSources.size() - 1) std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }
    }
    
    return m_Conflicts.empty();
}

void NamespaceManager::Clear() {
    m_Namespaces.clear();
    m_Conflicts.clear();
    m_NamespaceAliases.clear();
    m_CurrentNamespace.clear();
    m_DefaultNamespace.clear();
}

} // namespace CHTL