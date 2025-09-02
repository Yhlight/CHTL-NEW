# CHTL编译器项目

CHTL（Cascading HyperText Language）是基于C++17实现的超文本语言编译器，提供了更符合开发者习惯的HTML编写方式。

## 项目状态

✅ **已完成的功能：**
- 项目基础架构搭建
- CHTL和CHTL JS独立的文件体系
- CHTLUnifiedScanner（统一扫描器）
- CompilerDispatcher（编译器调度器）
- CHTL编译器核心组件：
  - Token（词法单元）
  - GlobalMap（全局映射表）
  - State（状态机）
  - Context（上下文管理）
  - Lexer（词法分析器）
  - AST节点定义
  - Generator（代码生成器）框架
- CHTL JS编译器核心组件（独立体系）
- CSS和JavaScript编译器占位实现
- CMOD和CJMOD处理器占位实现

## 项目结构

```
CHTL/
├── src/
│   ├── CHTL/           # CHTL编译器实现
│   │   ├── Core/       # 核心组件（Token、State、Context、GlobalMap）
│   │   ├── Lexer/      # 词法分析器
│   │   ├── Parser/     # 语法分析器
│   │   ├── AST/        # AST节点
│   │   ├── Generator/  # 代码生成器
│   │   └── Utils/      # 工具类
│   ├── CHTLJS/         # CHTL JS编译器（完全独立）
│   │   ├── Core/       # 独立的核心组件
│   │   ├── Lexer/      # 独立的词法分析器
│   │   ├── Parser/     # 独立的语法分析器
│   │   ├── AST/        # 独立的AST节点
│   │   ├── Generator/  # 独立的代码生成器
│   │   └── Utils/      # 独立的工具类
│   ├── Scanner/        # 统一扫描器
│   ├── Dispatcher/     # 编译器调度器
│   ├── CSS/            # CSS编译器（ANTLR）
│   ├── JavaScript/     # JavaScript编译器（ANTLR）
│   ├── CMOD/           # CMOD处理器
│   └── CJMOD/          # CJMOD处理器
├── include/            # 头文件
├── tests/              # 测试
├── module/             # 模块目录
├── examples/           # 示例文件
└── CMakeLists.txt      # CMake配置
```

## 编译和使用

### 编译项目
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### 使用编译器
```bash
./bin/chtl [选项] <输入文件>

选项:
  -o <文件>    指定输出文件名（默认: 输入文件名.html）
  -m <目录>    指定模块目录（默认: ./module）
  --debug      启用调试模式
  --help       显示此帮助信息
  --version    显示版本信息
```

## 架构特点

1. **完全分离的架构**：CHTL和CHTL JS拥有完全独立的编译器架构，各自管理Token、GlobalMap、State、Context、Lexer等组件。

2. **模块化设计**：采用模块化开发，代码结构清晰，便于扩展和维护。

3. **统一扫描器**：CHTLUnifiedScanner负责精准代码切割，将源代码分割成不同类型的片段。

4. **编译器调度**：CompilerDispatcher负责将代码片段分发到对应的编译器进行处理。

5. **大驼峰命名法**：遵循C++最佳实践，使用大驼峰命名法。

## 待完成功能

- [ ] 完整的CHTL语法解析实现
- [ ] CHTL JS语法解析实现
- [ ] 模板系统（@Style、@Element、@Var）
- [ ] 自定义系统（[Custom]）
- [ ] 导入系统（[Import]）
- [ ] 命名空间（[Namespace]）
- [ ] CMOD打包和解包
- [ ] CJMOD API实现
- [ ] ANTLR集成（CSS和JS解析器）
- [ ] 完整的测试套件

## 开发说明

- 使用C++17标准
- 支持UTF-8编码
- Windows下自动添加UTF-8支持
- 遵循MIT开源协议

## 注意事项

1. CHTL和CHTL JS是完全独立的架构，不共用文件体系
2. 局部script属于CHTL，虽然文档写在CHTL JS部分
3. CJMOD不是CHTL JS的内置内容
4. 需要手动构建ANTLR静态库（版本：antlr-4.13.2）