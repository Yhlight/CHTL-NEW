#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/TemplateNode.h"
#include "../../CHTL/CHTLNode/CustomNode.h"

void TestCustomStyleNode() {
    std::cout << "\n=== 自定义样式组节点测试 ===" << std::endl;
    
    // 测试无值样式组
    auto textSet = CHTL::CHTLCustomNodeFactory::CreateCustomStyleNode("TextSet");
    
    // 添加无值属性
    textSet->AddNoValueProperty("color");
    textSet->AddNoValueProperty("font-size");
    
    std::cout << "无值样式组: " << textSet->ToString() << std::endl;
    std::cout << "无值属性数量: " << textSet->GetNoValueProperties().size() << std::endl;
    std::cout << "color是否为无值属性: " << (textSet->IsNoValueProperty("color") ? "是" : "否") << std::endl;
    std::cout << "font-size是否为无值属性: " << (textSet->IsNoValueProperty("font-size") ? "是" : "否") << std::endl;
    
    // 测试特例化：填充无值属性
    textSet->FillNoValueProperty("color", "red");
    textSet->FillNoValueProperty("font-size", "16px");
    
    std::cout << "特例化后: " << textSet->ToString() << std::endl;
    std::cout << "color属性: " << textSet->GetStyleProperty("color") << std::endl;
    std::cout << "font-size属性: " << textSet->GetStyleProperty("font-size") << std::endl;
    std::cout << "生成CSS:\n" << textSet->GenerateCSS() << std::endl;
}

void TestCustomElementNode() {
    std::cout << "\n=== 自定义元素节点测试 ===" << std::endl;
    
    // 创建自定义元素
    auto boxElement = CHTL::CHTLCustomNodeFactory::CreateCustomElementNode("Box");
    
    // 添加元素
    auto div1 = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto div2 = CHTL::CHTLNodeFactory::CreateElementNode("div");
    auto span1 = CHTL::CHTLNodeFactory::CreateElementNode("span");
    auto div3 = CHTL::CHTLNodeFactory::CreateElementNode("div");
    
    boxElement->AddCustomElement(div1);
    boxElement->AddCustomElement(div2);
    boxElement->AddCustomElement(span1);
    boxElement->AddCustomElement(div3);
    
    std::cout << "自定义元素: " << boxElement->ToString() << std::endl;
    std::cout << "元素数量: " << boxElement->GetCustomElementCount() << std::endl;
    
    // 测试索引访问
    auto elementByIndex = boxElement->GetElementByIndex(1);
    if (elementByIndex) {
        std::cout << "索引1的元素: " << elementByIndex->GetNodeName() << std::endl;
    }
    
    // 测试按类型获取
    auto divElements = boxElement->GetElementsByType("div");
    std::cout << "div元素数量: " << divElements.size() << std::endl;
    
    // 测试删除操作
    boxElement->DeleteElement("span");  // 删除所有span
    boxElement->DeleteElementByIndex(1); // 删除索引1的元素
    
    std::cout << "删除操作后: " << boxElement->ToString() << std::endl;
    std::cout << "span是否被删除: " << (boxElement->IsElementTypeDeleted("span") ? "是" : "否") << std::endl;
    std::cout << "索引1是否被删除: " << (boxElement->IsElementDeleted(1) ? "是" : "否") << std::endl;
    
    // 测试插入操作
    auto newDiv = CHTL::CHTLNodeFactory::CreateElementNode("div");
    boxElement->InsertElementAtIndex(CHTL::CHTLInsertPosition::AFTER, 0, newDiv);
    
    std::cout << "插入操作后元素数量: " << boxElement->GetCustomElementCount() << std::endl;
    
    // 生成HTML
    std::cout << "生成HTML:\n" << boxElement->GenerateHTML() << std::endl;
}

void TestCustomVarNode() {
    std::cout << "\n=== 自定义变量组节点测试 ===" << std::endl;
    
    // 创建自定义变量组
    auto themeColor = CHTL::CHTLCustomNodeFactory::CreateCustomVarNode("ThemeColor");
    
    // 添加变量
    themeColor->AddVariable("tableColor", "rgb(255, 192, 203)");
    themeColor->AddVariable("textColor", "black");
    
    std::cout << "自定义变量组: " << themeColor->ToString() << std::endl;
    std::cout << "变量数量: " << themeColor->GetAllVariables().size() << std::endl;
    
    // 测试普通变量引用
    std::string normalRef = "背景色是 ThemeColor(tableColor)";
    std::cout << "普通引用: " << normalRef << std::endl;
    std::cout << "处理后: " << themeColor->ProcessVariableReferences(normalRef) << std::endl;
    
    // 测试变量组特例化
    std::string specializedRef = "颜色是 ThemeColor(tableColor = rgb(145, 155, 200))";
    std::cout << "特例化引用: " << specializedRef << std::endl;
    std::cout << "处理后: " << themeColor->ProcessVariableReferences(specializedRef) << std::endl;
}

