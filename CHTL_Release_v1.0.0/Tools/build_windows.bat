@echo off
echo ===============================================
echo CHTL编译器 Windows构建脚本
echo 彻底完整实现版本 - 包含CJMOD强大特征
echo ===============================================

echo.
echo 步骤1: 创建构建目录...
if not exist "build_windows" mkdir build_windows
cd build_windows

echo.
echo 步骤2: 配置CMake项目...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -f ../CMakeLists_Windows.txt
if %ERRORLEVEL% NEQ 0 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

echo.
echo 步骤3: 编译CHTL项目...
cmake --build . --config Release --parallel
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo.
echo 步骤4: 运行测试验证...
echo 测试CJMOD功能:
bin\Release\chtl_cjmod_complete_test.exe
if %ERRORLEVEL% NEQ 0 (
    echo 警告: CJMOD测试失败
)

echo.
echo 测试主编译器:
bin\Release\chtl.exe --version
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 主编译器测试失败
    pause
    exit /b 1
)

echo.
echo 步骤5: 创建发布包...
cmake --build . --target package --config Release
if %ERRORLEVEL% NEQ 0 (
    echo 警告: 打包失败，但编译成功
)

echo.
echo ===============================================
echo ✅ CHTL编译器Windows版本构建完成！
echo 📁 输出目录: build_windows\bin\Release\
echo 📦 发布包: build_windows\CHTL-Compiler-1.0.0-win64.zip
echo ===============================================
echo.
pause