#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

// 辅助函数，将 CodeType 转换为字符串以便打印
const char* CodeTypeToString(CHTL::CodeType type)
{
    switch (type)
    {
        case CHTL::CodeType::CHTL: return "CHTL";
        case CHTL::CodeType::CHTL_JS: return "CHTL_JS";
        case CHTL::CodeType::CSS: return "CSS";
        case CHTL::CodeType::JS: return "JS";
        default: return "Unknown";
    }
}

int main() {
    std::string filePath = "../src/test2.chtl"; // 使用包含多种块的测试文件
    std::ifstream fileStream(filePath);
    if (!fileStream) {
        std::cerr << "错误: 无法打开文件 " << filePath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string source = buffer.str();

    std::cout << R"(--- CHTL 统一编译流程测试 ---)" << std::endl;
    std::cout << R"(--- 源代码 ---)" << std::endl; // Removed the extra newline here, as source already has one.
    std::cout << source << std::endl;

    try {
        // 1. 扫描器 (Scanner)
        CHTL::CHTLUnifiedScanner scanner(source);
        auto chunks = scanner.Scan();

        // 验证扫描器输出
        std::cout << R"(--- 扫描器输出的代码块 ---)" << std::endl;
        for (const auto& chunk : chunks)
        {
            std::cout << "  - 类型: " << CodeTypeToString(chunk.type)
                      << R"(, 内容:
------------------------
)" << chunk.content
                      << R"(
------------------------
)";
        }

        // 2. 调度器 (Dispatcher)
        CHTL::CompilerDispatcher dispatcher;
        std::string final_html = dispatcher.Dispatch(chunks);

        // 3. 最终输出
        std::cout << R"(
--- 最终生成的 HTML ---
)" << final_html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
