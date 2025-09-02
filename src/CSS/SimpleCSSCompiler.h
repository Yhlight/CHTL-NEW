#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * 简化CSS编译结果
 */
struct SimpleCSSCompilationResult {
    std::string OptimizedCSS;              // 优化后的CSS
    std::string SourceMap;                 // 源映射
    std::vector<std::string> Warnings;    // 警告信息
    std::vector<std::string> Errors;      // 错误信息
    bool IsSuccess;                        // 是否成功
    
    SimpleCSSCompilationResult() : IsSuccess(false) {}
};

/**
 * 简化CSS编译器
 * 基于现有ANTLR库的轻量级CSS处理器
 */
class SimpleCSSCompiler {
private:
    std::vector<std::string> m_Warnings;
    std::vector<std::string> m_Errors;
    bool m_EnableOptimization;
    bool m_EnableMinification;

public:
    /**
     * 构造函数
     */
    SimpleCSSCompiler();
    
    /**
     * 析构函数
     */
    ~SimpleCSSCompiler() = default;
    
    /**
     * 编译CSS片段
     * @param cssFragments CSS片段列表
     * @return 编译结果
     */
    SimpleCSSCompilationResult CompileFragments(const std::vector<std::string>& cssFragments);
    
    /**
     * 编译单个CSS字符串
     * @param cssContent CSS内容
     * @return 编译结果
     */
    SimpleCSSCompilationResult Compile(const std::string& cssContent);
    
    /**
     * 重置编译器状态
     */
    void Reset();
    
    /**
     * 设置优化选项
     * @param enable 是否启用优化
     */
    void SetOptimization(bool enable) { m_EnableOptimization = enable; }
    
    /**
     * 设置压缩选项
     * @param enable 是否启用压缩
     */
    void SetMinification(bool enable) { m_EnableMinification = enable; }

private:
    /**
     * 优化CSS代码
     * @param css 原始CSS
     * @return 优化后的CSS
     */
    std::string OptimizeCSS(const std::string& css);
    
    /**
     * 压缩CSS代码
     * @param css 原始CSS
     * @return 压缩后的CSS
     */
    std::string MinifyCSS(const std::string& css);
    
    /**
     * 验证CSS语法
     * @param css CSS内容
     * @return 是否有效
     */
    bool ValidateCSS(const std::string& css);
    
    /**
     * 添加警告
     * @param warning 警告信息
     */
    void AddWarning(const std::string& warning);
    
    /**
     * 添加错误
     * @param error 错误信息
     */
    void AddError(const std::string& error);
};

} // namespace CHTL