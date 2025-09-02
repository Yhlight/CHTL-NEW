#pragma once

#include "ComponentManager.h"
#include "../Error/ErrorHandler.h"
#include <chrono>

namespace CHTL {
namespace Core {

/**
 * 编译结果基类
 */
struct CompilationResultBase {
    std::string Output;                         // 输出内容
    std::vector<std::string> Warnings;         // 警告信息
    std::vector<std::string> Errors;           // 错误信息
    bool IsSuccess;                             // 是否成功
    std::chrono::milliseconds CompileTime;      // 编译时间
    size_t InputSize;                           // 输入大小
    size_t OutputSize;                          // 输出大小
    
    CompilationResultBase() : IsSuccess(false), CompileTime(0), InputSize(0), OutputSize(0) {}
    
    /**
     * 获取压缩率
     * @return 压缩率百分比
     */
    double GetCompressionRatio() const {
        if (InputSize == 0) return 0.0;
        return (1.0 - static_cast<double>(OutputSize) / InputSize) * 100.0;
    }
    
    /**
     * 获取编译速度
     * @return 字符/秒
     */
    double GetCompileSpeed() const {
        if (CompileTime.count() == 0) return 0.0;
        return (static_cast<double>(InputSize) / CompileTime.count()) * 1000.0;
    }
};

/**
 * 编译器组件基类
 * 为所有编译器提供统一的基础实现
 */
class CompilerComponentBase : public virtual ComponentBase, public virtual ICompilerComponent {
protected:
    std::unique_ptr<CHTLErrorHandler> m_ErrorHandler;
    std::unordered_map<std::string, size_t> m_CompilationStats;
    std::vector<std::string> m_SupportedExtensions;
    bool m_EnableOptimization;
    bool m_EnableMinification;

public:
    /**
     * 构造函数
     */
    CompilerComponentBase(const std::string& name, const std::string& version);
    
    /**
     * 析构函数
     */
    virtual ~CompilerComponentBase() = default;
    
    // IComponent接口实现
    bool Initialize() override;
    bool Start() override;
    bool Stop() override;
    void Reset() override;
    
    // ICompilerComponent接口实现
    std::vector<std::string> GetSupportedExtensions() const override {
        return m_SupportedExtensions;
    }
    
    std::unordered_map<std::string, size_t> GetCompilationStats() const override {
        return m_CompilationStats;
    }
    
    /**
     * 编译接口（模板方法）
     * @param input 输入内容
     * @return 编译结果
     */
    std::string Compile(const std::string& input) override final {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // 验证输入
            if (!ValidateInput(input)) {
                return "";
            }
            
            // 执行编译
            std::string result = DoCompile(input);
            
            // 后处理
            if (m_EnableOptimization) {
                result = DoOptimize(result);
            }
            
            if (m_EnableMinification) {
                result = DoMinify(result);
            }
            
            // 更新统计
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            UpdateCompilationStats(input.size(), result.size(), duration);
            
            return result;
            
        } catch (const std::exception& e) {
            if (m_ErrorHandler) {
                m_ErrorHandler->AddError(ErrorLevel::ERROR, ErrorType::GENERATION_ERROR, 
                                       "编译异常: " + std::string(e.what()));
            }
            return "";
        }
    }

protected:
    /**
     * 执行实际编译（子类实现）
     * @param input 输入内容
     * @return 编译结果
     */
    virtual std::string DoCompile(const std::string& input) = 0;
    
    /**
     * 执行优化（子类可重写）
     * @param code 原始代码
     * @return 优化后的代码
     */
    virtual std::string DoOptimize(const std::string& code) {
        return code; // 默认不优化
    }
    
    /**
     * 执行压缩（子类可重写）
     * @param code 原始代码
     * @return 压缩后的代码
     */
    virtual std::string DoMinify(const std::string& code) {
        return code; // 默认不压缩
    }
    
    /**
     * 添加支持的扩展名
     * @param extension 扩展名
     */
    void AddSupportedExtension(const std::string& extension) {
        m_SupportedExtensions.push_back(extension);
    }
    
    /**
     * 更新编译统计
     * @param inputSize 输入大小
     * @param outputSize 输出大小
     * @param duration 编译时间
     */
    void UpdateCompilationStats(size_t inputSize, size_t outputSize, 
                               std::chrono::milliseconds duration) {
        m_CompilationStats["total_compilations"]++;
        m_CompilationStats["total_input_size"] += inputSize;
        m_CompilationStats["total_output_size"] += outputSize;
        m_CompilationStats["total_compile_time"] += duration.count();
        
        if (inputSize > 0) {
            double compressionRatio = (1.0 - static_cast<double>(outputSize) / inputSize) * 100.0;
            m_CompilationStats["average_compression"] = 
                (m_CompilationStats["average_compression"] + static_cast<size_t>(compressionRatio)) / 2;
        }
    }
    
    /**
     * 获取错误处理器
     * @return 错误处理器指针
     */
    CHTLErrorHandler* GetErrorHandler() const {
        return m_ErrorHandler.get();
    }
};

/**
 * 扫描器组件基类
 */
class ScannerComponentBase : public virtual ComponentBase, public virtual IScannerComponent {
protected:
    std::vector<std::string> m_ScanResults;
    std::unordered_map<std::string, size_t> m_ScanStats;

public:
    /**
     * 构造函数
     */
    ScannerComponentBase(const std::string& name, const std::string& version);
    
    /**
     * 析构函数
     */
    virtual ~ScannerComponentBase() = default;
    
    // IComponent接口实现
    bool Initialize() override;
    bool Start() override;
    bool Stop() override;
    void Reset() override;
    
    // IScannerComponent接口实现
    std::vector<std::string> GetScanResults() const override {
        return m_ScanResults;
    }
    
    std::unordered_map<std::string, size_t> GetScanStats() const override {
        return m_ScanStats;
    }

protected:
    /**
     * 执行实际扫描（子类实现）
     * @param source 源代码
     * @return 是否成功
     */
    virtual bool DoScan(const std::string& source) = 0;
    
    /**
     * 添加扫描结果
     * @param result 扫描结果
     */
    void AddScanResult(const std::string& result) {
        m_ScanResults.push_back(result);
    }
    
    /**
     * 更新扫描统计
     * @param key 统计键
     * @param value 统计值
     */
    void UpdateScanStats(const std::string& key, size_t value) {
        m_ScanStats[key] = value;
    }
};

} // namespace Core
} // namespace CHTL