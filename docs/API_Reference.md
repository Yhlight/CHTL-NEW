# CHTL编译器API参考文档

## 📚 概述

CHTL编译器提供了完整的C++ API，支持编程方式使用编译器功能。本文档详细介绍了所有公共API接口。

## 🏗️ 核心架构API

### CompilerDispatcher

编译器调度器，负责协调四个编译器的工作。

```cpp
#include "CompilerDispatcher/CompilerDispatcher.h"

// 创建编译器调度器
CHTL::CompilerDispatcher dispatcher;

// 编译CHTL源代码
bool success = dispatcher.Compile(sourceCode);

// 获取编译结果
auto result = dispatcher.GetMergedResult();
std::string html = result.HTMLContent;
std::string css = result.CSSContent;
std::string js = result.JavaScriptContent;
std::string fullHTML = result.FullHTML;
```

#### 主要方法

| 方法 | 描述 | 返回值 |
|------|------|--------|
| `Compile(sourceCode)` | 编译CHTL源代码 | `bool` |
| `GetMergedResult()` | 获取合并后的编译结果 | `MergedCompilationResult` |
| `GetHTMLOutput()` | 获取HTML输出 | `std::string` |
| `GetCSSOutput()` | 获取CSS输出 | `std::string` |
| `GetJavaScriptOutput()` | 获取JavaScript输出 | `std::string` |
| `HasError()` | 检查是否有编译错误 | `bool` |
| `GetErrorMessage()` | 获取错误信息 | `std::string` |
| `Reset()` | 重置编译器状态 | `void` |

### CHTLUnifiedScanner

统一扫描器，负责精准代码切割。

```cpp
#include "Scanner/CHTLUnifiedScanner.h"

// 创建扫描器
CHTL::CHTLUnifiedScanner scanner(sourceCode);

// 执行扫描
bool success = scanner.Scan();

// 获取代码片段
auto fragments = scanner.GetFragments();
```

#### 主要方法

| 方法 | 描述 | 返回值 |
|------|------|--------|
| `Scan()` | 执行代码扫描和切割 | `bool` |
| `GetFragments()` | 获取切割后的代码片段 | `std::vector<CodeFragment>` |
| `SetSourceCode(code)` | 设置源代码 | `void` |
| `Reset()` | 重置扫描器状态 | `void` |

## 🔀 代码合并器API

### CHTLCodeMerger

代码合并器，将片段合并为完整代码。

```cpp
#include "CodeMerger/CHTLCodeMerger.h"

// 创建代码合并器
auto merger = CHTL::CodeMergerFactory::CreateProductionMerger();

// 添加代码片段
merger->AddHTMLFragment(htmlContent, sourceFile, line, column);
merger->AddCSSFragment(cssContent, sourceFile, line, column);
merger->AddJavaScriptFragment(jsContent, sourceFile, line, column);

// 执行合并
auto result = merger->MergeCode();

if (result.IsSuccess) {
    std::string mergedHTML = result.MergedHTML;
    std::string mergedCSS = result.MergedCSS;
    std::string mergedJS = result.MergedJavaScript;
    std::string fullHTML = result.FullHTML;
}
```

#### 工厂方法

| 方法 | 描述 | 合并策略 |
|------|------|----------|
| `CreateInlineMerger()` | 创建内联合并器 | CSS/JS嵌入HTML |
| `CreateSeparateFilesMerger()` | 创建分离文件合并器 | CSS/JS独立文件 |
| `CreateHybridMerger()` | 创建混合合并器 | 关键CSS内联，其他分离 |
| `CreateProductionMerger()` | 创建生产环境合并器 | 全优化配置 |
| `CreateDevelopmentMerger()` | 创建开发环境合并器 | 保留调试信息 |

#### 合并选项

```cpp
CHTL::MergeOptions options;
options.Strategy = CHTL::MergeStrategy::INLINE_MERGE;
options.MinifyCSS = true;
options.MinifyJavaScript = true;
options.RemoveComments = true;
options.OptimizeOrder = true;
options.GenerateSourceMap = true;

merger->SetMergeOptions(options);
```

## 🎨 CSS编译器API

### CSSCompiler

基于ANTLR的CSS编译器。

