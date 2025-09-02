#pragma once

#include "ComponentInterface.h"
#include "../Error/ErrorHandler.h"
#include <memory>
#include <chrono>

namespace CHTL {
namespace Core {

/**
 * 简化的编译器组件
 * 使用组合而不是多重继承
 */
class ModularCompilerComponent {
private:
    std::string m_Name;
    std::string m_Version;
    ComponentState m_State;
    std::unique_ptr<CHTLErrorHandler> m_ErrorHandler;
    std::unordered_map<std::string, size_t> m_Stats;
    std::vector<std::string> m_SupportedExtensions;

public:
    /**
     * 构造函数
     */
    ModularCompilerComponent(const std::string& name, const std::string& version)
        : m_Name(name), m_Version(version), m_State(ComponentState::UNINITIALIZED) {
        m_ErrorHandler = std::make_unique<CHTLErrorHandler>();
    }
    
    /**
     * 虚析构函数
     */
    virtual ~ModularCompilerComponent() = default;
    
    /**
     * 初始化组件
     */
    virtual bool Initialize() {
        m_State = ComponentState::INITIALIZING;
        bool success = DoInitialize();
        m_State = success ? ComponentState::INITIALIZED : ComponentState::ERROR_STATE;
        return success;
    }
    
    /**
     * 启动组件
     */
    virtual bool Start() {
        if (m_State != ComponentState::INITIALIZED) return false;
        m_State = ComponentState::RUNNING;
        return true;
    }
    
    /**
     * 停止组件
     */
    virtual bool Stop() {
        m_State = ComponentState::STOPPED;
        return true;
    }
    
    /**
     * 重置组件
     */
    virtual void Reset() {
        m_Stats.clear();
        if (m_ErrorHandler) {
            m_ErrorHandler->Clear();
        }
        DoReset();
    }
    
    /**
     * 编译接口
     */
    virtual std::string Compile(const std::string& input) {
        if (m_State != ComponentState::RUNNING) {
            return "";
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        std::string result = DoCompile(input);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        UpdateStats(input.size(), result.size(), duration.count());
        
        return result;
    }
    
    // Getter方法
    const std::string& GetName() const { return m_Name; }
    const std::string& GetVersion() const { return m_Version; }
    ComponentState GetState() const { return m_State; }
    const std::vector<std::string>& GetSupportedExtensions() const { return m_SupportedExtensions; }
    const std::unordered_map<std::string, size_t>& GetStats() const { return m_Stats; }
    CHTLErrorHandler* GetErrorHandler() const { return m_ErrorHandler.get(); }
    
    /**
     * 检查是否健康
     */
    bool IsHealthy() const {
        return m_State == ComponentState::RUNNING || m_State == ComponentState::INITIALIZED;
    }

protected:
    /**
     * 子类实现的初始化
     */
    virtual bool DoInitialize() = 0;
    
    /**
     * 子类实现的重置
     */
    virtual void DoReset() = 0;
    
    /**
     * 子类实现的编译
     */
    virtual std::string DoCompile(const std::string& input) = 0;
    
    /**
     * 添加支持的扩展名
     */
    void AddSupportedExtension(const std::string& ext) {
        m_SupportedExtensions.push_back(ext);
    }
    
    /**
     * 更新统计信息
     */
    void UpdateStats(size_t inputSize, size_t outputSize, size_t timeMs) {
        m_Stats["total_compilations"]++;
        m_Stats["total_input_size"] += inputSize;
        m_Stats["total_output_size"] += outputSize;
        m_Stats["total_time_ms"] += timeMs;
    }
};

/**
 * 简化的扫描器组件
 */
class ModularScannerComponent {
private:
    std::string m_Name;
    std::string m_Version;
    ComponentState m_State;
    std::vector<std::string> m_Results;
    std::unordered_map<std::string, size_t> m_Stats;

public:
    /**
     * 构造函数
     */
    ModularScannerComponent(const std::string& name, const std::string& version)
        : m_Name(name), m_Version(version), m_State(ComponentState::UNINITIALIZED) {
    }
    
    /**
     * 虚析构函数
     */
    virtual ~ModularScannerComponent() = default;
    
    /**
     * 初始化组件
     */
    virtual bool Initialize() {
        m_State = ComponentState::INITIALIZING;
        bool success = DoInitialize();
        m_State = success ? ComponentState::INITIALIZED : ComponentState::ERROR_STATE;
        return success;
    }
    
    /**
     * 启动组件
     */
    virtual bool Start() {
        if (m_State != ComponentState::INITIALIZED) return false;
        m_State = ComponentState::RUNNING;
        return true;
    }
    
    /**
     * 停止组件
     */
    virtual bool Stop() {
        m_State = ComponentState::STOPPED;
        return true;
    }
    
    /**
     * 重置组件
     */
    virtual void Reset() {
        m_Results.clear();
        m_Stats.clear();
        DoReset();
    }
    
    /**
     * 扫描接口
     */
    virtual bool Scan(const std::string& source) {
        if (m_State != ComponentState::RUNNING) {
            return false;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = DoScan(source);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        m_Stats["total_scan_time_ms"] += duration.count();
        m_Stats["total_scans"]++;
        
        return result;
    }
    
    // Getter方法
    const std::string& GetName() const { return m_Name; }
    const std::string& GetVersion() const { return m_Version; }
    ComponentState GetState() const { return m_State; }
    const std::vector<std::string>& GetResults() const { return m_Results; }
    const std::unordered_map<std::string, size_t>& GetStats() const { return m_Stats; }
    
    /**
     * 检查是否健康
     */
    bool IsHealthy() const {
        return m_State == ComponentState::RUNNING || m_State == ComponentState::INITIALIZED;
    }

protected:
    /**
     * 子类实现的初始化
     */
    virtual bool DoInitialize() = 0;
    
    /**
     * 子类实现的重置
     */
    virtual void DoReset() = 0;
    
    /**
     * 子类实现的扫描
     */
    virtual bool DoScan(const std::string& source) = 0;
    
    /**
     * 添加扫描结果
     */
    void AddResult(const std::string& result) {
        m_Results.push_back(result);
    }
};

} // namespace Core
} // namespace CHTL