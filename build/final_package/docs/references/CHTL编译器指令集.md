# CHTL编译器指令集

## 🎯 指令集概述

CHTL编译器提供完整的命令行界面，支持丰富的编译选项、模块管理、调试功能等。本文档详细说明了所有可用的命令行指令和选项。

---

## 🚀 基础指令

### 📝 编译指令

#### 基础编译
```bash
# 基本语法
chtl <输入文件> [输出文件]

# 示例
chtl input.chtl                    # 编译为 input.html
chtl input.chtl output.html        # 指定输出文件
chtl input.chtl -o result.html     # 使用 -o 参数指定输出
```

#### 批量编译
```bash
# 编译目录下所有CHTL文件
chtl *.chtl --batch

# 编译指定目录
chtl --compile-dir ./src --output-dir ./dist

# 递归编译
chtl --compile-recursive ./project --output ./build
```

---

## 🔧 编译选项

### 📤 输出控制

#### 输出格式
```bash
# HTML输出 (默认)
chtl input.chtl --format html

# 仅CSS输出
chtl input.chtl --format css -o styles.css

# 仅JavaScript输出  
chtl input.chtl --format js -o scripts.js

# 分离输出 (HTML + CSS + JS)
chtl input.chtl --format all --separate
```

#### 输出优化
```bash
# 优化输出代码
chtl input.chtl --optimize

# 压缩输出代码
chtl input.chtl --minify

# 美化输出代码
chtl input.chtl --prettify

# 添加源映射
chtl input.chtl --source-map
```

### 🔍 调试选项

#### 详细输出
```bash
# 详细模式
chtl input.chtl --verbose

# 调试模式
chtl input.chtl --debug

# 显示编译统计
chtl input.chtl --stats

# 显示所有信息
chtl input.chtl --verbose --debug --stats
```

#### 验证模式
```bash
# 仅验证语法，不生成输出
chtl --validate input.chtl

# 验证并显示详细错误
chtl --validate input.chtl --verbose

# 批量验证
chtl --validate *.chtl --summary
```

---

## 🔥 CJMOD选项

### ⚙️ CJMOD控制

#### 启用/禁用CJMOD
```bash
# 启用CJMOD (默认)
chtl input.chtl --enable-cjmod

# 禁用CJMOD
chtl input.chtl --disable-cjmod

# CJMOD调试模式
chtl input.chtl --cjmod-debug
```

#### CJMOD扩展管理
```bash
# 列出可用CJMOD扩展
chtl --list-cjmod-extensions

# 启用特定扩展
chtl input.chtl --cjmod-extension=Chtholly

# 禁用特定扩展
chtl input.chtl --disable-cjmod-extension=CustomExt

# 扩展信息
chtl --cjmod-info Chtholly
```

#### CJMOD性能
```bash
# CJMOD性能分析
chtl input.chtl --cjmod-profile

# CJMOD基准测试
chtl --cjmod-benchmark input.chtl

# CJMOD内存使用分析
chtl input.chtl --cjmod-memory-analysis
```

---

## 📦 模块管理指令

### 📥 模块操作

#### 列出模块
```bash
# 列出所有可用模块
chtl --list-modules

# 列出官方模块
chtl --list-official-modules

# 列出本地模块
chtl --list-local-modules

# 搜索模块
chtl --search-modules chtholly
```

#### 模块信息
```bash
# 显示模块详细信息
chtl --module-info chtl::Chtholly

# 显示模块依赖
chtl --module-deps chtl::Chtholly

# 验证模块
chtl --validate-module ./MyModule/
```

#### 模块路径
```bash
# 添加模块搜索路径
chtl input.chtl -M ./custom-modules

# 多个模块路径
chtl input.chtl -M ./modules1 -M ./modules2

# 设置默认模块路径
chtl --set-module-path ./default-modules
```

### 📦 模块打包

#### CMOD打包
```bash
# 打包CMOD模块
chtl --package-cmod ./MyModule

# 验证CMOD格式
chtl --validate-cmod ./MyModule

# 批量打包
chtl --package-all-cmods ./modules-dir
```

#### CJMOD打包
```bash
# 打包CJMOD模块
chtl --package-cjmod ./MyCJMODModule

# 混合模块打包 (CMOD + CJMOD)
chtl --package-mixed ./ChthollyModule

# 验证CJMOD格式
chtl --validate-cjmod ./MyCJMODModule
```

---

## ⚙️ 配置管理

### 📄 配置文件

#### 使用配置文件
```bash
# 使用指定配置文件
chtl input.chtl --config ./chtl.conf

# 使用默认配置
chtl input.chtl --config default

# 生成默认配置文件
chtl --dump-config > chtl.conf
```