```cpp
#include "CSS/CSSCompiler.h"

// 创建CSS编译器
CHTL::CSSCompiler cssCompiler;

// 设置编译选项
cssCompiler.SetCompilationOptions(
    true,  // minify
    true,  // removeComments
    true,  // optimizeSelectors
    true   // mergeRules
);

// 编译CSS代码
auto result = cssCompiler.Compile(cssCode);

if (result.IsSuccess) {
    std::string optimizedCSS = result.OptimizedCSS;
    auto warnings = result.Warnings;
}
```

#### 编译选项

| 选项 | 描述 | 默认值 |
|------|------|--------|
| `minify` | 是否压缩CSS | `false` |
| `removeComments` | 是否移除注释 | `false` |
| `optimizeSelectors` | 是否优化选择器 | `true` |
| `mergeRules` | 是否合并规则 | `true` |

## ⚡ JavaScript编译器API

### JavaScriptCompiler

基于ANTLR的JavaScript编译器。

```cpp
#include "JS/JavaScriptCompiler.h"

// 创建JavaScript编译器
CHTL::JavaScriptCompiler jsCompiler;

// 设置编译选项
jsCompiler.SetCompilationOptions(
    true,  // minify
    true,  // removeComments
    true,  // optimizeVariables
    true   // deadCodeElimination
);

// 编译JavaScript代码
auto result = jsCompiler.Compile(jsCode);

if (result.IsSuccess) {
    std::string optimizedJS = result.OptimizedJS;
    auto warnings = result.Warnings;
}
```

## 🔧 CJMOD扩展API

### Syntax类

语法分析和类型检测。

```cpp
#include "CJMOD/CJMODSyntax.h"

// 创建语法分析器
CJMOD::Syntax syntax(sourceCode);

// 分析语法
auto result = syntax.analyze(code);

// 类型检测
bool isObject = syntax.isObject(code);
bool isFunction = syntax.isFunction(code);
bool isArray = syntax.isArray(code);
bool isCHTLJSFunction = syntax.isCHTLJSFunction(code);
```

### Arg类

参数绑定和转换。

```cpp
// 创建参数
CJMOD::Arg arg("paramName", "paramValue", "string");

// 绑定参数值
bool success = arg.bind("newValue");

// 填充模板
std::string result = arg.fillValue("Template with $ placeholder");

// 参数转换
std::string transformed = arg.transform([](const std::string& value) {
    return "processed_" + value;
});
```

### AtomArg类

原子参数和占位符。

```cpp
// 解析占位符
auto atomArg = CJMOD::AtomArg::parsePlaceholder("$?");

// 获取占位符信息
auto type = atomArg->getType();
auto name = atomArg->getName();
auto placeholder = atomArg->getPlaceholderString();
```

### CHTLJSFunction类

CHTL JS函数创建。

```cpp
// 创建CHTL JS函数
auto functionInfo = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
    "listen",           // 函数类型
    "MyListener",       // 函数名称
    "click: () => {}"   // 函数体
);

// 绑定虚对象
std::string binding = CJMOD::CHTLJSFunction::bindVirtualObject(
    "myVirObject", 
    functionInfo
);

// 生成函数包装器
std::string wrapper = CJMOD::CHTLJSFunction::generateFunctionWrapper(functionInfo);
```

## 🛡️ 约束系统API

### CHTLConstraintValidator

语法边界验证。

```cpp
#include "ConstraintSystem/CHTLConstraintValidator.h"

// 创建约束验证器
CHTL::CHTLConstraintValidator validator;

// 验证约束
bool isValid = validator.ValidateConstraints(
    constraintList,
    targetElement,
    context
);

// 验证语法边界
bool boundaryValid = validator.ValidateSyntaxBoundary(
    syntax,
    targetLanguage,
    context
);
```

## 📦 导入系统API

### CHTLImportManager

模块导入管理。

```cpp
#include "ImportSystem/CHTLImportManager.h"

// 创建导入管理器
CHTL::CHTLImportManager importManager;

// 解析导入语句
auto importInfo = importManager.ParseImportStatement(importStatement);

// 解析导入路径
auto pathInfo = importManager.ResolveImportPath(
    importPath,
    currentDirectory,
    searchPaths
);

// 加载模块
bool success = importManager.LoadModule(modulePath);
```

## 🎯 AST节点API

### CHTL AST节点

```cpp
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"

// 创建元素节点
auto element = std::make_unique<CHTL::ElementNode>("div", 1, 1);
element->SetAttribute("class", "container");

// 创建模板节点
auto template_node = std::make_unique<CHTL::TemplateNode>(
    CHTL::TemplateType::STYLE_TEMPLATE, 
    "ButtonStyle", 
    1, 1
);

// 创建自定义节点
auto custom = std::make_unique<CHTL::CustomNode>(
    CHTL::CustomType::ELEMENT_CUSTOM,
    "Card",
    1, 1
);
```

