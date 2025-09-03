# CMOD开发指南

## 📦 CMOD模块系统概述

**CMOD (CHTL Module)** 是CHTL提供的模块化方式，允许开发者创建可重用的CHTL组件和样式。CMOD模块可以包含模板、自定义元素、样式组等，支持继承、特例化等高级功能。

---

## 🏗️ CMOD标准格式

### 📁 目录结构

```
ModuleName/
├── src/
│   ├── ModuleName.chtl      # 主模块文件（可选，如有子模块）
│   ├── Component1.chtl      # 组件文件
│   ├── Component2.chtl      # 组件文件
│   └── ...                  # 其他CHTL文件
└── info/
    └── ModuleName.chtl      # 模块信息文件（必需）
```

### 📋 命名规范

- **模块文件夹名** = **主模块文件名** = **模块信息文件名**
- 必须同名，确保模块识别正确
- 推荐使用PascalCase命名法

---

## 📝 模块信息文件

### 🔧 info/ModuleName.chtl

```chtl
// 模块信息定义
[Info] {
    name = "Chtholly";
    version = "1.0.0";
    description = "珂朵莉主题模块 - 世界上最幸福的女孩";
    author = "CHTL Team";
    license = "MIT";
    dependencies = "";
    category = "theme";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
    keywords = ["theme", "character", "chtholly", "romantic"];
    homepage = "https://chtl-official.github.io/modules/chtholly";
}

// 外部查询表（由系统自动生成，无需手写）
[Export] {
    [Template] @Style ChthollyCard, ChthollyButton, ChthollyHeader;
    [Custom] @Element Accordion, FourLeafAlbum;
    [Custom] @Variable $chtholly-primary, $chtholly-secondary;
}
```

### 📊 Info字段说明

| 字段 | 必需 | 说明 |
|------|------|------|
| `name` | ✅ | 模块名称，必须与文件夹名相同 |
| `version` | ✅ | 版本号，推荐语义化版本 |
| `description` | ✅ | 模块描述 |
| `author` | ✅ | 作者信息 |
| `license` | ✅ | 开源协议 |
| `dependencies` | ❌ | 依赖模块列表 |
| `category` | ❌ | 模块分类 |
| `minCHTLVersion` | ❌ | 最小CHTL版本要求 |
| `maxCHTLVersion` | ❌ | 最大CHTL版本支持 |
| `keywords` | ❌ | 关键词标签 |
| `homepage` | ❌ | 模块主页 |

---

## 🎨 模块内容开发

### 🏷️ 模板定义

```chtl
// src/Chtholly.chtl - 主模块文件
[Template]
<ChthollyCard> {
    background: linear-gradient(135deg, #ffe8e8 0%, #ffcccb 100%);
    border-radius: 12px;
    border-left: 4px solid #ff6b6b;
    padding: 20px;
    margin: 15px 0;
    box-shadow: 0 4px 20px rgba(255, 107, 107, 0.2);
    transition: all 0.3s ease;
    position: relative;
    overflow: hidden;
}

<ChthollyButton> {
    background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
    color: white;
    border: none;
    padding: 12px 24px;
    border-radius: 25px;
    font-weight: bold;
    cursor: pointer;
    transition: all 0.3s ease;
    box-shadow: 0 4px 15px rgba(255, 107, 107, 0.3);
}

<ChthollyHeader> {
    background: linear-gradient(135deg, #ff6b6b, #4ecdc4, #ffd93d);
    color: white;
    text-align: center;
    padding: 40px 20px;
    font-size: 2.5em;
    text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
    position: relative;
}

// 变量组模板
<ChthollyColors> {
    $primary: #ff6b6b;      // 珂朵莉的红发色
    $secondary: #4ecdc4;    // 珂朵莉的眼睛色
    $accent: #ffd93d;       // 温暖的黄色
    $text: #2c3e50;         // 深色文字
    $background: #fff5f5;   // 淡粉背景
}
```

### 🎭 自定义元素

