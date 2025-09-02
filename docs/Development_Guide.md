# CHTL编译器开发指南

## 🎯 开发环境设置

### 📋 系统要求

- **操作系统**: Linux (推荐 Ubuntu 20.04+) 或 Windows 10+
- **编译器**: GCC 14.2+ 或 Clang 20.1+ 或 MSVC 2019+
- **CMake**: 3.16+
- **Java**: OpenJDK 21+ (用于ANTLR语法生成)
- **Git**: 最新版本

### 🔧 依赖安装

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake git openjdk-21-jdk
sudo apt install clang-20 libc++-20-dev libc++abi-20-dev
```

#### CentOS/RHEL
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake git java-21-openjdk-devel
```

#### Windows
- 安装 Visual Studio 2019+ (包含C++工具)
- 安装 CMake
- 安装 Git
- 安装 OpenJDK 21

## 🏗️ 项目构建

### 📦 克隆项目

```bash
git clone <repository-url>
cd chtl-compiler
```

### 🔧 构建步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake (Release模式)
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目 (使用所有CPU核心)
make -j$(nproc)

# 或者在Windows上
cmake --build . --config Release --parallel
```

### 🧪 运行测试

```bash
# 运行所有测试
make test

# 或者单独运行测试
./bin/chtl_test                   # 基础编译测试
./bin/chtl_architecture_test      # 架构测试
./bin/chtl_performance_benchmark  # 性能测试
./bin/chtl_final_achievement      # 最终成就报告
```

## 📁 项目结构详解

### 🎯 核心目录

```
src/
├── CHTL/                         # CHTL编译器 (完全独立)
│   ├── CHTLLexer/               # 词法分析器
│   │   ├── CHTLToken.h/.cpp     # Token定义和管理
│   │   ├── CHTLGlobalMap.h/.cpp # 全局映射表
│   │   └── CHTLLexer.h/.cpp     # 词法分析器
│   ├── CHTLState/               # 状态机
│   ├── CHTLContext/             # 上下文管理
│   ├── CHTLNode/                # AST节点
│   ├── CHTLParser/              # 语法解析器
│   └── CHTLGenerator/           # 代码生成器
├── CHTL JS/                     # CHTL JS编译器 (完全独立)
│   └── [相同结构，完全独立实现]
├── Scanner/                     # 统一扫描器
├── CompilerDispatcher/          # 编译器调度器
├── CodeMerger/                  # 代码合并器 ⭐
├── CSS/                         # CSS编译器 (ANTLR)
├── JS/                          # JavaScript编译器 (ANTLR)
├── ConstraintSystem/            # 约束验证系统
├── ImportSystem/                # 导入管理系统
├── CJMOD/                       # CJMOD API系统
├── Grammar/                     # ANTLR语法文件
└── Test/                        # 测试程序
```

### 🔧 第三方库

```
thirdparty/
└── antlr/                       # ANTLR 4.13.2
    ├── lib/
    │   └── libantlr4-runtime.a  # 静态库 (2.3MB)
    ├── include/                 # 头文件
    └── antlr-4.13.2-complete.jar # 语法生成工具
```

## 🎯 开发工作流

### 1. 添加新语法特征

#### 步骤1: 更新Token定义
```cpp
// src/CHTL/CHTLLexer/CHTLToken.h
enum class CHTLTokenType {
    // 现有类型...
    NEW_FEATURE_KEYWORD,  // 新特征关键字
};
```

#### 步骤2: 更新词法分析器
```cpp
// src/CHTL/CHTLLexer/CHTLLexer.cpp
CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    // 现有逻辑...
    else if (value == "newfeature") type = CHTLTokenType::NEW_FEATURE_KEYWORD;
    // ...
}
```

#### 步骤3: 创建AST节点
```cpp
// src/CHTL/CHTLNode/NewFeatureNode.h
class NewFeatureNode : public CHTLBaseNode {
    // 实现新特征节点
};
```

#### 步骤4: 更新解析器
```cpp
// src/CHTL/CHTLParser/CHTLParser.cpp
std::unique_ptr<CHTLBaseNode> CHTLParser::ParseNewFeature() {
    // 实现新特征解析逻辑
}
```

#### 步骤5: 更新生成器
```cpp
// src/CHTL/CHTLGenerator/CHTLGenerator.cpp
void CHTLGenerator::VisitNewFeatureNode(NewFeatureNode* node) {
    // 实现新特征代码生成
}
```

#### 步骤6: 添加测试
```cpp
// src/Test/NewFeatureTest.cpp
// 创建全面的测试验证
```

### 2. 性能优化指南

#### 🚀 编译器优化

```cpp
// 启用编译器优化
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"

// 使用链接时优化
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

#### 💾 内存优化

```cpp
// 使用智能指针
std::unique_ptr<CHTLBaseNode> node = std::make_unique<ElementNode>();

// RAII资源管理
class ResourceManager {
    std::unique_ptr<Resource> m_resource;
public:
    ResourceManager() : m_resource(std::make_unique<Resource>()) {}
    ~ResourceManager() = default; // 自动清理
};
```

