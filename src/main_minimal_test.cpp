/**
 * CHTL Minimal Test
 * 最小测试程序，用于验证核心功能
 */

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <string>

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "🌸 CHTL最小测试程序" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    std::string testCode = "text { 你好，CHTL！ }";
    if (argc > 1) {
        testCode = argv[1];
    }
    
    std::cout << "📝 测试代码: " << testCode << std::endl;
    std::cout << "📏 代码长度: " << testCode.length() << " 字符" << std::endl;
    
    try {
        // 测试词法分析器
        std::cout << "🔄 创建词法分析器..." << std::endl;
        CHTLLexer lexer(testCode);
        
        std::cout << "🔄 执行词法分析..." << std::endl;
        bool success = lexer.Tokenize();
        
        if (success) {
            std::cout << "✅ 词法分析成功！" << std::endl;
            
            auto tokens = lexer.GetTokens();
            std::cout << "📋 令牌数量: " << tokens.size() << std::endl;
            
            for (size_t i = 0; i < tokens.size() && i < 10; ++i) {
                const auto& token = tokens[i];
                std::cout << "   令牌 " << i << ": " 
                         << "类型=" << static_cast<int>(token.Type)
                         << ", 值=\"" << token.Value << "\""
                         << ", 行=" << token.Line
                         << ", 列=" << token.Column << std::endl;
            }
            
            std::cout << std::endl;
            std::cout << "🎉 测试成功完成！" << std::endl;
            return 0;
        } else {
            std::cout << "❌ 词法分析失败!" << std::endl;
            std::cout << "错误信息: " << lexer.GetErrorMessage() << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cout << "❌ 程序异常: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cout << "❌ 未知异常!" << std::endl;
        return 1;
    }
}