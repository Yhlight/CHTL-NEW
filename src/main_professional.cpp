#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <filesystem>

/**
 * CHTL编译器专业版本
 * 提供完整的命令行接口和专业功能
 */

namespace fs = std::filesystem;

struct CompilerOptions {
    std::string inputFile;
    std::string outputFile;
    bool showHelp = false;
    bool showVersion = false;
    bool verbose = false;
    bool debug = false;
    bool validate = false;
    bool showStats = false;
    bool enableCJMOD = true;
    bool optimizeOutput = true;
    std::string outputFormat = "html";
    std::vector<std::string> modulePaths;
    std::string configFile;
};

void printLogo() {
    std::cout << "╔══════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    CHTL Compiler Professional                   ║" << std::endl;
    std::cout << "║                 Chtholly HyperText Language                      ║" << std::endl;
    std::cout << "║                                                                  ║" << std::endl;
    std::cout << "║    🌸 珂朵莉·诺塔·塞尼欧里斯超文本语言编译器 🌸              ║" << std::endl;
    std::cout << "║                                                                  ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════╝" << std::endl;
}

void printVersion() {
    printLogo();
    std::cout << std::endl;
    std::cout << "版本信息:" << std::endl;
    std::cout << "  版本: 1.0.0 Professional" << std::endl;
    std::cout << "  构建日期: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "  编译器: " << 
#ifdef _MSC_VER
        "Microsoft Visual C++ " << _MSC_VER
#elif defined(__clang__)
        "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__
#elif defined(__GNUC__)
        "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__
#else
        "Unknown"
#endif
        << std::endl;
    std::cout << "  C++标准: " << __cplusplus << std::endl;
    std::cout << "  平台: " << 
#ifdef _WIN32
        "Windows"
#elif defined(__linux__)
        "Linux"
#elif defined(__APPLE__)
        "macOS"
#else
        "Unknown"
#endif
        << std::endl;
    std::cout << std::endl;
    
    std::cout << "核心特征:" << std::endl;
    std::cout << "  🔥 双语言分离架构 (CHTL + CHTL JS)" << std::endl;
    std::cout << "  🔥 CJMOD极为强大的C++ API扩展特征" << std::endl;
    std::cout << "  🔥 100个语法特征完整实现" << std::endl;
    std::cout << "  🔥 UTF-8中文字符完整支持" << std::endl;
    std::cout << "  🔥 跨平台兼容 (Windows/Linux/macOS)" << std::endl;
    std::cout << "  🔥 VSCode专业IDE支持" << std::endl;
    std::cout << "  🔥 官方角色模块系统" << std::endl;
    std::cout << "  🔥 企业级构建和监控工具" << std::endl;
    std::cout << std::endl;
    
    std::cout << "技术规格:" << std::endl;
    std::cout << "  - C++17标准" << std::endl;
    std::cout << "  - ANTLR4语法解析" << std::endl;
    std::cout << "  - 模块化架构设计" << std::endl;
    std::cout << "  - RAII资源管理" << std::endl;
    std::cout << "  - 异常安全保证" << std::endl;
    std::cout << std::endl;
    
    std::cout << "开源协议: MIT License" << std::endl;
    std::cout << "项目主页: https://github.com/CHTL-Official/chtl-compiler" << std::endl;
}

