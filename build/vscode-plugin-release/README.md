# CHTL Language Support

官方CHTL (Chtholly HyperText Language) VSCode IDE支持插件

## 功能特性

### 🌟 核心功能

- **实时预览**: 支持CHTL文件的实时预览，保存时自动更新
- **内置编译器**: 集成官方CHTL编译器和模块系统
- **智能提示**: 完整的语法提示、自动补全和错误检查
- **代码格式化**: 支持C++风格和JavaScript风格的代码格式化
- **语法高亮**: 完整的CHTL和CHTL JS语法高亮支持

### 🎯 右键菜单功能

- **右键编译**: 快速编译CHTL文件
- **右键浏览器打开**: 编译并在浏览器中预览
- **右键格式化**: 智能代码格式化
- **右键打开文档**: 快速查看语法文档
- **右键导出**: 支持导出HTML、CSS、JavaScript

### 🔧 高级功能

- **模块智能提示**: 自动解包CMOD/CJMOD模块，生成JSON查询表
- **CJMOD语法分析**: 自动扫描syntax、scan、createCHTLJSFunction方法
- **括号冲突解决**: 智能处理VSCode自动补齐[]与CHTL语法的冲突
- **多语言支持**: 完整支持CHTL (.chtl) 和 CHTL JS (.cjjs) 文件

## 安装要求

- VSCode 1.74.0 或更高版本
- CHTL编译器 (可选，插件包含内置编译器)

## 配置选项

```json
{
    "chtl.compilerPath": "",                    // CHTL编译器路径
    "chtl.enableRealTimePreview": true,         // 启用实时预览
    "chtl.previewPort": 3000,                   // 预览服务器端口
    "chtl.formatStyle": "cpp",                  // 格式化风格: cpp | js
    "chtl.enableIntelliSense": true,            // 启用智能提示
    "chtl.officialModulesPath": ""              // 官方模块路径
}
```

## 快捷键

- `Ctrl+Shift+C`: 编译当前文件
- `Ctrl+Shift+F`: 格式化代码
- `Ctrl+Shift+P`: 切换实时预览

## 语言支持

### CHTL (.chtl)
- 完整的HTML元素支持
- 局部样式块和脚本块
- 特殊语法块: [Import], [Namespace], [Configuration]等
- 模块导入和导出

### CHTL JS (.cjjs)
- JavaScript语法基础
- CHTL JS特殊语法: {{选择器}}, ->, &->
- 增强功能: listen, delegate, animate, vir
- 模块化支持: module{}

## 模块系统

### CMOD模块
- 自动扫描[Export]导出表
- 智能提示可用的导出项
- 支持嵌套模块和子模块

### CJMOD模块
- 自动分析C++ API函数
- 提取syntax语法模式
- 生成完整的函数签名提示

## 开发指南

1. 克隆仓库
2. 安装依赖: `npm install`
3. 编译: `npm run compile`
4. 调试: 按F5启动调试会话

## 版本历史

### 1.0.0
- 初始版本发布
- 完整的CHTL语言支持
- 实时预览功能
- 模块智能提示

## 支持

如需帮助或报告问题，请访问:
- [官方文档](https://chtl.org/docs)
- [GitHub仓库](https://github.com/chtl-lang/vscode-extension)
- [问题反馈](https://github.com/chtl-lang/vscode-extension/issues)

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件

---

**享受使用CHTL进行开发！** 🚀