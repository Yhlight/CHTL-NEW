#include "CompilerDispatcher.h"
#include <iostream>
#include <sstream>

namespace CHTL {

    CompilerDispatcher::CompilerDispatcher() {
        InitializeCompilers();
    }

    void CompilerDispatcher::InitializeCompilers() {
        // 初始化临时编译器实现（用于测试）
        chtlCompiler = std::make_unique<MockCHTLCompiler>();
        chtljsCompiler = std::make_unique<MockCHTLJSCompiler>();
        cssCompiler = std::make_unique<MockCSSCompiler>();
        jsCompiler = std::make_unique<MockJSCompiler>();
    }

    std::string CompilerDispatcher::Dispatch(const std::vector<CodeChunk>& chunks) {
        // 清空之前的结果
        ClearResults();
        
        std::cout << "编译器调度器开始处理 " << chunks.size() << " 个代码片段" << std::endl;
        
        // 分发各类型代码片段到对应编译器
        for (const auto& chunk : chunks) {
            std::string result;
            
            switch (chunk.type) {
                case CodeType::CHTL:
                    result = DispatchCHTL(chunk);
                    if (!result.empty()) {
                        htmlFragments.push_back(result);
                    }
                    break;
                    
                case CodeType::CHTL_JS:
                    result = DispatchCHTLJS(chunk);
                    if (!result.empty()) {
                        jsFragments.push_back(result);
                    }
                    break;
                    
                case CodeType::CSS:
                    result = DispatchCSS(chunk);
                    if (!result.empty()) {
                        cssFragments.push_back(result);
                    }
                    break;
                    
                case CodeType::JS:
                    result = DispatchJS(chunk);
                    if (!result.empty()) {
                        jsFragments.push_back(result);
                    }
                    break;
                    
                default:
                    std::cerr << "警告: 未知的代码类型" << std::endl;
                    break;
            }
        }
        
        // 合并编译结果
        return MergeResults();
    }

    std::string CompilerDispatcher::DispatchCHTL(const CodeChunk& chunk) {
        std::cout << "调度CHTL代码片段到CHTL编译器..." << std::endl;
        if (chtlCompiler) {
            return chtlCompiler->Compile(chunk.content);
        }
        return "";
    }

    std::string CompilerDispatcher::DispatchCHTLJS(const CodeChunk& chunk) {
        std::cout << "调度CHTL JS代码片段到CHTL JS编译器..." << std::endl;
        if (chtljsCompiler) {
            return chtljsCompiler->Compile(chunk.content);
        }
        return "";
    }

    std::string CompilerDispatcher::DispatchCSS(const CodeChunk& chunk) {
        std::cout << "调度CSS代码片段到CSS编译器..." << std::endl;
        if (cssCompiler) {
            return cssCompiler->Compile(chunk.content);
        }
        return "";
    }

    std::string CompilerDispatcher::DispatchJS(const CodeChunk& chunk) {
        std::cout << "调度JS代码片段到JS编译器..." << std::endl;
        if (jsCompiler) {
            return jsCompiler->Compile(chunk.content);
        }
        return "";
    }

    std::string CompilerDispatcher::MergeResults() {
        std::stringstream finalHTML;
        
        // 生成HTML文档结构
        finalHTML << "<!DOCTYPE html>\n";
        finalHTML << "<html>\n";
        finalHTML << "<head>\n";
        finalHTML << "    <meta charset=\"UTF-8\">\n";
        finalHTML << "    <title>CHTL编译结果</title>\n";
        
        // 添加CSS样式
        if (!cssFragments.empty()) {
            finalHTML << "    <style>\n";
            for (const auto& cssFragment : cssFragments) {
                finalHTML << cssFragment << "\n";
            }
            finalHTML << "    </style>\n";
        }
        
        finalHTML << "</head>\n";
        finalHTML << "<body>\n";
        
        // 添加HTML内容
        for (const auto& htmlFragment : htmlFragments) {
            finalHTML << htmlFragment << "\n";
        }
        
        // 添加JavaScript代码
        if (!jsFragments.empty()) {
            finalHTML << "    <script>\n";
            for (const auto& jsFragment : jsFragments) {
                finalHTML << jsFragment << "\n";
            }
            finalHTML << "    </script>\n";
        }
        
        finalHTML << "</body>\n";
        finalHTML << "</html>\n";
        
        return finalHTML.str();
    }

    // 单独编译方法实现
    std::string CompilerDispatcher::CompileCHTL(const std::string& chtlCode) {
        if (chtlCompiler) {
            return chtlCompiler->Compile(chtlCode);
        }
        return "";
    }

    std::string CompilerDispatcher::CompileCHTLJS(const std::string& chtljsCode) {
        if (chtljsCompiler) {
            return chtljsCompiler->Compile(chtljsCode);
        }
        return "";
    }

    std::string CompilerDispatcher::CompileCSS(const std::string& cssCode) {
        if (cssCompiler) {
            return cssCompiler->Compile(cssCode);
        }
        return "";
    }

    std::string CompilerDispatcher::CompileJS(const std::string& jsCode) {
        if (jsCompiler) {
            return jsCompiler->Compile(jsCode);
        }
        return "";
    }

    // 结果获取方法实现
    std::vector<std::string> CompilerDispatcher::GetHTMLFragments() const {
        return htmlFragments;
    }

    std::vector<std::string> CompilerDispatcher::GetCSSFragments() const {
        return cssFragments;
    }

    std::vector<std::string> CompilerDispatcher::GetJSFragments() const {
        return jsFragments;
    }

    // 工具方法实现
    void CompilerDispatcher::Reset() {
        ClearResults();
    }

    void CompilerDispatcher::ClearResults() {
        htmlFragments.clear();
        cssFragments.clear();
        jsFragments.clear();
    }

    void CompilerDispatcher::PrintDispatchInfo(const std::vector<CodeChunk>& chunks) const {
        std::cout << "=== 编译器调度信息 ===" << std::endl;
        std::cout << "代码片段总数: " << chunks.size() << std::endl;
        
        int chtlCount = 0, chtljsCount = 0, cssCount = 0, jsCount = 0;
        
        for (const auto& chunk : chunks) {
            switch (chunk.type) {
                case CodeType::CHTL: chtlCount++; break;
                case CodeType::CHTL_JS: chtljsCount++; break;
                case CodeType::CSS: cssCount++; break;
                case CodeType::JS: jsCount++; break;
            }
        }
        
        std::cout << "CHTL片段: " << chtlCount << std::endl;
        std::cout << "CHTL JS片段: " << chtljsCount << std::endl;
        std::cout << "CSS片段: " << cssCount << std::endl;
        std::cout << "JS片段: " << jsCount << std::endl;
        std::cout << "=====================" << std::endl;
    }

} // namespace CHTL