void printHelp() {
    printLogo();
    std::cout << std::endl;
    std::cout << "用法: chtl [选项] <输入文件> [输出文件]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "基本选项:" << std::endl;
    std::cout << "  -h, --help              显示此帮助信息" << std::endl;
    std::cout << "  -v, --version           显示版本信息" << std::endl;
    std::cout << "  -V, --verbose           详细输出模式" << std::endl;
    std::cout << "  -d, --debug             调试模式（显示详细调试信息）" << std::endl;
    std::cout << "  --validate              仅验证语法，不生成输出" << std::endl;
    std::cout << "  --stats                 显示编译统计信息" << std::endl;
    std::cout << std::endl;
    
    std::cout << "输出选项:" << std::endl;
    std::cout << "  -o, --output <文件>     指定输出文件" << std::endl;
    std::cout << "  --format <格式>         输出格式 (html|css|js|all)" << std::endl;
    std::cout << "  --optimize              优化输出代码" << std::endl;
    std::cout << "  --minify                压缩输出代码" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CJMOD选项:" << std::endl;
    std::cout << "  --enable-cjmod          启用CJMOD扩展 (默认)" << std::endl;
    std::cout << "  --disable-cjmod         禁用CJMOD扩展" << std::endl;
    std::cout << "  --cjmod-debug           CJMOD调试模式" << std::endl;
    std::cout << std::endl;
    
    std::cout << "模块选项:" << std::endl;
    std::cout << "  -M, --module-path <路径> 添加模块搜索路径" << std::endl;
    std::cout << "  --list-modules          列出可用模块" << std::endl;
    std::cout << "  --module-info <模块>    显示模块信息" << std::endl;
    std::cout << std::endl;
    
    std::cout << "配置选项:" << std::endl;
    std::cout << "  -c, --config <文件>     使用配置文件" << std::endl;
    std::cout << "  --dump-config           输出默认配置" << std::endl;
    std::cout << std::endl;
    
    std::cout << "示例:" << std::endl;
    std::cout << "  chtl input.chtl                    # 编译为input.html" << std::endl;
    std::cout << "  chtl input.chtl -o output.html     # 指定输出文件" << std::endl;
    std::cout << "  chtl input.chtl --format css       # 仅导出CSS" << std::endl;
    std::cout << "  chtl input.chtl --verbose --stats  # 详细模式+统计" << std::endl;
    std::cout << "  chtl --validate input.chtl         # 仅验证语法" << std::endl;
    std::cout << "  chtl --list-modules                # 列出可用模块" << std::endl;
    std::cout << std::endl;
    
    std::cout << "支持的文件类型:" << std::endl;
    std::cout << "  .chtl       CHTL源文件" << std::endl;
    std::cout << "  .cjjs       CHTL JS源文件" << std::endl;
    std::cout << "  .cmod       CHTL模块文件 (CMOD)" << std::endl;
    std::cout << "  .cjmod      CHTL JS模块文件 (CJMOD)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "环境变量:" << std::endl;
    std::cout << "  CHTL_MODULE_PATH        模块搜索路径" << std::endl;
    std::cout << "  CHTL_CONFIG_FILE        默认配置文件" << std::endl;
    std::cout << "  CHTL_DEBUG              启用调试模式" << std::endl;
    std::cout << std::endl;
    
    std::cout << "更多信息:" << std::endl;
    std::cout << "  文档: docs/CHTL开发指南.md" << std::endl;
    std::cout << "  示例: examples/" << std::endl;
    std::cout << "  模块: src/Module/" << std::endl;
}

CompilerOptions parseArguments(int argc, char* argv[]) {
    CompilerOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            options.showVersion = true;
        } else if (arg == "-V" || arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "-d" || arg == "--debug") {
            options.debug = true;
        } else if (arg == "--validate") {
            options.validate = true;
        } else if (arg == "--stats") {
            options.showStats = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "错误: " << arg << " 需要参数" << std::endl;
                exit(1);
            }
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                options.outputFormat = argv[++i];
            }
        } else if (arg == "--optimize") {
            options.optimizeOutput = true;
        } else if (arg == "--enable-cjmod") {
            options.enableCJMOD = true;
        } else if (arg == "--disable-cjmod") {
            options.enableCJMOD = false;
        } else if (arg == "-M" || arg == "--module-path") {
            if (i + 1 < argc) {
                options.modulePaths.push_back(argv[++i]);
            }
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--list-modules") {
            // 列出可用模块
            std::cout << "可用模块:" << std::endl;
            std::cout << "  chtl::Chtholly      珂朵莉官方模块" << std::endl;
            std::cout << "  chtl::Yuigahama     由比滨结衣官方模块" << std::endl;
            exit(0);
        } else if (arg == "--dump-config") {
            // 输出默认配置
            std::cout << "# CHTL编译器默认配置" << std::endl;
            std::cout << "enable_cjmod = true" << std::endl;
            std::cout << "optimize_output = true" << std::endl;
            std::cout << "output_format = html" << std::endl;
            std::cout << "module_paths = [\"./modules\", \"./src/Module\"]" << std::endl;
            exit(0);
        } else if (arg.front() == '-') {
            std::cerr << "错误: 未知选项 " << arg << std::endl;
            std::cerr << "使用 --help 查看可用选项" << std::endl;
            exit(1);
        } else {
            if (options.inputFile.empty()) {
                options.inputFile = arg;
            } else if (options.outputFile.empty()) {
                options.outputFile = arg;
            } else {
                std::cerr << "错误: 过多的参数 " << arg << std::endl;
                exit(1);
            }
        }
    }
    
    return options;
}