```chtl
// src/Components.chtl - 组件文件
[Custom]
<ChthollySpecialCard inherit="ChthollyCard"> {
    // 特例化操作
    box-shadow: 0 8px 32px rgba(255, 107, 107, 0.4);
    transform: scale(1.02);
    
    // 添加特殊效果
    &:before {
        content: "🌸";
        position: absolute;
        top: 10px;
        right: 15px;
        font-size: 1.2em;
        opacity: 0.7;
    }
}

// 无值样式组（用于重置）
<ChthollyReset> {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: 'Microsoft YaHei', sans-serif;
}
```

---

## 🎪 高级功能

### 🔗 模块继承

```chtl
// BaseTheme.chtl - 基础主题模块
[Template]
<BaseCard> {
    border-radius: 8px;
    padding: 20px;
    margin: 10px 0;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

// ChthollyTheme.chtl - 继承基础主题
[Import] ./BaseTheme.chtl

[Custom]
<ChthollyCard inherit="BaseCard"> {
    border-left: 4px solid #ff6b6b;
    background: linear-gradient(135deg, #fff 0%, #ffe8e8 100%);
}
```

### 🎯 条件模块

```chtl
// 条件加载模块内容
[Configuration] {
    THEME_MODE = "chtholly";
    ENABLE_ANIMATIONS = true;
}

[Template]
<ConditionalCard> {
    // 基础样式
    padding: 20px;
    border-radius: 8px;
    
    // 条件样式
    except when THEME_MODE == "chtholly" {
        border-left: 4px solid #ff6b6b;
        background: linear-gradient(135deg, #fff 0%, #ffe8e8 100%);
    }
    
    except when ENABLE_ANIMATIONS == true {
        transition: all 0.3s ease;
        
        &:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 25px rgba(0,0,0,0.15);
        }
    }
}
```

---

## 🌸 珂朵莉模块示例

### 📸 四叶相册组件

```chtl
// src/FourLeafAlbum.chtl
[Template]
<FourLeafAlbumContainer> {
    display: grid;
    grid-template-areas: 
        "top-left top-right"
        "bottom-left bottom-right";
    grid-template-columns: 1fr 1fr;
    grid-template-rows: 1fr 1fr;
    gap: 10px;
    width: 400px;
    height: 400px;
    border-radius: 50%;
    overflow: hidden;
    position: relative;
    box-shadow: 0 8px 32px rgba(255, 107, 107, 0.3);
}

<FourLeafAlbumItem> {
    background-size: cover;
    background-position: center;
    transition: all 0.5s ease;
    cursor: pointer;
    
    &:hover {
        transform: scale(1.1);
        z-index: 10;
        border-radius: 8px;
        box-shadow: 0 4px 20px rgba(0,0,0,0.3);
    }
}

// 四个象限的特殊样式
<FourLeafTopLeft inherit="FourLeafAlbumItem"> {
    grid-area: top-left;
    border-radius: 100% 0 0 0;
}

<FourLeafTopRight inherit="FourLeafAlbumItem"> {
    grid-area: top-right;
    border-radius: 0 100% 0 0;
}

<FourLeafBottomLeft inherit="FourLeafAlbumItem"> {
    grid-area: bottom-left;
    border-radius: 0 0 0 100%;
}

<FourLeafBottomRight inherit="FourLeafAlbumItem"> {
    grid-area: bottom-right;
    border-radius: 0 0 100% 0;
}
```

### 🎵 手风琴组件

```chtl
// src/Accordion.chtl
[Template]
<AccordionContainer> {
    border: 1px solid #e9ecef;
    border-radius: 8px;
    overflow: hidden;
    background: white;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

<AccordionHeader> {
    background: linear-gradient(135deg, #ff6b6b, #4ecdc4);
    color: white;
    padding: 15px 20px;
    cursor: pointer;
    transition: all 0.3s ease;
    position: relative;
    
    &:hover {
        background: linear-gradient(135deg, #ff5252, #26c6da);
    }
    
    &:after {
        content: "▼";
        position: absolute;
        right: 20px;
        top: 50%;
        transform: translateY(-50%);
        transition: transform 0.3s ease;
    }
    
    &.active:after {
        transform: translateY(-50%) rotate(180deg);
    }
}

<AccordionContent> {
    padding: 0 20px;
    max-height: 0;
    overflow: hidden;
    transition: all 0.3s ease;
    background: #f8f9fa;
    
    &.active {
        padding: 20px;
        max-height: 500px;
    }
}
```

