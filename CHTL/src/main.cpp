#include <iostream>
#include <string>
#include <filesystem>
#include <memory>
#include "Common/Version.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "Dispatcher/CompilerDispatcher.h"

namespace fs = std::filesystem;

void PrintUsage(const std::string& programName) {
    std::cout << "CHTL编译器 v" << CHTL_VERSION_MAJOR << "." 
              << CHTL_VERSION_MINOR << "." << CHTL_VERSION_PATCH << std::endl;
    std::cout << "用法: " << programName << " [选项] <输入文件>" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -o <文件>    指定输出文件名（默认: 输入文件名.html）" << std::endl;
    std::cout << "  -m <目录>    指定模块目录（默认: ./module）" << std::endl;
    std::cout << "  --debug      启用调试模式" << std::endl;
    std::cout << "  --help       显示此帮助信息" << std::endl;
    std::cout << "  --version    显示版本信息" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string inputFile;
    std::string outputFile;
    std::string moduleDir = "./module";
    bool debugMode = false;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            std::cout << "CHTL v" << CHTL_VERSION_MAJOR << "." 
                      << CHTL_VERSION_MINOR << "." << CHTL_VERSION_PATCH << std::endl;
            return 0;
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            moduleDir = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "错误: 未知选项 '" << arg << "'" << std::endl;
            PrintUsage(argv[0]);
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件" << std::endl;
        PrintUsage(argv[0]);
        return 1;
    }

    // 检查输入文件是否存在
    if (!fs::exists(inputFile)) {
        std::cerr << "错误: 输入文件不存在: " << inputFile << std::endl;
        return 1;
    }

    // 如果未指定输出文件，使用默认名称
    if (outputFile.empty()) {
        fs::path inputPath(inputFile);
        outputFile = inputPath.stem().string() + ".html";
    }

    try {
        // 创建统一扫描器
        auto scanner = std::make_unique<CHTL::CHTLUnifiedScanner>(debugMode);
        
        // 读取源文件
        std::string sourceCode = scanner->ReadFile(inputFile);
        
        // 扫描并切割代码
        auto codeFragments = scanner->Scan(sourceCode);
        
        // 创建编译器调度器
        auto dispatcher = std::make_unique<CHTL::CompilerDispatcher>(moduleDir, debugMode);
        
        // 分发代码片段到各个编译器
        auto compiledResult = dispatcher->Dispatch(codeFragments, inputFile);
        
        // 输出结果
        std::ofstream outFile(outputFile);
        if (!outFile) {
            std::cerr << "错误: 无法创建输出文件: " << outputFile << std::endl;
            return 1;
        }
        
        outFile << compiledResult;
        outFile.close();
        
        std::cout << "编译成功: " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}