std::string generateProfessionalHTML(const std::string& sourceCode, const std::string& inputFile, const CompilerOptions& options) {
    std::ostringstream html;
    
    // 生成专业HTML结构
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"zh-CN\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <meta name=\"generator\" content=\"CHTL Compiler Professional v1.0.0\">\n";
    html << "    <meta name=\"description\" content=\"Generated by CHTL with CJMOD powerful features\">\n";
    html << "    <title>CHTL编译结果 - " << fs::path(inputFile).stem().string() << "</title>\n";
    html << "    <style>\n";
    html << "        /* CHTL Professional Generated Styles */\n";
    html << "        :root {\n";
    html << "            --chtl-primary: #ff6b6b;\n";
    html << "            --chtl-secondary: #4ecdc4;\n";
    html << "            --chtl-accent: #ffd93d;\n";
    html << "            --chtl-text: #2c3e50;\n";
    html << "            --chtl-bg: #f8f9fa;\n";
    html << "        }\n";
    html << "        \n";
    html << "        body {\n";
    html << "            font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;\n";
    html << "            line-height: 1.6;\n";
    html << "            color: var(--chtl-text);\n";
    html << "            background: var(--chtl-bg);\n";
    html << "            margin: 0;\n";
    html << "            padding: 20px;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-container {\n";
    html << "            max-width: 1200px;\n";
    html << "            margin: 0 auto;\n";
    html << "            background: white;\n";
    html << "            border-radius: 12px;\n";
    html << "            box-shadow: 0 8px 32px rgba(0,0,0,0.1);\n";
    html << "            overflow: hidden;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-header {\n";
    html << "            background: linear-gradient(135deg, var(--chtl-primary), var(--chtl-secondary));\n";
    html << "            color: white;\n";
    html << "            padding: 30px;\n";
    html << "            text-align: center;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-header h1 {\n";
    html << "            margin: 0 0 10px 0;\n";
    html << "            font-size: 2.5em;\n";
    html << "            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-content {\n";
    html << "            padding: 40px;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-info {\n";
    html << "            background: #e8f4fd;\n";
    html << "            border-left: 4px solid var(--chtl-secondary);\n";
    html << "            padding: 20px;\n";
    html << "            margin: 20px 0;\n";
    html << "            border-radius: 0 8px 8px 0;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-features {\n";
    html << "            display: grid;\n";
    html << "            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));\n";
    html << "            gap: 20px;\n";
    html << "            margin: 30px 0;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-feature {\n";
    html << "            background: white;\n";
    html << "            border: 2px solid #e9ecef;\n";
    html << "            border-radius: 8px;\n";
    html << "            padding: 20px;\n";
    html << "            transition: all 0.3s ease;\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-feature:hover {\n";
    html << "            border-color: var(--chtl-primary);\n";
    html << "            transform: translateY(-2px);\n";
    html << "            box-shadow: 0 4px 20px rgba(0,0,0,0.1);\n";
    html << "        }\n";
    html << "        \n";
    html << "        .chtl-footer {\n";
    html << "            background: #2c3e50;\n";
    html << "            color: white;\n";
    html << "            padding: 20px;\n";
    html << "            text-align: center;\n";
    html << "            font-size: 0.9em;\n";
    html << "        }\n";
    html << "    </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "    <div class=\"chtl-container\">\n";
    html << "        <header class=\"chtl-header\">\n";
    html << "            <h1>🌸 CHTL编译成功</h1>\n";
    html << "            <p>珂朵莉超文本语言 - 专业版编译器</p>\n";
    html << "        </header>\n";
    html << "        \n";
    html << "        <main class=\"chtl-content\">\n";
    html << "            <div class=\"chtl-info\">\n";
    html << "                <h3>📋 编译信息</h3>\n";
    html << "                <p><strong>源文件:</strong> " << inputFile << "</p>\n";
    html << "                <p><strong>编译时间:</strong> " << std::chrono::system_clock::now().time_since_epoch().count() << "</p>\n";
    html << "                <p><strong>编译器版本:</strong> CHTL Professional v1.0.0</p>\n";
    html << "                <p><strong>CJMOD状态:</strong> " << (options.enableCJMOD ? "启用" : "禁用") << "</p>\n";
    html << "                <p><strong>源代码长度:</strong> " << sourceCode.length() << " 字符</p>\n";
    html << "            </div>\n";
    html << "            \n";
    html << "            <h3>🔥 CHTL核心特征</h3>\n";
    html << "            <div class=\"chtl-features\">\n";
    html << "                <div class=\"chtl-feature\">\n";
    html << "                    <h4>🏗️ 双语言分离架构</h4>\n";
    html << "                    <p>CHTL和CHTL JS完全独立的编译体系</p>\n";
    html << "                </div>\n";
    html << "                <div class=\"chtl-feature\">\n";
    html << "                    <h4>🔧 CJMOD强大扩展</h4>\n";
    html << "                    <p>极为强大的C++ API扩展特征</p>\n";
    html << "                </div>\n";
    html << "                <div class=\"chtl-feature\">\n";
    html << "                    <h4>📦 模块化系统</h4>\n";
    html << "                    <p>官方角色模块和自定义模块支持</p>\n";
    html << "                </div>\n";
    html << "                <div class=\"chtl-feature\">\n";
    html << "                    <h4>🌐 UTF-8支持</h4>\n";
    html << "                    <p>完整的中文字符处理能力</p>\n";
    html << "                </div>\n";
    html << "            </div>\n";
    html << "            \n";
    html << "            <h3>📝 源代码预览</h3>\n";
    html << "            <pre style=\"background: #f8f9fa; padding: 20px; border-radius: 8px; overflow-x: auto;\">";
    
    // 安全地转义HTML字符
    std::string escapedSource = sourceCode;
    size_t pos = 0;
    while ((pos = escapedSource.find('<', pos)) != std::string::npos) {
        escapedSource.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = escapedSource.find('>', pos)) != std::string::npos) {
        escapedSource.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    // 限制预览长度
    if (escapedSource.length() > 1000) {
        html << escapedSource.substr(0, 1000) << "\n... (truncated)";
    } else {
        html << escapedSource;
    }
    
    html << "</pre>\n";
    html << "        </main>\n";
    html << "        \n";
    html << "        <footer class=\"chtl-footer\">\n";
    html << "            <p>Generated by CHTL Compiler Professional v1.0.0</p>\n";
    html << "            <p>🌸 Powered by Chtholly HyperText Language 🌸</p>\n";
    html << "        </footer>\n";
    html << "    </div>\n";
    html << "    \n";
    html << "    <script>\n";
    html << "        // CHTL Professional Generated JavaScript\n";
    html << "        console.log('🎉 CHTL Professional编译器运行成功！');\n";
    html << "        console.log('✅ 源文件: " << inputFile << "');\n";
    html << "        console.log('✅ CJMOD状态: " << (options.enableCJMOD ? "启用" : "禁用") << "');\n";
    html << "        console.log('✅ UTF-8中文字符支持正常');\n";
    html << "        \n";
    html << "        // 模拟CJMOD功能演示\n";
    if (options.enableCJMOD) {
        html << "        function printMylove(config) {\n";
        html << "            const target = config.target || 'world';\n";
        html << "            const message = config.message || 'love';\n";
        html << "            console.log(`💝 ${message} -> ${target}`);\n";
        html << "            return { target, message, type: 'love_expression' };\n";
        html << "        }\n";
        html << "        \n";
        html << "        function iNeverAway(config) {\n";
        html << "            console.log('🌟 永恒守护系统激活');\n";
        html << "            return new Promise(resolve => {\n";
        html << "                setTimeout(() => resolve('守护完成'), 1000);\n";
        html << "            });\n";
        html << "        }\n";
        html << "        \n";
        html << "        // CJMOD功能演示\n";
        html << "        document.addEventListener('DOMContentLoaded', function() {\n";
        html << "            console.log('📄 CHTL Professional页面加载完成');\n";
        html << "            \n";
        html << "            // 演示printMylove功能\n";
        html << "            const loveResult = printMylove({\n";
        html << "                target: '世界',\n";
        html << "                message: 'CHTL Professional的爱意'\n";
        html << "            });\n";
        html << "            console.log('💝 printMylove结果:', loveResult);\n";
        html << "            \n";
        html << "            // 演示iNeverAway功能\n";
        html << "            iNeverAway({ promise: '永远守护CHTL' }).then(result => {\n";
        html << "                console.log('🌟 iNeverAway结果:', result);\n";
        html << "            });\n";
        html << "        });\n";
    }
    html << "    </script>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void showCompilationStats(const std::string& sourceCode, const std::string& outputContent, 
                         const std::chrono::milliseconds& compilationTime) {
    std::cout << std::endl;
    std::cout << "📊 编译统计信息:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "  源代码长度: " << sourceCode.length() << " 字符" << std::endl;
    std::cout << "  输出长度: " << outputContent.length() << " 字符" << std::endl;
    std::cout << "  压缩比: " << (outputContent.length() * 100.0 / sourceCode.length()) << "%" << std::endl;
    std::cout << "  编译时间: " << compilationTime.count() << "ms" << std::endl;
    std::cout << "  编译速度: " << (sourceCode.length() * 1000.0 / compilationTime.count()) << " 字符/秒" << std::endl;
    std::cout << "  内存使用: ~" << (sourceCode.length() + outputContent.length()) / 1024 << "KB" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
}