---

## 🎀 由比滨结衣模块示例

### 📝 备忘录组件

```chtl
// src/Memo.chtl
[Template]
<MemoContainer> {
    background: white;
    border-radius: 12px;
    box-shadow: 0 4px 20px rgba(255, 105, 180, 0.2);
    overflow: hidden;
    border-left: 4px solid #ff69b4;
}

<MemoHeader> {
    background: linear-gradient(135deg, #ff69b4, #ffc0cb);
    color: white;
    padding: 20px;
    text-align: center;
}

<MemoItem> {
    padding: 15px 20px;
    border-bottom: 1px solid #f0f0f0;
    transition: all 0.3s ease;
    cursor: pointer;
    
    &:hover {
        background: #fef7f7;
        transform: translateX(5px);
    }
    
    &:last-child {
        border-bottom: none;
    }
}

<MemoDate> {
    font-size: 0.9em;
    color: #666;
    margin-bottom: 5px;
}

<MemoContent> {
    line-height: 1.6;
    color: #333;
}

<MemoMood> {
    display: inline-block;
    padding: 4px 8px;
    border-radius: 12px;
    font-size: 0.8em;
    margin-top: 8px;
}

// 心情样式变体
<MemoMoodHappy inherit="MemoMood"> {
    background: #fff3cd;
    color: #856404;
}

<MemoMoodSad inherit="MemoMood"> {
    background: #d1ecf1;
    color: #0c5460;
}

<MemoMoodExcited inherit="MemoMood"> {
    background: #f8d7da;
    color: #721c24;
}
```

---

## 🔧 模块开发工作流

### 1️⃣ 创建模块结构

```bash
# 创建模块目录
mkdir MyModule
mkdir MyModule/src
mkdir MyModule/info

# 创建主文件
touch MyModule/src/MyModule.chtl
touch MyModule/info/MyModule.chtl
```

### 2️⃣ 编写模块信息

```chtl
// MyModule/info/MyModule.chtl
[Info] {
    name = "MyModule";
    version = "1.0.0";
    description = "我的自定义CHTL模块";
    author = "Your Name";
    license = "MIT";
    category = "custom";
    minCHTLVersion = "1.0.0";
}
```

### 3️⃣ 开发模块内容

```chtl
// MyModule/src/MyModule.chtl
[Template]
<MyCard> {
    background: white;
    border-radius: 8px;
    padding: 20px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

<MyButton> {
    background: #007bff;
    color: white;
    border: none;
    padding: 10px 20px;
    border-radius: 4px;
    cursor: pointer;
}

[Custom]
<MySpecialCard inherit="MyCard"> {
    border-left: 4px solid #007bff;
    background: #f8f9fa;
}
```

### 4️⃣ 测试模块

```chtl
// test.chtl - 测试文件
[Import] ./MyModule

<div use="MyCard">
    <h3>测试卡片</h3>
    <p>这是使用MyCard模板的内容</p>
    <button use="MyButton">测试按钮</button>
</div>

<div use="MySpecialCard">
    <h3>特殊卡片</h3>
    <p>这是使用MySpecialCard的内容</p>
</div>
```

### 5️⃣ 打包模块

```bash
# 使用CMOD打包脚本
./scripts/package_cmod.sh

# 查看打包结果
ls build/packages/cmod/MyModule.cmod
```

---

## 🎯 高级模块功能

### 🔗 模块依赖

