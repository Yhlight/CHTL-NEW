#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/TemplateNode.h"

void TestTemplateStyleNode() {
    std::cout << "\n=== 样式组模板节点测试 ===" << std::endl;
    
    // 创建样式组模板
    auto defaultText = CHTL::CHTLTemplateNodeFactory::CreateTemplateStyleNode("DefaultText");
    
    // 添加样式属性
    defaultText->AddStyleProperty("color", "black");
    defaultText->AddStyleProperty("line-height", "1.6");
    defaultText->AddStyleProperty("font-size", "16px");
    
    std::cout << "样式组模板: " << defaultText->ToString() << std::endl;
    std::cout << "属性数量: " << defaultText->GetAllStyleProperties().size() << std::endl;
    
    // 测试属性获取
    std::cout << "color属性: " << defaultText->GetStyleProperty("color") << std::endl;
    std::cout << "line-height属性: " << defaultText->GetStyleProperty("line-height") << std::endl;
    
    // 生成CSS
    std::cout << "生成CSS:\n" << defaultText->GenerateCSS() << std::endl;
    std::cout << "生成内联样式: " << defaultText->GenerateInlineStyle() << std::endl;
}

void TestTemplateElementNode() {
    std::cout << "\n=== 元素模板节点测试 ===" << std::endl;
    
    // 创建元素模板
    auto boxTemplate = CHTL::CHTLTemplateNodeFactory::CreateTemplateElementNode("Box");
    
    // 创建模板中的元素
    auto spanElement = CHTL::CHTLNodeFactory::CreateElementNode("span");
    auto textNode = CHTL::CHTLNodeFactory::CreateTextNode("这是一组div");
    spanElement->AddChild(textNode);
    
    auto divElement1 = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto divElementPtr1 = std::dynamic_pointer_cast<CHTL::CHTLElementNode>(divElement1);
    if (divElementPtr1) {
        divElementPtr1->SetHTMLAttribute("style", "background-color: red;");
    }
    
    auto divElement2 = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto divElementPtr2 = std::dynamic_pointer_cast<CHTL::CHTLElementNode>(divElement2);
    if (divElementPtr2) {
        divElementPtr2->SetHTMLAttribute("style", "background-color: blue;");
    }
    
    // 添加到模板
    boxTemplate->AddTemplateElement(spanElement);
    boxTemplate->AddTemplateElement(divElement1);
    boxTemplate->AddTemplateElement(divElement2);
    
    std::cout << "元素模板: " << boxTemplate->ToString() << std::endl;
    std::cout << "模板元素数量: " << boxTemplate->GetTemplateElementCount() << std::endl;
    
    // 生成HTML
    std::cout << "生成HTML:\n" << boxTemplate->GenerateHTML() << std::endl;
}

void TestTemplateVarNode() {
    std::cout << "\n=== 变量组模板节点测试 ===" << std::endl;
    
    // 创建变量组模板
    auto themeColor = CHTL::CHTLTemplateNodeFactory::CreateTemplateVarNode("ThemeColor");
    
    // 添加变量
    themeColor->AddVariable("tableColor", "rgb(255, 192, 203)");
    themeColor->AddVariable("textColor", "black");
    themeColor->AddVariable("borderColor", "gray");
    
    std::cout << "变量组模板: " << themeColor->ToString() << std::endl;
    std::cout << "变量数量: " << themeColor->GetAllVariables().size() << std::endl;
    
    // 测试变量获取
    std::cout << "tableColor变量: " << themeColor->GetVariable("tableColor") << std::endl;
    std::cout << "textColor变量: " << themeColor->GetVariable("textColor") << std::endl;
    
    // 测试变量引用处理
    std::string testText = "背景色是 ThemeColor(tableColor)，文字色是 ThemeColor(textColor)";
    std::cout << "原始文本: " << testText << std::endl;
    std::cout << "处理后文本: " << themeColor->ProcessVariableReferences(testText) << std::endl;
}

void TestTemplateInheritance() {
    std::cout << "\n=== 模板继承测试 ===" << std::endl;
    
    // 创建父模板
    auto parentStyle = CHTL::CHTLTemplateNodeFactory::CreateTemplateStyleNode("ThemeColor");
    parentStyle->AddStyleProperty("color", "rgba(255, 192, 203, 1)");
    parentStyle->AddStyleProperty("background-color", "rgba(253, 144, 162, 1)");
    
    // 创建子模板
    auto childStyle = CHTL::CHTLTemplateNodeFactory::CreateTemplateStyleNode("ThemeColor2");
    childStyle->AddStyleProperty("background-color", "yellow"); // 重复属性，会替换
    
    // 添加组合式继承
    childStyle->AddInheritance("ThemeColor", CHTL::CHTLInheritanceType::COMPOSITION);
    
    std::cout << "父模板: " << parentStyle->ToString() << std::endl;
    std::cout << "父模板CSS:\n" << parentStyle->GenerateCSS() << std::endl;
    
    std::cout << "子模板（继承前）: " << childStyle->ToString() << std::endl;
    std::cout << "子模板CSS（继承前）:\n" << childStyle->GenerateCSS() << std::endl;
    
    // 应用继承
    childStyle->ApplyInheritance(*parentStyle);
    
    std::cout << "子模板（继承后）: " << childStyle->ToString() << std::endl;
    std::cout << "子模板CSS（继承后）:\n" << childStyle->GenerateCSS() << std::endl;
}

void TestTemplateUsage() {
    std::cout << "\n=== 模板使用测试 ===" << std::endl;
    
    // 创建模板使用节点
    auto styleUsage = CHTL::CHTLTemplateNodeFactory::CreateStyleUsageNode("DefaultText");
    auto elementUsage = CHTL::CHTLTemplateNodeFactory::CreateElementUsageNode("Box");
    auto varUsage = CHTL::CHTLTemplateNodeFactory::CreateVarUsageNode("ThemeColor");
    
    std::cout << "样式模板使用: " << styleUsage->ToString() << std::endl;
    std::cout << "元素模板使用: " << elementUsage->ToString() << std::endl;
    std::cout << "变量模板使用: " << varUsage->ToString() << std::endl;
    
    // 测试全缀名
    styleUsage->SetFullQualified(true, "[Template]");
    std::cout << "全缀名样式使用: " << styleUsage->ToString() << std::endl;
}

int main() {
    try {
        std::cout << "CHTL第三阶段模板系统测试开始..." << std::endl;
        
        // 执行第三阶段模板系统测试
        TestTemplateStyleNode();
        TestTemplateElementNode();
        TestTemplateVarNode();
        TestTemplateInheritance();
        TestTemplateUsage();
        
        std::cout << "\n第三阶段模板系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}