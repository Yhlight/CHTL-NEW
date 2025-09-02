#ifndef CHTL_CHTL_AST_TEMPLATENODES_H
#define CHTL_CHTL_AST_TEMPLATENODES_H

#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"
#include <map>

namespace CHTL {

class TemplateStyleNode : public ASTNode {
private:
    std::string styleName;
    
public:
    TemplateStyleNode(const std::string& name = "") 
        : ASTNode(ASTNodeType::TemplateStyle), styleName(name) {}
    
    const std::string& GetName() const { return styleName; }
    void SetName(const std::string& name) { styleName = name; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "TemplateStyleNode(" + styleName + ")"; 
    }
};

class TemplateElementNode : public ASTNode {
private:
    std::string elementName;
    
public:
    TemplateElementNode(const std::string& name = "") 
        : ASTNode(ASTNodeType::TemplateElement), elementName(name) {}
    
    const std::string& GetName() const { return elementName; }
    void SetName(const std::string& name) { elementName = name; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "TemplateElementNode(" + elementName + ")"; 
    }
};

class TemplateVarNode : public ASTNode {
private:
    std::string varGroupName;
    std::map<std::string, std::string> variables;
    
public:
    TemplateVarNode(const std::string& name = "") 
        : ASTNode(ASTNodeType::TemplateVar), varGroupName(name) {}
    
    const std::string& GetName() const { return varGroupName; }
    void SetName(const std::string& name) { varGroupName = name; }
    
    void AddVariable(const std::string& name, const std::string& value) {
        variables[name] = value;
    }
    
    std::string GetVariable(const std::string& name) const {
        auto it = variables.find(name);
        return (it != variables.end()) ? it->second : "";
    }
    
    const std::map<std::string, std::string>& GetVariables() const {
        return variables;
    }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "TemplateVarNode(" + varGroupName + ")"; 
    }
};

class CustomStyleNode : public ASTNode {
public:
    CustomStyleNode() : ASTNode(ASTNodeType::CustomStyle) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomStyleNode"; }
};

class CustomElementNode : public ASTNode {
public:
    CustomElementNode() : ASTNode(ASTNodeType::CustomElement) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomElementNode"; }
};

class CustomVarNode : public ASTNode {
public:
    CustomVarNode() : ASTNode(ASTNodeType::CustomVar) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CustomVarNode"; }
};

class OriginNode : public ASTNode {
public:
    OriginNode() : ASTNode(ASTNodeType::Origin) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "OriginNode"; }
};



class NamespaceNode : public ASTNode {
private:
    std::string namespaceName;
    
public:
    NamespaceNode(const std::string& name = "") 
        : ASTNode(ASTNodeType::Namespace), namespaceName(name) {}
    
    const std::string& GetName() const { return namespaceName; }
    void SetName(const std::string& name) { namespaceName = name; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "NamespaceNode(" + namespaceName + ")"; 
    }
};

class ConfigurationNode : public ASTNode {
private:
    std::string configName;  // 配置组名称（可选）
    std::map<std::string, std::string> options;  // 配置选项
    std::map<std::string, std::shared_ptr<ASTNode>> subGroups;  // 子配置组如[Name], [OriginType]
    
public:
    ConfigurationNode(const std::string& name = "") 
        : ASTNode(ASTNodeType::Configuration), configName(name) {}
    
    const std::string& GetName() const { return configName; }
    void SetName(const std::string& name) { configName = name; }
    
    void AddOption(const std::string& key, const std::string& value) {
        options[key] = value;
    }
    
    std::string GetOption(const std::string& key) const {
        auto it = options.find(key);
        return (it != options.end()) ? it->second : "";
    }
    
    const std::map<std::string, std::string>& GetOptions() const { return options; }
    
    void AddSubGroup(const std::string& name, std::shared_ptr<ASTNode> group) {
        subGroups[name] = group;
    }
    
    std::shared_ptr<ASTNode> GetSubGroup(const std::string& name) const {
        auto it = subGroups.find(name);
        return (it != subGroups.end()) ? it->second : nullptr;
    }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "ConfigurationNode(" + (configName.empty() ? "unnamed" : configName) + ")"; 
    }
};

