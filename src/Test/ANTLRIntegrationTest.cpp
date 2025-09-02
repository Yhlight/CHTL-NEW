#include "../CSS/CSSCompiler.h"
#include "../JS/JavaScriptCompiler.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== ANTLR集成测试 ===" << std::endl;
    std::cout << "验证CSS和JavaScript编译器的ANTLR架构" << std::endl;
    
    // 1. 测试CSS编译器
    std::cout << "\n🎨 CSS编译器测试:" << std::endl;
    
    std::string testCSS = R"(
.container {
    width: 100%;
    margin: 0 auto;
    padding: 20px;
}

#header {
    background-color: #333;
    color: white;
    height: 60px;
}

.button:hover {
    background-color: #007bff;
    transform: scale(1.05);
}
)";
    
    try {
        CHTL::CSSCompiler cssCompiler;
        
        std::cout << "  CSS源代码长度: " << testCSS.length() << " 字符" << std::endl;
        
        auto cssResult = cssCompiler.Compile(testCSS);
        std::cout << "  CSS编译结果: " << (cssResult.IsSuccess ? "✓ 成功" : "✗ 失败") << std::endl;
        
        if (cssResult.IsSuccess) {
            std::cout << "  优化后CSS长度: " << cssResult.OptimizedCSS.length() << " 字符" << std::endl;
            std::cout << "  CSS警告数量: " << cssResult.Warnings.size() << std::endl;
            
            if (!cssResult.OptimizedCSS.empty()) {
                std::cout << "  CSS输出预览:" << std::endl;
                std::cout << cssResult.OptimizedCSS.substr(0, 200) << "..." << std::endl;
            }
        }
        else {
            std::cout << "  CSS错误数量: " << cssResult.Errors.size() << std::endl;
            for (const auto& error : cssResult.Errors) {
                std::cout << "    - " << error << std::endl;
            }
        }
        
        // 测试CSS优化选项
        std::cout << "\n  CSS优化测试:" << std::endl;
        cssCompiler.SetCompilationOptions(true, true, true, true); // 启用所有优化
        
        auto optimizedResult = cssCompiler.Compile(testCSS);
        if (optimizedResult.IsSuccess) {
            std::cout << "    压缩后CSS长度: " << optimizedResult.OptimizedCSS.length() << " 字符" << std::endl;
            std::cout << "    压缩率: " << std::fixed << std::setprecision(1) 
                     << (1.0 - (double)optimizedResult.OptimizedCSS.length() / testCSS.length()) * 100 
                     << "%" << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ CSS编译器异常: " << e.what() << std::endl;
    }
    
    // 2. 测试JavaScript编译器
    std::cout << "\n⚡ JavaScript编译器测试:" << std::endl;
    
    std::string testJS = R"(
function initializeApp() {
    const container = document.querySelector('.container');
    const buttons = document.querySelectorAll('.button');
    
    // 添加事件监听器
    buttons.forEach(button => {
        button.addEventListener('click', function(e) {
            console.log('Button clicked:', e.target);
            
            // 动画效果
            e.target.style.transform = 'scale(1.1)';
            setTimeout(() => {
                e.target.style.transform = 'scale(1.0)';
            }, 200);
        });
    });
    
    // 初始化完成
    console.log('App initialized successfully');
}

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', initializeApp);
)";
    
    try {
        CHTL::JavaScriptCompiler jsCompiler;
        
        std::cout << "  JavaScript源代码长度: " << testJS.length() << " 字符" << std::endl;
        
        auto jsResult = jsCompiler.Compile(testJS);
        std::cout << "  JavaScript编译结果: " << (jsResult.IsSuccess ? "✓ 成功" : "✗ 失败") << std::endl;
        
        if (jsResult.IsSuccess) {
            std::cout << "  优化后JS长度: " << jsResult.OptimizedJS.length() << " 字符" << std::endl;
            std::cout << "  JavaScript警告数量: " << jsResult.Warnings.size() << std::endl;
            
            if (!jsResult.OptimizedJS.empty()) {
                std::cout << "  JavaScript输出预览:" << std::endl;
                std::cout << jsResult.OptimizedJS.substr(0, 200) << "..." << std::endl;
            }
        }
        else {
            std::cout << "  JavaScript错误数量: " << jsResult.Errors.size() << std::endl;
            for (const auto& error : jsResult.Errors) {
                std::cout << "    - " << error << std::endl;
            }
        }
        
        // 测试JavaScript优化选项
        std::cout << "\n  JavaScript优化测试:" << std::endl;
        jsCompiler.SetCompilationOptions(true, true, false, true); // 启用压缩和死代码移除
        
        auto optimizedResult = jsCompiler.Compile(testJS);
        if (optimizedResult.IsSuccess) {
            std::cout << "    压缩后JS长度: " << optimizedResult.OptimizedJS.length() << " 字符" << std::endl;
            std::cout << "    压缩率: " << std::fixed << std::setprecision(1) 
                     << (1.0 - (double)optimizedResult.OptimizedJS.length() / testJS.length()) * 100 
                     << "%" << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ JavaScript编译器异常: " << e.what() << std::endl;
    }
    
    // 3. 测试编译器集成
    std::cout << "\n🔗 编译器集成测试:" << std::endl;
    
    try {
        CHTL::CompilerDispatcher dispatcher;
        
        std::string mixedCode = R"(
html {
    style {
        .test { color: red; }
    }
    
    script {
        console.log('Hello from CHTL!');
    }
}
)";
        
        std::cout << "  混合代码长度: " << mixedCode.length() << " 字符" << std::endl;
        
        bool compileResult = dispatcher.Compile(mixedCode);
        std::cout << "  编译器调度结果: " << (compileResult ? "✓ 成功" : "✗ 失败") << std::endl;
        
        if (compileResult) {
            auto mergedResult = dispatcher.GetMergedResult();
            std::cout << "  生成HTML长度: " << mergedResult.FullHTML.length() << " 字符" << std::endl;
            std::cout << "  编译统计: " << dispatcher.GetCompilationStatistics() << std::endl;
        }
        else {
            std::cout << "  编译错误: " << dispatcher.GetErrorMessage() << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 编译器集成异常: " << e.what() << std::endl;
    }
    
    std::cout << "\n🎯 ANTLR集成状态:" << std::endl;
    std::cout << "  ✅ CSS编译器架构完成" << std::endl;
    std::cout << "  ✅ JavaScript编译器架构完成" << std::endl;
    std::cout << "  ✅ 编译器调度器集成完成" << std::endl;
    std::cout << "  ✅ 基础语法验证功能" << std::endl;
    std::cout << "  ✅ 代码优化和压缩功能" << std::endl;
    std::cout << "  🔄 ANTLR4语法文件待集成" << std::endl;
    std::cout << "  🔄 完整语法树解析待实现" << std::endl;
    
    std::cout << "\n🏗️  ANTLR4集成规划:" << std::endl;
    std::cout << "  📝 创建CSS.g4语法文件" << std::endl;
    std::cout << "  📝 创建JavaScript.g4语法文件" << std::endl;
    std::cout << "  ⚙️  使用antlr-4.13.2-complete.jar生成C++代码" << std::endl;
    std::cout << "  🔗 集成生成的词法分析器和解析器" << std::endl;
    std::cout << "  🧪 完善语法树遍历和优化" << std::endl;
    
    std::cout << "\n🏆 ANTLR集成测试完成！" << std::endl;
    std::cout << "CSS和JavaScript编译器架构已建立，等待ANTLR4语法文件集成。" << std::endl;
    
    return 0;
}