#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    // 前向声明
    class CHTLCompiler;
    class CHTLJSCompiler;
    class CSSCompiler;
    class JSCompiler;

    // 编译器调度器
    // 负责将扫描器输出的代码片段分发给对应的编译器
    // 严格按照目标规划.ini中的架构要求实现
    class CompilerDispatcher {
    private:
        // 编译器实例
        std::unique_ptr<CHTLCompiler> chtlCompiler;         // CHTL编译器
        std::unique_ptr<CHTLJSCompiler> chtljsCompiler;     // CHTL JS编译器
        std::unique_ptr<CSSCompiler> cssCompiler;           // CSS编译器（基于ANTLR4）
        std::unique_ptr<JSCompiler> jsCompiler;             // JS编译器（基于ANTLR4）
        
        // 编译结果存储
        std::vector<std::string> htmlFragments;             // HTML片段
        std::vector<std::string> cssFragments;              // CSS片段
        std::vector<std::string> jsFragments;               // JavaScript片段
        
        // 私有方法
        void InitializeCompilers();                         // 初始化编译器
        std::string MergeResults();                         // 合并编译结果
        
        // 分发方法
        std::string DispatchCHTL(const CodeChunk& chunk);   // 分发CHTL代码片段
        std::string DispatchCHTLJS(const CodeChunk& chunk); // 分发CHTL JS代码片段
        std::string DispatchCSS(const CodeChunk& chunk);    // 分发CSS代码片段
        std::string DispatchJS(const CodeChunk& chunk);     // 分发JS代码片段

    public:
        CompilerDispatcher();
        ~CompilerDispatcher() = default;

        // 主要调度方法
        std::string Dispatch(const std::vector<CodeChunk>& chunks);  // 调度编译过程
        
        // 单独编译方法
        std::string CompileCHTL(const std::string& chtlCode);        // 单独编译CHTL代码
        std::string CompileCHTLJS(const std::string& chtljsCode);    // 单独编译CHTL JS代码
        std::string CompileCSS(const std::string& cssCode);          // 单独编译CSS代码
        std::string CompileJS(const std::string& jsCode);            // 单独编译JS代码
        
        // 结果获取方法
        std::vector<std::string> GetHTMLFragments() const;   // 获取HTML片段
        std::vector<std::string> GetCSSFragments() const;    // 获取CSS片段
        std::vector<std::string> GetJSFragments() const;     // 获取JS片段
        
        // 配置方法
        void SetCHTLCompiler(std::unique_ptr<CHTLCompiler> compiler);     // 设置CHTL编译器
        void SetCHTLJSCompiler(std::unique_ptr<CHTLJSCompiler> compiler); // 设置CHTL JS编译器
        void SetCSSCompiler(std::unique_ptr<CSSCompiler> compiler);       // 设置CSS编译器
        void SetJSCompiler(std::unique_ptr<JSCompiler> compiler);         // 设置JS编译器
        
        // 工具方法
        void Reset();                                       // 重置调度器状态
        void ClearResults();                               // 清空编译结果
        
        // 调试方法
        void PrintDispatchInfo(const std::vector<CodeChunk>& chunks) const;  // 打印调度信息
    };

    // 临时编译器接口（用于当前测试）
    class CHTLCompiler {
    public:
        virtual ~CHTLCompiler() = default;
        virtual std::string Compile(const std::string& code) = 0;
    };

    class CHTLJSCompiler {
    public:
        virtual ~CHTLJSCompiler() = default;
        virtual std::string Compile(const std::string& code) = 0;
    };

    class CSSCompiler {
    public:
        virtual ~CSSCompiler() = default;
        virtual std::string Compile(const std::string& code) = 0;
    };

    class JSCompiler {
    public:
        virtual ~JSCompiler() = default;
        virtual std::string Compile(const std::string& code) = 0;
    };

    // 临时实现类（用于测试）
    class MockCHTLCompiler : public CHTLCompiler {
    public:
        std::string Compile(const std::string& code) override {
            return "<!-- CHTL编译结果 -->\n" + code + "\n<!-- /CHTL编译结果 -->";
        }
    };

    class MockCHTLJSCompiler : public CHTLJSCompiler {
    public:
        std::string Compile(const std::string& code) override {
            return "/* CHTL JS编译结果 */\n" + code + "\n/* /CHTL JS编译结果 */";
        }
    };

    class MockCSSCompiler : public CSSCompiler {
    public:
        std::string Compile(const std::string& code) override {
            return "/* CSS编译结果 */\n" + code + "\n/* /CSS编译结果 */";
        }
    };

    class MockJSCompiler : public JSCompiler {
    public:
        std::string Compile(const std::string& code) override {
            return "/* JS编译结果 */\n" + code + "\n/* /JS编译结果 */";
        }
    };

} // namespace CHTL