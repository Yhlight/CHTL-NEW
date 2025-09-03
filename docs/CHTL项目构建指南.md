# CHTL项目构建指南

## 🏗️ 构建系统概述

CHTL项目采用专业的跨平台构建系统，支持Windows (Visual Studio 2019/2022)、Linux (GCC/Clang) 和 macOS (Clang/Xcode)。构建系统包含编译计时器、安全监控、模块打包等专业功能。

---

## 📋 环境要求

### 🪟 Windows环境
- **操作系统**: Windows 10/11 (64位)
- **编译器**: Visual Studio 2019 或 Visual Studio 2022
- **CMake**: 3.16 或更高版本
- **Git**: 用于源码管理
- **Node.js**: 14+ (用于VSCode插件开发)

### 🐧 Linux环境
- **操作系统**: Ubuntu 18.04+, CentOS 7+, 或其他现代Linux发行版
- **编译器**: GCC 8+ 或 Clang 10+
- **CMake**: 3.16 或更高版本
- **开发工具**: build-essential包
- **Node.js**: 14+ (用于VSCode插件开发)

### 🍎 macOS环境
- **操作系统**: macOS 10.14 (Mojave) 或更高版本
- **编译器**: Xcode Command Line Tools
- **CMake**: 3.16 或更高版本 (可通过Homebrew安装)
- **Node.js**: 14+ (用于VSCode插件开发)

---

## 🚀 快速开始

### 📥 获取源码

```bash
# 克隆仓库
git clone https://github.com/CHTL-Official/chtl-compiler.git
cd chtl-compiler

# 切换到请求分支
git checkout cursor/implement-chtl-compiler-with-strict-adherence-bae6
```

### ⚡ 一键构建

```bash
# Linux/macOS
./scripts/unified_build.sh release

# Windows
scripts\windows\build_release.bat
```

---

## 🔧 详细构建步骤

### 🪟 Windows构建

#### 使用Visual Studio 2022

```cmd
# 1. 打开Visual Studio 2022 Developer Command Prompt

# 2. 导航到项目目录
cd C:\path\to\chtl-compiler

# 3. 运行构建脚本
scripts\windows\build_release.bat

# 4. 或手动构建
mkdir build\release
cd build\release
cmake ..\.. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

#### 使用Visual Studio 2019

```cmd
# 使用VS2019构建
mkdir build\release
cd build\release
cmake ..\.. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

### 🐧 Linux构建

#### 使用GCC

```bash
# 1. 安装依赖
sudo apt-get update
sudo apt-get install build-essential cmake git

# 2. 构建Release版本
export CC=gcc
export CXX=g++
./scripts/linux/build_release.sh

# 3. 或手动构建
mkdir -p build/release
cd build/release
cmake ../.. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### 使用Clang

```bash
# 1. 安装Clang
sudo apt-get install clang

# 2. 构建
export CC=clang
export CXX=clang++
./scripts/linux/build_release.sh
```

### 🍎 macOS构建

```bash
# 1. 安装Xcode Command Line Tools
xcode-select --install

# 2. 安装CMake (通过Homebrew)
brew install cmake

# 3. 构建
./scripts/macos/build_release.sh

# 4. 或手动构建
mkdir -p build/release
cd build/release
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14
make -j$(sysctl -n hw.ncpu)
```

---

## 🧪 调试构建

### 🔍 Debug模式

```bash
# Linux/macOS Debug构建
./scripts/linux/build_debug.sh

# Windows Debug构建
scripts\windows\build_debug.bat
```

Debug模式特征：
- **AddressSanitizer** (Linux/macOS) - 内存错误检测
- **调试符号** - 完整调试信息
- **所有测试** - 包含完整测试套件
- **详细日志** - 调试输出信息

### 🧪 运行测试

```bash
# 运行所有测试
cd build/debug
make test

# 运行特定测试
./bin/chtl_end_to_end_test      # 端到端测试
./bin/chtl_cjmod_test           # CJMOD功能测试
./bin/chtl_official_module_test # 官方模块测试
```

---

## 📦 模块构建

### 🎭 CMOD模块打包

```bash
# 打包CMOD模块
./scripts/package_cmod.sh

# 查看打包结果
ls build/packages/cmod/
cat build/packages/cmod/cmod_packages.txt
```

### 🔧 CJMOD模块打包

```bash
# 打包CJMOD模块
./scripts/package_cjmod.sh

