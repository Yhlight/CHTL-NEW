#ifndef CHTL_CORE_SPECIALIZATIONPROCESSOR_H
#define CHTL_CORE_SPECIALIZATIONPROCESSOR_H

#include <memory>
#include <vector>
#include <string>
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "Common/Logger.h"

namespace CHTL {

/**
 * 特例化处理器
 * 负责在AST上应用特例化操作（delete、insert、元素匹配等）
 */
class SpecializationProcessor {
private:
    /**
     * 应用删除操作
     */
    void ApplyDelete(std::shared_ptr<ASTNode> parent, const DeleteNode* deleteNode);
    
    /**
     * 应用插入操作
     */
    void ApplyInsert(std::shared_ptr<ASTNode> parent, const InsertNode* insertNode);
    
    /**
     * 应用元素匹配操作
     */
    void ApplyElementMatch(std::shared_ptr<ASTNode> parent, const ElementMatchNode* matchNode);
    
    /**
     * 在子节点中查找匹配的元素
     */
    std::vector<std::shared_ptr<ASTNode>> FindMatchingElements(
        const std::vector<std::shared_ptr<ASTNode>>& children,
        const std::string& elementName,
        int index = -1
    );
    
    /**
     * 在元素中查找插入位置
     */
    int FindInsertPosition(
        const std::vector<std::shared_ptr<ASTNode>>& children,
        const std::string& selector,
        InsertNode::InsertPosition position
    );

public:
    /**
     * 处理元素的特例化操作
     * @param element 要处理的元素
     * @param specializations 特例化操作列表
     */
    void ProcessSpecializations(
        std::shared_ptr<ElementNode> element,
        const std::vector<std::shared_ptr<ASTNode>>& specializations
    );
    
    /**
     * 处理自定义样式的特例化操作
     * @param style 要处理的样式
     * @param specializations 特例化操作列表
     */
    void ProcessStyleSpecializations(
        std::shared_ptr<TemplateStyleNode> style,
        const std::vector<std::shared_ptr<ASTNode>>& specializations
    );
};

} // namespace CHTL

#endif // CHTL_CORE_SPECIALIZATIONPROCESSOR_H