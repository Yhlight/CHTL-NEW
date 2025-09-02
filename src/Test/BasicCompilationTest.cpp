#include "../CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

/**
 * 基础编译测试
 * 验证CHTL编译器整体架构功能
 */
class BasicCompilationTest {
public:
    /**
     * 运行所有测试
     */
    static void RunAllTests() {
        std::cout << "开始CHTL编译器基础测试...\n" << std::endl;
        
        TestCHTLUnifiedScanner();
        TestCHTLTokenization();
        TestCHTLJSTokenization();
        TestCompilerDispatcher();
        TestCHTLBasicSyntax();
        TestCHTLJSBasicSyntax();
        TestUTF8Support();
        
        std::cout << "\n所有基础测试完成！" << std::endl;
    }

private:
    /**
     * 测试CHTLUnifiedScanner
     */
    static void TestCHTLUnifiedScanner() {
        std::cout << "测试CHTLUnifiedScanner..." << std::endl;
        
        std::string testCode = R"(
            div {
                style {
                    .box {
                        width: 100px;
                    }
                }
                
                script {
                    {{box}}.addEventListener('click', () => {
                        console.log('clicked');
                    });
                }
            }
        )";
        
        CHTLUnifiedScanner scanner(testCode);
        bool scanResult = scanner.Scan();
        
        assert(scanResult && "Scanner应该成功扫描代码");
        
        const auto& fragments = scanner.GetFragments();
        assert(!fragments.empty() && "应该产生代码片段");
        
        std::cout << "  ✓ 扫描器测试通过，产生 " << fragments.size() << " 个片段" << std::endl;
        
        // 验证片段类型
        bool hasCHTL = false, hasCHTLJS = false;
        for (const auto& fragment : fragments) {
            if (fragment.Type == FragmentType::CHTL_FRAGMENT) {
                hasCHTL = true;
            }
            else if (fragment.Type == FragmentType::CHTL_JS_FRAGMENT) {
                hasCHTLJS = true;
            }
        }
        