class LocalStyleNode : public ASTNode {
private:
    // 存储CSS样式属性
    std::map<std::string, std::string> properties;
    
public:
    LocalStyleNode(int line = 0, int column = 0) 
        : ASTNode(ASTNodeType::LocalStyle, line, column) {}
    
    // 添加样式属性
    void AddProperty(const std::string& name, const std::string& value) {
        properties[name] = value;
    }
    
    // 获取所有属性
    const std::map<std::string, std::string>& GetProperties() const {
        return properties;
    }
    
    // 生成内联样式字符串
    std::string GenerateInlineStyle() const {
        std::string result;
        for (const auto& [name, value] : properties) {
            if (!result.empty()) result += " ";
            result += name + ": " + value + ";";
        }
        return result;
    }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "LocalStyleNode(" + std::to_string(properties.size()) + " properties)"; 
    }
};

class LocalScriptNode : public ASTNode {
public:
    LocalScriptNode() : ASTNode(ASTNodeType::LocalScript) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "LocalScriptNode"; }
};

class StyleRuleNode : public ASTNode {
public:
    StyleRuleNode() : ASTNode(ASTNodeType::StyleRule) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "StyleRuleNode"; }
};

class StylePropertyNode : public ASTNode {
private:
    std::string propertyName;
    std::string propertyValue;
    
public:
    StylePropertyNode(const std::string& name = "", const std::string& value = "") 
        : ASTNode(ASTNodeType::StyleProperty), propertyName(name), propertyValue(value) {}
    
    const std::string& GetName() const { return propertyName; }
    const std::string& GetValue() const { return propertyValue; }
    
    void SetName(const std::string& name) { propertyName = name; }
    void SetValue(const std::string& value) { propertyValue = value; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { 
        return "StylePropertyNode(" + propertyName + ": " + propertyValue + ")"; 
    }
};



class SpecializationNode : public ASTNode {
public:
    SpecializationNode() : ASTNode(ASTNodeType::Specialization) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "SpecializationNode"; }
};

class DeleteNode : public ASTNode {
public:
    DeleteNode() : ASTNode(ASTNodeType::Delete) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "DeleteNode"; }
};

class InsertNode : public ASTNode {
public:
    InsertNode() : ASTNode(ASTNodeType::Insert) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "InsertNode"; }
};

class CommentNode : public ASTNode {
public:
    CommentNode() : ASTNode(ASTNodeType::Comment) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "CommentNode"; }
};

class UseNode : public ASTNode {
private:
    std::string useType; // "html5" or config name
    
public:
    UseNode(const std::string& type = "html5") : ASTNode(ASTNodeType::Use), useType(type) {}
    
    const std::string& GetUseType() const { return useType; }
    
    void Accept(ASTVisitor* visitor) override {
        // UseNode不需要特殊的访问逻辑
    }
    std::string ToString() const override { return "UseNode(" + useType + ")"; }
};

class ImportNode : public ASTNode {
public:
    enum class ImportType {
        Html,        // [Import] @Html
        Style,       // [Import] @Style  
        JavaScript,  // [Import] @JavaScript
        Chtl,        // [Import] @Chtl
        CJmod,       // [Import] @CJmod
        Config,      // [Import] @Config
        
        // 自定义类型
        CustomElement,     // [Import] [Custom] @Element
        CustomStyle,       // [Import] [Custom] @Style
        CustomVar,         // [Import] [Custom] @Var
        
        // 模板类型
        TemplateElement,   // [Import] [Template] @Element
        TemplateStyle,     // [Import] [Template] @Style
        TemplateVar,       // [Import] [Template] @Var
        
        // 原始嵌入类型
        OriginHtml,        // [Import] [Origin] @Html
        OriginStyle,       // [Import] [Origin] @Style
        OriginJavaScript,  // [Import] [Origin] @JavaScript
        OriginCustom,      // [Import] [Origin] @CustomType
        
