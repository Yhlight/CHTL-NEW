#include "ModularCompilerDispatcher.h"
#include "../Core/ServiceLocator.h"
#include <iostream>
#include <sstream>

namespace CHTL {
namespace Components {

// ModularCompilerDispatcher实现
ModularCompilerDispatcher::ModularCompilerDispatcher() 
    : Core::ComponentBase("ModularCompilerDispatcher", "1.0.0"),
      m_ScannerComponent(nullptr), m_CHTLCompilerComponent(nullptr), m_CHTLJSCompilerComponent(nullptr) {
    
    AddDependency("UnifiedScanner");
    AddDependency("CHTLCompiler");
    AddDependency("CHTLJSCompiler");
    AddDependency("CodeMerger");
}

bool ModularCompilerDispatcher::Initialize() {
    SetState(Core::ComponentState::INITIALIZING);
    
    try {
        // 初始化代码合并器
        m_CodeMerger = std::make_unique<CHTLCodeMerger>();
        
        // 初始化组件引用
        if (!InitializeComponentReferences()) {
            SetState(Core::ComponentState::ERROR_STATE);
            return false;
        }
        
        SetState(Core::ComponentState::INITIALIZED);
        std::cout << "✅ 模块化编译器调度器已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        SetState(Core::ComponentState::ERROR_STATE);
        std::cerr << "❌ 模块化编译器调度器初始化失败: " << e.what() << std::endl;
        return false;
    }
}

bool ModularCompilerDispatcher::Start() {
    if (GetState() != Core::ComponentState::INITIALIZED) {
        return false;
    }
    
    if (!CheckComponentDependencies()) {
        std::cerr << "❌ 组件依赖检查失败" << std::endl;
        return false;
    }
    
    SetState(Core::ComponentState::RUNNING);
    std::cout << "🚀 模块化编译器调度器已启动" << std::endl;
    return true;
}

bool ModularCompilerDispatcher::Stop() {
    SetState(Core::ComponentState::STOPPED);
    std::cout << "⏹️ 模块化编译器调度器已停止" << std::endl;
    return true;
}

void ModularCompilerDispatcher::Reset() {
    m_Fragments.clear();
    m_FragmentsByType.clear();
    m_CurrentSourceFile.clear();
    
    if (m_CodeMerger) {
        m_CodeMerger->Reset();
    }
    
    std::cout << "🔄 模块化编译器调度器已重置" << std::endl;
}

bool ModularCompilerDispatcher::Compile(const std::string& sourceCode, const std::string& sourceFile) {
    if (GetState() != Core::ComponentState::RUNNING) {
        std::cerr << "❌ 调度器未运行" << std::endl;
        return false;
    }
    
    m_CurrentSourceFile = sourceFile;
    
    try {
        // 步骤1: 执行代码扫描
        if (!PerformScanning(sourceCode)) {
            std::cerr << "❌ 代码扫描失败" << std::endl;
            return false;
        }
        
        // 步骤2: 按类型分组片段
        GroupFragmentsByType();
        
        // 步骤3: 编译各类型片段
        std::string chtlResult = CompileCHTLFragments(m_FragmentsByType[FragmentType::CHTL_FRAGMENT]);
        std::string chtljsResult = CompileCHTLJSFragments(m_FragmentsByType[FragmentType::CHTL_JS_FRAGMENT]);
        
        // 步骤4: 合并编译结果
        std::string finalHTML = MergeCompilationResults(chtlResult, chtljsResult);
        
        std::cout << "✅ 编译完成: " << finalHTML.size() << " 字符" << std::endl;
        return !finalHTML.empty();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 编译异常: " << e.what() << std::endl;
        return false;
    }
}

bool ModularCompilerDispatcher::InitializeComponentReferences() {
    auto& serviceLocator = Core::ServiceLocator::GetInstance();
    
    // 获取扫描器组件
    m_ScannerComponent = serviceLocator.GetService<UnifiedScannerComponent>();
    if (!m_ScannerComponent) {
        std::cerr << "❌ 无法获取统一扫描器组件" << std::endl;
        return false;
    }
    
    // 获取CHTL编译器组件
    m_CHTLCompilerComponent = serviceLocator.GetService<CHTLCompilerComponent>();
    if (!m_CHTLCompilerComponent) {
        std::cerr << "❌ 无法获取CHTL编译器组件" << std::endl;
        return false;
    }
    
    // 获取CHTL JS编译器组件
    m_CHTLJSCompilerComponent = serviceLocator.GetService<CHTLJSCompilerComponent>();
    if (!m_CHTLJSCompilerComponent) {
        std::cerr << "❌ 无法获取CHTL JS编译器组件" << std::endl;
        return false;
    }
    
    std::cout << "✅ 组件引用已初始化" << std::endl;
    return true;
}

bool ModularCompilerDispatcher::PerformScanning(const std::string& sourceCode) {
    if (!m_ScannerComponent) {
        return false;
    }
    
    bool scanResult = m_ScannerComponent->Scan(sourceCode);
    
    if (scanResult) {
        m_Fragments = m_ScannerComponent->GetFragments();
        std::cout << "📄 扫描完成: " << m_Fragments.size() << " 个片段" << std::endl;
    }
    
    return scanResult;
}

void ModularCompilerDispatcher::GroupFragmentsByType() {
    m_FragmentsByType.clear();
    
    for (const auto& fragment : m_Fragments) {
        m_FragmentsByType[fragment.Type].push_back(fragment);
    }
    
    std::cout << "📊 片段分组完成:" << std::endl;
    for (const auto& pair : m_FragmentsByType) {
        std::cout << "  " << static_cast<int>(pair.first) << " 类型: " << pair.second.size() << " 个片段" << std::endl;
    }
}

std::string ModularCompilerDispatcher::CompileCHTLFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_CHTLCompilerComponent || fragments.empty()) {
        return "";
    }
    
