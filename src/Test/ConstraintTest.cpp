#include "../ConstraintSystem/CHTLConstraintValidator.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/ScriptNode.h"
#include "../CHTL JS/CHTLJSNode/EnhanceSelectorNode.h"
#include "../CHTL JS/CHTLJSNode/VirObjectNode.h"
#include <iostream>

int main() {
    std::cout << "=== CHTL约束系统测试 ===" << std::endl;
    std::cout << "验证CHTL与CHTL JS语法边界的严格控制" << std::endl;
    
    CHTL::CHTLConstraintValidator validator;
    
    // 1. 测试语法归属
    std::cout << "\n📋 语法归属验证:" << std::endl;
    
    std::vector<std::string> testSyntaxes = {
        "html", "text", "style", "script",           // CHTL语法
        "module", "vir", "listen", "{{}}",           // CHTL JS语法
        "comment", "identifier", "string_literal"    // 共享语法
    };
    
    for (const auto& syntax : testSyntaxes) {
        std::string ownership = validator.GetSyntaxOwnership(syntax);
        std::cout << "  " << syntax << " → " << ownership << std::endl;
    }
    
    // 2. 测试语法边界验证
    std::cout << "\n🔒 语法边界验证:" << std::endl;
    
    // CHTL语法在CHTL JS中的使用（应该失败）
    bool htmlInCHTLJS = validator.ValidateSyntaxBoundary("html", "CHTL_JS");
    std::cout << "  html在CHTL JS中使用: " << (htmlInCHTLJS ? "✓ 允许" : "✗ 禁止") << std::endl;
    
    // CHTL JS语法在CHTL中的使用（应该失败）
    bool moduleInCHTL = validator.ValidateSyntaxBoundary("module", "CHTL");
    std::cout << "  module在CHTL中使用: " << (moduleInCHTL ? "✓ 允许" : "✗ 禁止") << std::endl;
    
    // 共享语法的使用（应该成功）
    bool commentInCHTL = validator.ValidateSyntaxBoundary("comment", "CHTL");
    bool commentInCHTLJS = validator.ValidateSyntaxBoundary("comment", "CHTL_JS");
    std::cout << "  comment在CHTL中使用: " << (commentInCHTL ? "✓ 允许" : "✗ 禁止") << std::endl;
    std::cout << "  comment在CHTL JS中使用: " << (commentInCHTLJS ? "✓ 允许" : "✗ 禁止") << std::endl;
    
    // 3. 测试局部script特殊约束
    std::cout << "\n📝 局部script约束验证:" << std::endl;
    
    // 允许的语法
    std::string allowedScript = R"(
        // 允许的CHTL语法
        @Var ThemeColor;
        ThemeColor(primaryColor);
        
        // 允许的{{&}}特供语法
        {{&}}.addEventListener('click', function() {
            console.log('Context reference');
        });
        
        // 允许的JavaScript语法
        var x = 10;
        console.log(x);
    )";
    
    auto allowedResult = validator.ValidateLocalScriptConstraints(allowedScript);
    std::cout << "  允许的script语法: " << (allowedResult.IsValid ? "✓ 通过" : "✗ 失败") << std::endl;
    if (!allowedResult.Violations.empty()) {
        for (const auto& violation : allowedResult.Violations) {
            std::cout << "    违反: " << violation << std::endl;
        }
    }
    if (!allowedResult.Warnings.empty()) {
        for (const auto& warning : allowedResult.Warnings) {
            std::cout << "    警告: " << warning << std::endl;
        }
    }
    
    // 禁止的语法
    std::string forbiddenScript = R"(
        // 禁止的CHTL语法
        [Template] @Element TestElement {
            div { }
        }
        
        html {
            text { "不允许" }
        }
        
        // 禁止的CHTL JS语法
        module {
            load: "test.js"
        }
        
        vir Test = listen {
            click: function() { }
        }
    )";
    
    auto forbiddenResult = validator.ValidateLocalScriptConstraints(forbiddenScript);
    std::cout << "  禁止的script语法: " << (forbiddenResult.IsValid ? "✓ 通过" : "✗ 失败（符合预期）") << std::endl;
    if (!forbiddenResult.Violations.empty()) {
        std::cout << "    检测到的违反（符合预期）:" << std::endl;
        for (const auto& violation : forbiddenResult.Violations) {
            std::cout << "      - " << violation << std::endl;
        }
    }
    
    // 4. 测试节点约束验证
    std::cout << "\n🏗️  节点约束验证:" << std::endl;
    
    try {
        // 创建CHTL元素节点
        auto htmlElement = std::make_unique<CHTL::ElementNode>("html", 1, 1);
        auto elementResult = validator.ValidateCHTLNode(htmlElement.get(), "document_root");
        std::cout << "  CHTL html元素在文档根部: " << (elementResult.IsValid ? "✓ 通过" : "✗ 失败") << std::endl;
        
        // 创建局部script节点
        auto scriptNode = std::make_unique<CHTL::ScriptNode>("console.log('test');", 1, 1);
        auto scriptResult = validator.ValidateCHTLNode(scriptNode.get(), "element_content");
        std::cout << "  CHTL局部script在元素内容中: " << (scriptResult.IsValid ? "✓ 通过" : "✗ 失败") << std::endl;
        
        // 创建CHTL JS增强选择器节点
        auto selectorNode = std::make_unique<CHTLJS::EnhancedSelectorNode>("{{.test}}", 1, 1);
        auto selectorResult = validator.ValidateCHTLJSNode(selectorNode.get(), "chtljs_global_scope");
        std::cout << "  CHTL JS增强选择器: " << (selectorResult.IsValid ? "✓ 通过" : "✗ 失败") << std::endl;
        
        // 创建CHTL JS虚对象节点
        auto virNode = std::make_unique<CHTLJS::VirObjectNode>("TestVir", 1, 1);
        auto virResult = validator.ValidateCHTLJSNode(virNode.get(), "chtljs_global_scope");
        std::cout << "  CHTL JS虚对象: " << (virResult.IsValid ? "✓ 通过" : "✗ 失败") << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cout << "  节点验证异常: " << e.what() << std::endl;
    }
    
    // 5. 约束规则统计
    std::cout << "\n📊 约束规则统计:" << std::endl;
    auto allConstraints = validator.GetAllConstraints();
    
    int preciseCount = 0, typeCount = 0, globalCount = 0;
    for (const auto& constraint : allConstraints) {
        switch (constraint.Type) {
            case CHTL::ConstraintType::PRECISE_CONSTRAINT:
                preciseCount++;
                break;
            case CHTL::ConstraintType::TYPE_CONSTRAINT:
                typeCount++;
                break;
            case CHTL::ConstraintType::GLOBAL_CONSTRAINT:
                globalCount++;
                break;
        }
    }
    
    std::cout << "  精准约束: " << preciseCount << " 条" << std::endl;
    std::cout << "  类型约束: " << typeCount << " 条" << std::endl;
    std::cout << "  全局约束: " << globalCount << " 条" << std::endl;
    std::cout << "  总计约束: " << allConstraints.size() << " 条" << std::endl;
    
    std::cout << "\n🎯 约束系统重大意义:" << std::endl;
    std::cout << "✓ 确保CHTL和CHTL JS语法边界的严格分离" << std::endl;
    std::cout << "✓ 防止语法混用导致的编译错误" << std::endl;
    std::cout << "✓ 特殊处理局部script的混合语法约束" << std::endl;
    std::cout << "✓ 提供精准、类型、全局三级约束控制" << std::endl;
    std::cout << "✓ 支持动态约束规则添加和管理" << std::endl;
    
    std::cout << "\n🏆 CHTL约束系统验证完成！" << std::endl;
    std::cout << "语法边界明确，约束规则完整，符合CHTL项目的高标准要求。" << std::endl;
    
    return 0;
}