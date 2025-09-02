#pragma once

#include "../Core/ModularComponents.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../Scanner/CHTLUnifiedScanner.h"

namespace CHTL {
namespace Components {

/**
 * 简化的CHTL编译器组件
 */
class SimpleCHTLCompilerComponent : public Core::ModularCompilerComponent {
private:
    std::unique_ptr<CHTLLexer> m_Lexer;
    std::unique_ptr<CHTLParser> m_Parser;
    std::unique_ptr<CHTLGenerator> m_Generator;

public:
    /**
     * 构造函数
     */
    SimpleCHTLCompilerComponent() : Core::ModularCompilerComponent("SimpleCHTLCompiler", "1.0.0") {
        AddSupportedExtension("chtl");
    }

protected:
    bool DoInitialize() override {
        try {
            m_Lexer = std::make_unique<CHTLLexer>("");
            m_Parser = std::make_unique<CHTLParser>("");
            m_Generator = std::make_unique<CHTLGenerator>();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    void DoReset() override {
        if (m_Lexer) m_Lexer->Reset();
        if (m_Parser) m_Parser->Reset();
        if (m_Generator) m_Generator->Reset();
    }
    
    std::string DoCompile(const std::string& input) override {
        if (!m_Lexer || !m_Parser || !m_Generator) {
            return "";
        }
        
        try {
            // 词法分析
            m_Lexer->SetSourceCode(input);
            auto lexResult = m_Lexer->Tokenize();
            if (!lexResult.IsSuccess) return "";
            
            // 语法解析
            m_Parser->SetSourceCode(input);
            auto parseResult = m_Parser->Parse();
            if (!parseResult.IsSuccess) return "";
            
            // 代码生成
            auto generateResult = m_Generator->Generate(std::move(parseResult.RootNode));
            if (!generateResult.IsSuccess) return "";
            
            return generateResult.HTMLContent;
            
        } catch (...) {
            return "";
        }
    }
};

/**
 * 简化的CHTL JS编译器组件
 */
class SimpleCHTLJSCompilerComponent : public Core::ModularCompilerComponent {
private:
    std::unique_ptr<CHTLJS::CHTLJSLexer> m_Lexer;
    std::unique_ptr<CHTLJS::CHTLJSParser> m_Parser;
    std::unique_ptr<CHTLJS::CHTLJSGenerator> m_Generator;

public:
    /**
     * 构造函数
     */
    SimpleCHTLJSCompilerComponent() : Core::ModularCompilerComponent("SimpleCHTLJSCompiler", "1.0.0") {
        AddSupportedExtension("cjjs");
    }

protected:
    bool DoInitialize() override {
        try {
            m_Lexer = std::make_unique<CHTLJS::CHTLJSLexer>("");
            m_Parser = std::make_unique<CHTLJS::CHTLJSParser>("");
            m_Generator = std::make_unique<CHTLJS::CHTLJSGenerator>();
            return true;
        } catch (...) {
            return false;
        }
    }
    
    void DoReset() override {
        if (m_Lexer) m_Lexer->Reset();
        if (m_Parser) m_Parser->Reset();
        if (m_Generator) m_Generator->Reset();
    }
    
    std::string DoCompile(const std::string& input) override {
        if (!m_Lexer || !m_Parser || !m_Generator) {
            return "";
        }
        
        try {
            // 词法分析
            m_Lexer->SetSourceCode(input);
            auto lexResult = m_Lexer->Tokenize();
            if (!lexResult.IsSuccess) return "";
            
            // 语法解析
            m_Parser->SetSourceCode(input);
            auto parseResult = m_Parser->Parse();
            if (!parseResult.IsSuccess) return "";
            
            // 代码生成
            auto generateResult = m_Generator->Generate(std::move(parseResult.RootNode));
            if (!generateResult.IsSuccess) return "";
            
            return generateResult.JavaScriptContent;
            
        } catch (...) {
            return "";
        }
    }
};

/**
 * 简化的统一扫描器组件
 */
class SimpleUnifiedScannerComponent : public Core::ModularScannerComponent {
private:
    std::unique_ptr<CHTLUnifiedScanner> m_Scanner;

public:
    /**
     * 构造函数
     */
    SimpleUnifiedScannerComponent() : Core::ModularScannerComponent("SimpleUnifiedScanner", "1.0.0") {
    }
    
    /**
     * 获取扫描器
     */
    CHTLUnifiedScanner* GetScanner() const {
        return m_Scanner.get();
    }

protected:
    bool DoInitialize() override {
        try {
            m_Scanner = std::make_unique<CHTLUnifiedScanner>("");
            return true;
        } catch (...) {
            return false;
        }
    }
    
    void DoReset() override {
        if (m_Scanner) {
            m_Scanner->Reset();
        }
    }
    
    bool DoScan(const std::string& source) override {
        if (!m_Scanner) {
            return false;
        }
        
        try {
            m_Scanner->SetSourceCode(source);
            bool result = m_Scanner->Scan();
            
            if (result) {
                auto fragments = m_Scanner->GetFragments();
                for (const auto& fragment : fragments) {
                    AddResult(fragment.Content);
                }
            }
            
            return result;
            
        } catch (...) {
            return false;
        }
    }
};

/**
 * 简化的模块化编译器调度器
 */
class SimpleModularDispatcher {
private:
    std::unique_ptr<SimpleCHTLCompilerComponent> m_CHTLCompiler;
    std::unique_ptr<SimpleCHTLJSCompilerComponent> m_CHTLJSCompiler;
    std::unique_ptr<SimpleUnifiedScannerComponent> m_Scanner;
    std::string m_LastCompiledHTML;

public:
    /**
     * 构造函数
     */
    SimpleModularDispatcher() {
        m_CHTLCompiler = std::make_unique<SimpleCHTLCompilerComponent>();
        m_CHTLJSCompiler = std::make_unique<SimpleCHTLJSCompilerComponent>();
        m_Scanner = std::make_unique<SimpleUnifiedScannerComponent>();
    }
    
    /**
     * 初始化所有组件
     */
    bool Initialize() {
        return m_Scanner->Initialize() && 
               m_CHTLCompiler->Initialize() && 
               m_CHTLJSCompiler->Initialize();
    }
    
    /**
     * 启动所有组件
     */
    bool Start() {
        return m_Scanner->Start() && 
               m_CHTLCompiler->Start() && 
               m_CHTLJSCompiler->Start();
    }
    
    /**
     * 编译源代码
     */
    bool Compile(const std::string& sourceCode) {
        try {
            // 扫描代码
            if (!m_Scanner->Scan(sourceCode)) {
                return false;
            }
            
            // 简化编译流程
            std::string chtlResult = m_CHTLCompiler->Compile(sourceCode);
            std::string chtljsResult = m_CHTLJSCompiler->Compile(sourceCode);
            
            // 合并结果
            m_LastCompiledHTML = chtlResult + "\n<script>\n" + chtljsResult + "\n</script>";
            
            return true;
            
        } catch (...) {
            return false;
        }
    }
    
    /**
     * 获取编译结果
     */
    const std::string& GetCompiledHTML() const {
        return m_LastCompiledHTML;
    }
    
    /**
     * 生成组件报告
     */
    std::string GenerateReport() const {
        std::ostringstream oss;
        
        oss << "🔧 简化模块化编译器调度器报告:" << std::endl;
        oss << "  扫描器状态: " << GetStateString(m_Scanner->GetState()) << std::endl;
        oss << "  CHTL编译器状态: " << GetStateString(m_CHTLCompiler->GetState()) << std::endl;
        oss << "  CHTL JS编译器状态: " << GetStateString(m_CHTLJSCompiler->GetState()) << std::endl;
        oss << "  最后编译结果大小: " << m_LastCompiledHTML.size() << " 字符" << std::endl;
        
        return oss.str();
    }

private:
    std::string GetStateString(ComponentState state) const {
        switch (state) {
            case ComponentState::UNINITIALIZED: return "未初始化";
            case ComponentState::INITIALIZING: return "初始化中";
            case ComponentState::INITIALIZED: return "已初始化";
            case ComponentState::RUNNING: return "运行中";
            case ComponentState::STOPPED: return "已停止";
            case ComponentState::ERROR_STATE: return "错误状态";
            default: return "未知状态";
        }
    }
};

} // namespace Components
} // namespace CHTL