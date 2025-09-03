# CHTL Compiler v1.0.0 - 完整发布包

🌸 **CHTL (Chtholly HyperText Language) Professional Compiler**

这是CHTL编译器的完整发布包，包含编译器、官方模块、文档和示例。

## 📦 包含内容

### 🔧 编译器
- `bin/chtl` - 标准CHTL编译器
- `bin/chtl-professional` - 专业版CHTL编译器（完整功能）

### 📚 官方模块
- `module/Chtholly.cmod` - 珂朵莉官方模块（CMOD + CJMOD混合）
- `module/Yuigahama.cmod` - 由比滨结衣官方模块（纯CMOD）

### 📖 文档
- `docs/` - 完整的中文文档
  - CHTL开发指南
  - CHTL JS开发指南
  - CMOD/CJMOD开发指南
  - 项目构建指南
  - 语法规范等

### 🎯 示例
- `examples/complete/` - 完整功能示例
- `examples/projects/` - 大型项目示例

## 🚀 快速开始

### 基本使用
```bash
# 编译CHTL文件
./bin/chtl input.chtl

# 使用专业版编译器
./bin/chtl-professional input.chtl -o output.html

# 查看版本信息
./bin/chtl-professional --version

# 运行基准测试
./bin/chtl-professional --benchmark
```

### 专业版功能
```bash
# 直接编译代码字符串
./bin/chtl-professional -c "text { 你好，CHTL！ }"

# 显示详细统计
./bin/chtl-professional --stats input.chtl

# 仅输出HTML
./bin/chtl-professional --html-only input.chtl

# 仅输出CSS
./bin/chtl-professional --css-only input.chtl

# 仅输出JavaScript
./bin/chtl-professional --js-only input.chtl
```

## 🌟 核心特性

### ✅ 已实现功能
- **完整的CHTL语法支持** - 基于严格的语法文档实现
- **CHTL JS独立语言** - 与CHTL完全分离的编程语言
- **CJMOD强大扩展系统** - C++ API扩展CHTL JS功能
- **模块化架构** - CMOD/CJMOD/CMOD+CJMOD三种模块结构
- **UTF-8中文支持** - 原生支持中文字符
- **专业级工具链** - 命令行接口、性能基准测试
- **官方模块** - 珂朵莉和由比滨结衣官方模块

### 🔥 技术亮点
- **双语言分离架构** - CHTL和CHTL JS完全独立
- **ANTLR集成** - 支持CSS和JavaScript编译（完整版）
- **统一扫描器** - 精准代码片段切割
- **代码合并器** - 智能合并编译结果
- **约束系统** - `except`关键字支持
- **命名空间系统** - 完整的命名空间支持

## 📋 系统要求

### Linux (推荐)
- Ubuntu 18.04+ 或其他现代Linux发行版
- GCC 7+ 或 Clang 10+
- CMake 3.16+
- 至少 2GB RAM

### 依赖库
- ANTLR4 C++ Runtime (已包含)
- 标准C++17库

## 🏗️ 架构说明

```
CHTL编译器架构
├── CHTLUnifiedScanner (统一扫描器)
├── CompilerDispatcher (编译器调度器)
│   ├── CHTL Compiler (手写)
│   ├── CHTL JS Compiler (手写)
│   ├── CSS Compiler (ANTLR)
│   └── JavaScript Compiler (ANTLR)
├── CHTLCodeMerger (代码合并器)
├── CJMOD System (扩展系统)
└── Module System (模块系统)
```

## 📄 开源协议

MIT License - 详见 `LICENSE` 文件

## 🌐 项目信息

- **项目名称**: CHTL Compiler
- **版本**: v1.0.0
- **构建日期**: 2025年9月3日
- **语言**: C++17
- **架构**: 模块化设计
- **特色**: 中文友好的超文本语言

## 🎉 使用示例

### 基础CHTL
```chtl
text {
    欢迎使用CHTL编译器！
    这是一个强大的超文本语言。
}

style {
    .welcome {
        color: #007bff;
        font-size: 18px;
    }
}
```

### CHTL JS功能
```cjjs
module {
    // CHTL JS独特语法
    {{.button}} -> click {
        animate.fadeIn({{.content}}, 500);
    }
}
```

### 模块使用
```chtl
[Import] chtl::Chtholly

text {
    {{Chtholly.greeting}}
}
```

## 🔧 故障排除

如果遇到问题，请查看 `docs/` 目录下的详细文档，或检查示例文件。

---

**感谢使用CHTL编译器！** 🌸

这是一个专业级的超文本语言编译器，为现代Web开发提供强大的工具支持。