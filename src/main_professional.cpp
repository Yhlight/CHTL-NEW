/**
 * CHTL Professional Compiler
 * 专业版CHTL编译器主程序
 * 
 * 提供完整的命令行接口和高级功能
 * 支持批量编译、模块管理、性能分析等专业特性
 */

#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Util/FileSystem/FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace CHTL;

// 版本信息
const std::string VERSION = "1.0.0";
const std::string BUILD_DATE = __DATE__;
const std::string BUILD_TIME = __TIME__;

/**
 * 显示版本信息
 */
void ShowVersion() {
    std::cout << "🌸 CHTL Professional Compiler v" << VERSION << std::endl;
    std::cout << "📅 构建日期: " << BUILD_DATE << " " << BUILD_TIME << std::endl;
    std::cout << "🏗️  构建系统: Professional CMake Configuration" << std::endl;
#ifdef CHTL_WITH_ANTLR
    std::cout << "🔥 特性支持: CHTL + CHTL JS + CSS + JavaScript + CJMOD (完整版)" << std::endl;
#else
    std::cout << "🔥 特性支持: CHTL + CHTL JS + CJMOD (核心版)" << std::endl;
#endif
    std::cout << "📄 开源协议: MIT License" << std::endl;
    std::cout << "🌐 项目主页: https://github.com/chtl-lang/chtl-compiler" << std::endl;
    std::cout << std::endl;
}

/**
 * 显示帮助信息
 */
void ShowHelp() {
    std::cout << "CHTL Professional Compiler - 专业版CHTL编译器" << std::endl;
    std::cout << std::endl;
    std::cout << "用法:" << std::endl;
    std::cout << "  chtl-professional [选项] <输入文件>" << std::endl;
    std::cout << "  chtl-professional [选项] -c \"<CHTL代码>\"" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help              显示此帮助信息" << std::endl;
    std::cout << "  -v, --version           显示版本信息" << std::endl;
    std::cout << "  -o, --output <文件>     指定输出文件路径" << std::endl;
    std::cout << "  -c, --code <代码>       直接编译代码字符串" << std::endl;
    std::cout << "  -w, --watch             监视文件变化并自动重编译" << std::endl;
    std::cout << "  -s, --stats             显示详细编译统计" << std::endl;
    std::cout << "  -q, --quiet             静默模式，减少输出" << std::endl;
    std::cout << "  --html-only             仅输出HTML内容" << std::endl;
    std::cout << "  --css-only              仅输出CSS内容" << std::endl;
    std::cout << "  --js-only               仅输出JavaScript内容" << std::endl;
    std::cout << "  --benchmark             运行性能基准测试" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  chtl-professional input.chtl" << std::endl;
    std::cout << "  chtl-professional -o output.html input.chtl" << std::endl;
    std::cout << "  chtl-professional -c \"text { 你好，CHTL！ }\"" << std::endl;
    std::cout << "  chtl-professional --stats --benchmark input.chtl" << std::endl;
    std::cout << std::endl;
}

/**
 * 运行性能基准测试
 */
void RunBenchmark(CompilerDispatcher& dispatcher) {
    std::cout << "🏃 运行CHTL编译器性能基准测试..." << std::endl;
    std::cout << std::endl;
    
    // 测试用例
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"简单文本", "text { 你好，CHTL！ }"},
        {"基础样式", "text { 标题 } style { .title { color: blue; } }"},
        {"模板系统", "[Template]\nname: TestTemplate\ntext { {{title}} }"},
        {"命名空间", "[Namespace] Test\ntext { 命名空间测试 }"},
        {"约束系统", "text { 内容 } except { @Element(div) }"}
    };
    
    for (const auto& testCase : testCases) {
        std::cout << "📊 测试: " << testCase.first << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool success = dispatcher.Compile(testCase.second);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        if (success) {
            std::cout << "   ✅ 编译成功 - " << duration.count() << "μs" << std::endl;
            std::cout << "   📄 输出长度: " << dispatcher.GetFullHTML().length() << " 字符" << std::endl;
        } else {
            std::cout << "   ❌ 编译失败 - " << dispatcher.GetErrorMessage() << std::endl;
        }
        std::cout << std::endl;
        
        dispatcher.Reset();
    }
    
    std::cout << "✅ 基准测试完成" << std::endl;
    std::cout << std::endl;
}

