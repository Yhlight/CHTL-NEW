#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"

void TestBaseNode() {
    std::cout << "\n=== 基础节点测试 ===" << std::endl;
    
    // 创建基础节点
    auto baseNode = CHTL::CHTLBaseNode::CreateNode(CHTL::CHTLNodeType::BASE, "test", "value");
    baseNode->SetPosition(0, 10, 1, 1);
    baseNode->SetAttribute("attr1", "value1");
    
    std::cout << "基础节点信息: " << baseNode->ToString() << std::endl;
    std::cout << "节点深度: " << baseNode->GetDepth() << std::endl;
    std::cout << "是否为叶子节点: " << (baseNode->IsLeafNode() ? "是" : "否") << std::endl;
    std::cout << "属性attr1: " << baseNode->GetAttribute("attr1") << std::endl;
}

void TestElementNode() {
    std::cout << "\n=== 元素节点测试 ===" << std::endl;
    
    // 创建div元素
    auto divElement = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto divElementPtr = std::dynamic_pointer_cast<CHTL::CHTLElementNode>(divElement);
    
    if (divElementPtr) {
        // 设置属性
        divElementPtr->SetHTMLAttribute("id", "container");
        divElementPtr->SetHTMLAttribute("class", "main-content");
        divElementPtr->SetAutoClassName("auto-class");
        
        std::cout << "元素节点信息: " << divElementPtr->ToString() << std::endl;
        std::cout << "标签名: " << divElementPtr->GetTagName() << std::endl;
        std::cout << "是否自闭合: " << (divElementPtr->IsSelfClosing() ? "是" : "否") << std::endl;
        std::cout << "是否块级元素: " << (divElementPtr->IsBlockElement() ? "是" : "否") << std::endl;
        std::cout << "HTML属性id: " << divElementPtr->GetHTMLAttribute("id") << std::endl;
        std::cout << "自动类名: " << divElementPtr->GetAutoClassName() << std::endl;
        
        // 生成HTML
        std::cout << "生成的开始标签: " << divElementPtr->GenerateOpeningTag() << std::endl;
        std::cout << "生成的结束标签: " << divElementPtr->GenerateClosingTag() << std::endl;
    }
    
    // 测试自闭合元素
    auto imgElement = CHTL::CHTLNodeFactory::CreateElementNode("img");
    auto imgElementPtr = std::dynamic_pointer_cast<CHTL::CHTLElementNode>(imgElement);
    
    if (imgElementPtr) {
        imgElementPtr->SetHTMLAttribute("src", "test.jpg");
        imgElementPtr->SetHTMLAttribute("alt", "测试图片");
        
        std::cout << "\n图片元素: " << imgElementPtr->ToString() << std::endl;
        std::cout << "是否自闭合: " << (imgElementPtr->IsSelfClosing() ? "是" : "否") << std::endl;
        std::cout << "生成的标签: " << imgElementPtr->GenerateOpeningTag() << std::endl;
    }
}

void TestTextNode() {
    std::cout << "\n=== 文本节点测试 ===" << std::endl;
    
    // 创建文本节点
    auto textNode = CHTL::CHTLNodeFactory::CreateTextNode("这是一段测试文本");
    auto textNodePtr = std::dynamic_pointer_cast<CHTL::CHTLTextNode>(textNode);
    
    if (textNodePtr) {
        std::cout << "文本节点信息: " << textNodePtr->ToString() << std::endl;
        std::cout << "文本内容: " << textNodePtr->GetTextContent() << std::endl;
        std::cout << "文本长度: " << textNodePtr->GetTextLength() << std::endl;
        std::cout << "是否为空: " << (textNodePtr->IsEmpty() ? "是" : "否") << std::endl;
        std::cout << "是否只有空白: " << (textNodePtr->ContainsOnlyWhitespace() ? "是" : "否") << std::endl;
        
        // 测试HTML转义
        textNodePtr->SetTextContent("<script>alert('test')</script>");
        std::cout << "转义前: " << textNodePtr->GetTextContent() << std::endl;
        std::cout << "转义后: " << textNodePtr->EscapeHTML() << std::endl;
    }
    
    // 测试包含变量引用的文本
    auto varTextNode = CHTL::CHTLNodeFactory::CreateTextNode("颜色是 ${color}，大小是 ${size}");
    auto varTextNodePtr = std::dynamic_pointer_cast<CHTL::CHTLTextNode>(varTextNode);
    
    if (varTextNodePtr) {
        std::cout << "\n变量文本节点: " << varTextNodePtr->ToString() << std::endl;
        std::cout << "是否包含变量引用: " << (varTextNodePtr->HasVariableReference() ? "是" : "否") << std::endl;
        
        const auto& varRefs = varTextNodePtr->GetVariableReferences();
        std::cout << "变量引用数量: " << varRefs.size() << std::endl;
        for (const auto& varRef : varRefs) {
            std::cout << "  变量: " << varRef << std::endl;
        }
        
        std::cout << "处理后内容: " << varTextNodePtr->ProcessVariableReferences() << std::endl;
    }
}