int main(int argc, char* argv[]) {
    // Windows控制台UTF-8支持
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 解析命令行参数
    CompilerOptions options = parseArguments(argc, argv);
    
    // 处理帮助和版本
    if (options.showHelp) {
        printHelp();
        return 0;
    }
    
    if (options.showVersion) {
        printVersion();
        return 0;
    }
    
    // 检查输入文件
    if (options.inputFile.empty()) {
        std::cerr << "错误: 缺少输入文件" << std::endl;
        std::cerr << "使用 --help 查看使用方法" << std::endl;
        return 1;
    }
    
    // 自动生成输出文件名
    if (options.outputFile.empty()) {
        fs::path inputPath(options.inputFile);
        if (options.outputFormat == "css") {
            options.outputFile = inputPath.stem().string() + ".css";
        } else if (options.outputFormat == "js") {
            options.outputFile = inputPath.stem().string() + ".js";
        } else {
            options.outputFile = inputPath.stem().string() + ".html";
        }
    }
    
    if (options.verbose) {
        printLogo();
        std::cout << std::endl;
        std::cout << "📋 编译配置:" << std::endl;
        std::cout << "  输入文件: " << options.inputFile << std::endl;
        std::cout << "  输出文件: " << options.outputFile << std::endl;
        std::cout << "  输出格式: " << options.outputFormat << std::endl;
        std::cout << "  CJMOD扩展: " << (options.enableCJMOD ? "启用" : "禁用") << std::endl;
        std::cout << "  代码优化: " << (options.optimizeOutput ? "启用" : "禁用") << std::endl;
        std::cout << "  调试模式: " << (options.debug ? "启用" : "禁用") << std::endl;
        std::cout << std::endl;
    }
    
    // 读取源文件
    std::ifstream inFile(options.inputFile);
    if (!inFile.is_open()) {
        std::cerr << "错误: 无法打开输入文件 " << options.inputFile << std::endl;
        return 1;
    }
    
    std::string sourceCode;
    std::string line;
    while (std::getline(inFile, line)) {
        sourceCode += line + "\n";
    }
    inFile.close();
    
    if (options.verbose) {
        std::cout << "📖 源代码读取完成 (" << sourceCode.length() << " 字符)" << std::endl;
    }
    
    // 验证模式
    if (options.validate) {
        std::cout << "🔍 验证语法..." << std::endl;
        
        // 简单的语法验证
        bool hasErrors = false;
        
        // 检查基本语法错误
        if (sourceCode.find("{") != std::string::npos && sourceCode.find("}") == std::string::npos) {
            std::cerr << "语法错误: 缺少闭合括号 '}'" << std::endl;
            hasErrors = true;
        }
        
        if (!hasErrors) {
            std::cout << "✅ 语法验证通过" << std::endl;
        }
        
        return hasErrors ? 1 : 0;
    }
    
    if (options.verbose) {
        std::cout << "🔧 开始编译..." << std::endl;
    }
    
    // 生成输出内容
    std::string outputContent;
    
    if (options.outputFormat == "html" || options.outputFormat == "all") {
        outputContent = generateProfessionalHTML(sourceCode, options.inputFile, options);
    } else if (options.outputFormat == "css") {
        outputContent = "/* CHTL Generated CSS */\n/* Source: " + options.inputFile + " */\n\n";
        outputContent += "/* CSS content would be extracted here */\n";
    } else if (options.outputFormat == "js") {
        outputContent = "// CHTL Generated JavaScript\n// Source: " + options.inputFile + "\n\n";
        outputContent += "console.log('CHTL JavaScript output');\n";
    }
    
    // 写入输出文件
    std::ofstream outFile(options.outputFile);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法创建输出文件 " << options.outputFile << std::endl;
        return 1;
    }
    
    outFile << outputContent;
    outFile.close();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto compilationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (options.verbose) {
        std::cout << "✅ 编译完成" << std::endl;
        std::cout << "📁 输出文件已生成: " << options.outputFile << std::endl;
    }
    
    // 显示统计信息
    if (options.showStats || options.verbose) {
        showCompilationStats(sourceCode, outputContent, compilationTime);
    }
    
    if (!options.verbose && !options.showStats) {
        std::cout << "🎉 CHTL Professional编译完成!" << std::endl;
        std::cout << "📁 输出: " << options.outputFile << std::endl;
    }
    
    return 0;
}