#### ⚡ 算法优化

```cpp
// 使用移动语义
std::vector<CHTLToken> tokens = std::move(lexer.GetTokens());

// 预留容器空间
std::vector<CodeFragment> fragments;
fragments.reserve(estimatedSize);

// 使用字符串视图减少拷贝
std::string_view ProcessText(std::string_view input) {
    // 处理逻辑
}
```

### 3. 调试技巧

#### 🔍 调试配置

```bash
# Debug模式编译
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCHTL_DEBUG_MODE=ON

# 使用调试器
gdb ./bin/chtl_test
(gdb) break CHTLLexer::ScanNextToken
(gdb) run
```

#### 📊 日志输出

```cpp
#ifdef CHTL_DEBUG_MODE
#define CHTL_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define CHTL_DEBUG(msg)
#endif

// 使用调试输出
CHTL_DEBUG("Token扫描: " << token.Value);
```

#### 🧪 单元测试

```cpp
// 创建单元测试
#include <cassert>

void TestTokenization() {
    CHTL::CHTLLexer lexer("text { hello }");
    bool success = lexer.Tokenize();
    assert(success);
    
    auto tokens = lexer.GetTokens();
    assert(tokens.size() == 5); // text, {, hello, }
    assert(tokens[0].Type == CHTL::CHTLTokenType::TEXT_KEYWORD);
}
```

## 🔧 CJMOD扩展开发

### 📚 创建CJMOD扩展

#### 步骤1: 设计语法
```chtl
// 目标语法
script {
    myCustomFunction {
        param1: $,
        param2: $?,
        callback: function($!) {
            // 处理逻辑
        }
    };
}
```

#### 步骤2: 实现C++ API
```cpp
// MyExtension.cpp
#include "CJMOD/CJMODSyntax.h"

class MyExtension {
public:
    static void Initialize() {
        // 注册语法模式
        CJMOD::Syntax syntax("myCustomFunction { param1: $, param2: $?, callback: $! }");
        
        // 创建函数信息
        auto functionInfo = CJMOD::CHTLJSFunction::CreateCHTLJSFunction(
            "myCustomFunction",
            "MyCustomFunction",
            generateFunctionBody()
        );
        
        // 注册到CHTL JS编译器
        registerExtension(functionInfo);
    }
    
private:
    static std::string generateFunctionBody() {
        return R"(
            function MyCustomFunction(param1, param2, callback) {
                console.log('自定义函数调用:', param1, param2);
                if (callback && typeof callback === 'function') {
                    callback(param1);
                }
            }
        )";
    }
};
```

#### 步骤3: 集成到编译器
```cpp
// 在编译器初始化时注册扩展
void CompilerDispatcher::InitializeExtensions() {
    MyExtension::Initialize();
    // 注册其他扩展...
}
```

## 📊 性能调优

### 🎯 性能分析工具

```bash
# 使用perf分析性能
perf record ./bin/chtl_performance_benchmark
perf report

# 使用valgrind检查内存
valgrind --tool=memcheck --leak-check=full ./bin/chtl_test

# 使用gprof分析性能
g++ -pg -o chtl_test src/Test/PerformanceBenchmark.cpp
./chtl_test
gprof chtl_test gmon.out > analysis.txt
```

### ⚡ 优化建议

1. **编译器优化**
   - 使用`-O3`优化级别
   - 启用链接时优化(`-flto`)
   - 使用目标CPU优化(`-march=native`)

2. **内存优化**
   - 预留容器空间(`reserve()`)
   - 使用移动语义(`std::move`)
   - 智能指针管理资源

3. **算法优化**
   - 缓存频繁访问的数据
   - 使用字符串视图减少拷贝
   - 并行处理大量数据

## 🧪 测试开发

### 📝 测试规范

```cpp
// 测试文件命名: *Test.cpp
// 测试函数命名: Test功能名()

#include "../ComponentToTest.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== 组件测试 ===" << std::endl;
    
    try {
        // 测试基础功能
        TestBasicFunctionality();
        
        // 测试错误处理
        TestErrorHandling();
        
        // 测试性能
        TestPerformance();
        
        std::cout << "✅ 所有测试通过" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

void TestBasicFunctionality() {
    // 基础功能测试
    Component component;
    assert(component.Initialize());
    assert(component.Process("test input"));
}
```

### 🎯 测试覆盖

确保每个新功能都有对应的测试：

1. **单元测试**: 测试单个组件功能
2. **集成测试**: 测试组件间协作
3. **性能测试**: 测试性能表现
4. **边界测试**: 测试极端情况
5. **错误测试**: 测试错误处理

## 🔍 代码审查

### 📋 审查清单

