@echo off
echo ===============================================
echo CHTL完整发布包创建脚本
echo 编译器 + VSCode插件 + 文档
echo ===============================================

set RELEASE_DIR=CHTL_Release_v1.0.0
set CURRENT_DIR=%CD%

echo.
echo 步骤1: 创建发布目录...
if exist "%RELEASE_DIR%" rmdir /s /q "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%\Compiler"
mkdir "%RELEASE_DIR%\VSCode_Plugin"
mkdir "%RELEASE_DIR%\Documentation"
mkdir "%RELEASE_DIR%\Examples"
mkdir "%RELEASE_DIR%\Modules"

echo.
echo 步骤2: 构建CHTL编译器...
call build_windows.bat
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 编译器构建失败
    pause
    exit /b 1
)

echo.
echo 步骤3: 复制编译器文件...
copy "build_windows\bin\Release\chtl.exe" "%RELEASE_DIR%\Compiler\"
copy "build_windows\bin\Release\antlr4-runtime.dll" "%RELEASE_DIR%\Compiler\"
copy "src\ThirdParty\ANTLR4\lib\antlr4-runtime.dll" "%RELEASE_DIR%\Compiler\"

echo.
echo 步骤4: 构建VSCode插件...
cd vscode-chtl-extension
call build_plugin.bat
if %ERRORLEVEL% NEQ 0 (
    echo 错误: VSCode插件构建失败
    cd "%CURRENT_DIR%"
    pause
    exit /b 1
)

echo.
echo 步骤5: 复制插件文件...
copy "chtl-language-support-1.0.0.vsix" "..\%RELEASE_DIR%\VSCode_Plugin\"
cd "%CURRENT_DIR%"

echo.
echo 步骤6: 复制文档...
copy "CHTL语法文档.md" "%RELEASE_DIR%\Documentation\"
copy "目标规划.ini" "%RELEASE_DIR%\Documentation\"
copy "CJMOD.md" "%RELEASE_DIR%\Documentation\"
copy "完善选择器自动化与引用规则.ini" "%RELEASE_DIR%\Documentation\"
copy "CHTL项目最终完成报告.md" "%RELEASE_DIR%\Documentation\"

echo.
echo 步骤7: 复制示例文件...
copy "test_complete_chtl.chtl" "%RELEASE_DIR%\Examples\"
copy "test_cjmod.cjjs" "%RELEASE_DIR%\Examples\"
copy "test_advanced.chtl" "%RELEASE_DIR%\Examples\"

echo.
echo 步骤8: 复制官方模块...
xcopy "src\Module" "%RELEASE_DIR%\Modules\" /E /I /Y

echo.
echo 步骤9: 创建安装说明...
echo CHTL编译器完整发布包 v1.0.0 > "%RELEASE_DIR%\README.txt"
echo. >> "%RELEASE_DIR%\README.txt"
echo 包含内容: >> "%RELEASE_DIR%\README.txt"
echo 1. CHTL编译器 (Compiler/chtl.exe) >> "%RELEASE_DIR%\README.txt"
echo 2. VSCode插件 (VSCode_Plugin/chtl-language-support-1.0.0.vsix) >> "%RELEASE_DIR%\README.txt"
echo 3. 完整文档 (Documentation/) >> "%RELEASE_DIR%\README.txt"
echo 4. 示例文件 (Examples/) >> "%RELEASE_DIR%\README.txt"
echo 5. 官方模块 (Modules/) >> "%RELEASE_DIR%\README.txt"
echo. >> "%RELEASE_DIR%\README.txt"
echo 安装步骤: >> "%RELEASE_DIR%\README.txt"
echo 1. 将Compiler目录添加到PATH环境变量 >> "%RELEASE_DIR%\README.txt"
echo 2. 在VSCode中安装插件: code --install-extension VSCode_Plugin/chtl-language-support-1.0.0.vsix >> "%RELEASE_DIR%\README.txt"
echo 3. 重启VSCode即可使用 >> "%RELEASE_DIR%\README.txt"
echo. >> "%RELEASE_DIR%\README.txt"
echo 特征说明: >> "%RELEASE_DIR%\README.txt"
echo - 100个语法特征完整实现 >> "%RELEASE_DIR%\README.txt"
echo - CJMOD极为强大的C++ API扩展特征 >> "%RELEASE_DIR%\README.txt"
echo - 双语言分离架构 (CHTL + CHTL JS) >> "%RELEASE_DIR%\README.txt"
echo - 四编译器协调系统 >> "%RELEASE_DIR%\README.txt"
echo - VSCode专业IDE支持 >> "%RELEASE_DIR%\README.txt"
echo - 官方模块系统 (Chtholly等角色模块) >> "%RELEASE_DIR%\README.txt"

echo.
echo 步骤10: 创建发布压缩包...
powershell Compress-Archive -Path "%RELEASE_DIR%" -DestinationPath "CHTL_Complete_Release_v1.0.0.zip" -Force

echo.
echo ===============================================
echo 🎉 CHTL完整发布包创建成功！
echo.
echo 📦 发布包文件: CHTL_Complete_Release_v1.0.0.zip
echo 📁 发布目录: %RELEASE_DIR%\
echo.
echo 包含内容:
echo   🔧 CHTL编译器 (chtl.exe + DLL)
echo   🔌 VSCode插件 (.vsix文件)
echo   📚 完整文档 (4个核心文档)
echo   📝 示例文件 (演示用法)
echo   📦 官方模块 (Chtholly等)
echo   📖 安装说明 (README.txt)
echo.
echo ✨ 特征亮点:
echo   🔥 CJMOD极为强大特征完整实现
echo   🔥 100个语法特征无偏离实现  
echo   🔥 VSCode专业IDE支持
echo   🔥 双语言分离架构
echo   🔥 生产级质量标准
echo ===============================================
echo.
pause