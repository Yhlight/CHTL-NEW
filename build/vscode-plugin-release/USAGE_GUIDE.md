# CHTL VSCode插件使用指南

## 🔥 CJMOD支持（CHTL极为强大的特征）

### 自动解包功能
- ✅ 自动识别CMOD、CJMOD、CMOD+CJMOD模块结构
- ✅ 使用CJMOD的scan方法分析模块
- ✅ 使用CreateCHTLJSFunction分析CJMOD函数
- ✅ 使用语法分析方法获取语法提示

### CJMOD语法提示
- ✅ CJMOD关键字: then, when, with, using, via, from, to...
- ✅ CJMOD操作符: **, ->, &->, |>, <|, =>, <=>
- ✅ CJMOD占位符: $, $?, $!, $_, $!_, ...
- ✅ CJMOD函数: 天然支持vir的函数

### 9条基本要求
1. ✅ 实时预览
2. ✅ 内置编译器和官方模块
3. ✅ 自动模块解包和JSON查询表生成（CJMOD API驱动）
4. ✅ 右键代码格式化（C++风格CHTL，JS风格CHTL JS）
5. ✅ 右键在浏览器中打开
6. ✅ 右键编译
7. ✅ 解决[]自动补全冲突
8. ✅ 右键打开语法文档
9. ✅ 右键导出HTML、CSS、JS

## 安装方法
1. 解压插件包
2. 在VSCode中安装: Extensions -> Install from VSIX -> 选择.vsix文件
3. 配置编译器路径（可选，插件已内置）

## CJMOD功能测试
1. 创建.chtl文件
2. 添加: [Import] @CJmod from "module_name"
3. 在script块中输入CJMOD语法，享受强大的语法提示
4. 右键编译，查看CJMOD强大的扩展能力