#### 配置文件格式
```ini
# chtl.conf - CHTL编译器配置文件

[General]
output_format = html
optimize_output = true
enable_cjmod = true
verbose = false

[CJMOD]
enable_extensions = true
default_extensions = Chtholly,Yuigahama
debug_mode = false
performance_monitoring = true

[Modules]
search_paths = ./modules,./src/Module,~/.chtl/modules
official_prefix = chtl::
auto_update = false

[Output]
minify = false
prettify = true
add_comments = true
source_maps = false

[Performance]
parallel_compilation = true
max_threads = 0  # 0 = auto-detect
cache_enabled = true
cache_size_mb = 100
```

### 🌐 环境变量

#### 编译器环境变量
```bash
# 模块路径
export CHTL_MODULE_PATH="/path/to/modules:/path/to/official/modules"

# 配置文件
export CHTL_CONFIG_FILE="/path/to/chtl.conf"

# 调试模式
export CHTL_DEBUG=1

# CJMOD设置
export CHTL_ENABLE_CJMOD=1
export CHTL_CJMOD_DEBUG=1

# 性能设置
export CHTL_MAX_THREADS=4
export CHTL_CACHE_SIZE=200
```

---

## 🧪 开发和调试指令

### 🔍 诊断工具

#### 编译器诊断
```bash
# 编译器自诊断
chtl --self-test

# 系统环境检查
chtl --check-environment

# 依赖检查
chtl --check-dependencies

# 性能基准
chtl --benchmark
```

#### 代码分析
```bash
# 语法分析
chtl --analyze input.chtl

# 依赖分析
chtl --analyze-deps input.chtl

# 性能分析
chtl --profile input.chtl

# 内存分析
chtl --memory-profile input.chtl
```

### 🧩 调试功能

#### 断点调试
```bash
# 启用调试模式
chtl input.chtl --debug --breakpoints

# 设置断点
chtl input.chtl --debug --break-at=line:50

# 逐步执行
chtl input.chtl --debug --step-by-step
```

#### 日志控制
```bash
# 设置日志级别
chtl input.chtl --log-level=debug    # debug, info, warn, error

# 日志输出到文件
chtl input.chtl --log-file=compilation.log

# 结构化日志
chtl input.chtl --log-format=json
```

---

## 📊 性能和监控指令

### ⏱️ 性能监控

#### 编译时监控
```bash
# 启用性能监控
chtl input.chtl --monitor-performance

# 内存使用监控
chtl input.chtl --monitor-memory

# 实时性能显示
chtl input.chtl --real-time-stats
```

#### 基准测试
```bash
# 单文件基准
chtl --benchmark-file input.chtl

# 批量基准
chtl --benchmark-batch *.chtl

# 性能回归测试
chtl --regression-test ./test-cases/
```

### 📈 统计报告

#### 编译统计
```bash
# 详细统计报告
chtl input.chtl --detailed-stats

# 统计输出格式
chtl input.chtl --stats-format=json    # json, xml, csv, text

# 保存统计到文件
chtl input.chtl --stats-output=stats.json
```

---

## 🛠️ 高级功能指令

### 🎨 代码生成控制

#### 生成选项
```bash
# 生成HTML5标准代码
chtl input.chtl --html5

# 生成响应式代码
chtl input.chtl --responsive

# 生成可访问性代码
chtl input.chtl --accessibility

# 生成SEO友好代码
chtl input.chtl --seo-optimized
```

#### 目标平台
```bash
# 现代浏览器
chtl input.chtl --target=modern

# 兼容性模式
chtl input.chtl --target=legacy

# 移动端优化
chtl input.chtl --target=mobile

# 特定浏览器
chtl input.chtl --target=chrome --target=firefox
```

### 🔧 编译器配置

#### 编译器行为
```bash
# 严格模式
chtl input.chtl --strict

# 宽松模式
chtl input.chtl --permissive

# 实验性功能
chtl input.chtl --experimental

# 向后兼容
chtl input.chtl --backward-compatible
```

#### 错误处理
```bash
# 错误时停止
chtl input.chtl --halt-on-error

# 继续处理错误
chtl input.chtl --continue-on-error

# 最大错误数
chtl input.chtl --max-errors=10

# 将警告视为错误
chtl input.chtl --warnings-as-errors
```

---

## 📚 帮助和文档指令

### 📖 帮助系统

#### 基础帮助
```bash
# 显示帮助
chtl --help
chtl -h

# 显示版本信息
chtl --version
chtl -v

# 显示编译器信息
chtl --compiler-info
```

#### 详细帮助
```bash
# 特定主题帮助
chtl --help-topic=modules
chtl --help-topic=cjmod
chtl --help-topic=configuration

# 示例帮助
chtl --help-examples

# 快速参考
chtl --quick-reference
```