# 查看打包结果
ls build/packages/cjmod/
cat build/packages/cjmod/cjmod_packages.txt
```

---

## 💻 VSCode插件构建

### 🔌 插件开发环境

```bash
# 1. 进入插件目录
cd vscode-chtl-extension

# 2. 安装依赖
npm install

# 3. 安装类型定义
npm install --save-dev @types/ws @types/chokidar

# 4. 编译TypeScript
npm run compile
```

### 📦 插件打包

```bash
# 使用构建脚本
./scripts/build_vscode_plugin.sh

# 或手动打包
cd vscode-chtl-extension
npm install -g @vscode/vsce
vsce package --no-dependencies
```

---

## ⏱️ 编译监控

### 🛡️ 使用编译计时器

```bash
# 监控构建过程
./scripts/compilation_timer.sh make -j4

# 监控CMake构建
./scripts/compilation_timer.sh cmake --build . --config Release

# 自定义监控配置
# 编辑scripts/compilation_timer.sh中的配置
MAX_COMPILATION_TIME=600  # 10分钟
MAX_MEMORY_MB=4096       # 4GB
```

### 📊 性能分析

编译计时器提供：
- **实时监控** - 时间、内存、CPU使用率
- **自动保护** - 超时和内存限制保护
- **详细日志** - 完整的性能记录
- **性能建议** - 基于监控数据的优化建议

---

## 🎯 构建配置

### 📝 CMake选项

```bash
# 基本配置选项
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \           # 构建类型
    -DCMAKE_CXX_STANDARD=17 \             # C++标准
    -DCMAKE_INSTALL_PREFIX=/usr/local \   # 安装路径
    -DWITH_TESTS=ON \                     # 启用测试
    -DWITH_CJMOD=ON \                     # 启用CJMOD
    -DWITH_ANTLR=ON                       # 启用ANTLR4

# Windows特定选项
cmake .. \
    -G "Visual Studio 17 2022" \
    -A x64 \
    -DCMAKE_CONFIGURATION_TYPES="Release;Debug"

# 跨平台编译 (Linux -> Windows)
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../mingw64-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release
```

### 🔧 环境变量

```bash
# Linux/macOS
export CC=clang                    # C编译器
export CXX=clang++                 # C++编译器
export CMAKE_BUILD_TYPE=Release    # 构建类型
export CHTL_ENABLE_CJMOD=1        # 启用CJMOD

# Windows (CMD)
set CC=cl
set CXX=cl
set CMAKE_BUILD_TYPE=Release

# Windows (PowerShell)
$env:CC="cl"
$env:CXX="cl"
$env:CMAKE_BUILD_TYPE="Release"
```

---

## 🧹 构建清理

### 🗑️ 清理构建产物

```bash
# 清理所有构建文件
./scripts/clean_build.sh

# 手动清理
rm -rf build/
rm -rf build_windows/
rm -f *.log
rm -f compilation_*.log
```

### 🔄 重新构建

```bash
# 完全重新构建
./scripts/clean_build.sh
./scripts/unified_build.sh release
```

---

## 📋 故障排除

### ❌ 常见问题

#### 问题1: CMake配置失败
```
症状: CMake configuration failed
原因: 缺少依赖或CMake版本过低
解决: 
  - 检查CMake版本: cmake --version
  - 安装缺少的依赖
  - 更新CMake到3.16+
```

#### 问题2: 编译器找不到
```
症状: C++ compiler not found
原因: 编译器未安装或不在PATH中
解决:
  - Windows: 安装Visual Studio
  - Linux: sudo apt-get install build-essential
  - macOS: xcode-select --install
```

#### 问题3: ANTLR4依赖问题
```
症状: antlr4-runtime.h file not found
原因: ANTLR4运行时库未正确安装
解决:
  - 使用核心版本构建: cp CMakeLists_CoreOnly.txt CMakeLists.txt
  - 或构建ANTLR4: 参见ANTLR4构建部分
```

#### 问题4: UTF-8编码问题
```
症状: 中文字符显示乱码
原因: 编码设置不正确
解决:
  - 确保源文件保存为UTF-8编码
  - Windows: 确保控制台支持UTF-8
  - 检查编译器UTF-8选项
