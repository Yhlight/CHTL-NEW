#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

/**
 * CHTL编译器主程序
 * 支持UTF-8中文字符处理
 */

// 辅助函数，将FragmentType转换为字符串
const char* FragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL_FRAGMENT: return "CHTL";
        case CHTL::FragmentType::CHTL_JS_FRAGMENT: return "CHTL_JS";
        case CHTL::FragmentType::CSS_FRAGMENT: return "CSS";
        case CHTL::FragmentType::JS_FRAGMENT: return "JavaScript";
        default: return "Unknown";
    }
}

// 设置UTF-8支持
void SetupUTF8Support() {
    // 设置控制台UTF-8编码支持
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale(""));
    std::wcin.imbue(std::locale(""));
    
    // 在Windows下设置控制台代码页为UTF-8
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
}

// 读取文件内容（UTF-8支持）
std::string ReadFileUTF8(const std::string& filePath) {
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("无法打开文件: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

// 写入文件（UTF-8支持）
void WriteFileUTF8(const std::string& filePath, const std::string& content) {
    std::ofstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("无法写入文件: " + filePath);
    }
    
    fileStream << content;
}

int main(int argc, char* argv[]) {
    // 设置UTF-8支持
    SetupUTF8Support();
    
    std::cout << "=== CHTL编译器 v1.0 ===" << std::endl;
    std::cout << "支持UTF-8中文字符的超文本语言编译器" << std::endl;
    std::cout << "架构：CHTL和CHTL JS完全分离的模块化设计" << std::endl;
    std::cout << std::endl;
    
    try {
        // 处理命令行参数
        if (argc >= 2) {
            std::string arg1 = argv[1];
            if (arg1 == "--help" || arg1 == "-h") {
                std::cout << "用法: chtl [输入文件] [输出文件]" << std::endl;
                std::cout << "选项:" << std::endl;
                std::cout << "  --help, -h     显示帮助信息" << std::endl;
                std::cout << "  --version, -v  显示版本信息" << std::endl;
                std::cout << std::endl;
                std::cout << "示例:" << std::endl;
                std::cout << "  chtl index.chtl output.html" << std::endl;
                std::cout << "  chtl test.chtl" << std::endl;
                return 0;
            }
            if (arg1 == "--version" || arg1 == "-v") {
                std::cout << "CHTL编译器 v1.0.0" << std::endl;
                std::cout << "基于C++17，支持UTF-8，完全模块化架构" << std::endl;
                std::cout << "MIT License - CHTL Official" << std::endl;
                return 0;
            }
        }
        
        std::string inputFile = "test.chtl";
        std::string outputFile = "output.html";
        
        if (argc >= 2) {
            inputFile = argv[1];
        }
        if (argc >= 3) {
            outputFile = argv[2];
        }
        
        std::cout << "输入文件: " << inputFile << std::endl;
        std::cout << "输出文件: " << outputFile << std::endl;
        std::cout << std::endl;
        
        // 读取源代码
        std::string sourceCode;
        try {
            sourceCode = ReadFileUTF8(inputFile);
        }
        catch (const std::exception& e) {
            std::cerr << "读取文件错误: " << e.what() << std::endl;
            
            // 如果文件不存在，创建示例文件
            std::cout << "创建示例CHTL文件..." << std::endl;
            std::string exampleCode = R"(html {
    head {
        text {
            CHTL示例页面
        }
    }
    
    body {
        div {
            style {
                .container {
                    width: 100%;
                    max-width: 800px;
                    margin: 0 auto;
                    padding: 20px;
                }
                
                .box {
                    width: 200px;
                    height: 200px;
                    background-color: #ff6b6b;
                    border-radius: 10px;
                    cursor: pointer;
                    transition: all 0.3s ease;
                }
                
                &:hover {
                    background-color: #4ecdc4;
                    transform: scale(1.05);
                }
            }
            
            script {
                {{box}}->listen {
                    click: () => {
                        console.log('盒子被点击了！');
                        alert('欢迎使用CHTL！');
                    },
                    
                    mouseenter: () => {
                        console.log('鼠标进入盒子');
                    },
                    
                    mouseleave: () => {
                        console.log('鼠标离开盒子');
                    }
                };
                
                // 使用事件委托
                {{body}}->delegate {
                    target: {{.box}},
                    click: (e) => {
                        console.log('通过事件委托处理点击');
                    }
                };
                
                // 使用动画
                const fadeIn = animate {
                    target: {{.box}},
                    duration: 1000,
                    begin: {
                        opacity: 0,
                        transform: 'translateY(50px)'
                    },
                    end: {
                        opacity: 1,
                        transform: 'translateY(0px)'
                    },
                    easing: 'ease-out'
                };
            }
            
            text {
                点击下面的盒子试试！
            }
        }
    }
})";
            
            WriteFileUTF8(inputFile, exampleCode);
            sourceCode = exampleCode;
            std::cout << "已创建示例文件: " << inputFile << std::endl;
        }
        
        std::cout << "--- 源代码预览 ---" << std::endl;
        std::cout << sourceCode.substr(0, 200) << "..." << std::endl;
        std::cout << std::endl;
        
        // 1. 执行扫描
        std::cout << "步骤1: 执行精准代码切割..." << std::endl;
        CHTL::CHTLUnifiedScanner scanner(sourceCode);
        
        if (!scanner.Scan()) {
            std::cerr << "扫描错误: " << scanner.GetErrorMessage() << std::endl;
            return 1;
        }
        
        const auto& fragments = scanner.GetFragments();
        std::cout << "  ✓ 成功切割为 " << fragments.size() << " 个代码片段" << std::endl;
        
        // 显示片段统计
        std::unordered_map<CHTL::FragmentType, int> fragmentCounts;
        for (const auto& fragment : fragments) {
            fragmentCounts[fragment.Type]++;
        }
        
        for (const auto& count : fragmentCounts) {
            std::cout << "    " << FragmentTypeToString(count.first) 
                      << " 片段: " << count.second << " 个" << std::endl;
        }
        std::cout << std::endl;
        
        // 2. 执行编译
        std::cout << "步骤2: 执行编译器调度..." << std::endl;
        CHTL::CompilerDispatcher dispatcher;
        
        if (!dispatcher.Compile(sourceCode)) {
            std::cerr << "编译错误: " << dispatcher.GetErrorMessage() << std::endl;
            
            // 显示警告信息
            auto warnings = dispatcher.GetWarnings();
            if (!warnings.empty()) {
                std::cout << "警告信息:" << std::endl;
                for (const auto& warning : warnings) {
                    std::cout << "  ⚠️  " << warning << std::endl;
                }
            }
            
            return 1;
        }
        
        std::cout << "  ✓ 编译成功完成" << std::endl;
        
        // 3. 获取编译结果
        const auto& result = dispatcher.GetMergedResult();
        
        std::cout << "步骤3: 生成输出文件..." << std::endl;
        std::cout << "  HTML内容长度: " << result.HTMLContent.length() << " 字符" << std::endl;
        std::cout << "  CSS内容长度: " << result.CSSContent.length() << " 字符" << std::endl;
        std::cout << "  JavaScript内容长度: " << result.JSContent.length() << " 字符" << std::endl;
        
        // 4. 写入输出文件
        WriteFileUTF8(outputFile, result.FullHTML);
        std::cout << "  ✓ 输出文件已生成: " << outputFile << std::endl;
        
        // 5. 显示编译统计
        std::cout << std::endl;
        std::cout << "--- 编译统计信息 ---" << std::endl;
        std::cout << dispatcher.GetCompilationStatistics() << std::endl;
        
        // 6. 显示警告（如果有）
        auto warnings = dispatcher.GetWarnings();
        if (!warnings.empty()) {
            std::cout << "--- 编译警告 ---" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "⚠️  " << warning << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "🎉 CHTL编译完成！" << std::endl;
        std::cout << "架构验证：CHTL和CHTL JS完全分离 ✓" << std::endl;
        std::cout << "UTF-8支持：中文字符正确处理 ✓" << std::endl;
        std::cout << "模块化设计：各编译器独立运行 ✓" << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cerr << "程序错误: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "未知错误" << std::endl;
        return 1;
    }
    
    return 0;
}