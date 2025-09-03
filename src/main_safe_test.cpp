/**
 * CHTL Safe Test - 安全测试完整编译器
 * 逐步测试各个组件，确定问题所在
 */

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "🌸 CHTL安全测试程序（完整实现）" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    std::string testCode = "text { 完整实现测试 }";
    if (argc > 1) {
        testCode = argv[1];
    }
    
    std::cout << "📝 测试代码: " << testCode << std::endl;
    
    try {
        // 1. 测试词法分析器
        std::cout << "🔄 步骤1: 测试词法分析器（完整实现）..." << std::endl;
        CHTLLexer lexer(testCode);
        
        bool lexSuccess = lexer.Tokenize();
        if (lexSuccess) {
            auto tokens = lexer.GetTokens();
            std::cout << "✅ 词法分析成功，令牌数量: " << tokens.size() << std::endl;
        } else {
            std::cout << "❌ 词法分析失败: " << lexer.GetErrorMessage() << std::endl;
            return 1;
        }
        
        // 2. 测试语法解析器
        std::cout << "🔄 步骤2: 测试语法解析器（完整实现）..." << std::endl;
        CHTLParser parser(testCode);
        
        auto parseResult = parser.Parse();
        if (parseResult.IsSuccess) {
            std::cout << "✅ 语法解析成功，处理令牌: " << parseResult.TokensProcessed << " 个" << std::endl;
            
            if (!parseResult.Warnings.empty()) {
                std::cout << "⚠️  解析警告: " << parseResult.Warnings.size() << " 个" << std::endl;
            }
            
            // 3. 测试AST获取
            std::cout << "🔄 步骤3: 测试AST获取..." << std::endl;
            try {
                auto ast = parser.GetAST();
                if (ast && ast.get()) {
                    std::cout << "✅ AST获取成功，节点类型: " << static_cast<int>(ast->GetType()) << std::endl;
                    
                    // 4. 测试代码生成器
                    std::cout << "🔄 步骤4: 测试代码生成器（完整实现）..." << std::endl;
                    CHTLGenerator generator;
                    
                    try {
                        generator.SetAST(ast);
                        std::string html = generator.GenerateHTML();
                        
                        if (!html.empty()) {
                            std::cout << "✅ HTML生成成功，长度: " << html.length() << " 字符" << std::endl;
                            std::cout << "📄 HTML预览: " << html.substr(0, 100) << "..." << std::endl;
                        } else {
                            std::cout << "⚠️  HTML生成为空" << std::endl;
                        }
                    } catch (const std::exception& genE) {
                        std::cout << "❌ 代码生成异常: " << genE.what() << std::endl;
                    }
                } else {
                    std::cout << "⚠️  AST为空或无效" << std::endl;
                }
            } catch (const std::exception& astE) {
                std::cout << "❌ AST获取异常: " << astE.what() << std::endl;
            }
        } else {
            std::cout << "❌ 语法解析失败: " << parseResult.ErrorMessage << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "🎉 完整实现测试完成！" << std::endl;
        return 0;
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