    std::ostringstream result;
    
    for (const auto& fragment : fragments) {
        std::string compiled = m_CHTLCompilerComponent->Compile(fragment.Content);
        if (!compiled.empty()) {
            result << compiled << "\n";
        }
    }
    
    return result.str();
}

std::string ModularCompilerDispatcher::CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    if (!m_CHTLJSCompilerComponent || fragments.empty()) {
        return "";
    }
    
    std::ostringstream result;
    
    for (const auto& fragment : fragments) {
        std::string compiled = m_CHTLJSCompilerComponent->Compile(fragment.Content);
        if (!compiled.empty()) {
            result << compiled << "\n";
        }
    }
    
    return result.str();
}

std::string ModularCompilerDispatcher::MergeCompilationResults(const std::string& chtlResult, const std::string& chtljsResult) {
    if (!m_CodeMerger) {
        return chtlResult + chtljsResult;
    }
    
    try {
        // 使用代码合并器合并结果
        std::vector<MergerCodeFragment> fragments;
        
        if (!chtlResult.empty()) {
            MergerCodeFragment chtlFragment;
            chtlFragment.Content = chtlResult;
            chtlFragment.Type = MergerFragmentType::HTML;
            fragments.push_back(chtlFragment);
        }
        
        if (!chtljsResult.empty()) {
            MergerCodeFragment jsFragment;
            jsFragment.Content = chtljsResult;
            jsFragment.Type = MergerFragmentType::JAVASCRIPT;
            fragments.push_back(jsFragment);
        }
        
        return m_CodeMerger->MergeFragments(fragments);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 代码合并异常: " << e.what() << std::endl;
        return chtlResult + chtljsResult;
    }
}

bool ModularCompilerDispatcher::CheckComponentDependencies() {
    return m_ScannerComponent && m_CHTLCompilerComponent && m_CHTLJSCompilerComponent;
}

std::string ModularCompilerDispatcher::GetCompiledHTML() const {
    // TODO: 实现获取最终HTML结果
    return "";
}

std::vector<std::string> ModularCompilerDispatcher::GetCompilationErrors() const {
    std::vector<std::string> allErrors;
    
    // 收集各组件的错误
    if (m_CHTLCompilerComponent) {
        auto chtlErrors = m_CHTLCompilerComponent->GetErrorHandler();
        if (chtlErrors) {
            auto errors = chtlErrors->GetErrorsByLevel(ErrorLevel::ERROR);
            for (const auto& error : errors) {
                allErrors.push_back(error.GetMessage());
            }
        }
    }
    
    if (m_CHTLJSCompilerComponent) {
        auto chtljsErrors = m_CHTLJSCompilerComponent->GetErrorHandler();
        if (chtljsErrors) {
            auto errors = chtljsErrors->GetErrorsByLevel(ErrorLevel::ERROR);
            for (const auto& error : errors) {
                allErrors.push_back(error.GetMessage());
            }
        }
    }
    
    return allErrors;
}

std::vector<std::string> ModularCompilerDispatcher::GetCompilationWarnings() const {
    std::vector<std::string> allWarnings;
    
    // 收集各组件的警告
    if (m_CHTLCompilerComponent) {
        auto chtlErrors = m_CHTLCompilerComponent->GetErrorHandler();
        if (chtlErrors) {
            auto warnings = chtlErrors->GetErrorsByLevel(ErrorLevel::WARNING);
            for (const auto& warning : warnings) {
                allWarnings.push_back(warning.GetMessage());
            }
        }
    }
    
    if (m_CHTLJSCompilerComponent) {
        auto chtljsErrors = m_CHTLJSCompilerComponent->GetErrorHandler();
        if (chtljsErrors) {
            auto warnings = chtljsErrors->GetErrorsByLevel(ErrorLevel::WARNING);
            for (const auto& warning : warnings) {
                allWarnings.push_back(warning.GetMessage());
            }
        }
    }
    
    return allWarnings;
}

