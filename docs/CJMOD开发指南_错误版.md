# CJMOD开发指南

## 🔥 CJMOD极为强大的特征

CJMOD是CHTL项目中**极为强大的特征**，提供了完整的C++ API扩展能力，是CHTL**不可缺少**的重要组成部分。通过CJMOD，开发者可以创建自定义的CHTL JS语法扩展，实现强大的功能定制。

---

## 🏗️ CJMOD架构

### 🔧 核心API系统

CJMOD提供6个核心API接口：

#### 1. Syntax API - 语法分析
```cpp
#include "CJMODCore.h"

// 分析代码语法
Arg result = CJMOD::Syntax::analyzeCode("listen { selector: '.button' }");
std::cout << "类型: " << result.Type << std::endl;

// 类型检测
bool isObject = CJMOD::Syntax::isObject(code);
bool isFunction = CJMOD::Syntax::isFunction(code);
bool isCHTLJSFunction = CJMOD::Syntax::isCHTLJSFunction(code);
```

#### 2. Arg API - 参数管理
```cpp
// 创建参数
CJMOD::Arg arg("pattern", "value", "type");

// 动态绑定
bool success = arg.bind("new_value");

// 参数转换
std::string result = arg.toString();
```

#### 3. AtomArg API - 原子参数
```cpp
// 5种占位符类型
auto required = CJMOD::AtomArg::parsePlaceholder("$");      // 必需参数
auto optional = CJMOD::AtomArg::parsePlaceholder("$?");     // 可选参数  
auto boolean = CJMOD::AtomArg::parsePlaceholder("$!");      // 布尔参数
auto ignore = CJMOD::AtomArg::parsePlaceholder("$_");       // 忽略参数
auto variadic = CJMOD::AtomArg::parsePlaceholder("...");    // 可变参数

// 参数操作
required->setValue("test_value");
bool isOptional = optional->isOptional();
std::string value = required->getValue();
```

#### 4. Scanner API - 代码扫描
```cpp
// 扫描占位符
auto placeholders = CJMOD::CJMODScanner::scanPlaceholders(
    "function test($, $?, $!) { return $; }"
);

// 扫描代码模式
Arg scanResult = CJMOD::CJMODScanner::scan(code, pattern);

// 占位符替换
std::unordered_map<std::string, std::string> replacements;
replacements["$target"] = "world";
std::string processed = CJMOD::CJMODScanner::processPlaceholderReplacement(
    template_code, replacements
);
```

#### 5. Generator API - 代码生成
```cpp
// 导出JavaScript
std::string jsCode = CJMOD::CJMODGenerator::exportJavaScript();

// 导出处理结果
std::string result = CJMOD::CJMODGenerator::exportResult(processedCode);

// 占位符绑定
std::string bound = CJMOD::CJMODGenerator::processPlaceholderBinding(
    template_str, args
);
```

#### 6. CHTLJSFunction API - 函数创建
```cpp
// 创建CHTL JS函数
auto functionInfo = CJMOD::CJMODFunctionGenerator::CreateCHTLJSFunction(
    "listen",                           // 函数类型
    "buttonHandler",                    // 函数名
    "console.log('Button clicked');"    // 函数体
);

// 生成函数包装器
std::string wrapper = CJMOD::CJMODFunctionGenerator::generateFunctionWrapper(
    functionInfo
);

// 虚拟对象绑定
std::string virBinding = CJMOD::CJMODFunctionGenerator::bindVirtualObject(
    "myHandler", functionInfo
);
```

---

## 🌟 创建CJMOD扩展

### 📁 扩展结构

```
MyExtension/
├── src/
│   ├── MyExtension.h          # 扩展头文件
│   ├── MyExtension.cpp        # 扩展实现
│   └── functions/             # 功能函数
│       ├── customFunction.h
│       └── customFunction.cpp
└── info/
    └── MyExtension.chtl       # 扩展信息
```

### 🔨 扩展实现

