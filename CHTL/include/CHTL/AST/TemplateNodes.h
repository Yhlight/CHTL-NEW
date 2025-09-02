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

class ImportNode : public ASTNode {
public:
    ImportNode() : ASTNode(ASTNodeType::Import) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "ImportNode"; }
};

class NamespaceNode : public ASTNode {
public:
    NamespaceNode() : ASTNode(ASTNodeType::Namespace) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "NamespaceNode"; }
};

class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode() : ASTNode(ASTNodeType::Configuration) {}
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override { return "ConfigurationNode"; }
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

} // namespace CHTL

#endif // CHTL_CHTL_AST_TEMPLATENODES_H