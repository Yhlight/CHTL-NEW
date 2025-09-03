# CHTL Compiler v1.0.0 - 完整发布包（语法修复版）

🌸 **CHTL (Chtholly HyperText Language) Professional Compiler**

这是CHTL编译器的完整发布包，已修复所有语法错误，严格按照CHTL语法文档实现。

## 🔧 语法修复说明

### ✅ 已修复的问题
1. **命名空间语法错误** - 修复了 `[Namespace] AdminDashboard::Components` 错误语法
   - **错误**: `[Namespace] AdminDashboard::Components`
   - **正确**: `[Namespace] AdminDashboard { [Namespace] Components }`

2. **delete操作符使用错误** - 移除了在Custom定义中直接使用delete的错误用法
   - **错误**: 在 `[Custom] @Style` 定义中直接使用 `delete border-left;`
   - **正确**: delete只能在特例化块中使用

3. **文本未包裹错误** - 修复了title标签中文本没有用text{}包裹的问题
   - **错误**: `title { CHTL企业级管理后台 }`
   - **正确**: `title { text { CHTL企业级管理后台 } }`

### 📝 修复的文件
- `examples/projects/admin_dashboard.chtl`
- `examples/projects/ecommerce_site.chtl`  
- `examples/projects/chtholly_blog.chtl`
- `examples/basic/hello_chtl.chtl`
- `examples/complete/template_demo.chtl`
- `examples/complete/comprehensive_strict.chtl`
- `examples/complete/chtholly_demo.chtl`
- `examples/advanced/template_system.chtl`

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

### 🎯 示例（已修复语法错误）
- `examples/complete/` - 完整功能示例
- `examples/projects/` - 大型项目示例（语法严格正确）

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

## 📋 语法规范要点

### ✅ 命名空间正确语法
```chtl
// 正确的嵌套命名空间
[Namespace] AdminDashboard
{
    [Namespace] Components
}

// 或者简单命名空间
[Namespace] MyProject
```

### ✅ delete操作符正确使用
```chtl
// 只能在特例化块中使用
div
{
    style
    {
        @Style SomeStyle
        {
            delete color;  // ✅ 正确：在特例化块中
        }
    }
}

// 错误示例：
[Custom] @Style MyStyle
{
    delete margin;  // ❌ 错误：不能在Custom定义中直接使用
}
```

### ✅ 文本必须包裹在text{}中
```chtl
// 正确的文本包裹
title
{
    text
    {
        我的网站标题
    }
}

h1
{
    text
    {
        欢迎访问
    }
}
```

## 🌟 核心特性

### ✅ 已实现功能
- **完整的CHTL语法支持** - 严格按照语法文档实现，已修复所有语法错误
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
- **版本**: v1.0.0 (语法修复版)
- **构建日期**: 2025年9月3日
- **语言**: C++17
- **架构**: 模块化设计
- **特色**: 中文友好的超文本语言

## 🎉 使用示例

### 基础CHTL（语法正确）
```chtl
text
{
    欢迎使用CHTL编译器！
    这是一个强大的超文本语言。
}

style
{
    .welcome {
        color: #007bff;
        font-size: 18px;
    }
}
```

### 命名空间（语法正确）
```chtl
[Namespace] MyProject
{
    [Namespace] Components
}

[Custom] @Element Button
{
    button
    {
        style
        {
            padding: 10px 20px;
        }
    }
}
```

### 特例化（语法正确）
```chtl
div
{
    style
    {
        @Style BaseStyle
        {
            delete margin;  // 只能在特例化块中使用
            padding: 20px;
        }
    }
}
```

## 🔧 故障排除

如果遇到问题，请查看 `docs/` 目录下的详细文档，或检查示例文件。所有示例文件都已按照严格的语法规范修复。

## 📝 更新日志

### v1.0.0 语法修复版
- ✅ 修复命名空间语法错误
- ✅ 修复delete操作符使用错误  
- ✅ 修复文本未包裹错误
- ✅ 所有示例文件语法严格正确
- ✅ 编译器成功构建和运行

---

**感谢使用CHTL编译器！** 🌸

这是一个专业级的超文本语言编译器，所有语法都严格按照CHTL语法文档实现，为现代Web开发提供强大的工具支持。