```cpp
// MyExtension.h
#pragma once
#include "CJMODCore.h"

namespace CJMOD {

class MyCustomExtension : public CJMODExtension {
public:
    MyCustomExtension() : CJMODExtension("MyExtension", "1.0.0") {
        m_SupportedSyntax = {
            "myCustomFunction",
            "anotherFeature",
            "specialSyntax"
        };
    }
    
    std::string ProcessFragment(const std::string& fragment, 
                               const std::unordered_map<std::string, std::string>& context) override;
    
    bool SupportsSyntax(const std::string& syntax) override;
    
    std::unordered_map<std::string, std::string> GetExtensionInfo() override;
    
    bool Initialize() override;
    void Cleanup() override;
    
private:
    void processCustomFunction(const std::string& code);
    std::string generateCustomCode(const std::string& input);
};

} // namespace CJMOD
```

```cpp
// MyExtension.cpp
#include "MyExtension.h"
#include <regex>

namespace CJMOD {

std::string MyCustomExtension::ProcessFragment(const std::string& fragment, 
                                              const std::unordered_map<std::string, std::string>& context) {
    std::string processed = fragment;
    
    // 检查是否包含自定义语法
    if (fragment.find("myCustomFunction") != std::string::npos) {
        processed = processCustomFunction(fragment);
    }
    
    return processed;
}

bool MyCustomExtension::SupportsSyntax(const std::string& syntax) {
    return std::find(m_SupportedSyntax.begin(), m_SupportedSyntax.end(), syntax) 
           != m_SupportedSyntax.end();
}

std::string MyCustomExtension::processCustomFunction(const std::string& code) {
    // 使用CJMOD API处理代码
    auto syntaxResult = Syntax::analyzeCode(code);
    auto placeholders = CJMODScanner::scanPlaceholders(code);
    
    // 生成自定义代码
    std::string customCode = generateCustomCode(code);
    
    // 导出结果
    return CJMODGenerator::exportResult(customCode);
}

} // namespace CJMOD
```

---

## 💝 官方模块开发

### 🌸 珂朵莉模块示例

```cpp
// printMylove功能实现
std::string ChthollyOfficialModule::implementPrintMylove(const std::string& config) {
    // 解析配置
    auto args = Syntax::analyzeCode(config);
    
    std::string target = extractParameter(config, "target");
    std::string message = extractParameter(config, "message");
    std::string mode = extractParameter(config, "mode", "ASCII");
    
    // 生成字符艺术
    std::string pixelArt = generatePixelArt(target, message, mode);
    
    // 创建JavaScript函数
    auto functionInfo = CJMODFunctionGenerator::CreateCHTLJSFunction(
        "printMylove",
        "expressLove",
        pixelArt
    );
    
    return CJMODFunctionGenerator::generateFunctionWrapper(functionInfo);
}

// iNeverAway功能实现
std::string ChthollyOfficialModule::implementINeverAway(const std::string& config) {
    // 创建虚拟对象
    auto functionInfo = CJMODFunctionGenerator::CreateCHTLJSFunction(
        "iNeverAway",
        "eternalGuardian", 
        generateGuardianCode(config)
    );
    
    return CJMODFunctionGenerator::bindVirtualObject("EternalGuardian", functionInfo);
}
```

---

## 🔗 编译器集成

### 📡 获取代码片段

```cpp
// CJMOD与编译器的集成机制
class CJMODCompilerIntegration {
public:
    // 获取真实代码片段
    std::string GetRealCodeFragment(size_t fragmentIndex) {
        if (m_Scanner && fragmentIndex < m_Scanner->GetFragmentCount()) {
            return m_Scanner->GetFragment(fragmentIndex).Content;
        }
        return "";
    }
    
    // 返回处理后的片段
    void ReturnProcessedFragment(size_t fragmentIndex, const std::string& processedCode) {
        if (m_Scanner) {
            m_Scanner->UpdateFragmentContent(fragmentIndex, processedCode);
        }
    }
};
```

### ⚡ 处理流程

1. **获取片段** - 从CHTLUnifiedScanner获取代码片段
2. **语法分析** - 使用Syntax API分析代码类型
3. **占位符扫描** - 检测和处理占位符
4. **功能处理** - 应用CJMOD扩展功能
5. **代码生成** - 生成最终JavaScript代码
6. **结果返回** - 更新扫描器中的片段

---

## 🎯 高级功能

### 🎭 虚拟对象系统

