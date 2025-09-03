@echo off
REM ANTLR4 Windows构建脚本
REM 需要在Windows环境中运行，确保已安装Visual Studio 2019或更新版本

echo 正在构建ANTLR4 Windows版本...

REM 设置编码为UTF-8
chcp 65001

REM 创建构建目录
if not exist build (
    mkdir build
)

cd build

REM 配置CMake（使用Visual Studio生成器）
cmake .. -G "Visual Studio 16 2019" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_CXX_STANDARD=17 ^
    -DCMAKE_INSTALL_PREFIX=../../../ANTLR4 ^
    -DWITH_DEMO=False ^
    -DWITH_LIBCXX=False

if %ERRORLEVEL% neq 0 (
    echo CMake配置失败
    pause
    exit /b 1
)

REM 构建项目
cmake --build . --config Release --parallel

if %ERRORLEVEL% neq 0 (
    echo 构建失败
    pause
    exit /b 1
)

REM 安装到指定目录
cmake --install . --config Release

if %ERRORLEVEL% neq 0 (
    echo 安装失败
    pause
    exit /b 1
)

echo ANTLR4 Windows版本构建完成！
echo 库文件位置：../../../ANTLR4/lib/
echo 头文件位置：../../../ANTLR4/include/

pause