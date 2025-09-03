#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {

    // 前向声明
    enum class CHTLTokenType;

    // CHTL全局映射表
    // 管理CHTL编译器的全局状态和映射关系
    // 完全独立于CHTL JS系统
    class CHTLGlobalMap {
    private:
        // 模板映射
        std::unordered_map<std::string, std::string> templateStyleMap;      // 样式组模板
        std::unordered_map<std::string, std::string> templateElementMap;    // 元素模板
        std::unordered_map<std::string, std::string> templateVarMap;        // 变量组模板
        
        // 自定义映射
        std::unordered_map<std::string, std::string> customStyleMap;        // 自定义样式组
        std::unordered_map<std::string, std::string> customElementMap;      // 自定义元素
        std::unordered_map<std::string, std::string> customVarMap;          // 自定义变量组
        
        // 原始嵌入映射
        std::unordered_map<std::string, std::string> originHtmlMap;         // HTML原始嵌入
        std::unordered_map<std::string, std::string> originStyleMap;        // Style原始嵌入
        std::unordered_map<std::string, std::string> originJSMap;           // JavaScript原始嵌入
        std::unordered_map<std::string, std::string> customOriginMap;       // 自定义类型原始嵌入
        
        // 命名空间映射
        std::unordered_map<std::string, std::unordered_set<std::string>> namespaceMap;  // 命名空间内容
        std::unordered_map<std::string, std::string> namespaceHierarchy;    // 命名空间层次结构
        
        // 导入映射
        std::unordered_map<std::string, std::string> importPathMap;         // 导入路径映射
        std::unordered_map<std::string, std::string> importAliasMap;        // 导入别名映射
        
        // 配置映射
        std::unordered_map<std::string, std::string> configurationMap;      // 配置项映射
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> namedConfigMap;  // 命名配置组
        
        // 选择器自动化映射
        std::unordered_map<std::string, std::string> autoClassMap;          // 自动添加的类名映射
        std::unordered_map<std::string, std::string> autoIdMap;             // 自动添加的ID映射
        
        // 约束映射
        std::unordered_map<std::string, std::unordered_set<std::string>> constraintMap;  // 约束规则映射

        static std::shared_ptr<CHTLGlobalMap> instance;

    public:
        // 单例模式
        static std::shared_ptr<CHTLGlobalMap> GetInstance();
        
        // 模板管理
        void RegisterTemplateStyle(const std::string& name, const std::string& content);
        void RegisterTemplateElement(const std::string& name, const std::string& content);
        void RegisterTemplateVar(const std::string& name, const std::string& content);
        
        std::string GetTemplateStyle(const std::string& name) const;
        std::string GetTemplateElement(const std::string& name) const;
        std::string GetTemplateVar(const std::string& name) const;
        
        bool HasTemplateStyle(const std::string& name) const;
        bool HasTemplateElement(const std::string& name) const;
        bool HasTemplateVar(const std::string& name) const;
        
        // 自定义管理
        void RegisterCustomStyle(const std::string& name, const std::string& content);
        void RegisterCustomElement(const std::string& name, const std::string& content);
        void RegisterCustomVar(const std::string& name, const std::string& content);
        
        std::string GetCustomStyle(const std::string& name) const;
        std::string GetCustomElement(const std::string& name) const;
        std::string GetCustomVar(const std::string& name) const;
        
        bool HasCustomStyle(const std::string& name) const;
        bool HasCustomElement(const std::string& name) const;
        bool HasCustomVar(const std::string& name) const;
        
        // 原始嵌入管理
        void RegisterOriginHtml(const std::string& name, const std::string& content);
        void RegisterOriginStyle(const std::string& name, const std::string& content);
        void RegisterOriginJS(const std::string& name, const std::string& content);
        void RegisterCustomOrigin(const std::string& type, const std::string& name, const std::string& content);
        
        std::string GetOriginHtml(const std::string& name) const;
        std::string GetOriginStyle(const std::string& name) const;
        std::string GetOriginJS(const std::string& name) const;
        std::string GetCustomOrigin(const std::string& type, const std::string& name) const;
        
        // 命名空间管理
        void RegisterNamespace(const std::string& name);
        void AddToNamespace(const std::string& namespaceName, const std::string& itemName);
        void SetNamespaceHierarchy(const std::string& child, const std::string& parent);
        
        bool HasNamespace(const std::string& name) const;
        std::unordered_set<std::string> GetNamespaceContent(const std::string& name) const;
        std::string GetFullNamespacePath(const std::string& name) const;
        
        // 导入管理
        void RegisterImport(const std::string& path, const std::string& alias = "");
        void RegisterImportAlias(const std::string& alias, const std::string& originalName);
        
        std::string GetImportPath(const std::string& alias) const;
        std::string GetImportAlias(const std::string& originalName) const;
        bool HasImport(const std::string& path) const;
        
        // 配置管理
        void SetConfiguration(const std::string& key, const std::string& value);
        void SetNamedConfiguration(const std::string& configName, const std::string& key, const std::string& value);
        
        std::string GetConfiguration(const std::string& key) const;
        std::string GetNamedConfiguration(const std::string& configName, const std::string& key) const;
        bool HasConfiguration(const std::string& key) const;
        
        // 选择器自动化管理
        void RegisterAutoClass(const std::string& elementId, const std::string& className);
        void RegisterAutoId(const std::string& elementId, const std::string& idName);
        
        std::string GetAutoClass(const std::string& elementId) const;
        std::string GetAutoId(const std::string& elementId) const;
        
        // 约束管理
        void AddConstraint(const std::string& scope, const std::string& constraint);
        bool IsConstrained(const std::string& scope, const std::string& item) const;
        std::unordered_set<std::string> GetConstraints(const std::string& scope) const;
        
        // 工具方法
        void Clear();                                           // 清空所有映射
        void Reset();                                           // 重置为初始状态
        std::vector<std::string> GetAllTemplateNames() const;   // 获取所有模板名称
        std::vector<std::string> GetAllCustomNames() const;     // 获取所有自定义名称
        std::vector<std::string> GetAllNamespaces() const;      // 获取所有命名空间
        
        // 调试方法
        void PrintMappings() const;                             // 打印所有映射（调试用）
    };

} // namespace CHTL