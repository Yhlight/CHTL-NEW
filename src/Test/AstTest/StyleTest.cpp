#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"

void TestStyleNode() {
    std::cout << "\n=== 样式节点测试 ===" << std::endl;
    
    // 创建局部样式节点
    auto localStyle = CHTL::CHTLStyleNodeFactory::CreateLocalStyleNode();
    localStyle->SetParentElementId("div-1");
    
    std::cout << "局部样式节点: " << localStyle->ToString() << std::endl;
    std::cout << "是否为局部: " << (localStyle->IsLocal() ? "是" : "否") << std::endl;
    
    // 添加内联样式属性
    localStyle->AddStyleProperty("width", "100px");
    localStyle->AddStyleProperty("height", "200px");
    localStyle->AddStyleProperty("background-color", "red");
    
    std::cout << "内联样式属性数量: " << localStyle->GetAllStyleProperties().size() << std::endl;
    std::cout << "生成内联CSS: " << localStyle->GenerateInlineCSS() << std::endl;
}

void TestSelectorNode() {
    std::cout << "\n=== 选择器节点测试 ===" << std::endl;
    
    // 测试类选择器
    auto classSelector = CHTL::CHTLStyleNodeFactory::CreateClassSelector("box");
    std::cout << "类选择器: " << classSelector->ToString() << std::endl;
    std::cout << "选择器值: " << classSelector->GetSelectorValue() << std::endl;
    std::cout << "选择器类型: " << CHTL::CHTLSelectorNode::SelectorTypeToString(classSelector->GetSelectorType()) << std::endl;
    
    // 测试ID选择器
    auto idSelector = CHTL::CHTLStyleNodeFactory::CreateIdSelector("container");
    std::cout << "ID选择器: " << idSelector->ToString() << std::endl;
    
    // 测试上下文引用选择器
    auto contextRef = CHTL::CHTLStyleNodeFactory::CreateContextReference();
    contextRef->ResolveContextReference("main-box", "");
    std::cout << "上下文引用: " << contextRef->ToString() << std::endl;
    std::cout << "解析后引用: " << contextRef->GetContextReference() << std::endl;
    
    // 测试伪类选择器
    auto pseudoClass = CHTL::CHTLStyleNodeFactory::CreatePseudoClassSelector("hover");
    std::cout << "伪类选择器: " << pseudoClass->ToString() << std::endl;
    std::cout << "伪类名: " << pseudoClass->GetPseudoClass() << std::endl;
    
    // 测试伪元素选择器
    auto pseudoElement = CHTL::CHTLStyleNodeFactory::CreatePseudoElementSelector("before");
    std::cout << "伪元素选择器: " << pseudoElement->ToString() << std::endl;
    std::cout << "伪元素名: " << pseudoElement->GetPseudoElement() << std::endl;
}

void TestStylePropertyNode() {
    std::cout << "\n=== 样式属性节点测试 ===" << std::endl;
    
    // 创建普通样式属性
    auto normalProp = CHTL::CHTLStyleNodeFactory::CreateStyleProperty("color", "red");
    std::cout << "普通属性: " << normalProp->ToString() << std::endl;
    std::cout << "生成CSS: " << normalProp->GenerateCSS() << std::endl;
    
    // 创建!important属性
    auto importantProp = CHTL::CHTLStyleNodeFactory::CreateStyleProperty("width", "100px !important");
    std::cout << "重要属性: " << importantProp->ToString() << std::endl;
    std::cout << "是否重要: " << (importantProp->IsImportant() ? "是" : "否") << std::endl;
    std::cout << "生成CSS: " << importantProp->GenerateCSS() << std::endl;
    
    // 创建包含变量引用的属性
    auto varProp = CHTL::CHTLStyleNodeFactory::CreateStyleProperty("background-color", "ThemeColor(primaryColor)");
    std::cout << "变量属性: " << varProp->ToString() << std::endl;
    std::cout << "包含变量引用: " << (varProp->HasVariableReference() ? "是" : "否") << std::endl;
}

void TestAutomationRules() {
    std::cout << "\n=== 自动化规则测试 ===" << std::endl;
    
    // 创建包含选择器的局部样式
    auto localStyle = CHTL::CHTLStyleNodeFactory::CreateLocalStyleNode();
    
    // 添加类选择器
    auto classSelector = CHTL::CHTLStyleNodeFactory::CreateClassSelector("container");
    auto widthProp = CHTL::CHTLStyleNodeFactory::CreateStyleProperty("width", "100%");
    classSelector->AddChild(widthProp);
    localStyle->AddChild(classSelector);
    
    // 添加上下文引用选择器
    auto contextRef = CHTL::CHTLStyleNodeFactory::CreateContextReference();
    auto hoverProp = CHTL::CHTLStyleNodeFactory::CreateStyleProperty("background-color", "lightgray");
    contextRef->AddChild(hoverProp);
    contextRef->SetPseudoClass("hover");
    localStyle->AddChild(contextRef);
    
    std::cout << "处理自动化前:" << std::endl;
    std::cout << "  局部样式: " << localStyle->ToString() << std::endl;
    std::cout << "  类选择器: " << classSelector->ToString() << std::endl;
    std::cout << "  上下文引用: " << contextRef->ToString() << std::endl;
    
    // 处理自动化
    localStyle->ProcessAutomation();
    
    std::cout << "处理自动化后:" << std::endl;
    std::cout << "  局部样式: " << localStyle->ToString() << std::endl;
    std::cout << "  自动类名: " << localStyle->GetAutoClassName() << std::endl;
    std::cout << "  应该添加到全局: " << (localStyle->ShouldAddToGlobal() ? "是" : "否") << std::endl;
    std::cout << "  上下文引用解析: " << contextRef->GetContextReference() << std::endl;
    
    // 生成CSS
    std::cout << "生成的全局CSS:\n" << localStyle->GenerateGlobalCSS() << std::endl;
}

int main() {
    try {
        std::cout << "CHTL第二阶段样式系统测试开始..." << std::endl;
        
        // 执行第二阶段样式节点测试
        TestStyleNode();
        TestSelectorNode();
        TestStylePropertyNode();
        TestAutomationRules();
        
        std::cout << "\n第二阶段样式系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}