### CHTL JS AST节点

```cpp
#include "CHTL JS/CHTLJSNode/EnhanceSelectorNode.h"
#include "CHTL JS/CHTLJSNode/VirObjectNode.h"
#include "CHTL JS/CHTLJSNode/AnimateNode.h"

// 创建增强选择器节点
auto selector = std::make_unique<CHTLJS::EnhanceSelectorNode>(
    "{{.button}}", 
    1, 1
);

// 创建虚对象节点
auto virObj = std::make_unique<CHTLJS::VirObjectNode>(
    "myVir",
    "listen { click: () => {} }",
    1, 1
);

// 创建动画节点
auto animate = std::make_unique<CHTLJS::AnimateNode>(
    "{{.element}}",
    500,  // duration
    1, 1
);
```

## 🔄 访问者模式API

### CHTLNodeVisitor

AST节点访问者。

```cpp
#include "CHTL/CHTLNode/Visitor.h"

// 创建HTML生成访问者
CHTL::CHTLHTMLGeneratorVisitor htmlGenerator;

// 访问AST节点
htmlGenerator.VisitElementNode(elementNode);
htmlGenerator.VisitTemplateNode(templateNode);
htmlGenerator.VisitCustomNode(customNode);

// 获取生成的HTML
std::string generatedHTML = htmlGenerator.GetGeneratedHTML();
```

## 📋 错误处理

### 错误代码

| 错误代码 | 描述 | 处理建议 |
|----------|------|----------|
| `SCAN_ERROR` | 扫描失败 | 检查源代码语法 |
| `PARSE_ERROR` | 解析失败 | 验证语法结构 |
| `GENERATE_ERROR` | 生成失败 | 检查AST完整性 |
| `MERGE_ERROR` | 合并失败 | 验证片段有效性 |
| `CONSTRAINT_ERROR` | 约束违反 | 检查语法边界 |
| `IMPORT_ERROR` | 导入失败 | 验证模块路径 |

### 异常处理

```cpp
try {
    CHTL::CompilerDispatcher dispatcher;
    bool success = dispatcher.Compile(sourceCode);
    
    if (!success) {
        std::cerr << "编译失败: " << dispatcher.GetErrorMessage() << std::endl;
        
        auto warnings = dispatcher.GetWarnings();
        for (const auto& warning : warnings) {
            std::cout << "警告: " << warning << std::endl;
        }
    }
}
catch (const std::exception& e) {
    std::cerr << "编译异常: " << e.what() << std::endl;
}
```

## 🔧 配置API

### 编译选项配置

```cpp
// CSS编译器配置
CHTL::CSSCompiler cssCompiler;
cssCompiler.SetCompilationOptions(
    true,  // minifyCSS
    true,  // removeComments
    true,  // optimizeSelectors
    true   // mergeRules
);

// JavaScript编译器配置
CHTL::JavaScriptCompiler jsCompiler;
jsCompiler.SetCompilationOptions(
    true,  // minifyJS
    true,  // removeComments
    true,  // optimizeVariables
    true   // deadCodeElimination
);

// 代码合并器配置
CHTL::MergeOptions mergeOptions;
mergeOptions.Strategy = CHTL::MergeStrategy::HYBRID_MERGE;
mergeOptions.MinifyCSS = true;
mergeOptions.MinifyJavaScript = true;
mergeOptions.GenerateSourceMap = true;
```

## 📊 性能监控API

### 编译统计

```cpp
// 获取编译统计信息
std::string stats = dispatcher.GetCompilationStatistics();

// 获取片段统计
auto fragmentStats = merger->GetFragmentStatistics();
size_t htmlFragments = fragmentStats["HTML"];
size_t cssFragments = fragmentStats["CSS"];
size_t jsFragments = fragmentStats["JavaScript"];
size_t totalFragments = fragmentStats["Total"];
```

### 性能测量

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();

// 执行编译
bool success = dispatcher.Compile(sourceCode);

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

std::cout << "编译耗时: " << duration.count() << " ms" << std::endl;
```

## 🎯 扩展开发API

### CJMOD扩展开发

```cpp
#include "CJMOD/CJMODSyntax.h"
#include "CJMOD/CJMODScanner.h"
#include "CJMOD/CJMODGenerator.h"