```cpp
// 创建虚拟对象
std::string createVirtualObject(const std::string& name, const CJMODFunctionInfo& info) {
    return CJMODFunctionGenerator::bindVirtualObject(name, info);
}

// 虚拟对象使用
// 在CHTL JS中：
// vir MyObject = myCustomFunction { ... }
```

### 🔄 异步链式操作

```cpp
// util...then实现
std::string implementUtilThen(const std::string& chainCode) {
    // 解析链式操作
    std::vector<std::string> operations = parseChainOperations(chainCode);
    
    // 生成异步代码
    std::string asyncCode = generateAsyncChain(operations);
    
    return CJMODGenerator::exportResult(asyncCode);
}
```

### 🎨 模板参数化

```cpp
// 模板参数化处理
std::string processTemplate(const std::string& templateCode, 
                           const std::unordered_map<std::string, Arg>& args) {
    return CJMODGenerator::processPlaceholderBinding(templateCode, args);
}
```

---

## 🧪 测试CJMOD扩展

### ✅ 单元测试

```cpp
// 测试CJMOD功能
void testCJMODExtension() {
    // 测试语法分析
    auto result = CJMOD::Syntax::analyzeCode("myFunction { param: 'value' }");
    assert(result.Type == "object");
    
    // 测试占位符扫描
    auto placeholders = CJMOD::CJMODScanner::scanPlaceholders("test($, $?)");
    assert(placeholders.size() == 2);
    
    // 测试代码生成
    std::string generated = CJMOD::CJMODGenerator::exportResult("test code");
    assert(!generated.empty());
    
    std::cout << "✅ CJMOD扩展测试通过" << std::endl;
}
```

### 🔄 集成测试

```cpp
// 测试与编译器集成
void testCompilerIntegration() {
    CJMOD::CJMODManager manager;
    bool initResult = manager.Initialize(scanner, lexer, parser);
    assert(initResult);
    
    std::string testFragment = "customSyntax { param: 'test' }";
    std::string processed = manager.ProcessCodeFragment(testFragment, 0);
    assert(!processed.empty());
    
    std::cout << "✅ 编译器集成测试通过" << std::endl;
}
```

---

## 📚 最佳实践

### 🎯 设计原则

1. **单一职责** - 每个扩展专注特定功能
2. **接口简洁** - 提供清晰易用的API
3. **性能优先** - 优化处理速度和内存使用
4. **错误处理** - 完善的异常处理机制
5. **文档完整** - 提供详细的使用说明

### 🔧 开发技巧

1. **充分利用占位符** - 使用5种占位符类型
2. **合理使用虚拟对象** - 创建可重用的功能组件
3. **优化代码生成** - 生成高质量的JavaScript代码
4. **测试驱动开发** - 先写测试再实现功能
5. **性能监控** - 监控处理时间和内存使用

### ⚠️ 注意事项

1. **线程安全** - 确保多线程环境下的安全性
2. **内存管理** - 正确使用智能指针
3. **异常安全** - 提供强异常安全保证
4. **编码规范** - 遵循C++17标准和项目规范
5. **文档同步** - 保持代码和文档的同步更新

---

## 🌈 示例项目

### 🎨 创建主题扩展

```cpp
// ThemeExtension.h
class ThemeExtension : public CJMODExtension {
public:
    ThemeExtension() : CJMODExtension("Theme", "1.0.0") {
        m_SupportedSyntax = { "theme", "colorScheme", "animation" };
    }
    
    std::string ProcessFragment(const std::string& fragment, 
                               const std::unordered_map<std::string, std::string>& context) override {
        if (fragment.find("theme") != std::string::npos) {
            return processTheme(fragment);
        }
        return fragment;
    }

private:
    std::string processTheme(const std::string& themeCode) {
        // 解析主题配置
        auto config = Syntax::analyzeCode(themeCode);
        
        // 生成主题CSS
        std::string cssCode = generateThemeCSS(config);
        
        // 返回处理结果
        return CJMODGenerator::exportResult(cssCode);
    }
};
```

### 🎮 创建交互扩展

