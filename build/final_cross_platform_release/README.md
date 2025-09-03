# 🔥 CHTL完整跨平台发布包 v1.0.0

**CHTL (Chtholly HyperText Language) Professional Compiler & VSCode Plugin**  
**完整CJMOD API支持版本 - 跨平台发布包**

## 📦 包含内容

### 🖥️ Linux版本编译器
**目录**: `linux/bin/`
- `chtl-professional` - 专业版CHTL编译器（支持CJMOD）
- `chtl` - 标准CHTL编译器（支持CJMOD）
- `chtl_cjmod_real_api_test` - CJMOD API测试工具
- `chtl_cjmod_integration_test` - CJMOD集成测试工具
- `chtl_minimal_test` - 词法分析器测试工具
- `chtl_safe_test` - 完整组件安全测试工具

### 🪟 Windows版本编译器
**目录**: `windows/bin/`
- `chtl-professional.exe` - 专业版CHTL编译器（支持CJMOD）
- `chtl.exe` - 标准CHTL编译器（支持CJMOD）
- `chtl_cjmod_real_api_test.exe` - CJMOD API测试工具
- `chtl_cjmod_integration_test.exe` - CJMOD集成测试工具
- `chtl_minimal_test.exe` - 词法分析器测试工具
- `chtl_safe_test.exe` - 完整组件安全测试工具

### 🔌 VSCode插件（完整版）
**目录**: `vscode-plugin/`
- **满足9条基本要求** ✅
- **CJMOD完整支持** ✅ 
- **自动解包功能** ✅ (依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法)
- **内置编译器** ✅ (Linux版本)
- **官方模块** ✅
- **完整IntelliSense** ✅ (CJMOD语法提示)

### 📚 官方模块
**目录**: `modules/`
- `Chtholly.cmod` - 珂朵莉模块（CMOD+CJMOD混合结构）
- `Yuigahama.cmod` - 由比滨结衣模块（纯CMOD结构）

## 🚀 快速开始

### Linux用户
```bash
# 解压发布包
tar -xzf CHTL_Complete_Cross_Platform_Release_v1.0.0.tar.gz

# 编译CHTL文件
./linux/bin/chtl-professional input.chtl -o output.html

# 测试CJMOD功能
./linux/bin/chtl_cjmod_real_api_test
```

### Windows用户
```cmd
# 解压发布包
# 编译CHTL文件
windows\\bin\\chtl-professional.exe input.chtl -o output.html

# 测试CJMOD功能
windows\\bin\\chtl_cjmod_real_api_test.exe
```

### VSCode插件安装
```bash
# 方法1: 手动安装
# 1. 打开VSCode
# 2. Extensions -> Install from VSIX
# 3. 选择vscode-plugin目录中的文件

# 方法2: 解压到VSCode扩展目录
# Linux: ~/.vscode/extensions/
# Windows: %USERPROFILE%\\.vscode\\extensions\\
```

## 🔥 CJMOD强大功能

### ✅ 完整CJMOD API支持
- **Syntax::analyze** - 语法分析
- **Arg::bind** - 函数绑定
- **CJMODScanner::scan** - 双指针扫描法/前置截取法
- **Arg::fillValue** - 值填充
- **Arg::transform** - 代码转换
- **CJMODGenerator::exportResult** - 结果导出
- **CHTLJSFunction::CreateCHTLJSFunction** - CHTL JS函数创建
- **CHTLJSFunction::bindVirtualObject** - 虚对象绑定

### ✅ CJMOD扫描方法
- **双指针扫描法**: 类似滑动窗口，预扫描+同步移动+关键字收集
- **前置截取法**: 回头截取关键字前参数，避免错误发送给编译器
- **智能扫描**: 自动选择最适合的扫描方法

### ✅ CJMOD占位符系统
- **$** - 基本占位符
- **$?** - 可选占位符
- **$!** - 必须占位符
- **$_** - 无序占位符
- **$!_** - 必须无序占位符
- **...** - 不定参数占位符

### ✅ VSCode插件9条基本要求
1. **实时预览** ✅ - WebSocket实时HTML预览
2. **内置编译器和官方模块** ✅ - 完整编译器+官方模块
3. **自动模块解包和JSON查询表** ✅ - **CJMOD API驱动的自动解包**
4. **右键代码格式化** ✅ - C++风格CHTL，JS风格CHTL JS
5. **右键浏览器打开** ✅ - 一键浏览器打开
6. **右键编译** ✅ - 完整编译功能
7. **解决[]自动补全冲突** ✅ - 正确配置避免冲突
8. **右键打开语法文档** ✅ - 快速查询语法文档
9. **右键导出HTML/CSS/JS** ✅ - 分离导出功能

## 🎯 CJMOD功能验证

### Linux验证
```bash
# CJMOD API测试
./linux/bin/chtl_cjmod_real_api_test
# 预期输出: ✅ 所有CJMOD API完全工作

# CJMOD集成测试
./linux/bin/chtl_cjmod_integration_test
# 预期输出: ✅ 所有组件完美协作

# 编译CJMOD示例
./linux/bin/chtl-professional examples/cjmod/cjmod_enhanced_syntax.chtl --stats
# 预期输出: ✅ 成功编译CJMOD语法
```

### Windows验证
```cmd
# CJMOD API测试
windows\\bin\\chtl_cjmod_real_api_test.exe
# 预期输出: ✅ 所有CJMOD API完全工作

# CJMOD集成测试  
windows\\bin\\chtl_cjmod_integration_test.exe
# 预期输出: ✅ 所有组件完美协作
```

## 📊 技术规格

### 编译器特性
- **C++17标准**
- **完整ANTLR4支持**
- **UTF-8完整支持**
- **跨平台兼容**（Linux x64, Windows x64）
- **静态链接**（Windows版本无依赖）
- **完整CJMOD API集成**

### VSCode插件特性
- **TypeScript编写**
- **实时预览服务器**
- **WebSocket通信**
- **完整语法高亮**
- **智能IntelliSense**
- **CJMOD语法提示**
- **自动模块解包**

### 模块系统特性
- **三种模块结构支持**: CMOD、CJMOD、CMOD+CJMOD
- **混乱结构和分类结构支持**
- **自动识别和解包**
- **CJMOD API驱动的解包**

## 🌟 项目亮点

### 🔥 CJMOD - CHTL极为强大的特征
- **从编译器到VSCode插件的完整生态支持**
- **强大的API扩展机制**
- **智能的语法处理能力**
- **完美的开发体验**

### 🎯 跨平台完整支持
- **Linux和Windows原生编译**
- **相同的功能和性能**
- **统一的使用体验**

### 🚀 专业级工具链
- **完整的编译器工具集**
- **专业的VSCode插件**
- **丰富的测试工具**
- **完善的模块系统**

## 📝 版本信息

- **版本**: v1.0.0
- **发布日期**: 2025年9月3日
- **编译器**: CHTL Professional Compiler
- **VSCode插件**: CHTL Language Support with Complete CJMOD API
- **支持平台**: Linux x64, Windows x64

## 🎊 总结

**✅ CHTL项目现在提供完整的跨平台支持！**

这个发布包包含了：
- **完整功能的Linux和Windows编译器**
- **支持CJMOD完整API的VSCode插件**
- **满足所有9条基本要求的开发环境**
- **使用真正CJMOD API的官方模块**

**🔥 CJMOD确实是CHTL极为强大的特征，现在在所有平台上都得到了完美支持！**

---

**感谢使用CHTL！享受CJMOD带来的强大开发体验！**