        // 批量导入
        AllTemplate,       // [Import] [Template]
        AllCustom,         // [Import] [Custom]
        AllOrigin,         // [Import] [Origin]
        AllConfiguration,  // [Import] [Configuration]
        
        // 特定类型的批量导入
        AllCustomElement,     // [Import] [Custom] @Element
        AllCustomStyle,       // [Import] [Custom] @Style
        AllCustomVar,         // [Import] [Custom] @Var
        AllTemplateElement,   // [Import] [Template] @Element
        AllTemplateStyle,     // [Import] [Template] @Style
        AllTemplateVar        // [Import] [Template] @Var
    };
    
private:
    ImportType importType;
    std::string itemName;      // 具体要导入的项目名称（可选）
    std::string fromPath;      // from后的路径
    std::string asName;        // as后的重命名（可选）
    std::string customTypeName; // 对于自定义Origin类型，如@Vue
    
public:
    ImportNode() : ASTNode(ASTNodeType::Import) {}
    
    void SetImportType(ImportType type) { importType = type; }
    ImportType GetImportType() const { return importType; }
    
    void SetItemName(const std::string& name) { itemName = name; }
    const std::string& GetItemName() const { return itemName; }
    
    void SetFromPath(const std::string& path) { fromPath = path; }
    const std::string& GetFromPath() const { return fromPath; }
    
    void SetAsName(const std::string& name) { asName = name; }
    const std::string& GetAsName() const { return asName; }
    
    void SetCustomTypeName(const std::string& name) { customTypeName = name; }
    const std::string& GetCustomTypeName() const { return customTypeName; }
    
    void Accept(ASTVisitor* visitor) override;
    
    std::string ToString() const override {
        std::string result = "ImportNode(";
        
        // 添加导入类型
        switch (importType) {
            case ImportType::Html: result += "@Html"; break;
            case ImportType::Style: result += "@Style"; break;
            case ImportType::JavaScript: result += "@JavaScript"; break;
            case ImportType::Chtl: result += "@Chtl"; break;
            case ImportType::CJmod: result += "@CJmod"; break;
            case ImportType::Config: result += "@Config"; break;
            case ImportType::CustomElement: result += "[Custom] @Element"; break;
            case ImportType::CustomStyle: result += "[Custom] @Style"; break;
            case ImportType::CustomVar: result += "[Custom] @Var"; break;
            case ImportType::TemplateElement: result += "[Template] @Element"; break;
            case ImportType::TemplateStyle: result += "[Template] @Style"; break;
            case ImportType::TemplateVar: result += "[Template] @Var"; break;
            case ImportType::OriginHtml: result += "[Origin] @Html"; break;
            case ImportType::OriginStyle: result += "[Origin] @Style"; break;
            case ImportType::OriginJavaScript: result += "[Origin] @JavaScript"; break;
            case ImportType::OriginCustom: result += "[Origin] @" + customTypeName; break;
            case ImportType::AllTemplate: result += "[Template]"; break;
            case ImportType::AllCustom: result += "[Custom]"; break;
            case ImportType::AllOrigin: result += "[Origin]"; break;
            case ImportType::AllConfiguration: result += "[Configuration]"; break;
            case ImportType::AllCustomElement: result += "[Custom] @Element (all)"; break;
            case ImportType::AllCustomStyle: result += "[Custom] @Style (all)"; break;
            case ImportType::AllCustomVar: result += "[Custom] @Var (all)"; break;
            case ImportType::AllTemplateElement: result += "[Template] @Element (all)"; break;
            case ImportType::AllTemplateStyle: result += "[Template] @Style (all)"; break;
            case ImportType::AllTemplateVar: result += "[Template] @Var (all)"; break;
        }
        
        if (!itemName.empty()) {
            result += " " + itemName;
        }
        
        result += " from " + fromPath;
        
        if (!asName.empty()) {
            result += " as " + asName;
        }
        
        result += ")";
        return result;
    }
};

} // namespace CHTL

#endif // CHTL_CHTL_AST_TEMPLATENODES_H