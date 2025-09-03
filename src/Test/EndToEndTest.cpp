#include <iostream>
#include <fstream>
#include <chrono>
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include "../CHTL JS/CJMODSystem/CJMODCore.h"

/**
 * 全面端到端测试
 * 确保所有功能正常，代码生成无问题
 */

class EndToEndTester {
private:
    int m_TestsPassed = 0;
    int m_TestsFailed = 0;
    std::chrono::steady_clock::time_point m_StartTime;

public:
    void StartTesting() {
        m_StartTime = std::chrono::steady_clock::now();
        std::cout << "=== CHTL全面端到端测试 ===" << std::endl;
        std::cout << "确保所有功能正常，代码生成无问题" << std::endl;
        std::cout << std::endl;
    }

    void RunTest(const std::string& testName, std::function<bool()> testFunc) {
        std::cout << "🧪 测试: " << testName << " ... ";
        
        auto start = std::chrono::steady_clock::now();
        bool result = false;
        
        try {
            result = testFunc();
        } catch (const std::exception& e) {
            std::cout << "异常: " << e.what() << std::endl;
            result = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        if (result) {
            std::cout << "✅ 通过 (" << duration.count() << "ms)" << std::endl;
            m_TestsPassed++;
        } else {
            std::cout << "❌ 失败 (" << duration.count() << "ms)" << std::endl;
            m_TestsFailed++;
        }
    }

    void FinishTesting() {
        auto end = std::chrono::steady_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_StartTime);
        
        std::cout << std::endl;
        std::cout << "=== 测试结果总结 ===" << std::endl;
        std::cout << "总测试时间: " << totalDuration.count() << "ms" << std::endl;
        std::cout << "通过测试: " << m_TestsPassed << std::endl;
        std::cout << "失败测试: " << m_TestsFailed << std::endl;
        std::cout << "总测试数: " << (m_TestsPassed + m_TestsFailed) << std::endl;
        std::cout << "成功率: " << (m_TestsPassed * 100.0 / (m_TestsPassed + m_TestsFailed)) << "%" << std::endl;
        
        if (m_TestsFailed == 0) {
            std::cout << "🎉 所有测试通过！" << std::endl;
        } else {
            std::cout << "⚠️ 有测试失败，需要修复。" << std::endl;
        }
    }
};

// 测试函数定义
bool testCJMODCoreAPI() {
    // 测试CJMOD核心API
    auto syntaxResult = CJMOD::Syntax::analyzeCode("listen { selector: '.test' }");
    if (syntaxResult.Type.empty()) return false;
    
    auto atomArg = CJMOD::AtomArg::parsePlaceholder("$?");
    if (!atomArg || !atomArg->isOptional()) return false;
    
    auto placeholders = CJMOD::CJMODScanner::scanPlaceholders("function test($, $?) { return $; }");
    if (placeholders.size() != 3) return false;
    
    return true;
}

bool testCHTLCompilerBasics() {
    // 测试CHTL编译器基础功能
    try {
        CHTL::CompilerDispatcher dispatcher;
        
        std::string testCode = R"(
<div>
    <h1>CHTL测试</h1>
    <p>这是一个测试段落</p>
</div>
        )";
        
        bool compileResult = dispatcher.Compile(testCode);
        if (!compileResult) return false;
        
        auto mergedResult = dispatcher.GetMergedResult();
        if (mergedResult.HTMLContent.empty()) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

bool testCHTLJSFeatures() {
    // 测试CHTL JS特征
    try {
        CHTL::CompilerDispatcher dispatcher;
        
        std::string testCode = R"(
listen { 
    selector: ".test-button",
    event: "click"
}
        )";
        
        bool compileResult = dispatcher.Compile(testCode);
        return compileResult;
    } catch (...) {
        return false;
    }
}

bool testCJMODIntegration() {
    // 测试CJMOD集成
    try {
        CJMOD::CJMODManager manager;
        bool initResult = manager.Initialize(nullptr, nullptr, nullptr);
        if (!initResult) return false;
        
        std::string testFragment = "printMylove { target: 'world' }";
        std::string processed = manager.ProcessCodeFragment(testFragment, 0);
        
        return !processed.empty();
    } catch (...) {
        return false;
    }
}