```

### 🔧 调试技巧

```bash
# 1. 详细构建日志
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
make VERBOSE=1

# 2. 编译器调试信息
export CXXFLAGS="-v -g"

# 3. 使用编译计时器监控
./scripts/compilation_timer.sh <build_command>

# 4. 检查依赖关系
ldd bin/chtl  # Linux
otool -L bin/chtl  # macOS
```

---

## 🎯 高级构建

### 🚀 优化构建

```bash
# 启用链接时优化 (LTO)
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# 启用本地优化
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native"

# 静态链接 (便于分发)
cmake .. -DCMAKE_BUILD_TYPE=Release -DSTATIC_LINKING=ON
```

### 📊 性能分析构建

```bash
# 启用性能分析
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_PROFILING=ON

# 生成性能报告
perf record ./bin/chtl input.chtl
perf report
```

### 🔒 安全构建

```bash
# 启用所有安全选项
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-fstack-protector-strong -D_FORTIFY_SOURCE=2" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,relro,-z,now"
```

---

## 📦 打包和分发

### 📋 创建发布包

```bash
# 创建完整发布包
./scripts/unified_package.sh

# 检查发布包内容
unzip -l build/CHTL_Complete_Release_v*.zip
```

### 🎯 平台特定打包

```bash
# Linux包 (DEB/RPM)
cd build/release
make package

# Windows包 (ZIP/NSIS)
# 在Windows环境中运行
cmake --build . --target package --config Release

# macOS包 (DMG)
cd build/release  
make package
```

---

## 🔬 持续集成

### 🤖 GitHub Actions配置

```yaml
# .github/workflows/build.yml
name: CHTL Compiler Build

on: [push, pull_request]

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        
    runs-on: ${{ matrix.os }}
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Setup CMake
      uses: jwlawson/actions-setup-cmake@v1.13
      
    - name: Build (Linux/macOS)
      if: runner.os != 'Windows'
      run: |
        chmod +x scripts/compilation_timer.sh
        ./scripts/compilation_timer.sh ./scripts/linux/build_release.sh
        
    - name: Build (Windows)
      if: runner.os == 'Windows'
      run: |
        scripts\windows\build_release.bat
        
    - name: Run Tests
      run: |
        cd build/debug
        ctest --output-on-failure
        
    - name: Package
      run: |
        ./scripts/unified_package.sh
```

### 🏭 Jenkins配置

```groovy
// Jenkinsfile
pipeline {
    agent any
    
    stages {
        stage('Checkout') {
            steps {
                git branch: 'cursor/implement-chtl-compiler-with-strict-adherence-bae6',
                    url: 'https://github.com/CHTL-Official/chtl-compiler.git'
            }
        }
        
        stage('Build') {
            parallel {
                stage('Linux') {
                    agent { label 'linux' }
                    steps {
                        sh './scripts/compilation_timer.sh ./scripts/linux/build_release.sh'
                    }
                }
                
                stage('Windows') {
                    agent { label 'windows' }
                    steps {
                        bat 'scripts\\windows\\build_release.bat'
                    }
                }
            }
        }
        
        stage('Test') {
            steps {
                sh './build/debug/bin/chtl_end_to_end_test'
            }
        }
        
        stage('Package') {
            steps {
                sh './scripts/unified_package.sh'
                archiveArtifacts artifacts: 'build/CHTL_*.zip', fingerprint: true
            }
        }
    }
}
```

---

## 🎛️ 构建选项详解

### 🔧 CMake变量

| 变量名 | 默认值 | 说明 |
|--------|--------|------|
| `CMAKE_BUILD_TYPE` | Release | 构建类型 (Release/Debug/RelWithDebInfo) |
| `CMAKE_CXX_STANDARD` | 17 | C++标准版本 |
| `WITH_TESTS` | ON | 是否构建测试 |
| `WITH_CJMOD` | ON | 是否启用CJMOD |
| `WITH_ANTLR` | ON | 是否启用ANTLR4 |
| `STATIC_LINKING` | OFF | 是否静态链接 |
| `WITH_PROFILING` | OFF | 是否启用性能分析 |

### 🎯 构建目标

```bash
# 基本目标
make chtl                    # 主编译器
make chtl_tests              # 所有测试
make chtl_cjmod_test         # CJMOD测试
make chtl_end_to_end_test    # 端到端测试