std::unordered_map<std::string, size_t> ModularCompilerDispatcher::GetCompilationStatistics() const {
    std::unordered_map<std::string, size_t> stats;
    
    stats["total_fragments"] = m_Fragments.size();
    stats["chtl_fragments"] = m_FragmentsByType.count(FragmentType::CHTL_FRAGMENT) ? 
                             m_FragmentsByType.at(FragmentType::CHTL_FRAGMENT).size() : 0;
    stats["chtljs_fragments"] = m_FragmentsByType.count(FragmentType::CHTL_JS_FRAGMENT) ?
                               m_FragmentsByType.at(FragmentType::CHTL_JS_FRAGMENT).size() : 0;
    
    // 合并各组件统计
    if (m_CHTLCompilerComponent) {
        auto chtlStats = m_CHTLCompilerComponent->GetCompilationStats();
        for (const auto& pair : chtlStats) {
            stats["chtl_" + pair.first] = pair.second;
        }
    }
    
    if (m_CHTLJSCompilerComponent) {
        auto chtljsStats = m_CHTLJSCompilerComponent->GetCompilationStats();
        for (const auto& pair : chtljsStats) {
            stats["chtljs_" + pair.first] = pair.second;
        }
    }
    
    return stats;
}

// CompilerFactory实现
bool CompilerFactory::CreateStandardConfiguration() {
    try {
        // 注册所有编译器组件
        size_t registeredCount = RegisterAllCompilerComponents();
        
        std::cout << "📦 已注册 " << registeredCount << " 个编译器组件" << std::endl;
        
        // 启动组件管理器
        auto& componentManager = Core::ComponentManager::GetInstance();
        if (!componentManager.Initialize()) {
            return false;
        }
        
        // 启动所有组件
        size_t startedCount = componentManager.StartAllComponents();
        std::cout << "🚀 已启动 " << startedCount << " 个组件" << std::endl;
        
        return ValidateCompilerConfiguration();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 标准配置创建失败: " << e.what() << std::endl;
        return false;
    }
}

size_t CompilerFactory::RegisterAllCompilerComponents() {
    RegisterCoreComponents();
    RegisterCompilerComponents();
    RegisterUtilityComponents();
    
    // 返回注册的组件数量
    return Core::ComponentManager::GetInstance().GetRegisteredComponents().size();
}

std::unique_ptr<ModularCompilerDispatcher> CompilerFactory::CreateModularDispatcher() {
    auto dispatcher = std::make_unique<ModularCompilerDispatcher>();
    
    if (dispatcher->Initialize() && dispatcher->Start()) {
        std::cout << "✅ 模块化编译器调度器创建成功" << std::endl;
        return dispatcher;
    }
    
    std::cerr << "❌ 模块化编译器调度器创建失败" << std::endl;
    return nullptr;
}

bool CompilerFactory::ValidateCompilerConfiguration() {
    auto& componentManager = Core::ComponentManager::GetInstance();
    
    // 检查关键组件是否存在
    std::vector<std::string> requiredComponents = {
        "UnifiedScanner", "CHTLCompiler", "CHTLJSCompiler"
    };
    
    for (const auto& componentName : requiredComponents) {
        if (!componentManager.ComponentExists(componentName)) {
            std::cerr << "❌ 缺少必需组件: " << componentName << std::endl;
            return false;
        }
        
        auto state = componentManager.GetComponentState(componentName);
        if (state != Core::ComponentState::RUNNING && state != Core::ComponentState::INITIALIZED) {
            std::cerr << "❌ 组件状态异常: " << componentName << std::endl;
            return false;
        }
    }
    
    std::cout << "✅ 编译器配置验证通过" << std::endl;
    return true;
}

void CompilerFactory::RegisterCoreComponents() {
    auto& componentManager = Core::ComponentManager::GetInstance();
    
    // 注册统一扫描器组件
    componentManager.RegisterComponent<UnifiedScannerComponent>(
        "UnifiedScanner", "1.0.0", "统一代码扫描器组件"
    );
}

void CompilerFactory::RegisterCompilerComponents() {
    auto& componentManager = Core::ComponentManager::GetInstance();
    
    // 注册CHTL编译器组件
    componentManager.RegisterComponent<CHTLCompilerComponent>(
        "CHTLCompiler", "1.0.0", "CHTL语言编译器组件"
    );
    
    // 注册CHTL JS编译器组件
    componentManager.RegisterComponent<CHTLJSCompilerComponent>(
        "CHTLJSCompiler", "1.0.0", "CHTL JS语言编译器组件"
    );
}

void CompilerFactory::RegisterUtilityComponents() {
    auto& componentManager = Core::ComponentManager::GetInstance();
    
    // 注册模块化编译器调度器
    componentManager.RegisterComponent<ModularCompilerDispatcher>(
        "ModularDispatcher", "1.0.0", "模块化编译器调度器"
    );
}

} // namespace Components
} // namespace CHTL