### 📚 文档访问

#### 内置文档
```bash
# 打开语法文档
chtl --open-syntax-docs

# 打开开发指南
chtl --open-dev-guide

# 打开API文档
chtl --open-api-docs

# 打开模块文档
chtl --open-module-docs chtl::Chtholly
```

---

## 🎪 实用工具指令

### 🔧 代码工具

#### 代码格式化
```bash
# 格式化CHTL代码
chtl --format input.chtl

# 格式化并覆盖原文件
chtl --format input.chtl --in-place

# 批量格式化
chtl --format *.chtl --in-place
```

#### 代码转换
```bash
# CHTL转HTML
chtl --convert input.chtl --to=html

# HTML转CHTL (实验性)
chtl --convert input.html --to=chtl

# 代码重构
chtl --refactor input.chtl --apply=modernize
```

### 🎨 模板工具

#### 模板生成
```bash
# 生成基础模板
chtl --create-template MyTemplate --type=card

# 生成角色主题模板
chtl --create-template MyCharacter --theme=chtholly

# 从现有代码生成模板
chtl --extract-template input.chtl --name=ExtractedTemplate
```

---

## 🎯 高级指令

### 🔬 分析工具

#### 代码分析
```bash
# 复杂度分析
chtl --analyze-complexity input.chtl

# 依赖分析
chtl --analyze-dependencies input.chtl

# 性能热点分析
chtl --analyze-hotspots input.chtl

# 代码质量分析
chtl --analyze-quality *.chtl
```

#### 统计工具
```bash
# 代码统计
chtl --count-lines *.chtl
chtl --count-elements input.chtl
chtl --count-templates input.chtl

# 模块使用统计
chtl --module-usage-stats ./project/

# 特征使用统计
chtl --feature-usage-stats input.chtl
```

### 🎮 交互模式

#### REPL模式
```bash
# 启动交互式编译器
chtl --interactive
chtl --repl

# 在REPL中的指令
> compile <div>Hello</div>
> template MyCard { background: white; }
> use MyCard
> exit
```

#### 监视模式
```bash
# 监视文件变化并自动编译
chtl --watch input.chtl

# 监视目录
chtl --watch-dir ./src --output-dir ./dist

# 实时预览
chtl --watch input.chtl --serve --port=3000
```

---

## 🎨 专业功能指令

### 🏗️ 项目管理

#### 项目初始化
```bash
# 创建新CHTL项目
chtl --init-project MyProject

# 使用模板创建项目
chtl --init-project MyProject --template=chtholly-theme

# 创建模块项目
chtl --init-module MyModule --type=cjmod
```

#### 项目构建
```bash
# 构建整个项目
chtl --build-project ./project/

# 增量构建
chtl --build-incremental ./project/

# 清理构建
chtl --clean-build ./project/
```

### 📊 质量控制

#### 代码检查
```bash
# 语法检查
chtl --lint input.chtl

# 风格检查
chtl --style-check input.chtl

# 最佳实践检查
chtl --best-practices-check input.chtl

# 安全检查
chtl --security-check input.chtl
```

#### 测试工具
```bash
# 运行内置测试
chtl --run-tests

# 测试特定功能
chtl --test-feature cjmod
chtl --test-feature modules

# 性能测试
chtl --performance-test input.chtl
```

---

## 🎯 指令组合示例

### 🚀 常用组合

#### 开发模式
```bash
# 开发时的完整编译
chtl input.chtl --verbose --debug --stats --cjmod-debug --watch

# 快速验证
chtl --validate input.chtl --verbose --quick
```

#### 生产构建
```bash
# 生产环境编译
chtl input.chtl --optimize --minify --enable-cjmod --stats

# 发布包构建
chtl --build-project ./project/ --release --package
```

#### 调试分析
```bash
# 完整调试分析
chtl input.chtl \
    --debug \
    --verbose \
    --cjmod-debug \
    --analyze-complexity \
    --analyze-dependencies \
    --profile \
    --log-file=debug.log
```

---

## 🔧 退出代码

### 📊 退出状态码

| 代码 | 含义 | 说明 |
|------|------|------|
| 0 | 成功 | 编译成功完成 |
| 1 | 一般错误 | 编译失败或参数错误 |
| 2 | 语法错误 | 源代码语法错误 |
| 3 | 文件错误 | 输入文件不存在或无法读取 |
| 4 | 输出错误 | 无法创建输出文件 |
| 5 | 模块错误 | 模块加载或处理失败 |
| 6 | CJMOD错误 | CJMOD扩展处理失败 |
| 7 | 配置错误 | 配置文件错误或参数冲突 |
| 8 | 系统错误 | 系统资源不足或权限问题 |
| 9 | 内部错误 | 编译器内部错误 |
| 10 | 超时错误 | 编译超时 |

