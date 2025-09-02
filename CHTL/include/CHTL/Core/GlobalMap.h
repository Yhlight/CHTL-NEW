#ifndef CHTL_CHTL_CORE_GLOBALMAP_H
#define CHTL_CHTL_CORE_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "CHTL/AST/ASTNode.h"

namespace CHTL {

// 前向声明
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;
class CustomStyleNode;
class CustomElementNode;
class CustomVarNode;
class OriginNode;
class NamespaceNode;
class ConfigurationNode;

/**
 * CHTL全局映射表
 * 管理所有全局定义的模板、自定义、原始嵌入等
 */
class GlobalMap {
private:
    // 模板映射
    std::unordered_map<std::string, std::shared_ptr<TemplateStyleNode>> templateStyles;
    std::unordered_map<std::string, std::shared_ptr<TemplateElementNode>> templateElements;
    std::unordered_map<std::string, std::shared_ptr<TemplateVarNode>> templateVars;
    
    // 自定义映射
    std::unordered_map<std::string, std::shared_ptr<CustomStyleNode>> customStyles;
    std::unordered_map<std::string, std::shared_ptr<CustomElementNode>> customElements;
    std::unordered_map<std::string, std::shared_ptr<CustomVarNode>> customVars;
    
    // 原始嵌入映射
    std::unordered_map<std::string, std::shared_ptr<OriginNode>> originBlocks;
    
    // 命名空间映射
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaces;
    
    // 配置组映射
    std::unordered_map<std::string, std::shared_ptr<ConfigurationNode>> configurations;
    std::shared_ptr<ConfigurationNode> defaultConfiguration;  // 默认（无名）配置
    
    // 当前命名空间栈
    std::vector<std::string> namespaceStack;
    
    /**
     * 获取完整的命名空间路径
     */
    std::string GetFullNamespacePath(const std::string& name) const;
    
public:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 模板操作
    void AddTemplateStyle(const std::string& name, std::shared_ptr<TemplateStyleNode> node);
    void AddTemplateElement(const std::string& name, std::shared_ptr<TemplateElementNode> node);
    void AddTemplateVar(const std::string& name, std::shared_ptr<TemplateVarNode> node);
    
    std::shared_ptr<TemplateStyleNode> GetTemplateStyle(const std::string& name) const;
    std::shared_ptr<TemplateElementNode> GetTemplateElement(const std::string& name) const;
    std::shared_ptr<TemplateVarNode> GetTemplateVar(const std::string& name) const;
    
    // 自定义操作
    void AddCustomStyle(const std::string& name, std::shared_ptr<CustomStyleNode> node);
    void AddCustomElement(const std::string& name, std::shared_ptr<CustomElementNode> node);
    void AddCustomVar(const std::string& name, std::shared_ptr<CustomVarNode> node);
    
    std::shared_ptr<CustomStyleNode> GetCustomStyle(const std::string& name) const;
    std::shared_ptr<CustomElementNode> GetCustomElement(const std::string& name) const;
    std::shared_ptr<CustomVarNode> GetCustomVar(const std::string& name) const;
    
    // 原始嵌入操作
    void AddOriginBlock(const std::string& key, std::shared_ptr<OriginNode> node);
    std::shared_ptr<OriginNode> GetOriginBlock(const std::string& key) const;
    
    // 命名空间操作
    void AddNamespace(const std::string& name, std::shared_ptr<NamespaceNode> node);
    std::shared_ptr<NamespaceNode> GetNamespace(const std::string& name) const;
    void EnterNamespace(const std::string& name);
    void ExitNamespace();
    std::string GetCurrentNamespace() const;
    
    // 配置组操作
    void AddConfiguration(const std::string& name, std::shared_ptr<ConfigurationNode> node);
    std::shared_ptr<ConfigurationNode> GetConfiguration(const std::string& name) const;
    void SetDefaultConfiguration(std::shared_ptr<ConfigurationNode> node);
    std::shared_ptr<ConfigurationNode> GetDefaultConfiguration() const;
    
    // 检查是否存在
    bool HasTemplateStyle(const std::string& name) const;
    bool HasTemplateElement(const std::string& name) const;
    bool HasTemplateVar(const std::string& name) const;
    bool HasCustomStyle(const std::string& name) const;
    bool HasCustomElement(const std::string& name) const;
    bool HasCustomVar(const std::string& name) const;
    bool HasOriginBlock(const std::string& key) const;
    bool HasNamespace(const std::string& name) const;
    bool HasConfiguration(const std::string& name) const;
    
    // 清空所有映射
    void Clear();
    
    // 合并另一个GlobalMap（用于导入）
    void Merge(const GlobalMap& other, const std::string& namespacePrefix = "");
};

} // namespace CHTL

#endif // CHTL_CHTL_CORE_GLOBALMAP_H