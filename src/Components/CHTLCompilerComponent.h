#pragma once

#include "../Core/CompilerComponentBase.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"

namespace CHTL {
namespace Components {

/**
 * CHTL编译器组件
 * 模块化的CHTL编译器实现
 */
class CHTLCompilerComponent : public Core::CompilerComponentBase {
private:
    std::unique_ptr<CHTLLexer> m_Lexer;
    std::unique_ptr<CHTLParser> m_Parser;
    std::unique_ptr<CHTLGenerator> m_Generator;

public:
    /**
     * 构造函数
     */
    CHTLCompilerComponent();
    
    /**
     * 析构函数
     */
    ~CHTLCompilerComponent() override = default;
    
    // ICompilerComponent接口实现
    bool ValidateInput(const std::string& input) override;

protected:
    // CompilerComponentBase接口实现
    std::string DoCompile(const std::string& input) override;
    std::string DoOptimize(const std::string& code) override;
    std::string DoMinify(const std::string& code) override;
    
    // ComponentBase接口实现
    bool Initialize() override;
    void Reset() override;

private:
    /**
     * 初始化编译器子组件
     * @return 是否成功
     */
    bool InitializeSubComponents();
    
    /**
     * 验证CHTL语法
     * @param input 输入代码
     * @return 是否有效
     */
    bool ValidateCHTLSyntax(const std::string& input);
};

/**
 * CHTL JS编译器组件
 * 模块化的CHTL JS编译器实现
 */
class CHTLJSCompilerComponent : public Core::CompilerComponentBase {
private:
    std::unique_ptr<CHTLJS::CHTLJSLexer> m_Lexer;
    std::unique_ptr<CHTLJS::CHTLJSParser> m_Parser;
    std::unique_ptr<CHTLJS::CHTLJSGenerator> m_Generator;

public:
    /**
     * 构造函数
     */
    CHTLJSCompilerComponent();
    
    /**
     * 析构函数
     */
    ~CHTLJSCompilerComponent() override = default;
    
    // ICompilerComponent接口实现
    bool ValidateInput(const std::string& input) override;

protected:
    // CompilerComponentBase接口实现
    std::string DoCompile(const std::string& input) override;
    std::string DoOptimize(const std::string& code) override;
    std::string DoMinify(const std::string& code) override;
    
    // ComponentBase接口实现
    bool Initialize() override;
    void Reset() override;

private:
    /**
     * 初始化编译器子组件
     * @return 是否成功
     */
    bool InitializeSubComponents();
    
    /**
     * 验证CHTL JS语法
     * @param input 输入代码
     * @return 是否有效
     */
    bool ValidateCHTLJSSyntax(const std::string& input);
};

/**
 * 统一扫描器组件
 * 模块化的扫描器实现
 */
class UnifiedScannerComponent : public Core::ScannerComponentBase {
private:
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;

public:
    /**
     * 构造函数
     */
    UnifiedScannerComponent();
    
    /**
     * 析构函数
     */
    ~UnifiedScannerComponent() override = default;

protected:
    // ScannerComponentBase接口实现
    bool DoScan(const std::string& source) override;
    
    // ComponentBase接口实现
    bool Initialize() override;
    void Reset() override;

public:
    /**
     * 获取扫描器实例
     * @return 扫描器指针
     */
    CHTLUnifiedScanner* GetScanner() const {
        return m_Scanner.get();
    }
    
    /**
     * 获取片段
     * @return 代码片段列表
     */
    std::vector<CodeFragment> GetFragments() const;
};

} // namespace Components
} // namespace CHTL