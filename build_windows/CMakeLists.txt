cmake_minimum_required(VERSION 3.16)
project(CHTL_Windows_Simple)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UTF-8支持
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 包含目录
include_directories(src)

# 创建简化的主程序（专注核心功能）
add_executable(chtl_windows ../src/main_simple.cpp)

# Windows特定设置
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(chtl_windows PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

message(STATUS "简化Windows编译配置完成")
message(STATUS "目标: chtl_windows.exe")