void TestCustomUsageNode() {
    std::cout << "\n=== 自定义使用节点测试 ===" << std::endl;
    
    // 创建自定义使用节点
    auto styleUsage = CHTL::CHTLCustomNodeFactory::CreateStyleUsageNode("TextSet");
    
    // 添加特例化值
    styleUsage->AddSpecialization("color", "blue");
    styleUsage->AddSpecialization("font-size", "18px");
    
    std::cout << "自定义样式使用: " << styleUsage->ToString() << std::endl;
    std::cout << "是否有特例化: " << (styleUsage->HasSpecialization() ? "是" : "否") << std::endl;
    std::cout << "color特例化值: " << styleUsage->GetSpecialization("color") << std::endl;
    
    // 添加删除操作
    styleUsage->AddDeletion("line-height");
    styleUsage->AddDeletion("border");
    
    std::cout << "添加删除操作后: " << styleUsage->ToString() << std::endl;
    std::cout << "line-height是否被删除: " << (styleUsage->IsDeleted("line-height") ? "是" : "否") << std::endl;
    
    // 测试全缀名
    styleUsage->SetFullQualified(true, "[Custom]");
    std::cout << "全缀名使用: " << styleUsage->ToString() << std::endl;
}

void TestOperatorNode() {
    std::cout << "\n=== 操作节点测试 ===" << std::endl;
    
    // 测试删除操作
    auto deleteOp = CHTL::CHTLCustomNodeFactory::CreateDeleteNode("line-height");
    std::cout << "删除操作: " << deleteOp->ToString() << std::endl;
    std::cout << "可以执行: " << (deleteOp->CanExecute() ? "是" : "否") << std::endl;
    
    // 测试插入操作
    auto insertOp = CHTL::CHTLCustomNodeFactory::CreateInsertNode(CHTL::CHTLInsertPosition::AFTER, "div[0]");
    std::cout << "插入操作: " << insertOp->ToString() << std::endl;
    std::cout << "插入位置: " << CHTL::CHTLOperatorNode::InsertPositionToString(insertOp->GetInsertPosition()) << std::endl;
}

void TestCustomInheritance() {
    std::cout << "\n=== 自定义继承测试 ===" << std::endl;
    
    // 创建模板样式（用于被自定义继承）
    auto templateStyle = CHTL::CHTLTemplateNodeFactory::CreateTemplateStyleNode("WhiteText");
    templateStyle->AddStyleProperty("color", "white");
    templateStyle->AddStyleProperty("font-size", "16px");
    templateStyle->AddStyleProperty("line-height", "1.6");
    templateStyle->AddStyleProperty("border", "1px black solid");
    
    // 创建自定义样式，继承模板
    auto customStyle = CHTL::CHTLCustomNodeFactory::CreateCustomStyleNode("YellowText");
    customStyle->AddInheritance("WhiteText", CHTL::CHTLInheritanceType::COMPOSITION);
    customStyle->AddStyleProperty("color", "yellow"); // 覆盖继承的color
    
    std::cout << "模板样式: " << templateStyle->ToString() << std::endl;
    std::cout << "自定义样式（继承前）: " << customStyle->ToString() << std::endl;
    
    // 应用继承
    customStyle->ApplyTemplateInheritance(*templateStyle);
    
    std::cout << "自定义样式（继承后）: " << customStyle->ToString() << std::endl;
    std::cout << "生成CSS:\n" << customStyle->GenerateCSS() << std::endl;
    
    // 测试删除操作
    customStyle->DeleteProperty("line-height");
    customStyle->DeleteProperty("border");
    
    std::cout << "删除属性后: " << customStyle->ToString() << std::endl;
    std::cout << "生成CSS（删除后）:\n" << customStyle->GenerateCSS() << std::endl;
}

int main() {
    try {
        std::cout << "CHTL第四阶段自定义系统测试开始..." << std::endl;
        
        // 执行第四阶段自定义系统测试
        TestCustomStyleNode();
        TestCustomElementNode();
        TestCustomVarNode();
        TestCustomUsageNode();
        TestOperatorNode();
        TestCustomInheritance();
        
        std::cout << "\n第四阶段自定义系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}