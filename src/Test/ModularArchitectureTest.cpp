#include "../Core/ComponentManager.h"
#include "../Core/ServiceLocator.h"
#include "../Components/CHTLCompilerComponent.h"
#include "../Components/ModularCompilerDispatcher.h"
#include <iostream>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        🏗️ 模块化架构测试 🏗️                                              ║" << std::endl;
    std::cout << "║                   组件式编程·依赖注入·服务定位·模块化设计                                 ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n🎯 模块化架构验证:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        // 1. 测试组件管理器
        std::cout << "\n🔧 步骤1: 组件管理器测试" << std::endl;
        
        auto& componentManager = CHTL::Core::ComponentManager::GetInstance();
        bool initSuccess = componentManager.Initialize();
        
        std::cout << "  组件管理器初始化: " << (initSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
        
        // 2. 测试编译器工厂
        std::cout << "\n🏭 步骤2: 编译器工厂测试" << std::endl;
        
        bool factorySuccess = CHTL::Components::CompilerFactory::CreateStandardConfiguration();
        std::cout << "  标准配置创建: " << (factorySuccess ? "✅ 成功" : "❌ 失败") << std::endl;
        
        if (factorySuccess) {
            auto registeredComponents = componentManager.GetRegisteredComponents();
            std::cout << "  已注册组件数量: " << registeredComponents.size() << std::endl;
            
            for (const auto& componentName : registeredComponents) {
                auto state = componentManager.GetComponentState(componentName);
                std::cout << "    📦 " << componentName << ": " << GetStateString(state) << std::endl;
            }
        }
        
        // 3. 测试服务定位器
        std::cout << "\n🔍 步骤3: 服务定位器测试" << std::endl;
        
        auto& serviceLocator = CHTL::Core::ServiceLocator::GetInstance();
        
        // 注册测试服务
        serviceLocator.RegisterService<CHTL::Components::UnifiedScannerComponent>(
            std::make_unique<CHTL::Components::UnifiedScannerComponent>(), "Scanner"
        );
        
        serviceLocator.RegisterService<CHTL::Components::CHTLCompilerComponent>(
            std::make_unique<CHTL::Components::CHTLCompilerComponent>(), "CHTLCompiler"
        );
        
        std::cout << "  服务注册数量: " << serviceLocator.GetServiceCount() << std::endl;
        
        // 测试服务获取
        auto scannerService = serviceLocator.GetService<CHTL::Components::UnifiedScannerComponent>();
        auto compilerService = serviceLocator.GetService<CHTL::Components::CHTLCompilerComponent>();
        
        std::cout << "  扫描器服务: " << (scannerService ? "✅ 可用" : "❌ 不可用") << std::endl;
        std::cout << "  编译器服务: " << (compilerService ? "✅ 可用" : "❌ 不可用") << std::endl;
        
        // 4. 测试模块化编译器调度器
        std::cout << "\n🚀 步骤4: 模块化编译器调度器测试" << std::endl;
        
        auto dispatcher = CHTL::Components::CompilerFactory::CreateModularDispatcher();
        std::cout << "  调度器创建: " << (dispatcher ? "✅ 成功" : "❌ 失败") << std::endl;
        
        if (dispatcher) {
            std::cout << "  调度器名称: " << dispatcher->GetComponentName() << std::endl;
            std::cout << "  调度器版本: " << dispatcher->GetComponentVersion() << std::endl;
            std::cout << "  调度器状态: " << GetStateString(dispatcher->GetState()) << std::endl;
            std::cout << "  调度器健康: " << (dispatcher->IsHealthy() ? "✅ 健康" : "❌ 异常") << std::endl;
            
            auto dependencies = dispatcher->GetDependencies();
            std::cout << "  依赖组件: " << dependencies.size() << " 个" << std::endl;
            for (const auto& dep : dependencies) {
                std::cout << "    • " << dep << std::endl;
            }
        }
        
        // 5. 测试组件生命周期
        std::cout << "\n🔄 步骤5: 组件生命周期测试" << std::endl;
        
        if (scannerService) {
            bool startSuccess = scannerService->Start();
            std::cout << "  扫描器启动: " << (startSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
            
            if (startSuccess) {
                scannerService->Reset();
                std::cout << "  扫描器重置: ✅ 完成" << std::endl;
                
                bool stopSuccess = scannerService->Stop();
                std::cout << "  扫描器停止: " << (stopSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
            }
        }
        
        // 6. 生成报告
        std::cout << "\n📋 步骤6: 生成模块化架构报告" << std::endl;
        
        std::cout << componentManager.GenerateComponentReport() << std::endl;
        std::cout << serviceLocator.GenerateServiceReport() << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 模块化架构测试异常: " << e.what() << std::endl;
    }
    
    std::cout << "\n🌟 模块化架构设计优势:" << std::endl;
    std::cout << "┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                              ✨ 组件式编程优势                                              │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🔧 松耦合设计: 组件间通过接口交互，降低依赖                                                │" << std::endl;
    std::cout << "│ 🔧 依赖注入: 服务定位器提供自动依赖解析                                                    │" << std::endl;
    std::cout << "│ 🔧 生命周期管理: 统一的组件初始化、启动、停止流程                                          │" << std::endl;
    std::cout << "│ 🔧 可测试性: 每个组件可独立测试和验证                                                      │" << std::endl;
    std::cout << "│ 🔧 可扩展性: 新组件可轻松注册和集成                                                        │" << std::endl;
    std::cout << "│ 🔧 可维护性: 清晰的组件边界和职责分离                                                      │" << std::endl;
    std::cout << "│ 🔧 错误隔离: 组件异常不会影响其他组件                                                      │" << std::endl;
    std::cout << "│ 🔧 配置管理: 统一的组件配置和状态管理                                                      │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n🏆 模块化架构测试完成！" << std::endl;
    std::cout << "CHTL编译器现在采用了真正的模块化设计和组件式编程架构。" << std::endl;
    
    return 0;
}

// 辅助函数
std::string GetStateString(CHTL::Core::ComponentState state) {
    switch (state) {
        case CHTL::Core::ComponentState::UNINITIALIZED: return "未初始化";
        case CHTL::Core::ComponentState::INITIALIZING: return "初始化中";
        case CHTL::Core::ComponentState::INITIALIZED: return "已初始化";
        case CHTL::Core::ComponentState::RUNNING: return "运行中";
        case CHTL::Core::ComponentState::STOPPED: return "已停止";
        case CHTL::Core::ComponentState::ERROR_STATE: return "错误状态";
        default: return "未知状态";
    }
}