#ifndef CHTL_DISPATCHER_COMPILERDISPATCHER_H
#define CHTL_DISPATCHER_COMPILERDISPATCHER_H

#include <string>
#include <memory>
#include <unordered_map>
#include "Common/CodeFragment.h"
#include "Common/Error.h"
#include "Common/Logger.h"

namespace CHTL {

// 前向声明编译器接口
class ICompiler;
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;

/**
 * 编译器调度器
 * 负责将代码片段分发到对应的编译器进行处理
 */
class CompilerDispatcher {
private:
    std::string moduleDir;              // 模块目录
    bool debugMode;                     // 调试模式
    
    // 编译器实例
    std::unique_ptr<CHTLCompiler> chtlCompiler;
    std::unique_ptr<CHTLJSCompiler> chtljsCompiler;
    std::unique_ptr<CSSCompiler> cssCompiler;
    std::unique_ptr<JavaScriptCompiler> jsCompiler;
    
    // 编译结果缓存
    struct CompileResult {
        std::string html;           // HTML内容
        std::string globalStyles;   // 全局样式
        std::string globalScripts;  // 全局脚本
        std::string headContent;    // <head>标签内容
        
        CompileResult() = default;
    };
    
    /**
     * 初始化编译器
     */
    void InitializeCompilers();
    
    /**
     * 处理CHTL片段
     */
    void ProcessCHTLFragment(const CodeFragmentPtr& fragment, CompileResult& result);
    
    /**
     * 处理CHTL JS片段
     */
    void ProcessCHTLJSFragment(const CodeFragmentPtr& fragment, CompileResult& result);
    
    /**
     * 处理CSS片段
     */
    void ProcessCSSFragment(const CodeFragmentPtr& fragment, CompileResult& result);
    
    /**
     * 处理JavaScript片段
     */
    void ProcessJavaScriptFragment(const CodeFragmentPtr& fragment, CompileResult& result);
    
    /**
     * 合并编译结果
     */
    std::string MergeResults(const CompileResult& result);
    
public:
    /**
     * 构造函数
     * @param modulePath 模块目录路径
     * @param debug 是否启用调试模式
     */
    CompilerDispatcher(const std::string& modulePath = "./module", bool debug = false);
    
    /**
     * 析构函数
     */
    ~CompilerDispatcher();
    
    /**
     * 分发代码片段到各个编译器
     * @param fragments 代码片段列表
     * @param sourceFile 源文件路径
     * @return 编译后的HTML结果
     */
    std::string Dispatch(const CodeFragmentList& fragments, const std::string& sourceFile);
    
    /**
     * 设置模块目录
     */
    void SetModuleDirectory(const std::string& dir) {
        moduleDir = dir;
    }
    
    /**
     * 获取模块目录
     */
    const std::string& GetModuleDirectory() const {
        return moduleDir;
    }
};

} // namespace CHTL

#endif // CHTL_DISPATCHER_COMPILERDISPATCHER_H