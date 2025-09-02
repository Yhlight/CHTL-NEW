cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    # Windows下使用UTF-8编码
    add_compile_options(/utf-8)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-fPIC)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# ANTLR相关配置
set(ANTLR_JAR_PATH ${CMAKE_SOURCE_DIR}/lib/antlr-4.13.2-complete.jar)
set(ANTLR_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/antlr4-runtime/include)
set(ANTLR_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/antlr4-runtime/lib)

# 查找Java（用于运行ANTLR）
find_package(Java REQUIRED)

# 添加子目录
add_subdirectory(src/Scanner)
add_subdirectory(src/Dispatcher)
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTLJS)
add_subdirectory(src/CSS)
add_subdirectory(src/JavaScript)
add_subdirectory(src/CMOD)
add_subdirectory(src/CJMOD)

# 主执行文件
add_executable(chtl src/main.cpp)

# 链接库
target_link_libraries(chtl
    CHTLUnifiedScanner
    CompilerDispatcher
    CHTLCompiler
    CHTLJSCompiler
    CSSCompiler
    JavaScriptCompiler
    CMODHandler
    CJMODHandler
)

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY module/ DESTINATION module OPTIONAL)

# 测试
enable_testing()
add_subdirectory(tests)