        assert(hasCHTL && "应该包含CHTL片段");
        std::cout << "  ✓ 片段类型识别正确" << std::endl;
    }
    
    /**
     * 测试CHTL词法分析
     */
    static void TestCHTLTokenization() {
        std::cout << "测试CHTL词法分析..." << std::endl;
        
        std::string chtlCode = R"(
            [Template] @Style DefaultText {
                color: black;
                font-size: 16px;
            }
            
            div {
                style {
                    @Style DefaultText;
                    .box {
                        width: 200px;
                    }
                }
                
                text {
                    Hello CHTL!
                }
            }
        )";
        
        CHTLLexer lexer(chtlCode);
        bool tokenizeResult = lexer.Tokenize();
        
        assert(tokenizeResult && "CHTL词法分析应该成功");
        
        const auto& tokens = lexer.GetTokens();
        assert(!tokens.empty() && "应该产生令牌");
        
        std::cout << "  ✓ CHTL词法分析通过，产生 " << tokens.size() << " 个令牌" << std::endl;
        
        // 验证关键令牌
        bool hasTemplate = false, hasAtStyle = false, hasStyle = false, hasText = false;
        for (const auto& token : tokens) {
            if (token.Type == CHTLTokenType::TEMPLATE_BLOCK) {
                hasTemplate = true;
            }
            else if (token.Type == CHTLTokenType::AT_STYLE) {
                hasAtStyle = true;
            }
            else if (token.Type == CHTLTokenType::STYLE) {
                hasStyle = true;
            }
            else if (token.Type == CHTLTokenType::TEXT) {
                hasText = true;
            }
        }
        
        assert(hasTemplate && "应该识别[Template]令牌");
        assert(hasAtStyle && "应该识别@Style令牌");
        assert(hasStyle && "应该识别style令牌");
        assert(hasText && "应该识别text令牌");
        
        std::cout << "  ✓ CHTL关键令牌识别正确" << std::endl;
    }
    
    /**
     * 测试CHTL JS词法分析
     */
    static void TestCHTLJSTokenization() {
        std::cout << "测试CHTL JS词法分析..." << std::endl;
        
        std::string chtljsCode = R"(
            module {
                load: ./module1.cjjs,
                load: ./module2.js
            }
            
            vir test = listen {
                click: () => {
                    console.log('clicked');
                },
                mouseenter: handleMouseEnter
            };
            
            {{box}}->listen {
                click: () => {
                    console.log('Box clicked!');
                }
            };
            
            {{parent}}->delegate {
                target: {{.child}},
                click: handleChildClick
            };
            
            const anim = animate {
                target: {{.box}},
                duration: 1000,
                begin: {
                    opacity: 0
                },
                end: {
                    opacity: 1
                }
            };
        )";
        
        CHTLJS::CHTLJSLexer lexer(chtljsCode);
        bool tokenizeResult = lexer.Tokenize();
        
        assert(tokenizeResult && "CHTL JS词法分析应该成功");
        
        const auto& tokens = lexer.GetTokens();
        assert(!tokens.empty() && "应该产生CHTL JS令牌");
        
        std::cout << "  ✓ CHTL JS词法分析通过，产生 " << tokens.size() << " 个令牌" << std::endl;
        
        // 验证CHTL JS关键令牌
        bool hasModule = false, hasVir = false, hasListen = false, hasDelegate = false, hasAnimate = false;
        for (const auto& token : tokens) {
            if (token.Type == CHTLJS::CHTLJSTokenType::MODULE) {
                hasModule = true;
            }
            else if (token.Type == CHTLJS::CHTLJSTokenType::VIR) {
                hasVir = true;
            }
            else if (token.Type == CHTLJS::CHTLJSTokenType::LISTEN) {
                hasListen = true;
            }
            else if (token.Type == CHTLJS::CHTLJSTokenType::DELEGATE) {
                hasDelegate = true;
            }
            else if (token.Type == CHTLJS::CHTLJSTokenType::ANIMATE) {
                hasAnimate = true;
            }
        }
        
        assert(hasModule && "应该识别module令牌");
        assert(hasVir && "应该识别vir令牌");
        assert(hasListen && "应该识别listen令牌");
        assert(hasDelegate && "应该识别delegate令牌");
        assert(hasAnimate && "应该识别animate令牌");
        
        std::cout << "  ✓ CHTL JS关键令牌识别正确" << std::endl;
    }
    
    /**
     * 测试编译器调度器
     */
    static void TestCompilerDispatcher() {
        std::cout << "测试CompilerDispatcher..." << std::endl;
        
        std::string mixedCode = R"(
            html {
                head {
                    text {
                        CHTL测试页面
                    }
                }
                
                body {
                    div {
                        id: container;
                        class: main-container;
                        
                        style {
                            .box {
                                width: 200px;
                                height: 200px;
                                background-color: red;
                            }
                            
                            &:hover {
                                background-color: blue;
                            }
                        }
                        
                        script {
                            {{box}}->listen {
                                click: () => {
                                    console.log('Box clicked!');
                                },
                                
                                mouseenter: () => {
                                    console.log('Mouse entered!');
                                }
                            };
                        }
                        
                        text {
                            点击这个盒子
                        }
                    }
                }
            }
        )";
        
        CompilerDispatcher dispatcher;
        bool compileResult = dispatcher.Compile(mixedCode);
        
        if (!compileResult) {
            std::cout << "  编译错误: " << dispatcher.GetErrorMessage() << std::endl;
        }
        
        assert(compileResult && "编译器调度器应该成功编译代码");
        
        const auto& result = dispatcher.GetMergedResult();
        assert(result.IsSuccess && "合并结果应该成功");
        assert(!result.FullHTML.empty() && "应该生成完整HTML文档");
        
        std::cout << "  ✓ 编译器调度器测试通过" << std::endl;
        std::cout << "  ✓ 生成HTML长度: " << result.HTMLContent.length() << " 字符" << std::endl;
        std::cout << "  ✓ 生成CSS长度: " << result.CSSContent.length() << " 字符" << std::endl;
        std::cout << "  ✓ 生成JS长度: " << result.JavaScriptContent.length() << " 字符" << std::endl;
    }
    
    /**
     * 测试CHTL基础语法
     */
    static void TestCHTLBasicSyntax() {
        std::cout << "测试CHTL基础语法..." << std::endl;
        
        // 测试文本节点
        std::string textCode = R"(
            text {
                这是一段中文文本
            }
        )";
        
        CHTLLexer textLexer(textCode);
        assert(textLexer.Tokenize() && "文本节点应该正确解析");
        
        // 测试CE对等式
        std::string ceCode = R"(
            div {
                id: box;
                class = container;
            }
        )";
        
        CHTLLexer ceLexer(ceCode);
        assert(ceLexer.Tokenize() && "CE对等式应该正确解析");
        
        // 测试无修饰字面量
        std::string literalCode = R"(
            style {
                color: red;
                font-size: 16px;
            }
        )";
        
        CHTLLexer literalLexer(literalCode);
        assert(literalLexer.Tokenize() && "无修饰字面量应该正确解析");
        
        std::cout << "  ✓ CHTL基础语法测试通过" << std::endl;
    }
    
    /**
     * 测试CHTL JS基础语法
     */
    static void TestCHTLJSBasicSyntax() {
        std::cout << "测试CHTL JS基础语法..." << std::endl;
        
        // 测试增强选择器
        std::string selectorCode = R"(
            {{box}}.textContent();
            {{.box}}->addEventListener('click', handler);
            {{#container}}->style.display = 'none';
            {{button[0]}}->click();
        )";
        
        CHTLJS::CHTLJSLexer selectorLexer(selectorCode);
        assert(selectorLexer.Tokenize() && "增强选择器应该正确解析");
        
        // 测试->操作符
        std::string arrowCode = R"(
            element->textContent();
            element->addEventListener('click', handler);
        )";
        
        CHTLJS::CHTLJSLexer arrowLexer(arrowCode);
        assert(arrowLexer.Tokenize() && "->操作符应该正确解析");
        
        // 测试事件绑定操作符
        std::string bindCode = R"(
            {{.box}} &-> click {
                console.log('Event bound!');
            }
        )";
        
        CHTLJS::CHTLJSLexer bindLexer(bindCode);
        assert(bindLexer.Tokenize() && "事件绑定操作符应该正确解析");
        
        std::cout << "  ✓ CHTL JS基础语法测试通过" << std::endl;
    }
    
    /**
     * 测试UTF-8支持
     */
    static void TestUTF8Support() {
        std::cout << "测试UTF-8中文字符支持..." << std::endl;
        
        std::string utf8Code = R"(
            div {
                text {
                    欢迎使用CHTL编译器！
                    这是一个支持中文的超文本语言。
                }
                
                style {
                    .中文类名 {
                        color: red;
                    }
                }
                
                script {
                    console.log('中文字符串测试');
                    {{.中文类名}}->textContent = '中文内容';
                }
            }
        )";
        
        CompilerDispatcher dispatcher;
        bool compileResult = dispatcher.Compile(utf8Code);
        
        assert(compileResult && "UTF-8代码应该正确编译");
        
        const auto& result = dispatcher.GetMergedResult();
        assert(result.IsSuccess && "UTF-8编译结果应该成功");
        
        // 验证输出包含中文字符
        assert(result.FullHTML.find("中文") != std::string::npos && "输出应该包含中文字符");
        
        std::cout << "  ✓ UTF-8中文字符支持测试通过" << std::endl;
    }
};

/**
 * 主测试函数
 */
int main() {
    try {
        BasicCompilationTest::RunAllTests();
        std::cout << "\n🎉 所有测试通过！CHTL编译器基础架构运行正常。" << std::endl;
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "\n❌ 未知错误导致测试失败" << std::endl;
        return 1;
    }
}