void TestCommentNode() {
    std::cout << "\n=== 注释节点测试 ===" << std::endl;
    
    // 测试不同类型的注释
    auto singleLineComment = std::make_shared<CHTL::CHTLCommentNode>(
        CHTL::CHTLCommentType::SINGLE_LINE, "这是单行注释"
    );
    
    auto multiLineComment = std::make_shared<CHTL::CHTLCommentNode>(
        CHTL::CHTLCommentType::MULTI_LINE, "这是多行注释\n可以跨行"
    );
    
    auto generatorComment = std::make_shared<CHTL::CHTLCommentNode>(
        CHTL::CHTLCommentType::GENERATOR, "这是生成器注释"
    );
    
    std::vector<std::shared_ptr<CHTL::CHTLCommentNode>> comments = {
        singleLineComment, multiLineComment, generatorComment
    };
    
    for (const auto& comment : comments) {
        std::cout << "注释信息: " << comment->ToString() << std::endl;
        std::cout << "  类型: " << CHTL::CHTLCommentNode::CommentTypeToString(comment->GetCommentType()) << std::endl;
        std::cout << "  应该生成: " << (comment->ShouldGenerate() ? "是" : "否") << std::endl;
        std::cout << "  HTML格式: " << comment->ToHTMLComment() << std::endl;
        std::cout << "  CSS格式: " << comment->ToCSSComment() << std::endl;
        std::cout << "  JS格式: " << comment->ToJSComment() << std::endl;
        std::cout << std::endl;
    }
}

void TestNodeHierarchy() {
    std::cout << "\n=== 节点层次结构测试 ===" << std::endl;
    
    // 简化测试，避免复杂的节点关系
    auto divElement = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto textNode1 = CHTL::CHTLNodeFactory::CreateTextNode("简单文本测试");
    
    std::cout << "创建了div元素和文本节点" << std::endl;
    std::cout << "div节点类型: " << CHTL::CHTLBaseNode::NodeTypeToString(divElement->GetNodeType()) << std::endl;
    std::cout << "文本节点类型: " << CHTL::CHTLBaseNode::NodeTypeToString(textNode1->GetNodeType()) << std::endl;
    
    // 基础功能测试
    divElement->AddChild(textNode1);
    std::cout << "子节点数量: " << divElement->GetChildCount() << std::endl;
    
    auto elementPtr = std::dynamic_pointer_cast<CHTL::CHTLElementNode>(divElement);
    if (elementPtr) {
        elementPtr->SetHTMLAttribute("class", "test-class");
        std::cout << "设置class属性完成" << std::endl;
        std::cout << "生成的开始标签: " << elementPtr->GenerateOpeningTag() << std::endl;
    }
}

int main() {
    try {
        std::cout << "CHTL AST节点系统测试开始..." << std::endl;
        
        // 执行第一阶段基础节点测试
        TestBaseNode();
        TestElementNode();
        TestTextNode();
        TestCommentNode();
        TestNodeHierarchy();
        
        std::cout << "\n第一阶段基础AST节点测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}