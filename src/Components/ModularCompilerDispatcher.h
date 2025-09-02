#pragma once

#include "../Core/ComponentManager.h"
#include "CHTLCompilerComponent.h"
#include "../CodeMerger/CHTLCodeMerger.h"
#include <memory>

namespace CHTL {
namespace Components {

/**
 * 模块化编译器调度器
 * 基于组件化设计的编译器调度器
 */
class ModularCompilerDispatcher : public Core::ComponentBase {
private:
    // 组件引用
    UnifiedScannerComponent* m_ScannerComponent;
    CHTLCompilerComponent* m_CHTLCompilerComponent;
    CHTLJSCompilerComponent* m_CHTLJSCompilerComponent;
    
    // 代码合并器
    std::unique_ptr<CHTLCodeMerger> m_CodeMerger;
    
    // 编译状态
    std::string m_CurrentSourceFile;
    std::vector<CodeFragment> m_Fragments;
    std::unordered_map<FragmentType, std::vector<CodeFragment>> m_FragmentsByType;

public:
    /**
     * 构造函数
     */
    ModularCompilerDispatcher();
    
    /**
     * 析构函数
     */
    ~ModularCompilerDispatcher() override = default;
    
    // IComponent接口实现
    bool Initialize() override;
    bool Start() override;
    bool Stop() override;
    void Reset() override;
    
    /**
     * 编译源代码
     * @param sourceCode 源代码
     * @param sourceFile 源文件路径
     * @return 是否成功
     */
    bool Compile(const std::string& sourceCode, const std::string& sourceFile = "");
    
    /**
     * 获取编译结果
     * @return HTML输出
     */
    std::string GetCompiledHTML() const;
    
    /**
     * 获取编译错误
     * @return 错误列表
     */
    std::vector<std::string> GetCompilationErrors() const;
    
    /**
     * 获取编译警告
     * @return 警告列表
     */
    std::vector<std::string> GetCompilationWarnings() const;
    
    /**
     * 获取编译统计
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetCompilationStatistics() const;

private:
    /**
     * 初始化组件引用
     * @return 是否成功
     */
    bool InitializeComponentReferences();
    
    /**
     * 执行代码扫描
     * @param sourceCode 源代码
     * @return 是否成功
     */
    bool PerformScanning(const std::string& sourceCode);
    
    /**
     * 按类型分组片段
     */
    void GroupFragmentsByType();
    
    /**
     * 编译CHTL片段
     * @param fragments CHTL片段列表
     * @return 编译结果
     */
    std::string CompileCHTLFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 编译CHTL JS片段
     * @param fragments CHTL JS片段列表
     * @return 编译结果
     */
    std::string CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    
    /**
     * 合并编译结果
     * @param chtlResult CHTL编译结果
     * @param chtljsResult CHTL JS编译结果
     * @return 最终HTML
     */
    std::string MergeCompilationResults(const std::string& chtlResult, const std::string& chtljsResult);
    
    /**
     * 检查组件依赖
     * @return 是否满足
     */
    bool CheckComponentDependencies();
};

/**
 * 编译器工厂
 * 用于创建和配置编译器组件
 */
class CompilerFactory {
public:
    /**
     * 创建标准编译器配置
     * @return 是否成功
     */
    static bool CreateStandardConfiguration();
    
    /**
     * 注册所有编译器组件
     * @return 注册的组件数量
     */
    static size_t RegisterAllCompilerComponents();
    
    /**
     * 创建模块化编译器调度器
     * @return 调度器实例
     */
    static std::unique_ptr<ModularCompilerDispatcher> CreateModularDispatcher();
    
    /**
     * 验证编译器配置
     * @return 验证结果
     */
    static bool ValidateCompilerConfiguration();

private:
    /**
     * 注册核心组件
     */
    static void RegisterCoreComponents();
    
    /**
     * 注册编译器组件
     */
    static void RegisterCompilerComponents();
    
    /**
     * 注册工具组件
     */
    static void RegisterUtilityComponents();
};

} // namespace Components
} // namespace CHTL