### 🔍 错误诊断

```bash
# 根据退出代码诊断问题
case $? in
    0) echo "✅ 编译成功" ;;
    1) echo "❌ 一般错误，检查参数和输入" ;;
    2) echo "❌ 语法错误，检查CHTL语法" ;;
    3) echo "❌ 文件错误，检查文件路径和权限" ;;
    4) echo "❌ 输出错误，检查输出目录权限" ;;
    5) echo "❌ 模块错误，检查模块路径和格式" ;;
    6) echo "❌ CJMOD错误，检查CJMOD扩展" ;;
    7) echo "❌ 配置错误，检查配置文件" ;;
    8) echo "❌ 系统错误，检查系统资源" ;;
    9) echo "❌ 内部错误，请报告Bug" ;;
    10) echo "❌ 编译超时，尝试简化代码或增加超时时间" ;;
esac
```

---

## 🎨 输出格式控制

### 📄 HTML输出选项

```bash
# HTML5标准
chtl input.chtl --html5-standard

# 兼容模式HTML
chtl input.chtl --html-compatible

# 内联样式
chtl input.chtl --inline-styles

# 外部样式
chtl input.chtl --external-styles

# 响应式HTML
chtl input.chtl --responsive-html
```

### 🎨 CSS输出选项

```bash
# CSS3特性
chtl input.chtl --css3-features

# 浏览器前缀
chtl input.chtl --css-prefixes

# CSS变量
chtl input.chtl --css-variables

# CSS网格布局
chtl input.chtl --css-grid
```

### ⚡ JavaScript输出选项

```bash
# ES6+特性
chtl input.chtl --es6-features

# 兼容模式JavaScript
chtl input.chtl --js-compatible

# 模块化JavaScript
chtl input.chtl --js-modules

# 压缩JavaScript
chtl input.chtl --js-minify
```

---

## 🎊 完整指令参考

### 📋 所有可用选项

```bash
chtl [选项] <输入文件> [输出文件]

基本选项:
  -h, --help                    显示帮助信息
  -v, --version                 显示版本信息
  -V, --verbose                 详细输出模式
  -d, --debug                   调试模式
  -q, --quiet                   静默模式
  --validate                    仅验证语法
  --stats                       显示编译统计

输出选项:
  -o, --output <文件>           指定输出文件
  --format <格式>               输出格式 (html|css|js|all)
  --optimize                    优化输出代码
  --minify                      压缩输出代码
  --prettify                    美化输出代码
  --source-map                  生成源映射

CJMOD选项:
  --enable-cjmod                启用CJMOD扩展 (默认)
  --disable-cjmod               禁用CJMOD扩展
  --cjmod-debug                 CJMOD调试模式
  --cjmod-extension=<名称>      启用特定CJMOD扩展
  --list-cjmod-extensions       列出可用CJMOD扩展

模块选项:
  -M, --module-path <路径>      添加模块搜索路径
  --list-modules                列出可用模块
  --module-info <模块>          显示模块信息
  --package-cmod <路径>         打包CMOD模块
  --package-cjmod <路径>        打包CJMOD模块

配置选项:
  -c, --config <文件>           使用配置文件
  --dump-config                 输出默认配置
  --set-option <键>=<值>        设置配置选项

调试选项:
  --analyze                     代码分析
  --profile                     性能分析
  --memory-profile              内存分析
  --benchmark                   性能基准测试
  --self-test                   编译器自测

高级选项:
  --target=<平台>               目标平台
  --html5                       HTML5标准输出
  --responsive                  响应式输出
  --accessibility               可访问性输出
  --experimental                实验性功能
  --parallel=<数量>             并行编译线程数
  --cache-size=<大小>           缓存大小 (MB)
```

---

## 🎉 总结

CHTL编译器指令集提供了：

- 🔥 **完整的编译控制** - 从基础编译到高级优化
- 🔥 **强大的CJMOD支持** - 完整的CJMOD扩展管理
- 🔥 **专业的模块管理** - CMOD/CJMOD模块的完整生命周期
- 🔥 **丰富的调试工具** - 性能分析、代码诊断、错误追踪
- 🔥 **灵活的配置系统** - 配置文件和环境变量支持
- 🔥 **友好的用户体验** - 详细的帮助和错误信息

通过完整的指令集，开发者可以充分利用CHTL编译器的所有强大功能，从简单的代码编译到复杂的项目构建，都能得到专业级的支持。

**🌟 掌握CHTL指令集，释放编译器的全部潜能！**