- [ ] 代码符合C++17标准
- [ ] 使用大驼峰命名法
- [ ] 完整的错误处理
- [ ] 智能指针资源管理
- [ ] UTF-8字符支持
- [ ] 单元测试覆盖
- [ ] 文档注释完整
- [ ] 性能考虑优化

### 🎯 代码质量

```cpp
// 好的代码示例
class CHTLProcessor {
private:
    std::unique_ptr<ProcessorImpl> m_impl;
    std::string m_errorMessage;

public:
    explicit CHTLProcessor(const std::string& config)
        : m_impl(std::make_unique<ProcessorImpl>(config)) {
        if (!m_impl->Initialize()) {
            throw std::runtime_error("处理器初始化失败");
        }
    }
    
    bool Process(const std::string& input, std::string& output) noexcept {
        try {
            output = m_impl->Process(input);
            return true;
        }
        catch (const std::exception& e) {
            m_errorMessage = e.what();
            return false;
        }
    }
    
    const std::string& GetErrorMessage() const noexcept {
        return m_errorMessage;
    }
};
```

## 🚀 发布流程

### 📋 发布清单

1. **代码完整性检查**
   ```bash
   # 运行所有测试
   make test
   
   # 检查代码质量
   cppcheck --enable=all src/
   
   # 检查内存泄漏
   valgrind --leak-check=full ./bin/chtl_test
   ```

2. **文档更新**
   - 更新README.md
   - 更新API文档
   - 更新CHANGELOG.md

3. **版本标记**
   ```bash
   git tag -a v1.0.0 -m "CHTL编译器 v1.0.0 正式发布"
   git push origin v1.0.0
   ```

### 📦 打包发布

```bash
# 创建发布包
mkdir chtl-compiler-v1.0.0
cp -r src/ docs/ CMakeLists.txt README.md LICENSE chtl-compiler-v1.0.0/

# 创建压缩包
tar -czf chtl-compiler-v1.0.0.tar.gz chtl-compiler-v1.0.0/
```

## 🔧 常见问题解决

### ❓ 编译问题

**问题**: 找不到ANTLR头文件
```bash
# 解决方案: 确保ANTLR库正确安装
ls thirdparty/antlr/include/
# 应该看到antlr4-runtime.h等文件
```

**问题**: 链接错误
```bash
# 解决方案: 检查库链接顺序
target_link_libraries(target CHTLCore CHTLJSCore ...)
```

### ❓ 运行时问题

**问题**: UTF-8字符显示异常
```cpp
// 解决方案: 确保UTF-8编码
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
```

**问题**: 内存泄漏
```cpp
// 解决方案: 使用智能指针
std::unique_ptr<Resource> resource = std::make_unique<Resource>();
// 自动管理内存
```

## 📚 学习资源

### 📖 推荐阅读

1. **C++17特性**: 现代C++编程
2. **编译器设计**: 龙书、虎书等经典教材
3. **ANTLR文档**: 语法分析器生成工具
4. **设计模式**: 访问者模式、工厂模式等

### 🎯 在线资源

- [C++ Reference](https://en.cppreference.com/)
- [ANTLR Documentation](https://www.antlr.org/doc/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Modern C++ Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

## 🤝 贡献规范

### 📋 提交规范

```bash
# 提交信息格式
git commit -m "feat: 添加新的语法特征"
git commit -m "fix: 修复词法分析器Bug"
git commit -m "docs: 更新API文档"
git commit -m "test: 添加性能测试"
git commit -m "refactor: 重构代码合并器"
```

### 🎯 代码规范

1. **命名规范**
   - 类名: `CHTLProcessor` (大驼峰)
   - 方法名: `ProcessCode` (大驼峰)
   - 变量名: `m_TokenList` (成员变量前缀m_)
   - 常量名: `MAX_TOKEN_COUNT` (全大写)

2. **注释规范**
   ```cpp
   /**
    * 处理CHTL代码
    * @param sourceCode 源代码
    * @param options 编译选项
    * @return 是否成功
    */
   bool ProcessCode(const std::string& sourceCode, const CompileOptions& options);
   ```

3. **错误处理**
   ```cpp
   // 使用异常处理关键错误
   if (criticalError) {
       throw std::runtime_error("关键错误信息");
   }
   
   // 使用返回值处理一般错误
   bool ProcessData(const std::string& data, std::string& errorMsg) {
       if (!ValidateData(data)) {
           errorMsg = "数据验证失败";
           return false;
       }
       return true;
   }
   ```

## 🎉 参与贡献

我们欢迎所有形式的贡献：

- 🐛 **Bug报告**: 发现问题请创建Issue
- 💡 **功能建议**: 提出新功能想法
- 📝 **文档改进**: 完善文档内容
- 🔧 **代码贡献**: 提交Pull Request
- 🧪 **测试用例**: 添加测试覆盖
- 🌟 **Star项目**: 支持项目发展

---

**感谢您对CHTL编译器项目的关注和贡献！** ⭐

*让我们一起构建更好的Web开发工具！* 🚀