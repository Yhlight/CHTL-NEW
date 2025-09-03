# ANTLR4 Windows 构建说明

## 环境要求

1. Windows 10/11
2. Visual Studio 2019 或更新版本（包含C++工具链）
3. CMake 3.16 或更新版本

## 构建步骤

1. 打开命令提示符（管理员权限推荐）
2. 切换到此目录：`cd build-windows`
3. 运行构建脚本：`build-windows.cmd`

## 构建结果

构建完成后，库文件将安装到：
- 静态库：`../../../ANTLR4/lib/antlr4-runtime-static.lib`
- 动态库：`../../../ANTLR4/lib/antlr4-runtime.lib` 和 `antlr4-runtime.dll`
- 头文件：`../../../ANTLR4/include/antlr4-runtime/`

## 故障排除

如果构建失败，请检查：
1. Visual Studio C++工具链是否正确安装
2. CMake是否在PATH环境变量中
3. 是否有足够的磁盘空间
4. 防病毒软件是否干扰构建过程

## 注意事项

- 构建过程需要网络连接以下载GoogleTest依赖
- 构建时间约5-15分钟，取决于系统性能
- 建议关闭不必要的程序以提高构建速度