bool testFileIO() {
    // 测试文件IO
    try {
        std::string testContent = "Hello CHTL Test";
        std::ofstream testFile("test_io.tmp");
        testFile << testContent;
        testFile.close();
        
        std::ifstream readFile("test_io.tmp");
        std::string readContent;
        std::getline(readFile, readContent);
        readFile.close();
        
        // Clean up
        std::remove("test_io.tmp");
        
        return readContent == testContent;
    } catch (...) {
        return false;
    }
}

bool testUTF8Support() {
    // 测试UTF-8中文字符支持
    try {
        std::string chineseText = "测试中文字符：珂朵莉·诺塔·塞尼欧里斯";
        
        // 创建包含中文的CHTL代码
        std::string chtlCode = R"(
<div>
    <h1>)" + chineseText + R"(</h1>
</div>
        )";
        
        CHTL::CompilerDispatcher dispatcher;
        bool compileResult = dispatcher.Compile(chtlCode);
        
        if (!compileResult) return false;
        
        auto mergedResult = dispatcher.GetMergedResult();
        return mergedResult.HTMLContent.find(chineseText) != std::string::npos;
    } catch (...) {
        return false;
    }
}

bool testComplexCHTLFeatures() {
    // 测试复杂CHTL特征
    try {
        std::string complexCode = R"(
[Template]
<TestButton> {
    background: #ff6b6b;
    border: none;
    padding: 10px;
}

[Custom]
<MyButton inherit="TestButton"> {
    color: white;
}

<button use="MyButton">测试按钮</button>
        )";
        
        CHTL::CompilerDispatcher dispatcher;
        bool compileResult = dispatcher.Compile(complexCode);
        
        return compileResult;
    } catch (...) {
        return false;
    }
}

bool testCJMODOfficialModule() {
    // 测试CJMOD官方模块
    try {
        CJMOD::ChthollyOfficialModule chthollyModule;
        
        std::string moduleName = chthollyModule.GetName();
        if (moduleName != "chtl::Chtholly") return false;
        
        auto supportedSyntax = chthollyModule.GetSupportedSyntax();
        if (supportedSyntax.empty()) return false;
        
        bool supportsFeature = chthollyModule.SupportsSyntax("printMylove");
        if (!supportsFeature) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

bool testCodeGeneration() {
    // 测试代码生成质量
    try {
        CHTL::CompilerDispatcher dispatcher;
        
        std::string sourceCode = R"(
<html>
<head>
    <title>CHTL测试页面</title>
</head>
<body>
    <h1>欢迎使用CHTL</h1>
    <div class="container">
        <p>这是一个包含中文的测试页面。</p>
        <button onclick="alert('CHTL功能正常')">点击测试</button>
    </div>
</body>
</html>
        )";
        
        bool compileResult = dispatcher.Compile(sourceCode);
        if (!compileResult) return false;
        
        auto mergedResult = dispatcher.GetMergedResult();
        
        // 验证生成的HTML结构
        if (mergedResult.HTMLContent.find("<!DOCTYPE html>") == std::string::npos) return false;
        if (mergedResult.HTMLContent.find("<html") == std::string::npos) return false;
        if (mergedResult.HTMLContent.find("CHTL测试页面") == std::string::npos) return false;
        if (mergedResult.HTMLContent.find("欢迎使用CHTL") == std::string::npos) return false;
        
        return true;
    } catch (...) {
        return false;
    }
}

int main() {
    EndToEndTester tester;
    tester.StartTesting();
    
    // 运行所有测试
    tester.RunTest("CJMOD核心API功能", testCJMODCoreAPI);
    tester.RunTest("CHTL编译器基础功能", testCHTLCompilerBasics);
    tester.RunTest("CHTL JS特征", testCHTLJSFeatures);
    tester.RunTest("CJMOD集成功能", testCJMODIntegration);
    tester.RunTest("文件IO操作", testFileIO);
    tester.RunTest("UTF-8中文字符支持", testUTF8Support);
    tester.RunTest("复杂CHTL特征", testComplexCHTLFeatures);
    tester.RunTest("CJMOD官方模块", testCJMODOfficialModule);
    tester.RunTest("代码生成质量", testCodeGeneration);
    
    tester.FinishTesting();
    
    return 0;
}