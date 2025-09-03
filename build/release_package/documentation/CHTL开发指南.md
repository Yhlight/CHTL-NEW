# CHTL开发指南

## 🚀 快速开始

### 📋 环境要求

- **操作系统**: Windows 10/11, Linux (Ubuntu 18+), macOS 10.14+
- **编译器**: Visual Studio 2019/2022 (Windows), GCC 8+ (Linux), Clang 10+ (macOS)
- **CMake**: 3.16 或更高版本
- **Node.js**: 14+ (用于VSCode插件开发)

### 🛠️ 安装CHTL编译器

#### Windows用户
```cmd
# 下载发布包
# 运行安装脚本
install.bat

# 或手动添加到PATH
set PATH=%PATH%;C:\path\to\chtl\bin
```

#### Linux/macOS用户
```bash
# 下载发布包
# 运行安装脚本
./install.sh

# 或手动添加到PATH
export PATH="/path/to/chtl/bin:$PATH"
```

---

## 📝 基础语法

### 🏷️ HTML元素

```chtl
// 基础元素
div {
    class = "container";
    id = "main";
    
    h1 { "欢迎使用CHTL" }
    p { "这是一个段落" }
}

// 嵌套结构
html {
    head {
        title { "CHTL页面" }
        meta { charset="UTF-8" }
    }
    
    body {
        div { class = "content"; }
    }
}
```

### 🎨 样式系统

```chtl
// 局部样式块
style {
    .container {
        max-width: 1200px;
        margin: 0 auto;
        padding: 20px;
    }
    
    h1 {
        color: #333;
        font-size: 2em;
    }
}
```

### 📜 脚本系统

```chtl
// 局部脚本块
script {
    console.log('CHTL脚本运行');
    
    document.addEventListener('DOMContentLoaded', function() {
        console.log('页面加载完成');
    });
}
```

---

## 🎯 模板系统

### 📋 模板定义

```chtl
[Template]
<ButtonStyle> {
    background: #007bff;
    color: white;
    border: none;
    padding: 10px 20px;
    border-radius: 4px;
    cursor: pointer;
}

<CardStyle> {
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    padding: 20px;
}
```

### 🔗 模板继承

```chtl
[Custom]
<PrimaryButton inherit="ButtonStyle"> {
    background: #28a745;
    font-weight: bold;
}

<SpecialCard inherit="CardStyle"> {
    border-left: 4px solid #007bff;
    background: #f8f9fa;
}
```

### 🎨 模板使用

```chtl
<button use="PrimaryButton">点击我</button>
<div use="SpecialCard">
    <h3>特殊卡片</h3>
    <p>使用了模板的卡片内容</p>
</div>
```

---

## ⚙️ 配置系统

### 🔧 全局配置

```chtl
[Configuration] {
    DISABLE_STYLE_AUTO_ADD_CLASS = false;
    DISABLE_STYLE_AUTO_ADD_ID = false;
    DISABLE_SCRIPT_AUTO_ADD_CLASS = false;
    ENABLE_CONTEXT_DEDUCTION = true;
    THEME_COLOR = "#007bff";
}
```

### 🎛️ 选择器自动化

```chtl
// 自动添加class和id
style {
    .auto-generated-class {
        // CHTL会自动为相关元素添加这个class
    }
}

// 禁用自动化
[Configuration] {
    DISABLE_STYLE_AUTO_ADD_CLASS = true;
}
```

---

## 📦 模块系统

### 📥 导入模块

```chtl
// 导入官方模块
[Import] @chtl::Chtholly as Chtholly
[Import] @chtl::Yuigahama as Yui

// 导入自定义模块
[Import] ./components/Button.chtl
[Import] ../styles/theme.css
```

### 🏷️ 命名空间

```chtl
[Namespace] MyProject::Components

// 嵌套命名空间
[Namespace] MyProject::Components::Buttons

// 禁用默认命名空间
[Namespace] "" // 空命名空间禁用默认
```

---

## 🔒 约束系统

### ⚡ 精确约束

```chtl
// 精确约束
div {
    class = "container";
    except {
        // 在mobile环境下不应用某些样式
        @media (max-width: 768px) {
            padding: 10px;
        }
    }
}
```

### 🎯 类型约束

```chtl
// 类型约束
[Template]
<ButtonStyle> {
    except type="submit" {
        background: #dc3545; // 提交按钮使用红色
    }
}
```

---

## 🎮 最佳实践

### 📁 项目结构

```
my-chtl-project/
├── src/
│   ├── pages/          # 页面文件
│   ├── components/     # 组件模板
│   ├── styles/         # 样式文件
│   └── scripts/        # 脚本文件
├── modules/            # 自定义模块
├── assets/             # 静态资源
└── build/              # 构建输出
```

### 🎨 代码风格

```chtl
// 推荐的代码风格
html {
    head {
        title { "页面标题" }
        meta { charset="UTF-8" }
    }
    
    body {
        // 使用有意义的class名
        div {
            class = "main-container";
            
            // 保持适当的缩进
            header {
                class = "page-header";
                h1 { "主标题" }
            }
            
            main {
                class = "page-content";
                // 内容区域
            }
        }
    }
}
```

### 🔧 性能优化

```chtl
// 使用配置优化
[Configuration] {
    ENABLE_CONTEXT_DEDUCTION = true;  // 启用上下文推导
    DISABLE_UNUSED_STYLES = true;     // 禁用未使用的样式
}

// 合理使用模板
[Template]
<CommonButton> {
    // 定义通用按钮样式
}

// 避免过度嵌套
div {
    // 保持合理的嵌套层级
}
```

---

## 🐛 调试技巧

### 📊 编译信息

```bash
# 显示详细编译信息
chtl --verbose input.chtl output.html

# 显示调试信息
chtl --debug input.chtl output.html

# 验证语法
chtl --validate input.chtl
```

### 🔍 常见问题

1. **编译错误** - 检查语法是否正确
2. **模板不生效** - 确认模板定义和使用语法
3. **中文字符问题** - 确保文件保存为UTF-8编码
4. **导入失败** - 检查文件路径是否正确

---

## 📚 进阶主题

### 🌟 CJMOD开发

参考《CJMOD开发指南》了解如何：
- 创建自定义CJMOD扩展
- 使用C++ API
- 开发官方模块

### 🎭 角色模块开发

参考《珂朵莉模块使用文档》和《由比滨结衣模块使用文档》了解：
- 官方模块使用方法
- 角色特色功能
- 模块定制技巧

### 🔧 编译器定制

参考《统一扫描器架构说明》了解：
- 编译器内部架构
- 扩展编译器功能
- 性能优化技巧

---

## 🎊 总结

CHTL为Web开发提供了一种全新的方式，通过强大的模板系统、灵活的配置选项、创新的双语言架构和极为强大的CJMOD扩展特征，让开发者能够更加高效地创建现代Web应用。

**🌟 开始您的CHTL开发之旅，体验前所未有的Web开发体验！**