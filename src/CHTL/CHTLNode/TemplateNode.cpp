#include "TemplateNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

    // CHTLTemplateNode实现
    CHTLTemplateNode::CHTLTemplateNode(CHTLTemplateType type, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::TEMPLATE, name), 
          templateType(type), templateName(name), isUsage(false) {
    }

    // 模板类型管理实现
    CHTLTemplateType CHTLTemplateNode::GetTemplateType() const {
        return templateType;
    }

    void CHTLTemplateNode::SetTemplateType(CHTLTemplateType type) {
        templateType = type;
    }

    // 模板名称管理实现
    const std::string& CHTLTemplateNode::GetTemplateName() const {
        return templateName;
    }

    void CHTLTemplateNode::SetTemplateName(const std::string& name) {
        templateName = name;
        SetNodeName(name);
    }

    // 使用/定义标记实现
    bool CHTLTemplateNode::IsUsage() const {
        return isUsage;
    }

    void CHTLTemplateNode::SetIsUsage(bool usage) {
        isUsage = usage;
    }

    // 继承管理实现
    void CHTLTemplateNode::AddInheritance(const std::string& templateName, CHTLInheritanceType type) {
        // 检查是否已经继承该模板
        if (std::find(inheritedTemplates.begin(), inheritedTemplates.end(), templateName) == inheritedTemplates.end()) {
            inheritedTemplates.push_back(templateName);
            inheritanceTypes.push_back(type);
        }
    }

    void CHTLTemplateNode::RemoveInheritance(const std::string& templateName) {
        auto it = std::find(inheritedTemplates.begin(), inheritedTemplates.end(), templateName);
        if (it != inheritedTemplates.end()) {
            size_t index = std::distance(inheritedTemplates.begin(), it);
            inheritedTemplates.erase(it);
            inheritanceTypes.erase(inheritanceTypes.begin() + index);
        }
    }

    const std::vector<std::string>& CHTLTemplateNode::GetInheritedTemplates() const {
        return inheritedTemplates;
    }

    const std::vector<CHTLInheritanceType>& CHTLTemplateNode::GetInheritanceTypes() const {
        return inheritanceTypes;
    }

    bool CHTLTemplateNode::HasInheritance(const std::string& templateName) const {
        return std::find(inheritedTemplates.begin(), inheritedTemplates.end(), templateName) != inheritedTemplates.end();
    }

    // 重写虚方法实现
    std::string CHTLTemplateNode::ToString() const {
        std::stringstream ss;
        ss << "Template[" << TemplateTypeToString(templateType) << "]";
        ss << " \"" << templateName << "\"";
        if (isUsage) ss << " (usage)";
        if (!inheritedTemplates.empty()) {
            ss << " inherits:" << inheritedTemplates.size();
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTemplateNode::Clone() const {
        auto cloned = std::make_shared<CHTLTemplateNode>(templateType, templateName);
        
        cloned->inheritedTemplates = inheritedTemplates;
        cloned->inheritanceTypes = inheritanceTypes;
        cloned->isUsage = isUsage;
        
        // 递归克隆子节点
        for (const auto& child : GetChildren()) {
            cloned->AddChild(child->Clone());
        }
        
        return cloned;
    }

    bool CHTLTemplateNode::Validate() const {
        // 验证模板名称是否有效
        if (templateName.empty()) return false;
        
        // 验证继承列表与类型列表长度一致
        if (inheritedTemplates.size() != inheritanceTypes.size()) return false;
        
        return CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLTemplateNode::TemplateTypeToString(CHTLTemplateType type) {
        switch (type) {
            case CHTLTemplateType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
            case CHTLTemplateType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
            case CHTLTemplateType::TEMPLATE_VAR: return "TEMPLATE_VAR";
            default: return "UNKNOWN";
        }
    }

    std::string CHTLTemplateNode::InheritanceTypeToString(CHTLInheritanceType type) {
        switch (type) {
            case CHTLInheritanceType::COMPOSITION: return "COMPOSITION";
            case CHTLInheritanceType::EXPLICIT: return "EXPLICIT";
            default: return "UNKNOWN";
        }
    }

    // CHTLTemplateStyleNode实现
    CHTLTemplateStyleNode::CHTLTemplateStyleNode(const std::string& styleName) 
        : CHTLTemplateNode(CHTLTemplateType::TEMPLATE_STYLE, styleName) {
    }

    // 样式属性管理实现
    void CHTLTemplateStyleNode::AddStyleProperty(const std::string& property, const std::string& value) {
        // 如果属性已存在，根据书写顺序进行值的替换
        if (styleProperties.find(property) == styleProperties.end()) {
            propertyOrder.push_back(property);
        }
        styleProperties[property] = value;
    }

    void CHTLTemplateStyleNode::RemoveStyleProperty(const std::string& property) {
        styleProperties.erase(property);
        
        auto it = std::find(propertyOrder.begin(), propertyOrder.end(), property);
        if (it != propertyOrder.end()) {
            propertyOrder.erase(it);
        }
    }

    std::string CHTLTemplateStyleNode::GetStyleProperty(const std::string& property) const {
        auto it = styleProperties.find(property);
        return (it != styleProperties.end()) ? it->second : "";
    }

    bool CHTLTemplateStyleNode::HasStyleProperty(const std::string& property) const {
        return styleProperties.find(property) != styleProperties.end();
    }

    const std::unordered_map<std::string, std::string>& CHTLTemplateStyleNode::GetAllStyleProperties() const {
        return styleProperties;
    }

    // 属性顺序管理实现
    const std::vector<std::string>& CHTLTemplateStyleNode::GetPropertyOrder() const {
        return propertyOrder;
    }

    void CHTLTemplateStyleNode::SetPropertyOrder(const std::vector<std::string>& order) {
        propertyOrder = order;
    }

    // 继承处理实现
    void CHTLTemplateStyleNode::ApplyInheritance(const CHTLTemplateStyleNode& parentTemplate) {
        // 获得父模板的所有属性
        const auto& parentProperties = parentTemplate.GetAllStyleProperties();
        const auto& parentOrder = parentTemplate.GetPropertyOrder();
        
        // 按照父模板的顺序添加属性
        for (const auto& propName : parentOrder) {
            if (parentProperties.find(propName) != parentProperties.end()) {
                // 如果当前模板没有该属性，则添加
                if (styleProperties.find(propName) == styleProperties.end()) {
                    AddStyleProperty(propName, parentProperties.at(propName));
                }
                // 如果已有该属性，保持当前值（重复元素按书写顺序替换）
            }
        }
    }

    void CHTLTemplateStyleNode::ProcessCompositionInheritance(const CHTLTemplateStyleNode& parent) {
        ApplyInheritance(parent);
    }

    void CHTLTemplateStyleNode::ProcessExplicitInheritance(const CHTLTemplateStyleNode& parent) {
        ApplyInheritance(parent);
    }

    // CSS生成实现
    std::string CHTLTemplateStyleNode::GenerateCSS() const {
        std::stringstream ss;
        
        // 按照属性顺序生成CSS
        for (const auto& propName : propertyOrder) {
            auto it = styleProperties.find(propName);
            if (it != styleProperties.end()) {
                ss << propName << ": " << it->second << ";\n";
            }
        }
        
        return ss.str();
    }

    std::string CHTLTemplateStyleNode::GenerateInlineStyle() const {
        std::stringstream ss;
        
        // 生成内联样式格式
        for (const auto& propName : propertyOrder) {
            auto it = styleProperties.find(propName);
            if (it != styleProperties.end()) {
                ss << propName << ": " << it->second << "; ";
            }
        }
        
        return ss.str();
    }

    // 重写虚方法实现
    std::string CHTLTemplateStyleNode::ToString() const {
        std::stringstream ss;
        ss << "TemplateStyle[" << GetTemplateName() << "]";
        ss << " props:" << styleProperties.size();
        if (!GetInheritedTemplates().empty()) {
            ss << " inherits:" << GetInheritedTemplates().size();
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTemplateStyleNode::Clone() const {
        auto cloned = std::make_shared<CHTLTemplateStyleNode>(GetTemplateName());
        
        cloned->styleProperties = styleProperties;
        cloned->propertyOrder = propertyOrder;
        
        // 复制继承信息
        for (size_t i = 0; i < GetInheritedTemplates().size(); i++) {
            cloned->AddInheritance(GetInheritedTemplates()[i], GetInheritanceTypes()[i]);
        }
        
        return cloned;
    }

    bool CHTLTemplateStyleNode::Validate() const {
        // 验证样式属性是否有效
        for (const auto& prop : styleProperties) {
            if (prop.first.empty() || prop.second.empty()) {
                return false;
            }
        }
        
        return CHTLTemplateNode::Validate();
    }

    // CHTLTemplateElementNode实现
    CHTLTemplateElementNode::CHTLTemplateElementNode(const std::string& elementName) 
        : CHTLTemplateNode(CHTLTemplateType::TEMPLATE_ELEMENT, elementName) {
    }

    // 模板元素管理实现
    void CHTLTemplateElementNode::AddTemplateElement(std::shared_ptr<CHTLBaseNode> element) {
        if (element) {
            templateElements.push_back(element);
            AddChild(element);
        }
    }

    void CHTLTemplateElementNode::RemoveTemplateElement(std::shared_ptr<CHTLBaseNode> element) {
        auto it = std::find(templateElements.begin(), templateElements.end(), element);
        if (it != templateElements.end()) {
            templateElements.erase(it);
            RemoveChild(element);
        }
    }

    const std::vector<std::shared_ptr<CHTLBaseNode>>& CHTLTemplateElementNode::GetTemplateElements() const {
        return templateElements;
    }

    size_t CHTLTemplateElementNode::GetTemplateElementCount() const {
        return templateElements.size();
    }

    // 继承处理实现
    void CHTLTemplateElementNode::ApplyInheritance(const CHTLTemplateElementNode& parentTemplate) {
        // 获得父模板的所有元素
        const auto& parentElements = parentTemplate.GetTemplateElements();
        
        // 添加父模板的元素到当前模板
        for (const auto& element : parentElements) {
            auto clonedElement = element->Clone();
            AddTemplateElement(clonedElement);
        }
    }

    // HTML生成实现
    std::string CHTLTemplateElementNode::GenerateHTML() const {
        std::stringstream ss;
        
        // 生成模板中的所有元素
        for (const auto& element : templateElements) {
            if (element->GetNodeType() == CHTLNodeType::ELEMENT) {
                auto elementNode = std::dynamic_pointer_cast<CHTLElementNode>(element);
                if (elementNode) {
                    ss << elementNode->GenerateHTML() << "\n";
                }
            } else if (element->GetNodeType() == CHTLNodeType::TEXT) {
                auto textNode = std::dynamic_pointer_cast<CHTLTextNode>(element);
                if (textNode) {
                    ss << textNode->GetTextContent() << "\n";
                }
            }
        }
        
        return ss.str();
    }

    // 重写虚方法实现
    std::string CHTLTemplateElementNode::ToString() const {
        std::stringstream ss;
        ss << "TemplateElement[" << GetTemplateName() << "]";
        ss << " elements:" << templateElements.size();
        if (!GetInheritedTemplates().empty()) {
            ss << " inherits:" << GetInheritedTemplates().size();
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTemplateElementNode::Clone() const {
        auto cloned = std::make_shared<CHTLTemplateElementNode>(GetTemplateName());
        
        // 克隆模板元素
        for (const auto& element : templateElements) {
            auto clonedElement = element->Clone();
            cloned->AddTemplateElement(clonedElement);
        }
        
        // 复制继承信息
        for (size_t i = 0; i < GetInheritedTemplates().size(); i++) {
            cloned->AddInheritance(GetInheritedTemplates()[i], GetInheritanceTypes()[i]);
        }
        
        return cloned;
    }

    bool CHTLTemplateElementNode::Validate() const {
        // 验证模板元素是否有效
        for (const auto& element : templateElements) {
            if (!element || !element->Validate()) {
                return false;
            }
        }
        
        return CHTLTemplateNode::Validate();
    }

    // CHTLTemplateVarNode实现
    CHTLTemplateVarNode::CHTLTemplateVarNode(const std::string& varGroupName) 
        : CHTLTemplateNode(CHTLTemplateType::TEMPLATE_VAR, varGroupName) {
    }

    // 变量管理实现
    void CHTLTemplateVarNode::AddVariable(const std::string& varName, const std::string& value) {
        // 如果变量已存在，更新值
        if (variables.find(varName) == variables.end()) {
            variableOrder.push_back(varName);
        }
        variables[varName] = value;
    }

    void CHTLTemplateVarNode::RemoveVariable(const std::string& varName) {
        variables.erase(varName);
        
        auto it = std::find(variableOrder.begin(), variableOrder.end(), varName);
        if (it != variableOrder.end()) {
            variableOrder.erase(it);
        }
    }

    std::string CHTLTemplateVarNode::GetVariable(const std::string& varName) const {
        auto it = variables.find(varName);
        return (it != variables.end()) ? it->second : "";
    }

    bool CHTLTemplateVarNode::HasVariable(const std::string& varName) const {
        return variables.find(varName) != variables.end();
    }

    const std::unordered_map<std::string, std::string>& CHTLTemplateVarNode::GetAllVariables() const {
        return variables;
    }

    // 变量顺序管理实现
    const std::vector<std::string>& CHTLTemplateVarNode::GetVariableOrder() const {
        return variableOrder;
    }

    void CHTLTemplateVarNode::SetVariableOrder(const std::vector<std::string>& order) {
        variableOrder = order;
    }

    // 变量引用处理实现
    std::string CHTLTemplateVarNode::ResolveVariableReference(const std::string& varName) const {
        return GetVariable(varName);
    }

    std::string CHTLTemplateVarNode::ProcessVariableReferences(const std::string& text) const {
        std::string result = text;
        
        // 处理 ThemeColor(tableColor) 格式的变量引用
        // 注意：这是值的替换，不是CSS变量
        std::string groupName = GetTemplateName();
        std::string pattern = groupName + "(";
        
        size_t pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            size_t start = pos + pattern.length();
            size_t end = result.find(")", start);
            
            if (end != std::string::npos) {
                std::string varName = result.substr(start, end - start);
                std::string varValue = GetVariable(varName);
                
                if (!varValue.empty()) {
                    // 替换整个引用为变量值
                    result.replace(pos, end - pos + 1, varValue);
                    pos += varValue.length();
                } else {
                    pos = end + 1;
                }
            } else {
                break;
            }
        }
        
        return result;
    }

    // 继承处理实现
    void CHTLTemplateVarNode::ApplyInheritance(const CHTLTemplateVarNode& parentTemplate) {
        // 获得父模板的所有变量
        const auto& parentVariables = parentTemplate.GetAllVariables();
        const auto& parentOrder = parentTemplate.GetVariableOrder();
        
        // 按照父模板的顺序添加变量
        for (const auto& varName : parentOrder) {
            if (parentVariables.find(varName) != parentVariables.end()) {
                // 如果当前模板没有该变量，则添加
                if (variables.find(varName) == variables.end()) {
                    AddVariable(varName, parentVariables.at(varName));
                }
                // 如果已有该变量，保持当前值
            }
        }
    }

    // 重写虚方法实现
    std::string CHTLTemplateVarNode::ToString() const {
        std::stringstream ss;
        ss << "TemplateVar[" << GetTemplateName() << "]";
        ss << " vars:" << variables.size();
        if (!GetInheritedTemplates().empty()) {
            ss << " inherits:" << GetInheritedTemplates().size();
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTemplateVarNode::Clone() const {
        auto cloned = std::make_shared<CHTLTemplateVarNode>(GetTemplateName());
        
        cloned->variables = variables;
        cloned->variableOrder = variableOrder;
        
        // 复制继承信息
        for (size_t i = 0; i < GetInheritedTemplates().size(); i++) {
            cloned->AddInheritance(GetInheritedTemplates()[i], GetInheritanceTypes()[i]);
        }
        
        return cloned;
    }

    bool CHTLTemplateVarNode::Validate() const {
        // 验证变量是否有效
        for (const auto& var : variables) {
            if (var.first.empty()) {
                return false;
            }
        }
        
        return CHTLTemplateNode::Validate();
    }

    // CHTLTemplateUsageNode实现
    CHTLTemplateUsageNode::CHTLTemplateUsageNode(CHTLTemplateType type, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::TEMPLATE, name), 
          templateType(type), templateName(name), isFullQualified(false) {
    }

    // 模板使用管理实现
    CHTLTemplateType CHTLTemplateUsageNode::GetTemplateType() const {
        return templateType;
    }

    const std::string& CHTLTemplateUsageNode::GetTemplateName() const {
        return templateName;
    }

    void CHTLTemplateUsageNode::SetTemplateName(const std::string& name) {
        templateName = name;
        SetNodeName(name);
    }

    // 全缀名管理实现
    bool CHTLTemplateUsageNode::IsFullQualified() const {
        return isFullQualified;
    }

    void CHTLTemplateUsageNode::SetFullQualified(bool qualified, const std::string& prefix) {
        isFullQualified = qualified;
        fullQualifiedPrefix = prefix;
    }

    const std::string& CHTLTemplateUsageNode::GetFullQualifiedPrefix() const {
        return fullQualifiedPrefix;
    }

    // 重写虚方法实现
    std::string CHTLTemplateUsageNode::ToString() const {
        std::stringstream ss;
        ss << "TemplateUsage[" << CHTLTemplateNode::TemplateTypeToString(templateType) << "]";
        ss << " \"" << templateName << "\"";
        if (isFullQualified) {
            ss << " (" << fullQualifiedPrefix << ")";
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTemplateUsageNode::Clone() const {
        auto cloned = std::make_shared<CHTLTemplateUsageNode>(templateType, templateName);
        cloned->isFullQualified = isFullQualified;
        cloned->fullQualifiedPrefix = fullQualifiedPrefix;
        return cloned;
    }

    bool CHTLTemplateUsageNode::Validate() const {
        return !templateName.empty() && CHTLBaseNode::Validate();
    }

    // CHTLTemplateNodeFactory实现
    std::shared_ptr<CHTLTemplateStyleNode> CHTLTemplateNodeFactory::CreateTemplateStyleNode(const std::string& styleName) {
        return std::make_shared<CHTLTemplateStyleNode>(styleName);
    }

    std::shared_ptr<CHTLTemplateElementNode> CHTLTemplateNodeFactory::CreateTemplateElementNode(const std::string& elementName) {
        return std::make_shared<CHTLTemplateElementNode>(elementName);
    }

    std::shared_ptr<CHTLTemplateVarNode> CHTLTemplateNodeFactory::CreateTemplateVarNode(const std::string& varGroupName) {
        return std::make_shared<CHTLTemplateVarNode>(varGroupName);
    }

    std::shared_ptr<CHTLTemplateUsageNode> CHTLTemplateNodeFactory::CreateStyleUsageNode(const std::string& styleName) {
        return std::make_shared<CHTLTemplateUsageNode>(CHTLTemplateType::TEMPLATE_STYLE, styleName);
    }

    std::shared_ptr<CHTLTemplateUsageNode> CHTLTemplateNodeFactory::CreateElementUsageNode(const std::string& elementName) {
        return std::make_shared<CHTLTemplateUsageNode>(CHTLTemplateType::TEMPLATE_ELEMENT, elementName);
    }

    std::shared_ptr<CHTLTemplateUsageNode> CHTLTemplateNodeFactory::CreateVarUsageNode(const std::string& varGroupName) {
        return std::make_shared<CHTLTemplateUsageNode>(CHTLTemplateType::TEMPLATE_VAR, varGroupName);
    }

    // 解析方法实现
    CHTLTemplateType CHTLTemplateNodeFactory::ParseTemplateType(const std::string& typeText) {
        if (typeText == "@Style") return CHTLTemplateType::TEMPLATE_STYLE;
        if (typeText == "@Element") return CHTLTemplateType::TEMPLATE_ELEMENT;
        if (typeText == "@Var") return CHTLTemplateType::TEMPLATE_VAR;
        return CHTLTemplateType::TEMPLATE_STYLE; // 默认
    }

    bool CHTLTemplateNodeFactory::IsValidTemplateName(const std::string& name) {
        if (name.empty()) return false;
        
        // 模板名验证：字母开头，字母数字下划线组成
        if (!std::isalpha(name[0])) return false;
        
        for (char c : name) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        
        return true;
    }

    bool CHTLTemplateNodeFactory::IsValidTemplateType(const std::string& type) {
        return type == "@Style" || type == "@Element" || type == "@Var";
    }

} // namespace CHTL