/**
 * 编译文件
 */
bool CompileFile(const std::string& inputFile, const std::string& outputFile, 
                bool showStats, bool quietMode, const std::string& outputType) {
    
    if (!quietMode) {
        std::cout << "📂 读取文件: " << inputFile << std::endl;
    }
    
    // 读取源文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "❌ 错误: 无法打开文件 " << inputFile << std::endl;
        return false;
    }
    
    std::ostringstream sourceStream;
    sourceStream << file.rdbuf();
    std::string sourceCode = sourceStream.str();
    file.close();
    
    if (!quietMode) {
        std::cout << "📝 源代码长度: " << sourceCode.length() << " 字符" << std::endl;
        std::cout << "🔄 开始编译..." << std::endl;
    }
    
    // 编译
    CompilerDispatcher dispatcher;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = dispatcher.Compile(sourceCode);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (!success) {
        std::cerr << "❌ 编译失败: " << dispatcher.GetErrorMessage() << std::endl;
        return false;
    }
    
    // 获取输出内容
    std::string output;
    if (outputType == "html") {
        output = dispatcher.GetHTMLOutput();
    } else if (outputType == "css") {
        output = dispatcher.GetCSSOutput();
    } else if (outputType == "js") {
        output = dispatcher.GetJavaScriptOutput();
    } else {
        output = dispatcher.GetFullHTML();
    }
    
    // 写入输出文件
    std::ofstream outputFileStream(outputFile);
    if (!outputFileStream.is_open()) {
        std::cerr << "❌ 错误: 无法创建输出文件 " << outputFile << std::endl;
        return false;
    }
    
    outputFileStream << output;
    outputFileStream.close();
    
    if (!quietMode) {
        std::cout << "✅ 编译成功！" << std::endl;
        std::cout << "📄 输出文件: " << outputFile << std::endl;
        std::cout << "📊 输出长度: " << output.length() << " 字符" << std::endl;
        std::cout << "⏱️  编译时间: " << duration.count() << "ms" << std::endl;
        
        auto warnings = dispatcher.GetWarnings();
        if (!warnings.empty()) {
            std::cout << "⚠️  警告 (" << warnings.size() << "):" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "   " << warning << std::endl;
            }
        }
    }
    
    if (showStats) {
        std::cout << std::endl;
        std::cout << dispatcher.GetCompilationStatistics();
    }
    
    return true;
}

/**
 * 编译代码字符串
 */
bool CompileCode(const std::string& sourceCode, const std::string& outputFile,
                bool showStats, bool quietMode, const std::string& outputType) {
    
    if (!quietMode) {
        std::cout << "📝 源代码长度: " << sourceCode.length() << " 字符" << std::endl;
        std::cout << "🔄 开始编译..." << std::endl;
    }
    
    // 编译
    CompilerDispatcher dispatcher;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = dispatcher.Compile(sourceCode);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (!success) {
        std::cerr << "❌ 编译失败: " << dispatcher.GetErrorMessage() << std::endl;
        return false;
    }
    
    // 获取输出内容
    std::string output;
    if (outputType == "html") {
        output = dispatcher.GetHTMLOutput();
    } else if (outputType == "css") {
        output = dispatcher.GetCSSOutput();
    } else if (outputType == "js") {
        output = dispatcher.GetJavaScriptOutput();
    } else {
        output = dispatcher.GetFullHTML();
    }
    
    // 输出到文件或控制台
    if (!outputFile.empty()) {
        std::ofstream outputFileStream(outputFile);
        if (!outputFileStream.is_open()) {
            std::cerr << "❌ 错误: 无法创建输出文件 " << outputFile << std::endl;
            return false;
        }
        outputFileStream << output;
        outputFileStream.close();
        
        if (!quietMode) {
            std::cout << "📄 输出文件: " << outputFile << std::endl;
        }
    } else {
        std::cout << output << std::endl;
    }
    
    if (!quietMode) {
        std::cout << "✅ 编译成功！" << std::endl;
        std::cout << "📊 输出长度: " << output.length() << " 字符" << std::endl;
        std::cout << "⏱️  编译时间: " << duration.count() << "ms" << std::endl;
    }
    
    if (showStats) {
        std::cout << std::endl;
        std::cout << dispatcher.GetCompilationStatistics();
    }
    
    return true;
}

