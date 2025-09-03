#include "CustomNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

    // CHTLCustomNode实现
    CHTLCustomNode::CHTLCustomNode(CHTLCustomType type, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::CUSTOM, name), 
          customType(type), customName(name), allowsSpecialization(true), 
          isUsage(false), isSpecialized(false) {
    }

    // 自定义类型管理实现
    CHTLCustomType CHTLCustomNode::GetCustomType() const {
        return customType;
    }

    void CHTLCustomNode::SetCustomType(CHTLCustomType type) {
        customType = type;
    }

    // 自定义名称管理实现
    const std::string& CHTLCustomNode::GetCustomName() const {
        return customName;
    }

    void CHTLCustomNode::SetCustomName(const std::string& name) {
        customName = name;
        SetNodeName(name);
    }

    // 特例化管理实现
    bool CHTLCustomNode::AllowsSpecialization() const {
        return allowsSpecialization;
    }

    void CHTLCustomNode::SetAllowsSpecialization(bool allows) {
        allowsSpecialization = allows;
    }

    bool CHTLCustomNode::IsSpecialized() const {
        return isSpecialized;
    }

    void CHTLCustomNode::SetSpecialized(bool specialized) {
        isSpecialized = specialized;
    }

    // 使用/定义标记实现
    bool CHTLCustomNode::IsUsage() const {
        return isUsage;
    }

    void CHTLCustomNode::SetIsUsage(bool usage) {
        isUsage = usage;
    }

    // 继承管理实现
    void CHTLCustomNode::AddInheritance(const std::string& itemName, CHTLInheritanceType type) {
        if (std::find(inheritedItems.begin(), inheritedItems.end(), itemName) == inheritedItems.end()) {
            inheritedItems.push_back(itemName);
            inheritanceTypes.push_back(type);
        }
    }

    void CHTLCustomNode::RemoveInheritance(const std::string& itemName) {
        auto it = std::find(inheritedItems.begin(), inheritedItems.end(), itemName);
        if (it != inheritedItems.end()) {
            size_t index = std::distance(inheritedItems.begin(), it);
            inheritedItems.erase(it);
            inheritanceTypes.erase(inheritanceTypes.begin() + index);
        }
    }

    const std::vector<std::string>& CHTLCustomNode::GetInheritedItems() const {
        return inheritedItems;
    }

    bool CHTLCustomNode::HasInheritance(const std::string& itemName) const {
        return std::find(inheritedItems.begin(), inheritedItems.end(), itemName) != inheritedItems.end();
    }

    // 重写虚方法实现
    std::string CHTLCustomNode::ToString() const {
        std::stringstream ss;
        ss << "Custom[" << CustomTypeToString(customType) << "]";
        ss << " \"" << customName << "\"";
        if (isUsage) ss << " (usage)";
        if (isSpecialized) ss << " (specialized)";
        if (!inheritedItems.empty()) ss << " inherits:" << inheritedItems.size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCustomNode::Clone() const {
        auto cloned = std::make_shared<CHTLCustomNode>(customType, customName);
        
        cloned->allowsSpecialization = allowsSpecialization;
        cloned->isUsage = isUsage;
        cloned->isSpecialized = isSpecialized;
        cloned->inheritedItems = inheritedItems;
        cloned->inheritanceTypes = inheritanceTypes;
        
        // 递归克隆子节点
        for (const auto& child : GetChildren()) {
            cloned->AddChild(child->Clone());
        }
        
        return cloned;
    }

    bool CHTLCustomNode::Validate() const {
        return !customName.empty() && CHTLBaseNode::Validate();
    }

    std::string CHTLCustomNode::CustomTypeToString(CHTLCustomType type) {
        switch (type) {
            case CHTLCustomType::CUSTOM_STYLE: return "CUSTOM_STYLE";
            case CHTLCustomType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
            case CHTLCustomType::CUSTOM_VAR: return "CUSTOM_VAR";
            default: return "UNKNOWN";
        }
    }

    // CHTLCustomStyleNode实现
    CHTLCustomStyleNode::CHTLCustomStyleNode(const std::string& styleName) 
        : CHTLCustomNode(CHTLCustomType::CUSTOM_STYLE, styleName) {
    }

    // 样式属性管理实现
    void CHTLCustomStyleNode::AddStyleProperty(const std::string& property, const std::string& value) {
        if (styleProperties.find(property) == styleProperties.end()) {
            propertyOrder.push_back(property);
        }
        styleProperties[property] = value;
        
        // 从无值属性集合中移除
        noValueProperties.erase(property);
    }

    void CHTLCustomStyleNode::AddNoValueProperty(const std::string& property) {
        noValueProperties.insert(property);
        if (styleProperties.find(property) == styleProperties.end()) {
            propertyOrder.push_back(property);
        }
        // 不设置值，等待特例化时填充
    }

    void CHTLCustomStyleNode::RemoveStyleProperty(const std::string& property) {
        styleProperties.erase(property);
        noValueProperties.erase(property);
        
        auto it = std::find(propertyOrder.begin(), propertyOrder.end(), property);
        if (it != propertyOrder.end()) {
            propertyOrder.erase(it);
        }
    }

    std::string CHTLCustomStyleNode::GetStyleProperty(const std::string& property) const {
        auto it = styleProperties.find(property);
        return (it != styleProperties.end()) ? it->second : "";
    }

    bool CHTLCustomStyleNode::HasStyleProperty(const std::string& property) const {
        return styleProperties.find(property) != styleProperties.end() || 
               noValueProperties.find(property) != noValueProperties.end();
    }

    bool CHTLCustomStyleNode::IsNoValueProperty(const std::string& property) const {
        return noValueProperties.find(property) != noValueProperties.end();
    }

    const std::unordered_set<std::string>& CHTLCustomStyleNode::GetNoValueProperties() const {
        return noValueProperties;
    }

    // 特例化操作实现
    void CHTLCustomStyleNode::FillNoValueProperty(const std::string& property, const std::string& value) {
        if (IsNoValueProperty(property)) {
            styleProperties[property] = value;
            noValueProperties.erase(property);
            SetSpecialized(true);
        }
    }

    bool CHTLCustomStyleNode::CanFillProperty(const std::string& property) const {
        return IsNoValueProperty(property);
    }

    // 删除操作实现
    void CHTLCustomStyleNode::DeleteProperty(const std::string& property) {
        deletedProperties.insert(property);
        styleProperties.erase(property);
        noValueProperties.erase(property);
        
        auto it = std::find(propertyOrder.begin(), propertyOrder.end(), property);
        if (it != propertyOrder.end()) {
            propertyOrder.erase(it);
        }
    }

    void CHTLCustomStyleNode::DeleteInheritance(const std::string& inheritanceName) {
        deletedInheritances.insert(inheritanceName);
        RemoveInheritance(inheritanceName);
    }

    bool CHTLCustomStyleNode::IsPropertyDeleted(const std::string& property) const {
        return deletedProperties.find(property) != deletedProperties.end();
    }

    bool CHTLCustomStyleNode::IsInheritanceDeleted(const std::string& inheritanceName) const {
        return deletedInheritances.find(inheritanceName) != deletedInheritances.end();
    }

    const std::unordered_set<std::string>& CHTLCustomStyleNode::GetDeletedProperties() const {
        return deletedProperties;
    }

    const std::unordered_set<std::string>& CHTLCustomStyleNode::GetDeletedInheritances() const {
        return deletedInheritances;
    }

    // 继承处理实现
    void CHTLCustomStyleNode::ApplyTemplateInheritance(const CHTLTemplateStyleNode& templateStyle) {
        // 从模板继承属性
        const auto& templateProperties = templateStyle.GetAllStyleProperties();
        const auto& templateOrder = templateStyle.GetPropertyOrder();
        
        for (const auto& propName : templateOrder) {
            if (templateProperties.find(propName) != templateProperties.end()) {
                // 如果当前自定义没有该属性且未被删除，则添加
                if (!HasStyleProperty(propName) && !IsPropertyDeleted(propName)) {
                    AddStyleProperty(propName, templateProperties.at(propName));
                }
            }
        }
    }

    void CHTLCustomStyleNode::ApplyCustomInheritance(const CHTLCustomStyleNode& customStyle) {
        // 从其他自定义继承属性
        const auto& customProperties = customStyle.styleProperties;
        const auto& customOrder = customStyle.propertyOrder;
        const auto& customNoValues = customStyle.GetNoValueProperties();
        
        for (const auto& propName : customOrder) {
            if (!IsPropertyDeleted(propName)) {
                if (customProperties.find(propName) != customProperties.end()) {
                    if (!HasStyleProperty(propName)) {
                        AddStyleProperty(propName, customProperties.at(propName));
                    }
                } else if (customNoValues.find(propName) != customNoValues.end()) {
                    if (!HasStyleProperty(propName)) {
                        AddNoValueProperty(propName);
                    }
                }
            }
        }
    }

    // CSS生成实现
    std::string CHTLCustomStyleNode::GenerateCSS() const {
        std::stringstream ss;
        
        // 按属性顺序生成CSS，跳过无值属性和删除的属性
        for (const auto& propName : propertyOrder) {
            if (!IsPropertyDeleted(propName) && !IsNoValueProperty(propName)) {
                auto it = styleProperties.find(propName);
                if (it != styleProperties.end()) {
                    ss << propName << ": " << it->second << ";\n";
                }
            }
        }
        
        return ss.str();
    }

    std::string CHTLCustomStyleNode::GenerateInlineStyle() const {
        std::stringstream ss;
        
        for (const auto& propName : propertyOrder) {
            if (!IsPropertyDeleted(propName) && !IsNoValueProperty(propName)) {
                auto it = styleProperties.find(propName);
                if (it != styleProperties.end()) {
                    ss << propName << ": " << it->second << "; ";
                }
            }
        }
        
        return ss.str();
    }

    // 重写虚方法实现
    std::string CHTLCustomStyleNode::ToString() const {
        std::stringstream ss;
        ss << "CustomStyle[" << GetCustomName() << "]";
        ss << " props:" << styleProperties.size();
        ss << " no-value:" << noValueProperties.size();
        if (!deletedProperties.empty()) ss << " deleted:" << deletedProperties.size();
        if (!GetInheritedItems().empty()) ss << " inherits:" << GetInheritedItems().size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCustomStyleNode::Clone() const {
        auto cloned = std::make_shared<CHTLCustomStyleNode>(GetCustomName());
        
        cloned->styleProperties = styleProperties;
        cloned->noValueProperties = noValueProperties;
        cloned->propertyOrder = propertyOrder;
        cloned->deletedProperties = deletedProperties;
        cloned->deletedInheritances = deletedInheritances;
        
        // 复制继承信息
        cloned->inheritedItems = inheritedItems;
        cloned->inheritanceTypes = inheritanceTypes;
        
        return cloned;
    }

    bool CHTLCustomStyleNode::Validate() const {
        // 验证无值属性不能同时在styleProperties中
        for (const auto& noValueProp : noValueProperties) {
            if (styleProperties.find(noValueProp) != styleProperties.end()) {
                return false; // 无值属性不应该有值
            }
        }
        
        return CHTLCustomNode::Validate();
    }

    // CHTLCustomElementNode实现
    CHTLCustomElementNode::CHTLCustomElementNode(const std::string& elementName) 
        : CHTLCustomNode(CHTLCustomType::CUSTOM_ELEMENT, elementName) {
    }

    // 自定义元素管理实现
    void CHTLCustomElementNode::AddCustomElement(std::shared_ptr<CHTLBaseNode> element) {
        if (element) {
            customElements.push_back(element);
            AddChild(element);
        }
    }

    void CHTLCustomElementNode::RemoveCustomElement(std::shared_ptr<CHTLBaseNode> element) {
        auto it = std::find(customElements.begin(), customElements.end(), element);
        if (it != customElements.end()) {
            customElements.erase(it);
            RemoveChild(element);
        }
    }

    const std::vector<std::shared_ptr<CHTLBaseNode>>& CHTLCustomElementNode::GetCustomElements() const {
        return customElements;
    }

    size_t CHTLCustomElementNode::GetCustomElementCount() const {
        return customElements.size();
    }

    // 索引访问实现
    std::shared_ptr<CHTLBaseNode> CHTLCustomElementNode::GetElementByIndex(size_t index) const {
        return (index < customElements.size()) ? customElements[index] : nullptr;
    }

    std::vector<std::shared_ptr<CHTLBaseNode>> CHTLCustomElementNode::GetElementsByType(const std::string& elementType) const {
        std::vector<std::shared_ptr<CHTLBaseNode>> result;
        
        for (const auto& element : customElements) {
            if (element->GetNodeType() == CHTLNodeType::ELEMENT) {
                auto elementNode = std::dynamic_pointer_cast<CHTLElementNode>(element);
                if (elementNode && elementNode->GetTagName() == elementType) {
                    result.push_back(element);
                }
            }
        }
        
        return result;
    }

    // 删除操作实现
    void CHTLCustomElementNode::DeleteElement(const std::string& elementType) {
        deletedElementTypes.insert(elementType);
        
        // 从customElements中移除对应类型的元素
        auto it = customElements.begin();
        while (it != customElements.end()) {
            if ((*it)->GetNodeType() == CHTLNodeType::ELEMENT) {
                auto elementNode = std::dynamic_pointer_cast<CHTLElementNode>(*it);
                if (elementNode && elementNode->GetTagName() == elementType) {
                    RemoveChild(*it);
                    it = customElements.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    void CHTLCustomElementNode::DeleteElementByIndex(size_t index) {
        if (index < customElements.size()) {
            deletedElementIndices.insert(index);
            auto element = customElements[index];
            RemoveChild(element);
            customElements.erase(customElements.begin() + index);
        }
    }

    void CHTLCustomElementNode::DeleteInheritance(const std::string& inheritanceName) {
        deletedInheritances.insert(inheritanceName);
        RemoveInheritance(inheritanceName);
    }

    bool CHTLCustomElementNode::IsElementDeleted(size_t index) const {
        return deletedElementIndices.find(index) != deletedElementIndices.end();
    }

    bool CHTLCustomElementNode::IsElementTypeDeleted(const std::string& elementType) const {
        return deletedElementTypes.find(elementType) != deletedElementTypes.end();
    }

    bool CHTLCustomElementNode::IsInheritanceDeleted(const std::string& inheritanceName) const {
        return deletedInheritances.find(inheritanceName) != deletedInheritances.end();
    }

    // 插入操作实现
    void CHTLCustomElementNode::InsertElement(CHTLInsertPosition position, const std::string& /* targetSelector */, 
                                             std::shared_ptr<CHTLBaseNode> element) {
        // 简化实现：根据位置插入元素
        if (element) {
            switch (position) {
                case CHTLInsertPosition::AT_TOP:
                    customElements.insert(customElements.begin(), element);
                    break;
                case CHTLInsertPosition::AT_BOTTOM:
                    customElements.push_back(element);
                    break;
                default:
                    // AFTER, BEFORE, REPLACE 需要更复杂的逻辑
                    customElements.push_back(element);
                    break;
            }
            AddChild(element);
        }
    }

    void CHTLCustomElementNode::InsertElementAtIndex(CHTLInsertPosition position, size_t targetIndex,
                                                    std::shared_ptr<CHTLBaseNode> element) {
        if (element && targetIndex <= customElements.size()) {
            switch (position) {
                case CHTLInsertPosition::BEFORE:
                    customElements.insert(customElements.begin() + targetIndex, element);
                    break;
                case CHTLInsertPosition::AFTER:
                    customElements.insert(customElements.begin() + targetIndex + 1, element);
                    break;
                case CHTLInsertPosition::REPLACE:
                    if (targetIndex < customElements.size()) {
                        RemoveChild(customElements[targetIndex]);
                        customElements[targetIndex] = element;
                    }
                    break;
                default:
                    customElements.insert(customElements.begin() + targetIndex, element);
                    break;
            }
            AddChild(element);
        }
    }

    // 特例化操作：增加样式实现
    void CHTLCustomElementNode::AddStyleToElement(size_t index, std::shared_ptr<CHTLBaseNode> styleNode) {
        if (index < customElements.size() && styleNode) {
            auto element = customElements[index];
            if (element->GetNodeType() == CHTLNodeType::ELEMENT) {
                element->AddChild(styleNode);
                SetSpecialized(true);
            }
        }
    }

    void CHTLCustomElementNode::AddStyleToElementType(const std::string& elementType, std::shared_ptr<CHTLBaseNode> styleNode) {
        for (auto& element : customElements) {
            if (element->GetNodeType() == CHTLNodeType::ELEMENT) {
                auto elementNode = std::dynamic_pointer_cast<CHTLElementNode>(element);
                if (elementNode && elementNode->GetTagName() == elementType && styleNode) {
                    element->AddChild(styleNode->Clone());
                    SetSpecialized(true);
                }
            }
        }
    }

    // HTML生成实现
    std::string CHTLCustomElementNode::GenerateHTML() const {
        std::stringstream ss;
        
        for (const auto& element : customElements) {
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
    std::string CHTLCustomElementNode::ToString() const {
        std::stringstream ss;
        ss << "CustomElement[" << GetCustomName() << "]";
        ss << " elements:" << customElements.size();
        if (!deletedElementTypes.empty()) ss << " deleted-types:" << deletedElementTypes.size();
        if (!deletedElementIndices.empty()) ss << " deleted-indices:" << deletedElementIndices.size();
        if (!GetInheritedItems().empty()) ss << " inherits:" << GetInheritedItems().size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCustomElementNode::Clone() const {
        auto cloned = std::make_shared<CHTLCustomElementNode>(GetCustomName());
        
        // 克隆自定义元素
        for (const auto& element : customElements) {
            auto clonedElement = element->Clone();
            cloned->AddCustomElement(clonedElement);
        }
        
        cloned->deletedElementIndices = deletedElementIndices;
        cloned->deletedElementTypes = deletedElementTypes;
        cloned->deletedInheritances = deletedInheritances;
        
        return cloned;
    }

    bool CHTLCustomElementNode::Validate() const {
        // 验证自定义元素
        for (const auto& element : customElements) {
            if (!element || !element->Validate()) {
                return false;
            }
        }
        
        return CHTLCustomNode::Validate();
    }

    // CHTLCustomVarNode实现
    CHTLCustomVarNode::CHTLCustomVarNode(const std::string& varGroupName) 
        : CHTLCustomNode(CHTLCustomType::CUSTOM_VAR, varGroupName) {
    }

    // 变量管理实现
    void CHTLCustomVarNode::AddVariable(const std::string& varName, const std::string& value) {
        if (variables.find(varName) == variables.end()) {
            variableOrder.push_back(varName);
        }
        variables[varName] = value;
    }

    void CHTLCustomVarNode::RemoveVariable(const std::string& varName) {
        variables.erase(varName);
        
        auto it = std::find(variableOrder.begin(), variableOrder.end(), varName);
        if (it != variableOrder.end()) {
            variableOrder.erase(it);
        }
    }

    std::string CHTLCustomVarNode::GetVariable(const std::string& varName) const {
        auto it = variables.find(varName);
        return (it != variables.end()) ? it->second : "";
    }

    bool CHTLCustomVarNode::HasVariable(const std::string& varName) const {
        return variables.find(varName) != variables.end();
    }

    const std::unordered_map<std::string, std::string>& CHTLCustomVarNode::GetAllVariables() const {
        return variables;
    }

    // 变量组特例化实现
    std::string CHTLCustomVarNode::ProcessSpecializedReference(const std::string& /* varName */, const std::string& specializedValue) const {
        // 处理特例化引用：ThemeColor(tableColor = rgb(145, 155, 200))
        return specializedValue; // 直接返回特例化值
    }

    // 变量引用处理实现
    std::string CHTLCustomVarNode::ResolveVariableReference(const std::string& varName) const {
        return GetVariable(varName);
    }

    std::string CHTLCustomVarNode::ProcessVariableReferences(const std::string& text) const {
        std::string result = text;
        
        // 处理 ThemeColor(varName) 和 ThemeColor(varName = value) 格式
        std::string groupName = GetCustomName();
        std::string pattern = groupName + "(";
        
        size_t pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            size_t start = pos + pattern.length();
            size_t end = result.find(")", start);
            
            if (end != std::string::npos) {
                std::string content = result.substr(start, end - start);
                
                // 检查是否为特例化引用（包含=）
                size_t equalPos = content.find("=");
                if (equalPos != std::string::npos) {
                    // 特例化引用：varName = value
                    std::string varName = content.substr(0, equalPos);
                    std::string specializedValue = content.substr(equalPos + 1);
                    
                    // 去除空格
                    varName.erase(0, varName.find_first_not_of(" \t"));
                    varName.erase(varName.find_last_not_of(" \t") + 1);
                    specializedValue.erase(0, specializedValue.find_first_not_of(" \t"));
                    specializedValue.erase(specializedValue.find_last_not_of(" \t") + 1);
                    
                    result.replace(pos, end - pos + 1, ProcessSpecializedReference(varName, specializedValue));
                } else {
                    // 普通引用
                    std::string varValue = GetVariable(content);
                    if (!varValue.empty()) {
                        result.replace(pos, end - pos + 1, varValue);
                    }
                }
                
                pos = end + 1;
            } else {
                break;
            }
        }
        
        return result;
    }

    // 重写虚方法实现
    std::string CHTLCustomVarNode::ToString() const {
        std::stringstream ss;
        ss << "CustomVar[" << GetCustomName() << "]";
        ss << " vars:" << variables.size();
        if (!GetInheritedItems().empty()) ss << " inherits:" << GetInheritedItems().size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCustomVarNode::Clone() const {
        auto cloned = std::make_shared<CHTLCustomVarNode>(GetCustomName());
        
        cloned->variables = variables;
        cloned->variableOrder = variableOrder;
        
        return cloned;
    }

    bool CHTLCustomVarNode::Validate() const {
        return CHTLCustomNode::Validate();
    }

    // CHTLCustomUsageNode实现
    CHTLCustomUsageNode::CHTLCustomUsageNode(CHTLCustomType type, const std::string& name) 
        : CHTLBaseNode(CHTLNodeType::CUSTOM, name), 
          customType(type), customName(name), isFullQualified(false), hasSpecialization(false) {
    }

    // 自定义使用管理实现
    CHTLCustomType CHTLCustomUsageNode::GetCustomType() const {
        return customType;
    }

    const std::string& CHTLCustomUsageNode::GetCustomName() const {
        return customName;
    }

    void CHTLCustomUsageNode::SetCustomName(const std::string& name) {
        customName = name;
        SetNodeName(name);
    }

    // 全缀名管理实现
    bool CHTLCustomUsageNode::IsFullQualified() const {
        return isFullQualified;
    }

    void CHTLCustomUsageNode::SetFullQualified(bool qualified, const std::string& prefix) {
        isFullQualified = qualified;
        fullQualifiedPrefix = prefix;
    }

    const std::string& CHTLCustomUsageNode::GetFullQualifiedPrefix() const {
        return fullQualifiedPrefix;
    }

    // 特例化管理实现
    bool CHTLCustomUsageNode::HasSpecialization() const {
        return hasSpecialization;
    }

    void CHTLCustomUsageNode::AddSpecialization(const std::string& key, const std::string& value) {
        specializationValues[key] = value;
        hasSpecialization = true;
    }

    void CHTLCustomUsageNode::RemoveSpecialization(const std::string& key) {
        specializationValues.erase(key);
        hasSpecialization = !specializationValues.empty();
    }

    std::string CHTLCustomUsageNode::GetSpecialization(const std::string& key) const {
        auto it = specializationValues.find(key);
        return (it != specializationValues.end()) ? it->second : "";
    }

    const std::unordered_map<std::string, std::string>& CHTLCustomUsageNode::GetAllSpecializations() const {
        return specializationValues;
    }

    // 删除操作实现
    void CHTLCustomUsageNode::AddDeletion(const std::string& itemName) {
        deletedItems.insert(itemName);
    }

    void CHTLCustomUsageNode::RemoveDeletion(const std::string& itemName) {
        deletedItems.erase(itemName);
    }

    bool CHTLCustomUsageNode::IsDeleted(const std::string& itemName) const {
        return deletedItems.find(itemName) != deletedItems.end();
    }

    const std::unordered_set<std::string>& CHTLCustomUsageNode::GetDeletedItems() const {
        return deletedItems;
    }

    // 重写虚方法实现
    std::string CHTLCustomUsageNode::ToString() const {
        std::stringstream ss;
        ss << "CustomUsage[" << CHTLCustomNode::CustomTypeToString(customType) << "]";
        ss << " \"" << customName << "\"";
        if (isFullQualified) ss << " (" << fullQualifiedPrefix << ")";
        if (hasSpecialization) ss << " specialized:" << specializationValues.size();
        if (!deletedItems.empty()) ss << " deleted:" << deletedItems.size();
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCustomUsageNode::Clone() const {
        auto cloned = std::make_shared<CHTLCustomUsageNode>(customType, customName);
        
        cloned->isFullQualified = isFullQualified;
        cloned->fullQualifiedPrefix = fullQualifiedPrefix;
        cloned->hasSpecialization = hasSpecialization;
        cloned->specializationValues = specializationValues;
        cloned->deletedItems = deletedItems;
        
        return cloned;
    }

    bool CHTLCustomUsageNode::Validate() const {
        return !customName.empty() && CHTLBaseNode::Validate();
    }

    // CHTLOperatorNode实现
    CHTLOperatorNode::CHTLOperatorNode(CHTLSpecializationType type, const std::string& target) 
        : CHTLBaseNode(CHTLNodeType::DELETE, "operator"), // 使用DELETE作为基础类型
          operationType(type), targetName(target), targetIndex(0), 
          insertPosition(CHTLInsertPosition::AFTER) {
    }

    // 操作类型管理实现
    CHTLSpecializationType CHTLOperatorNode::GetOperationType() const {
        return operationType;
    }

    void CHTLOperatorNode::SetOperationType(CHTLSpecializationType type) {
        operationType = type;
    }

    // 目标管理实现
    const std::string& CHTLOperatorNode::GetTargetName() const {
        return targetName;
    }

    const std::string& CHTLOperatorNode::GetTargetSelector() const {
        return targetSelector;
    }

    size_t CHTLOperatorNode::GetTargetIndex() const {
        return targetIndex;
    }

    void CHTLOperatorNode::SetTargetName(const std::string& target) {
        targetName = target;
    }

    void CHTLOperatorNode::SetTargetSelector(const std::string& selector) {
        targetSelector = selector;
    }

    void CHTLOperatorNode::SetTargetIndex(size_t index) {
        targetIndex = index;
    }

    // 插入位置管理实现
    CHTLInsertPosition CHTLOperatorNode::GetInsertPosition() const {
        return insertPosition;
    }

    void CHTLOperatorNode::SetInsertPosition(CHTLInsertPosition position) {
        insertPosition = position;
    }

    // 操作执行实现
    bool CHTLOperatorNode::CanExecute() const {
        return !targetName.empty() || !targetSelector.empty();
    }

    // 重写虚方法实现
    std::string CHTLOperatorNode::ToString() const {
        std::stringstream ss;
        ss << "Operator[" << OperationTypeToString(operationType) << "]";
        ss << " target:\"" << targetName << "\"";
        if (operationType == CHTLSpecializationType::ELEMENT_INSERT) {
            ss << " pos:" << InsertPositionToString(insertPosition);
        }
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLOperatorNode::Clone() const {
        auto cloned = std::make_shared<CHTLOperatorNode>(operationType, targetName);
        cloned->targetSelector = targetSelector;
        cloned->targetIndex = targetIndex;
        cloned->insertPosition = insertPosition;
        return cloned;
    }

    bool CHTLOperatorNode::Validate() const {
        return CanExecute() && CHTLBaseNode::Validate();
    }

    // 静态工具方法实现
    std::string CHTLOperatorNode::OperationTypeToString(CHTLSpecializationType type) {
        switch (type) {
            case CHTLSpecializationType::VALUE_FILL: return "VALUE_FILL";
            case CHTLSpecializationType::PROPERTY_DELETE: return "PROPERTY_DELETE";
            case CHTLSpecializationType::INHERITANCE_DELETE: return "INHERITANCE_DELETE";
            case CHTLSpecializationType::ELEMENT_DELETE: return "ELEMENT_DELETE";
            case CHTLSpecializationType::ELEMENT_INSERT: return "ELEMENT_INSERT";
            case CHTLSpecializationType::STYLE_ADD: return "STYLE_ADD";
            default: return "UNKNOWN";
        }
    }

    std::string CHTLOperatorNode::InsertPositionToString(CHTLInsertPosition position) {
        switch (position) {
            case CHTLInsertPosition::AFTER: return "AFTER";
            case CHTLInsertPosition::BEFORE: return "BEFORE";
            case CHTLInsertPosition::REPLACE: return "REPLACE";
            case CHTLInsertPosition::AT_TOP: return "AT_TOP";
            case CHTLInsertPosition::AT_BOTTOM: return "AT_BOTTOM";
            default: return "UNKNOWN";
        }
    }

    // CHTLCustomNodeFactory实现
    std::shared_ptr<CHTLCustomStyleNode> CHTLCustomNodeFactory::CreateCustomStyleNode(const std::string& styleName) {
        return std::make_shared<CHTLCustomStyleNode>(styleName);
    }

    std::shared_ptr<CHTLCustomElementNode> CHTLCustomNodeFactory::CreateCustomElementNode(const std::string& elementName) {
        return std::make_shared<CHTLCustomElementNode>(elementName);
    }

    std::shared_ptr<CHTLCustomVarNode> CHTLCustomNodeFactory::CreateCustomVarNode(const std::string& varGroupName) {
        return std::make_shared<CHTLCustomVarNode>(varGroupName);
    }

    std::shared_ptr<CHTLCustomUsageNode> CHTLCustomNodeFactory::CreateStyleUsageNode(const std::string& styleName) {
        return std::make_shared<CHTLCustomUsageNode>(CHTLCustomType::CUSTOM_STYLE, styleName);
    }

    std::shared_ptr<CHTLCustomUsageNode> CHTLCustomNodeFactory::CreateElementUsageNode(const std::string& elementName) {
        return std::make_shared<CHTLCustomUsageNode>(CHTLCustomType::CUSTOM_ELEMENT, elementName);
    }

    std::shared_ptr<CHTLCustomUsageNode> CHTLCustomNodeFactory::CreateVarUsageNode(const std::string& varGroupName) {
        return std::make_shared<CHTLCustomUsageNode>(CHTLCustomType::CUSTOM_VAR, varGroupName);
    }

    std::shared_ptr<CHTLOperatorNode> CHTLCustomNodeFactory::CreateDeleteNode(const std::string& target) {
        return std::make_shared<CHTLOperatorNode>(CHTLSpecializationType::PROPERTY_DELETE, target);
    }

    std::shared_ptr<CHTLOperatorNode> CHTLCustomNodeFactory::CreateInsertNode(CHTLInsertPosition position, const std::string& target) {
        auto node = std::make_shared<CHTLOperatorNode>(CHTLSpecializationType::ELEMENT_INSERT, target);
        node->SetInsertPosition(position);
        return node;
    }

    // 解析方法实现
    CHTLCustomType CHTLCustomNodeFactory::ParseCustomType(const std::string& typeText) {
        if (typeText == "@Style") return CHTLCustomType::CUSTOM_STYLE;
        if (typeText == "@Element") return CHTLCustomType::CUSTOM_ELEMENT;
        if (typeText == "@Var") return CHTLCustomType::CUSTOM_VAR;
        return CHTLCustomType::CUSTOM_STYLE; // 默认
    }

    CHTLInsertPosition CHTLCustomNodeFactory::ParseInsertPosition(const std::string& positionText) {
        if (positionText == "after") return CHTLInsertPosition::AFTER;
        if (positionText == "before") return CHTLInsertPosition::BEFORE;
        if (positionText == "replace") return CHTLInsertPosition::REPLACE;
        if (positionText == "at top") return CHTLInsertPosition::AT_TOP;
        if (positionText == "at bottom") return CHTLInsertPosition::AT_BOTTOM;
        return CHTLInsertPosition::AFTER; // 默认
    }

    bool CHTLCustomNodeFactory::IsValidCustomName(const std::string& name) {
        if (name.empty()) return false;
        
        // 自定义名验证：字母开头，字母数字下划线组成
        if (!std::isalpha(name[0])) return false;
        
        for (char c : name) {
            if (!std::isalnum(c) && c != '_') {
                return false;
            }
        }
        
        return true;
    }

    bool CHTLCustomNodeFactory::IsValidCustomType(const std::string& type) {
        return type == "@Style" || type == "@Element" || type == "@Var";
    }

} // namespace CHTL