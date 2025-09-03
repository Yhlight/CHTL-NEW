@echo off
echo ===============================================
echo CHTL VSCode插件构建脚本
echo 9个基本要求完整实现版本
echo ===============================================

echo.
echo 步骤1: 安装依赖...
call npm install
if %ERRORLEVEL% NEQ 0 (
    echo 错误: npm install失败
    pause
    exit /b 1
)

echo.
echo 步骤2: 安装类型定义...
call npm install --save-dev @types/ws @types/chokidar
if %ERRORLEVEL% NEQ 0 (
    echo 警告: 类型定义安装失败
)

echo.
echo 步骤3: 编译TypeScript...
call npm run compile
if %ERRORLEVEL% NEQ 0 (
    echo 错误: TypeScript编译失败
    pause
    exit /b 1
)

echo.
echo 步骤4: 安装VSCE打包工具...
call npm install -g @vscode/vsce
if %ERRORLEVEL% NEQ 0 (
    echo 尝试旧版本vsce...
    call npm install -g vsce
)

echo.
echo 步骤5: 打包插件...
call vsce package --no-dependencies
if %ERRORLEVEL% NEQ 0 (
    echo 尝试使用@vscode/vsce...
    call npx @vscode/vsce package --no-dependencies
    if %ERRORLEVEL% NEQ 0 (
        echo 错误: 插件打包失败
        pause
        exit /b 1
    )
)

echo.
echo 步骤6: 验证插件包...
if exist "chtl-language-support-1.0.0.vsix" (
    echo ✅ 插件包创建成功: chtl-language-support-1.0.0.vsix
) else (
    echo 错误: 插件包未找到
    pause
    exit /b 1
)

echo.
echo ===============================================
echo ✅ CHTL VSCode插件构建完成！
echo 📦 插件文件: chtl-language-support-1.0.0.vsix
echo 💻 功能包含: 9个基本要求完整实现
echo   - 实时预览
echo   - 内置编译器
echo   - 智能提示
echo   - 代码格式化
echo   - 右键菜单功能
echo   - 语法高亮
echo   - 自动补全
echo ===============================================
echo.
pause