/**
 * 主函数
 */
int main(int argc, char* argv[]) {
    // 设置UTF-8输出
    std::cout.imbue(std::locale(""));
    
    // 显示启动信息
    std::cout << "🌸 CHTL Professional Compiler v" << VERSION << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    std::string sourceCode;
    std::string outputType = "full";
    bool showStats = false;
    bool quietMode = false;
    bool runBenchmark = false;
    bool watchMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            ShowHelp();
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            ShowVersion();
            return 0;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "❌ 错误: -o 选项需要指定输出文件路径" << std::endl;
                return 1;
            }
        }
        else if (arg == "-c" || arg == "--code") {
            if (i + 1 < argc) {
                sourceCode = argv[++i];
            } else {
                std::cerr << "❌ 错误: -c 选项需要指定代码字符串" << std::endl;
                return 1;
            }
        }
        else if (arg == "-s" || arg == "--stats") {
            showStats = true;
        }
        else if (arg == "-q" || arg == "--quiet") {
            quietMode = true;
        }
        else if (arg == "-w" || arg == "--watch") {
            watchMode = true;
        }
        else if (arg == "--html-only") {
            outputType = "html";
        }
        else if (arg == "--css-only") {
            outputType = "css";
        }
        else if (arg == "--js-only") {
            outputType = "js";
        }
        else if (arg == "--benchmark") {
            runBenchmark = true;
        }
        else if (arg[0] != '-') {
            inputFile = arg;
        }
        else {
            std::cerr << "❌ 错误: 未知选项 " << arg << std::endl;
            std::cerr << "使用 --help 查看帮助信息" << std::endl;
            return 1;
        }
    }
    
    // 运行基准测试
    if (runBenchmark) {
        CompilerDispatcher dispatcher;
        RunBenchmark(dispatcher);
        if (inputFile.empty() && sourceCode.empty()) {
            return 0;
        }
    }
    
    // 检查输入
    if (inputFile.empty() && sourceCode.empty()) {
        std::cerr << "❌ 错误: 请指定输入文件或使用 -c 选项提供代码" << std::endl;
        std::cerr << "使用 --help 查看帮助信息" << std::endl;
        return 1;
    }
    
    // 确定输出文件名
    if (outputFile.empty()) {
        if (!inputFile.empty()) {
            // 从输入文件名生成输出文件名
            size_t lastDot = inputFile.find_last_of('.');
            if (lastDot != std::string::npos) {
                outputFile = inputFile.substr(0, lastDot);
            } else {
                outputFile = inputFile;
            }
            
            if (outputType == "html") {
                outputFile += ".html";
            } else if (outputType == "css") {
                outputFile += ".css";
            } else if (outputType == "js") {
                outputFile += ".js";
            } else {
                outputFile += ".html";
            }
        } else {
            outputFile = "output.html";
        }
    }
    
    // 执行编译
    bool success;
    if (!sourceCode.empty()) {
        success = CompileCode(sourceCode, outputFile, showStats, quietMode, outputType);
    } else {
        success = CompileFile(inputFile, outputFile, showStats, quietMode, outputType);
    }
    
    if (!success) {
        return 1;
    }
    
    if (!quietMode) {
        std::cout << std::endl;
        std::cout << "🎉 编译完成！感谢使用CHTL Professional Compiler！" << std::endl;
    }
    
    return 0;
}