```chtl
// 依赖其他模块
[Info] {
    name = "ExtendedModule";
    dependencies = "BaseModule@1.0.0, UtilModule@^2.0.0";
}

// 在模块中导入依赖
[Import] @BaseModule
[Import] @UtilModule as Utils

[Custom]
<ExtendedCard inherit="BaseModule::BaseCard"> {
    // 扩展基础卡片
    border-top: 2px solid Utils::$accent-color;
}
```

### 🎨 主题模块

```chtl
// 主题模块示例
[Template]
// 颜色变量组
<ThemeColors> {
    $primary: #ff6b6b;
    $secondary: #4ecdc4;
    $accent: #ffd93d;
    $text: #2c3e50;
    $background: #f8f9fa;
    $success: #28a745;
    $warning: #ffc107;
    $danger: #dc3545;
    $info: #17a2b8;
}

// 尺寸变量组
<ThemeSizes> {
    $font-small: 0.875rem;
    $font-base: 1rem;
    $font-large: 1.25rem;
    $font-xl: 1.5rem;
    $spacing-xs: 0.25rem;
    $spacing-sm: 0.5rem;
    $spacing-md: 1rem;
    $spacing-lg: 1.5rem;
    $spacing-xl: 3rem;
}

// 组件模板
<ThemeCard> {
    background: var(--background);
    color: var(--text);
    border-radius: 8px;
    padding: var(--spacing-lg);
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}
```

### 🎪 动态模块

```chtl
// 支持动态内容的模块
[Template]
<DynamicComponent> {
    // 基础样式
    padding: 20px;
    border-radius: 8px;
    
    // 动态内容占位符
    .dynamic-content {
        min-height: 100px;
        display: flex;
        align-items: center;
        justify-content: center;
    }
    
    // 加载状态
    &.loading .dynamic-content:before {
        content: "加载中...";
        color: #666;
    }
    
    // 错误状态
    &.error .dynamic-content:before {
        content: "加载失败";
        color: #dc3545;
    }
}
```

---

## 📚 模块使用

### 📥 导入模块

```chtl
// 导入官方模块
[Import] @chtl::Chtholly as Chtholly
[Import] @chtl::Yuigahama as Yui

// 导入本地模块
[Import] ./modules/MyTheme.chtl
[Import] ../shared/CommonComponents.chtl

// 选择性导入
[Import] @chtl::Chtholly { ChthollyCard, ChthollyButton }
```

### 🎨 使用模块组件

```chtl
<div use="Chtholly::ChthollyCard">
    <h3>珂朵莉卡片</h3>
    <p>使用珂朵莉模块的卡片组件</p>
    <button use="Chtholly::ChthollyButton">珂朵莉按钮</button>
</div>

<div use="Yui::FourLeafAlbum" data-images="img1.jpg,img2.jpg,img3.jpg,img4.jpg">
    <h3>由比滨的四叶相册</h3>
</div>
```

---

## 🔍 模块验证

### ✅ 模块验证清单

1. **结构验证**
   - [ ] src/目录存在
   - [ ] info/目录存在
   - [ ] 模块信息文件存在
   - [ ] [Info]部分完整

2. **内容验证**
   - [ ] 模块名称一致
   - [ ] 版本号格式正确
   - [ ] 必需字段完整
   - [ ] 语法正确无误

3. **功能验证**
   - [ ] 模板定义正确
   - [ ] 自定义元素可用
   - [ ] 继承关系正常
   - [ ] 导入导出正常

### 🧪 模块测试

```chtl
// 创建模块测试文件
// test/MyModuleTest.chtl
[Import] ../MyModule

// 测试所有模板
<div use="MyCard">模板测试1</div>
<div use="MySpecialCard">模板测试2</div>
<button use="MyButton">按钮测试</button>

// 测试继承
[Custom]
<TestCard inherit="MyCard"> {
    border: 2px solid red;
}

<div use="TestCard">继承测试</div>
```

---

## 📦 模块分发

### 📤 模块打包

```bash
# 使用CHTL编译器打包
chtl --package-cmod MyModule/

# 或使用打包脚本
./scripts/package_cmod.sh

# 验证包内容
unzip -l build/packages/cmod/MyModule.cmod
```

### 🌐 模块发布