# 库目标
make CHTLCore               # CHTL核心库
make CHTLJSCore             # CHTL JS核心库
make CJMOD                  # CJMOD库
make CHTLDispatcher         # 编译器调度器

# 打包目标
make package                # 创建发布包
make install                # 安装到系统
```

---

## 🔍 构建验证

### ✅ 验证构建结果

```bash
# 1. 检查可执行文件
ls -la build/release/bin/chtl
file build/release/bin/chtl

# 2. 测试基本功能
./build/release/bin/chtl --version
./build/release/bin/chtl --help

# 3. 测试编译功能
echo '<div>测试</div>' > test.chtl
./build/release/bin/chtl test.chtl test.html

# 4. 验证输出
cat test.html
```

### 📊 性能基准

```bash
# 运行性能测试
time ./build/release/bin/chtl examples/comprehensive_demo.chtl

# 内存使用分析
valgrind --tool=massif ./build/release/bin/chtl examples/comprehensive_demo.chtl

# 性能分析
perf record ./build/release/bin/chtl examples/comprehensive_demo.chtl
perf report
```

---

## 🛠️ 开发者构建

### 🔧 开发环境设置

```bash
# 1. 设置开发环境
export CHTL_DEV_MODE=1
export CHTL_DEBUG=1

# 2. 启用所有调试选项
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_TESTS=ON \
    -DWITH_SANITIZERS=ON \
    -DWITH_COVERAGE=ON

# 3. 增量构建
make -j$(nproc)
```

### 🧪 测试驱动开发

```bash
# 1. 运行测试
make test

# 2. 代码覆盖率
make coverage
open coverage/index.html

# 3. 静态分析
make static-analysis

# 4. 内存检查
make memcheck
```

---

## 📈 构建优化

### ⚡ 编译速度优化

```bash
# 1. 并行编译
make -j$(nproc)  # Linux/macOS
cmake --build . --parallel  # 跨平台

# 2. 使用ccache (Linux/macOS)
export CC="ccache gcc"
export CXX="ccache g++"

# 3. 预编译头文件
cmake .. -DWITH_PCH=ON

# 4. 分布式编译 (distcc)
export CC="distcc gcc"
export CXX="distcc g++"
```

### 💾 磁盘空间优化

```bash
# 1. 清理中间文件
make clean

# 2. 仅构建必要目标
make chtl  # 仅构建主程序

# 3. 禁用测试
cmake .. -DWITH_TESTS=OFF

# 4. 使用外部依赖
cmake .. -DUSE_SYSTEM_ANTLR=ON
```

---

## 🎊 构建最佳实践

### 📋 推荐工作流

1. **开发阶段**
   ```bash
   # Debug构建 + 所有测试
   ./scripts/linux/build_debug.sh
   ```

2. **测试阶段**
   ```bash
   # 运行完整测试套件
   ./build/debug/bin/chtl_end_to_end_test
   ```

3. **发布准备**
   ```bash
   # Release构建 + 打包
   ./scripts/linux/build_release.sh
   ./scripts/unified_package.sh
   ```

4. **质量检查**
   ```bash
   # 代码优化分析
   ./scripts/optimize_code.sh
   ```

### 🎯 性能建议

- **使用Release构建** 用于生产环境
- **启用LTO** 获得最佳性能
- **使用编译计时器** 监控构建过程
- **定期清理** 保持构建环境整洁
- **并行构建** 充分利用多核CPU

---

## 📚 相关文档

- [CHTL开发指南](CHTL开发指南.md) - 基础开发指南
- [CJMOD开发指南](CJMOD开发指南.md) - CJMOD强大特征开发
- [编译计时器说明](编译计时器说明.md) - 编译监控工具
- [项目环境依赖文档](项目环境依赖文档.md) - 详细依赖说明

---

## 🎉 总结

CHTL项目的构建系统提供了：

- 🔥 **跨平台支持** - Windows/Linux/macOS
- 🔥 **专业工具** - 编译监控、性能分析
- 🔥 **自动化流程** - 一键构建和打包
- 🔥 **安全保护** - 编译计时器保护系统
- 🔥 **质量保证** - 完整的测试和验证

通过专业的构建系统，开发者可以轻松地在任何平台上构建、测试和分发CHTL编译器。

**🌟 开始您的CHTL构建之旅，体验专业级的构建体验！**