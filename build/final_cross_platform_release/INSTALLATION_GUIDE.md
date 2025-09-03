# 📦 CHTL跨平台安装指南

## 🖥️ Linux安装

### 1. 解压发布包
```bash
tar -xzf CHTL_Complete_Cross_Platform_Release_v1.0.0.tar.gz
cd CHTL_Complete_Cross_Platform_Release_v1.0.0
```

### 2. 安装编译器
```bash
# 复制到系统路径（可选）
sudo cp linux/bin/chtl-professional /usr/local/bin/
sudo cp linux/bin/chtl /usr/local/bin/

# 或添加到PATH
echo 'export PATH="$PATH:'$(pwd)'/linux/bin"' >> ~/.bashrc
source ~/.bashrc
```

### 3. 安装VSCode插件
```bash
# 方法1: 手动安装
code --install-extension vscode-plugin/

# 方法2: 复制到扩展目录
cp -r vscode-plugin ~/.vscode/extensions/chtl-language-support
```

### 4. 验证安装
```bash
# 测试编译器
chtl-professional --version

# 测试CJMOD功能
./linux/bin/chtl_cjmod_real_api_test
```

## 🪟 Windows安装

### 1. 解压发布包
- 解压ZIP文件到任意目录
- 建议解压到 `C:\CHTL\`

### 2. 安装编译器
```cmd
# 添加到PATH环境变量
# 1. 右键"此电脑" -> "属性"
# 2. "高级系统设置" -> "环境变量"
# 3. 在"系统变量"中找到"Path"，点击"编辑"
# 4. 点击"新建"，添加: C:\CHTL\windows\bin

# 或使用命令行添加
setx PATH "%PATH%;C:\CHTL\windows\bin" /M
```

### 3. 安装VSCode插件
```cmd
# 方法1: 手动安装
# 1. 打开VSCode
# 2. Ctrl+Shift+P -> "Extensions: Install from VSIX"
# 3. 选择vscode-plugin目录

# 方法2: 复制到扩展目录
xcopy vscode-plugin %USERPROFILE%\.vscode\extensions\chtl-language-support /E /I
```

### 4. 验证安装
```cmd
# 测试编译器
chtl-professional.exe --version

# 测试CJMOD功能
windows\bin\chtl_cjmod_real_api_test.exe
```

## 🔌 VSCode插件配置

### 1. 基本配置
打开VSCode设置，搜索"CHTL"：

```json
{
    "chtl.compilerPath": "/path/to/chtl-professional",
    "chtl.enableRealTimePreview": true,
    "chtl.previewPort": 3000,
    "chtl.formatStyle": "cpp",
    "chtl.officialModulesPath": "/path/to/modules",
    "chtl.enableCJMODSupport": true,
    "chtl.cjmodAutoUnpack": true,
    "chtl.cjmodScanningMethod": "auto"
}
```

### 2. CJMOD专用配置
```json
{
    "chtl.enableCJMODSupport": true,
    "chtl.cjmodAutoUnpack": true,
    "chtl.cjmodScanningMethod": "auto"
}
```

## 🎯 快速测试

### 创建测试文件
创建 `test.chtl`:
```chtl
[Import] @CJmod from "test_module"

html
{
    head
    {
        title
        {
            text
            {
                CHTL测试页面
            }
        }
    }
    
    body
    {
        div
        {
            id: main;
            class: container;
            
            text
            {
                🔥 CHTL编译器测试成功！
            }
            
            script
            {
                // CJMOD语法测试
                let result = 3 ** 4;  // ** 会被CJMOD处理
                
                vir data = processData({
                    input: "test",
                    output: "result"
                });
                
                {{#main}} -> click {
                    console.log('CJMOD增强事件绑定成功！');
                }
            }
            
            style
            {
                background: linear-gradient(45deg, #667eea, #764ba2);
                padding: 20px;
                border-radius: 10px;
                color: white;
                text-align: center;
            }
        }
    }
}
```

### 编译测试
```bash
# Linux
chtl-professional test.chtl -o test.html

# Windows
chtl-professional.exe test.chtl -o test.html
```

## 🔧 故障排除

### 编译器问题
1. **"命令未找到"**
   - 确认PATH环境变量设置正确
   - Linux: 检查文件执行权限 `chmod +x chtl-professional`

2. **"依赖库缺失"** (Linux)
   - 安装必要库: `sudo apt-get install libstdc++6`

3. **编译错误**
   - 检查CHTL语法是否正确
   - 使用 `--verbose` 参数查看详细错误

### VSCode插件问题
1. **语法高亮不工作**
   - 重启VSCode
   - 检查文件扩展名是否为 `.chtl` 或 `.cjjs`

2. **IntelliSense不工作**
   - 检查 `chtl.enableCJMODSupport` 设置
   - 重新加载窗口: Ctrl+Shift+P -> "Developer: Reload Window"

3. **实时预览不工作**
   - 检查端口3000是否被占用
   - 修改 `chtl.previewPort` 设置

### CJMOD问题
1. **CJMOD语法提示不显示**
   - 确保文件中有 `[Import] @CJmod`
   - 检查 `chtl.cjmodAutoUnpack` 设置

2. **模块解包失败**
   - 检查 `chtl.officialModulesPath` 路径
   - 确认模块文件完整性

## 📞 技术支持

如果遇到问题，请：
1. 检查本安装指南
2. 运行相应的测试工具验证功能
3. 查看VSCode开发者控制台错误信息

**🔥 享受CHTL和CJMOD带来的强大开发体验！**