// 创建自定义语法扩展
class MyExtension {
public:
    static void RegisterExtension() {
        // 注册自定义语法
        CJMOD::Syntax syntax("myKeyword($, $?)");
        
        // 创建扫描器
        CJMOD::CJMODScanner scanner(code);
        auto scanResult = scanner.scan(code);
        
        // 创建生成器
        CJMOD::CJMODGenerator generator(code);
        auto genResult = generator.exportResult(scanResult);
        
        // 注册到CHTL JS
        registerCHTLJSExtension("myKeyword", genResult);
    }
};
```

### 自定义AST节点

```cpp
#include "CHTL/CHTLNode/CHTLBaseNode.h"

class MyCustomNode : public CHTL::CHTLBaseNode {
public:
    MyCustomNode(const std::string& name, size_t line, size_t column)
        : CHTLBaseNode(CHTL::CHTLNodeType::CUSTOM_NODE, name, line, column) {}
    
    std::string GenerateHTML() const override {
        return "<div class=\"my-custom\">" + GetContent() + "</div>";
    }
    
    std::string GetTypeString() const override {
        return "MyCustomNode";
    }
    
    std::unique_ptr<CHTLBaseNode> Clone() const override {
        return std::make_unique<MyCustomNode>(GetValue(), GetLine(), GetColumn());
    }
    
    void Accept(CHTLNodeVisitor* visitor) override {
        // 自定义访问逻辑
    }
};
```

## 📚 使用示例

### 基础编译示例

```cpp
#include "CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>

int main() {
    // CHTL源代码
    std::string chtlCode = R"(
        use html5;
        
        html {
            head {
                text { "CHTL示例" }
            }
            
            body {
                div {
                    class: "container";
                    
                    style {
                        .container {
                            max-width: 1200px;
                            margin: 0 auto;
                        }
                    }
                    
                    text { "Hello CHTL!" }
                }
            }
        }
    )";
    
    // 编译
    CHTL::CompilerDispatcher dispatcher;
    bool success = dispatcher.Compile(chtlCode);
    
    if (success) {
        auto result = dispatcher.GetMergedResult();
        std::cout << "编译成功！" << std::endl;
        std::cout << "HTML长度: " << result.HTMLContent.length() << std::endl;
        std::cout << "CSS长度: " << result.CSSContent.length() << std::endl;
        std::cout << "完整HTML: " << result.FullHTML << std::endl;
    }
    else {
        std::cerr << "编译失败: " << dispatcher.GetErrorMessage() << std::endl;
    }
    
    return 0;
}
```

### 高级编译示例

```cpp
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CHTLCodeMerger.h"
#include "CSS/CSSCompiler.h"
#include "JS/JavaScriptCompiler.h"

int main() {
    // 创建生产环境编译配置
    auto merger = CHTL::CodeMergerFactory::CreateProductionMerger();
    
    CHTL::CSSCompiler cssCompiler;
    cssCompiler.SetCompilationOptions(true, true, true, true);
    
    CHTL::JavaScriptCompiler jsCompiler;
    jsCompiler.SetCompilationOptions(true, true, true, true);
    
    // 手动添加代码片段
    merger->AddHTMLFragment("<div>内容</div>");
    merger->AddCSSFragment(".test { color: red; }");
    merger->AddJavaScriptFragment("console.log('test');");
    
    // 执行合并
    auto mergeResult = merger->MergeCode();
    
    if (mergeResult.IsSuccess) {
        // 编译CSS
        auto cssResult = cssCompiler.Compile(mergeResult.MergedCSS);
        
        // 编译JavaScript
        auto jsResult = jsCompiler.Compile(mergeResult.MergedJavaScript);
        
        if (cssResult.IsSuccess && jsResult.IsSuccess) {
            std::cout << "高级编译成功！" << std::endl;
            std::cout << "CSS压缩率: " << 
                (1.0 - (double)cssResult.OptimizedCSS.length() / mergeResult.MergedCSS.length()) * 100 
                << "%" << std::endl;
        }
    }
    
    return 0;
}
```

## 📖 更多资源

- **[CHTL语法文档](../CHTL语法文档.md)**: 完整语法规范
- **[项目源码](../src/)**: 完整实现代码
- **[测试示例](../src/Test/)**: 功能验证示例
- **[语法特征报告](../CHTL语法特征完整记录与验证报告.md)**: 实现验证报告

---

*CHTL编译器API参考文档 v1.0*  
*更新时间: 2024年*