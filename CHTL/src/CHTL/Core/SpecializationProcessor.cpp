#include "CHTL/Core/SpecializationProcessor.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

void SpecializationProcessor::ProcessSpecializations(
    std::shared_ptr<ElementNode> element,
    const std::vector<std::shared_ptr<ASTNode>>& specializations) {
    
    if (!element || specializations.empty()) {
        return;
    }
    
    LOG_DEBUG("处理元素特例化，操作数: " + std::to_string(specializations.size()));
    
    for (const auto& spec : specializations) {
        if (!spec) continue;
        
        switch (spec->GetType()) {
            case ASTNodeType::Delete: {
                auto deleteNode = std::static_pointer_cast<DeleteNode>(spec);
                ApplyDelete(element, deleteNode.get());
                break;
            }
            
            case ASTNodeType::Insert: {
                auto insertNode = std::static_pointer_cast<InsertNode>(spec);
                ApplyInsert(element, insertNode.get());
                break;
            }
            
            case ASTNodeType::ElementMatch: {
                auto matchNode = std::static_pointer_cast<ElementMatchNode>(spec);
                ApplyElementMatch(element, matchNode.get());
                break;
            }
            
            default:
                LOG_INFO("未知的特例化操作类型: " + std::to_string(static_cast<int>(spec->GetType())));
                break;
        }
    }
}

void SpecializationProcessor::ProcessStyleSpecializations(
    std::shared_ptr<TemplateStyleNode> style,
    const std::vector<std::shared_ptr<ASTNode>>& specializations) {
    
    if (!style || specializations.empty()) {
        return;
    }
    
    LOG_DEBUG("处理样式特例化，操作数: " + std::to_string(specializations.size()));
    
    for (const auto& spec : specializations) {
        if (!spec) continue;
        
        if (spec->GetType() == ASTNodeType::Delete) {
            auto deleteNode = std::static_pointer_cast<DeleteNode>(spec);
            
            // 只处理样式属性删除
            if (deleteNode->GetTargetType() == DeleteNode::DeleteTarget::StyleProperty) {
                const auto& targets = deleteNode->GetTargets();
                auto& children = style->GetChildren();
                
                // 删除指定的样式属性
                for (const auto& propName : targets) {
                    children.erase(
                        std::remove_if(children.begin(), children.end(),
                            [&propName](const std::shared_ptr<ASTNode>& child) {
                                if (child->GetType() == ASTNodeType::StyleProperty) {
                                    auto prop = std::static_pointer_cast<StylePropertyNode>(child);
                                    return prop->GetName() == propName;
                                }
                                return false;
                            }),
                        children.end()
                    );
                    LOG_DEBUG("删除样式属性: " + propName);
                }
            }
        }
    }
}

void SpecializationProcessor::ApplyDelete(std::shared_ptr<ASTNode> parent, const DeleteNode* deleteNode) {
    if (!parent || !deleteNode) return;
    
    auto elementParent = std::dynamic_pointer_cast<ElementNode>(parent);
    if (!elementParent) return;
    
    auto targetType = deleteNode->GetTargetType();
    const auto& targets = deleteNode->GetTargets();
    
    switch (targetType) {
        case DeleteNode::DeleteTarget::Element: {
            // 删除匹配的子元素
            auto& children = elementParent->GetChildren();
            
            for (const auto& targetName : targets) {
                // 解析元素名和可选的索引
                std::string elementName = targetName;
                int targetIndex = -1;
                
                // 检查是否有索引，如 "div[1]"
                size_t bracketPos = targetName.find('[');
                if (bracketPos != std::string::npos) {
                    elementName = targetName.substr(0, bracketPos);
                    size_t endBracket = targetName.find(']', bracketPos);
                    if (endBracket != std::string::npos) {
                        std::string indexStr = targetName.substr(bracketPos + 1, endBracket - bracketPos - 1);
                        targetIndex = std::stoi(indexStr);
                    }
                }
                
                int currentIndex = 0;
                children.erase(
                    std::remove_if(children.begin(), children.end(),
                        [&](const std::shared_ptr<ASTNode>& child) {
                            if (child->GetType() == ASTNodeType::Element) {
                                auto elem = std::static_pointer_cast<ElementNode>(child);
                                if (elem->GetTagName() == elementName) {
                                    if (targetIndex == -1 || targetIndex == currentIndex) {
                                        LOG_DEBUG("删除元素: " + targetName);
                                        return true;
                                    }
                                    currentIndex++;
                                }
                            }
                            return false;
                        }),
                    children.end()
                );
            }
            break;
        }
        
        case DeleteNode::DeleteTarget::StyleProperty: {
            // 在局部样式中删除属性
            for (auto& child : elementParent->GetChildren()) {
                if (child->GetType() == ASTNodeType::LocalStyle) {
                    auto styleNode = std::static_pointer_cast<LocalStyleNode>(child);
                    // TODO: 实现从LocalStyleNode中删除属性
                    for (const auto& propName : targets) {
                        LOG_DEBUG("删除样式属性: " + propName);
                    }
                }
            }
            break;
        }
        
        case DeleteNode::DeleteTarget::Template: {
            // 删除模板引用
            for (const auto& templateName : targets) {
                LOG_DEBUG("删除模板引用: " + templateName);
            }
            break;
        }
    }
}

