#include "Common/CMODHandler.h"
#include "Common/Logger.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void PrintUsage(const char* programName) {
    std::cout << "CMOD工具 - CHTL模块打包/解包工具" << std::endl;
    std::cout << std::endl;
    std::cout << "用法:" << std::endl;
    std::cout << "  " << programName << " pack <模块目录> <输出文件.cmod>" << std::endl;
    std::cout << "  " << programName << " unpack <输入文件.cmod> <输出目录>" << std::endl;
    std::cout << "  " << programName << " validate <文件.cmod>" << std::endl;
    std::cout << "  " << programName << " find <模块名>" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << " pack MyModule MyModule.cmod" << std::endl;
    std::cout << "  " << programName << " unpack MyModule.cmod output/" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        PrintUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    CHTL::CMODHandler handler;
    
    if (command == "pack") {
        if (argc < 4) {
            std::cerr << "错误: pack命令需要模块目录和输出文件参数" << std::endl;
            return 1;
        }
        
        fs::path modulePath = argv[2];
        fs::path outputPath = argv[3];
        
        std::cout << "正在打包模块: " << modulePath << " -> " << outputPath << std::endl;
        
        if (handler.PackModule(modulePath, outputPath)) {
            std::cout << "打包成功！" << std::endl;
            return 0;
        } else {
            std::cerr << "打包失败！" << std::endl;
            return 1;
        }
        
    } else if (command == "unpack") {
        if (argc < 4) {
            std::cerr << "错误: unpack命令需要输入文件和输出目录参数" << std::endl;
            return 1;
        }
        
        fs::path inputPath = argv[2];
        fs::path outputPath = argv[3];
        
        std::cout << "正在解包模块: " << inputPath << " -> " << outputPath << std::endl;
        
        if (handler.UnpackModule(inputPath, outputPath)) {
            std::cout << "解包成功！" << std::endl;
            return 0;
        } else {
            std::cerr << "解包失败！" << std::endl;
            return 1;
        }
        
    } else if (command == "validate") {
        fs::path cmodPath = argv[2];
        
        std::cout << "正在验证CMOD文件: " << cmodPath << std::endl;
        
        if (handler.ValidateCMODFile(cmodPath)) {
            std::cout << "CMOD文件有效！" << std::endl;
            return 0;
        } else {
            std::cerr << "CMOD文件无效！" << std::endl;
            return 1;
        }
        
    } else if (command == "find") {
        std::string moduleName = argv[2];
        
        std::cout << "正在查找模块: " << moduleName << std::endl;
        
        auto found = handler.FindModules(moduleName);
        
        if (found.empty()) {
            std::cout << "未找到模块" << std::endl;
        } else {
            std::cout << "找到以下模块:" << std::endl;
            for (const auto& path : found) {
                std::cout << "  - " << path << std::endl;
            }
        }
        
        return 0;
        
    } else {
        std::cerr << "错误: 未知命令 '" << command << "'" << std::endl;
        PrintUsage(argv[0]);
        return 1;
    }
}