```bash
# 1. 验证模块
chtl --validate-module MyModule/

# 2. 创建发布包
./scripts/package_cmod.sh

# 3. 生成文档
chtl --generate-docs MyModule/

# 4. 发布到模块仓库
chtl --publish MyModule.cmod --registry https://modules.chtl.org
```

---

## 🎭 官方模块标准

### 🌸 珂朵莉模块标准

基于《末日时在做什么？有没有空？可不可以来拯救？》中的珂朵莉·诺塔·塞尼欧里斯角色设计：

- **设计理念**: 温柔、善良、强大、牺牲精神
- **色彩方案**: 红色长发(#ff6b6b) + 翠绿眼睛(#4ecdc4)
- **组件特色**: 圆润、温暖、优雅的设计
- **功能特点**: 保护性、守护性的交互

### 🎀 由比滨结衣模块标准

基于《我的青春恋爱物语果然有问题。》中的由比滨结衣角色设计：

- **设计理念**: 活泼、开朗、善于理解他人
- **色彩方案**: 粉色系(#ff69b4) + 温暖色调
- **组件特色**: 友好、亲和的界面设计
- **功能特点**: 社交性、互动性的组件

---

## 📈 模块生态

### 🌟 官方模块库

```
chtl::Chtholly          珂朵莉主题模块
chtl::Yuigahama         由比滨结衣模块
chtl::Bootstrap         Bootstrap风格模块
chtl::Material          Material Design模块
chtl::Animations        动画效果模块
chtl::Forms             表单组件模块
chtl::Charts            图表组件模块
chtl::Icons             图标库模块
```

### 🎯 社区模块

```
community::DarkTheme    深色主题模块
community::Gaming       游戏UI模块
community::Admin        管理界面模块
community::Blog         博客组件模块
community::Ecommerce    电商组件模块
```

---

## 🛠️ 开发工具

### 🔧 模块开发CLI

```bash
# 创建新模块
chtl create-module MyModule --template=basic

# 验证模块
chtl validate-module MyModule/

# 预览模块
chtl preview-module MyModule/ --browser

# 发布模块
chtl publish-module MyModule.cmod
```

### 🎨 模块编辑器

```bash
# 启动模块编辑器
chtl module-editor MyModule/

# 功能包括：
# - 可视化组件编辑
# - 实时预览
# - 语法高亮
# - 自动补全
# - 错误检查
```

---

## 🎊 最佳实践

### 📋 设计原则

1. **单一职责** - 每个模块专注特定功能
2. **可复用性** - 设计通用的组件
3. **一致性** - 保持设计风格统一
4. **可扩展性** - 支持继承和定制
5. **文档完整** - 提供详细使用说明

### 🎯 命名规范

```chtl
// 模块命名：PascalCase
MyAwesomeModule

// 模板命名：PascalCase
<MyCardTemplate>
<ButtonPrimary>

// 变量命名：kebab-case with $
$primary-color
$font-size-large

// CSS类命名：kebab-case
.my-component
.button-primary
```

### 🔒 版本管理

```chtl
// 语义化版本
version = "1.2.3";  // 主版本.次版本.修订版本

// 版本兼容性
minCHTLVersion = "1.0.0";   // 最低兼容版本
maxCHTLVersion = "2.0.0";   // 最高兼容版本

// 依赖版本
dependencies = "BaseModule@^1.0.0, UtilModule@~2.1.0";
```

---

## 🎉 总结

CMOD模块系统为CHTL提供了强大的模块化能力：

- 🔥 **标准化格式** - 严格的目录结构和命名规范
- 🔥 **丰富功能** - 模板、自定义、继承、特例化
- 🔥 **官方模块** - 高质量的角色主题模块
- 🔥 **开发工具** - 完整的开发和打包工具链
- 🔥 **生态系统** - 官方和社区模块库

通过CMOD模块系统，开发者可以创建可重用的高质量组件，构建丰富的CHTL应用生态。

**🌟 开始创建您的CMOD模块，为CHTL生态贡献力量！**