```cpp
// InteractionExtension.h
class InteractionExtension : public CJMODExtension {
public:
    InteractionExtension() : CJMODExtension("Interaction", "1.0.0") {
        m_SupportedSyntax = { "gesture", "touch", "voice" };
    }
    
private:
    std::string processGesture(const std::string& gestureCode) {
        // 手势识别处理
        auto gestures = CJMODScanner::scanPlaceholders(gestureCode);
        
        // 生成手势处理代码
        auto functionInfo = CJMODFunctionGenerator::CreateCHTLJSFunction(
            "gesture",
            "gestureHandler",
            generateGestureHandler(gestureCode)
        );
        
        return CJMODFunctionGenerator::generateFunctionWrapper(functionInfo);
    }
};
```

---

## 📖 API参考

### 🔍 Syntax类详细API

```cpp
namespace CJMOD {
    class Syntax {
    public:
        // 主要方法
        static Arg analyzeCode(const std::string& code);
        static bool isObject(const std::string& code);
        static bool isFunction(const std::string& code);
        static bool isArray(const std::string& code);
        static bool isCHTLJSFunction(const std::string& code);
        
        // 类型常量
        static const std::string TYPE_OBJECT;
        static const std::string TYPE_FUNCTION;
        static const std::string TYPE_ARRAY;
        static const std::string TYPE_CHTLJS_FUNCTION;
        static const std::string TYPE_LITERAL;
    };
}
```

### 📋 Arg类详细API

```cpp
namespace CJMOD {
    class Arg {
    public:
        // 构造函数
        Arg();
        explicit Arg(const std::string& pattern);
        explicit Arg(const std::vector<std::string>& values);
        Arg(const std::string& name, const std::string& value, const std::string& type);
        
        // 核心方法
        void fillValue(const Arg& result) const;
        void transform(const std::string& transformExpression);
        bool bind(const std::string& value);
        std::string toString() const;
        
        // 访问器
        size_t size() const;
        AtomArg& operator[](size_t index);
        const AtomArg& operator[](size_t index) const;
        std::string getTransformResult() const;
        
        // 公共属性
        std::string Type;
        bool IsObject;
        bool IsFunction;
        bool IsCHTLJSFunction;
    };
}
```

---

## 🔬 调试和优化

### 🐛 调试技巧

```cpp
// 启用调试输出
#define CJMOD_DEBUG 1

// 调试信息输出
void debugCJMODProcessing(const std::string& fragment) {
    std::cout << "🔍 CJMOD处理调试:" << std::endl;
    std::cout << "  输入片段: " << fragment << std::endl;
    
    auto syntaxResult = Syntax::analyzeCode(fragment);
    std::cout << "  语法类型: " << syntaxResult.Type << std::endl;
    
    auto placeholders = CJMODScanner::scanPlaceholders(fragment);
    std::cout << "  占位符数量: " << placeholders.size() << std::endl;
}
```

### ⚡ 性能优化

```cpp
// 缓存机制
class CJMODCache {
private:
    std::unordered_map<std::string, std::string> m_ProcessedCache;
    
public:
    std::string getCachedResult(const std::string& input) {
        auto it = m_ProcessedCache.find(input);
        return (it != m_ProcessedCache.end()) ? it->second : "";
    }
    
    void cacheResult(const std::string& input, const std::string& output) {
        m_ProcessedCache[input] = output;
    }
};

// 批处理优化
std::vector<std::string> processBatch(const std::vector<std::string>& fragments) {
    std::vector<std::string> results;
    results.reserve(fragments.size());
    
    for (const auto& fragment : fragments) {
        results.push_back(ProcessFragment(fragment, {}));
    }
    
    return results;
}
```

---

## 🎊 总结

CJMOD作为CHTL的**极为强大的特征**，提供了：

- 🔥 **完整的C++ API扩展能力**
- 🔥 **6个核心API接口**
- 🔥 **真实代码片段处理机制**
- 🔥 **与编译器的深度集成**
- 🔥 **官方角色模块系统**
- 🔥 **强大的扩展性和灵活性**

通过CJMOD，开发者可以创建几乎任何自定义的CHTL JS语法扩展，实现无限的可能性。

**🌟 开始您的CJMOD开发之旅，释放CHTL的无限潜能！**