void SpecializationProcessor::ApplyInsert(std::shared_ptr<ASTNode> parent, const InsertNode* insertNode) {
    if (!parent || !insertNode) return;
    
    auto elementParent = std::dynamic_pointer_cast<ElementNode>(parent);
    if (!elementParent) return;
    
    auto& children = elementParent->GetChildren();
    int insertPos = FindInsertPosition(children, insertNode->GetSelector(), insertNode->GetPosition());
    
    if (insertPos >= 0) {
        // 在指定位置插入内容
        const auto& content = insertNode->GetContent();
        
        if (insertNode->GetPosition() == InsertNode::InsertPosition::Replace) {
            // 替换操作：先删除目标元素
            if (insertPos < children.size()) {
                children.erase(children.begin() + insertPos);
            }
        }
        
        // 插入新内容
        children.insert(children.begin() + insertPos, content.begin(), content.end());
        
        LOG_DEBUG("在位置 " + std::to_string(insertPos) + " 插入 " + 
                  std::to_string(content.size()) + " 个节点");
    }
}

void SpecializationProcessor::ApplyElementMatch(std::shared_ptr<ASTNode> parent, const ElementMatchNode* matchNode) {
    if (!parent || !matchNode) return;
    
    auto elementParent = std::dynamic_pointer_cast<ElementNode>(parent);
    if (!elementParent) return;
    
    // 查找匹配的元素
    auto matchingElements = FindMatchingElements(
        elementParent->GetChildren(),
        matchNode->GetElementName(),
        matchNode->GetIndex()
    );
    
    // 为匹配的元素添加样式或属性
    for (auto& elem : matchingElements) {
        auto element = std::static_pointer_cast<ElementNode>(elem);
        
        // 添加matchNode的所有子节点到匹配的元素
        for (const auto& child : matchNode->GetChildren()) {
            element->AddChild(child);
        }
        
        LOG_DEBUG("为元素 " + element->GetTagName() + " 添加特例化内容");
    }
}

std::vector<std::shared_ptr<ASTNode>> SpecializationProcessor::FindMatchingElements(
    const std::vector<std::shared_ptr<ASTNode>>& children,
    const std::string& elementName,
    int index) {
    
    std::vector<std::shared_ptr<ASTNode>> matches;
    int currentIndex = 0;
    
    for (const auto& child : children) {
        if (child->GetType() == ASTNodeType::Element) {
            auto elem = std::static_pointer_cast<ElementNode>(child);
            if (elem->GetTagName() == elementName) {
                if (index == -1 || index == currentIndex) {
                    matches.push_back(child);
                }
                currentIndex++;
            }
        }
    }
    
    return matches;
}

int SpecializationProcessor::FindInsertPosition(
    const std::vector<std::shared_ptr<ASTNode>>& children,
    const std::string& selector,
    InsertNode::InsertPosition position) {
    
    // 查找选择器匹配的元素
    for (size_t i = 0; i < children.size(); ++i) {
        if (children[i]->GetType() == ASTNodeType::Element) {
            auto elem = std::static_pointer_cast<ElementNode>(children[i]);
            
            // 简单的选择器匹配（可以扩展支持更复杂的选择器）
            bool matches = false;
            if (selector == elem->GetTagName()) {
                matches = true;
            } else if (!selector.empty() && selector[0] == '.') {
                // 类选择器
                auto attrs = elem->GetAttributes();
                auto it = attrs.find("class");
                if (it != attrs.end() && it->second == selector.substr(1)) {
                    matches = true;
                }
            } else if (!selector.empty() && selector[0] == '#') {
                // ID选择器
                auto attrs = elem->GetAttributes();
                auto it = attrs.find("id");
                if (it != attrs.end() && it->second == selector.substr(1)) {
                    matches = true;
                }
            }
            
            if (matches) {
                switch (position) {
                    case InsertNode::InsertPosition::Before:
                        return i;
                    case InsertNode::InsertPosition::After:
                        return i + 1;
                    case InsertNode::InsertPosition::Replace:
                        return i;
                    case InsertNode::InsertPosition::AtTop:
                        return 0;
                    case InsertNode::InsertPosition::AtBottom:
                        return children.size();
                }
            }
        }
    }
    
    // 如果没有找到匹配的元素，根据位置返回默认值
    if (position == InsertNode::InsertPosition::AtTop) {
        return 0;
    } else if (position == InsertNode::InsertPosition::AtBottom) {
        return children.size();
    }
    
    return -1;  // 未找到插入位置
}

} // namespace CHTL