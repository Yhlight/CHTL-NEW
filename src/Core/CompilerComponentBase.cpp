#include "CompilerComponentBase.h"
#include <iostream>

namespace CHTL {
namespace Core {

// CompilerComponentBase实现
CompilerComponentBase::CompilerComponentBase(const std::string& name, const std::string& version)
    : ComponentBase(name, version), m_EnableOptimization(true), m_EnableMinification(false) {
    
    m_ErrorHandler = std::make_unique<CHTLErrorHandler>();
    
    // 初始化统计
    m_CompilationStats["total_compilations"] = 0;
    m_CompilationStats["total_input_size"] = 0;
    m_CompilationStats["total_output_size"] = 0;
    m_CompilationStats["total_compile_time"] = 0;
    m_CompilationStats["average_compression"] = 0;
}

bool CompilerComponentBase::Initialize() {
    SetState(ComponentState::INITIALIZING);
    
    try {
        // 初始化错误处理器
        if (!m_ErrorHandler) {
            m_ErrorHandler = std::make_unique<CHTLErrorHandler>();
        }
        
        // 设置默认配置
        SetConfigValue("optimization_enabled", m_EnableOptimization ? "true" : "false");
        SetConfigValue("minification_enabled", m_EnableMinification ? "true" : "false");
        
        SetState(ComponentState::INITIALIZED);
        std::cout << "✅ " << GetComponentName() << " 编译器组件已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        SetState(ComponentState::ERROR_STATE);
        std::cerr << "❌ " << GetComponentName() << " 编译器组件初始化失败: " << e.what() << std::endl;
        return false;
    }
}

bool CompilerComponentBase::Start() {
    if (GetState() != ComponentState::INITIALIZED) {
        return false;
    }
    
    SetState(ComponentState::RUNNING);
    std::cout << "🚀 " << GetComponentName() << " 编译器组件已启动" << std::endl;
    return true;
}

bool CompilerComponentBase::Stop() {
    SetState(ComponentState::STOPPED);
    std::cout << "⏹️ " << GetComponentName() << " 编译器组件已停止" << std::endl;
    return true;
}

void CompilerComponentBase::Reset() {
    if (m_ErrorHandler) {
        m_ErrorHandler->Clear();
    }
    
    m_CompilationStats.clear();
    m_CompilationStats["total_compilations"] = 0;
    m_CompilationStats["total_input_size"] = 0;
    m_CompilationStats["total_output_size"] = 0;
    m_CompilationStats["total_compile_time"] = 0;
    m_CompilationStats["average_compression"] = 0;
    
    std::cout << "🔄 " << GetComponentName() << " 编译器组件已重置" << std::endl;
}

// ScannerComponentBase实现
ScannerComponentBase::ScannerComponentBase(const std::string& name, const std::string& version)
    : ComponentBase(name, version) {
    
    // 初始化统计
    m_ScanStats["total_scans"] = 0;
    m_ScanStats["total_fragments"] = 0;
    m_ScanStats["total_scan_time"] = 0;
}

bool ScannerComponentBase::Initialize() {
    SetState(ComponentState::INITIALIZING);
    
    try {
        SetState(ComponentState::INITIALIZED);
        std::cout << "✅ " << GetComponentName() << " 扫描器组件已初始化" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        SetState(ComponentState::ERROR_STATE);
        std::cerr << "❌ " << GetComponentName() << " 扫描器组件初始化失败: " << e.what() << std::endl;
        return false;
    }
}

bool ScannerComponentBase::Start() {
    if (GetState() != ComponentState::INITIALIZED) {
        return false;
    }
    
    SetState(ComponentState::RUNNING);
    std::cout << "🚀 " << GetComponentName() << " 扫描器组件已启动" << std::endl;
    return true;
}

bool ScannerComponentBase::Stop() {
    SetState(ComponentState::STOPPED);
    std::cout << "⏹️ " << GetComponentName() << " 扫描器组件已停止" << std::endl;
    return true;
}

void ScannerComponentBase::Reset() {
    m_ScanResults.clear();
    m_ScanStats.clear();
    m_ScanStats["total_scans"] = 0;
    m_ScanStats["total_fragments"] = 0;
    m_ScanStats["total_scan_time"] = 0;
    
    std::cout << "🔄 " << GetComponentName() << " 扫描器组件已重置" << std::endl;
}

bool ScannerComponentBase::Scan(const std::string& source) {
    if (GetState() != ComponentState::RUNNING) {
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        bool result = DoScan(source);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 更新统计
        m_ScanStats["total_scans"]++;
        m_ScanStats["total_scan_time"] += duration.count();
        m_ScanStats["total_fragments"] = m_ScanResults.size();
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "扫描异常: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Core
} // namespace CHTL