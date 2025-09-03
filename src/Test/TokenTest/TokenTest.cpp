#include <iostream>
#include <string>
#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"

// CHTL Token测试函数
void TestCHTLTokens() {
    std::cout << "\n=== CHTL Token测试 ===" << std::endl;
    
    // 测试CHTL语法
    std::string chtlCode = R"(
        [Template] @Style DefaultText
        {
            color: "black";
            line-height: 1.6;
        }
        
        div
        {
            id: box;
            class: welcome;
            
            style
            {
                .box
                {
                    width: 100px;
                    height: 200px;
                }
                
                &:hover
                {
                    background-color: red;
                }
            }
            
            text
            {
                HelloWorld
            }
        }
    )";
    
    CHTL::CHTLLexer chtlLexer(chtlCode);
    auto chtlTokens = chtlLexer.Tokenize();
    
    std::cout << "CHTL代码词法分析完成，Token数量: " << chtlTokens.size() << std::endl;
    
    // 打印前10个Token
    std::cout << "前10个CHTL Token:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), chtlTokens.size()); i++) {
        const auto& token = chtlTokens[i];
        std::cout << "  " << i << ": " << CHTL::CHTLTokenUtil::TokenTypeToString(token.type) 
                  << " [" << token.value << "]" << std::endl;
    }
}

// CHTL JS Token测试函数
void TestCHTLJSTokens() {
    std::cout << "\n=== CHTL JS Token测试 ===" << std::endl;
    
    // 测试CHTL JS语法
    std::string chtljsCode = R"(
        module {
            load: ./module.cjjs,
            load: ./module2.cjjs
        }
        
        {{.box}}->listen {
            click: () => {
                console.log('Box clicked!');
            },
            mouseenter: mouseEnterEvent
        };
        
        {{.parent}}->delegate {
            target: [{{.child1}}, {{.child2}}],
            click: handleClick
        };
        
        vir test = listen {
            click: () => {
                console.log('Virtual object test');
            }
        };
        
        {{.box}} &-> click {
            console.log('Event binding test');
        }
        
        util a > b -> change print("变化") -> then print("完成");
    )";
    
    CHTLJS::CHTLJSLexer chtljsLexer(chtljsCode);
    auto chtljsTokens = chtljsLexer.Tokenize();
    
    std::cout << "CHTL JS代码词法分析完成，Token数量: " << chtljsTokens.size() << std::endl;
    
    // 打印前10个Token
    std::cout << "前10个CHTL JS Token:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), chtljsTokens.size()); i++) {
        const auto& token = chtljsTokens[i];
        std::cout << "  " << i << ": " << CHTLJS::CHTLJSTokenUtil::TokenTypeToString(token.type) 
                  << " [" << token.value << "]" << std::endl;
    }
}

// Token工具测试
void TestTokenUtils() {
    std::cout << "\n=== Token工具测试 ===" << std::endl;
    
    // 测试CHTL Token工具
    std::cout << "CHTL关键字测试:" << std::endl;
    std::vector<std::string> chtlKeywords = {"[Template]", "@Style", "inherit", "delete", "text", "style"};
    for (const auto& keyword : chtlKeywords) {
        bool isKeyword = CHTL::CHTLTokenUtil::IsKeyword(keyword);
        std::cout << "  " << keyword << ": " << (isKeyword ? "是关键字" : "不是关键字") << std::endl;
    }
    
    // 测试CHTL JS Token工具
    std::cout << "\nCHTL JS关键字测试:" << std::endl;
    std::vector<std::string> chtljsKeywords = {"module", "load", "listen", "delegate", "vir", "util"};
    for (const auto& keyword : chtljsKeywords) {
        bool isKeyword = CHTLJS::CHTLJSTokenUtil::IsKeyword(keyword);
        std::cout << "  " << keyword << ": " << (isKeyword ? "是关键字" : "不是关键字") << std::endl;
    }
    
    // 测试选择器语法
    std::cout << "\n选择器语法测试:" << std::endl;
    std::vector<std::string> selectors = {"{{.box}}", "{{#container}}", "{{button[0]}}", "{{.parent .child}}"};
    for (const auto& selector : selectors) {
        bool isValid = CHTLJS::CHTLJSTokenUtil::IsValidSelectorSyntax(selector);
        std::cout << "  " << selector << ": " << (isValid ? "有效" : "无效") << std::endl;
    }
}

int main() {
    try {
        std::cout << "CHTL编译器Token系统测试开始..." << std::endl;
        
        // 执行测试
        TestTokenUtils();
        TestCHTLTokens();
        TestCHTLJSTokens();
        
        std::cout << "\n所有测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}