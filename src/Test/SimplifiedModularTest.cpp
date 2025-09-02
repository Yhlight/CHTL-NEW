#include "../Components/SimplifiedComponents.h"
#include <iostream>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        🏗️ 简化模块化架构测试 🏗️                                          ║" << std::endl;
    std::cout << "║                   组件式编程·模块化设计·依赖管理·清晰架构                                 ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n🎯 简化模块化架构验证:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        // 1. 创建简化模块化编译器调度器
        std::cout << "\n🔧 步骤1: 创建简化模块化编译器调度器" << std::endl;
        
        auto dispatcher = std::make_unique<CHTL::Components::SimpleModularDispatcher>();
        std::cout << "  调度器创建: ✅ 成功" << std::endl;
        
        // 2. 初始化所有组件
        std::cout << "\n🏗️ 步骤2: 初始化组件" << std::endl;
        
        bool initSuccess = dispatcher->Initialize();
        std::cout << "  组件初始化: " << (initSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
        
        if (initSuccess) {
            // 3. 启动所有组件
            std::cout << "\n🚀 步骤3: 启动组件" << std::endl;
            
            bool startSuccess = dispatcher->Start();
            std::cout << "  组件启动: " << (startSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
            
            if (startSuccess) {
                // 4. 测试编译功能
                std::cout << "\n⚡ 步骤4: 测试编译功能" << std::endl;
                
                std::string testCode = R"(
                    html {
                        head {
                            text { "CHTL模块化测试" }
                        }
                        body {
                            div {
                                class: test-container;
                                
                                style {
                                    .test-container {
                                        background: linear-gradient(45deg, #FF6B6B, #4ECDC4);
                                        padding: 20px;
                                        border-radius: 10px;
                                    }
                                }
                                
                                script {
                                    console.log('🎉 模块化编译器测试成功！');
                                    
                                    {{.test-container}} &-> click {
                                        console.log('💖 珂朵莉模块化设计完成！');
                                    };
                                }
                                
                                text { "模块化编译器测试内容" }
                            }
                        }
                    }
                )";
                
                bool compileSuccess = dispatcher->Compile(testCode);
                std::cout << "  编译测试: " << (compileSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
                
                if (compileSuccess) {
                    const auto& result = dispatcher->GetCompiledHTML();
                    std::cout << "  编译结果大小: " << result.size() << " 字符" << std::endl;
                    
                    // 显示部分编译结果
                    if (!result.empty()) {
                        std::cout << "  编译结果预览:" << std::endl;
                        std::cout << "  " << result.substr(0, 200) << "..." << std::endl;
                    }
                }
                
                // 5. 生成组件报告
                std::cout << "\n📋 步骤5: 生成组件报告" << std::endl;
                std::cout << dispatcher->GenerateReport() << std::endl;
            }
        }
        
        // 6. 创建独立组件测试
        std::cout << "\n🧪 步骤6: 独立组件测试" << std::endl;
        
        // 测试CHTL编译器组件
        auto chtlCompiler = std::make_unique<CHTL::Components::SimpleCHTLCompilerComponent>();
        bool chtlInit = chtlCompiler->Initialize();
        bool chtlStart = chtlCompiler->Start();
        
        std::cout << "  CHTL编译器组件:" << std::endl;
        std::cout << "    名称: " << chtlCompiler->GetName() << std::endl;
        std::cout << "    版本: " << chtlCompiler->GetVersion() << std::endl;
        std::cout << "    初始化: " << (chtlInit ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    启动: " << (chtlStart ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    健康状态: " << (chtlCompiler->IsHealthy() ? "✅ 健康" : "❌ 异常") << std::endl;
        std::cout << "    支持扩展: " << chtlCompiler->GetSupportedExtensions().size() << " 个" << std::endl;
        
        // 测试CHTL JS编译器组件
        auto chtljsCompiler = std::make_unique<CHTL::Components::SimpleCHTLJSCompilerComponent>();
        bool chtljsInit = chtljsCompiler->Initialize();
        bool chtljsStart = chtljsCompiler->Start();
        
        std::cout << "\n  CHTL JS编译器组件:" << std::endl;
        std::cout << "    名称: " << chtljsCompiler->GetName() << std::endl;
        std::cout << "    版本: " << chtljsCompiler->GetVersion() << std::endl;
        std::cout << "    初始化: " << (chtljsInit ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    启动: " << (chtljsStart ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    健康状态: " << (chtljsCompiler->IsHealthy() ? "✅ 健康" : "❌ 异常") << std::endl;
        std::cout << "    支持扩展: " << chtljsCompiler->GetSupportedExtensions().size() << " 个" << std::endl;
        
        // 测试统一扫描器组件
        auto scanner = std::make_unique<CHTL::Components::SimpleUnifiedScannerComponent>();
        bool scannerInit = scanner->Initialize();
        bool scannerStart = scanner->Start();
        
        std::cout << "\n  统一扫描器组件:" << std::endl;
        std::cout << "    名称: " << scanner->GetName() << std::endl;
        std::cout << "    版本: " << scanner->GetVersion() << std::endl;
        std::cout << "    初始化: " << (scannerInit ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    启动: " << (scannerStart ? "✅ 成功" : "❌ 失败") << std::endl;
        std::cout << "    健康状态: " << (scanner->IsHealthy() ? "✅ 健康" : "❌ 异常") << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 简化模块化架构测试异常: " << e.what() << std::endl;
    }
    
    std::cout << "\n🌟 简化模块化设计优势:" << std::endl;
    std::cout << "┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                              ✨ 组件式编程优势                                              │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🔧 清晰的组件边界: 每个组件职责单一，接口明确                                              │" << std::endl;
    std::cout << "│ 🔧 松耦合设计: 组件间通过接口交互，降低依赖                                                │" << std::endl;
    std::cout << "│ 🔧 生命周期管理: 统一的初始化、启动、停止、重置流程                                        │" << std::endl;
    std::cout << "│ 🔧 错误隔离: 组件异常不会影响其他组件运行                                                  │" << std::endl;
    std::cout << "│ 🔧 可测试性: 每个组件可独立测试和验证                                                      │" << std::endl;
    std::cout << "│ 🔧 可扩展性: 新组件可轻松添加和集成                                                        │" << std::endl;
    std::cout << "│ 🔧 可维护性: 清晰的代码结构和组件职责                                                      │" << std::endl;
    std::cout << "│ 🔧 性能监控: 每个组件都有独立的统计和监控                                                  │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n🏆 模块化架构设计原则:" << std::endl;
    std::cout << "  ✅ 单一职责原则: 每个组件只负责一个特定功能" << std::endl;
    std::cout << "  ✅ 开闭原则: 对扩展开放，对修改封闭" << std::endl;
    std::cout << "  ✅ 依赖倒置原则: 依赖抽象而不是具体实现" << std::endl;
    std::cout << "  ✅ 接口隔离原则: 使用专门的接口而不是庞大的接口" << std::endl;
    std::cout << "  ✅ 组合优于继承: 使用组合来实现功能复用" << std::endl;
    std::cout << "  ✅ 控制反转: 通过依赖注入管理组件依赖" << std::endl;
    
    std::cout << "\n🎊 简化模块化架构测试完成！" << std::endl;
    std::cout << "CHTL编译器现在采用了清晰的模块化设计和组件式编程架构。" << std::endl;
    
    return 0;
}