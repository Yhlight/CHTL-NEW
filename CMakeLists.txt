cmake_minimum_required(VERSION 3.16)
project(CHTLCompiler VERSION 1.0.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置UTF-8编码支持
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# 设置编译器警告
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(src)

# CHTL核心组件源文件
set(CHTL_CORE_SOURCES
    src/CHTL/CHTLLexer/CHTLToken.cpp
    src/CHTL/CHTLLexer/CHTLGlobalMap.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLContext/CHTLContext.cpp
    src/CHTL/CHTLNode/CHTLBaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
)

# CHTL JS核心组件源文件（完全独立）
set(CHTLJS_CORE_SOURCES
    "src/CHTL JS/CHTLJSLexer/CHTLJSToken.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSGlobalMap.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "src/CHTL JS/CHTLJSState/CHTLJSState.cpp"
    "src/CHTL JS/CHTLJSContext/CHTLJSContext.cpp"
    "src/CHTL JS/CHTLJSNode/CHTLJSBaseNode.cpp"
    "src/CHTL JS/CHTLJSNode/EnhanceSelectorNode.cpp"
    "src/CHTL JS/CHTLJSNode/VirObjectNode.cpp"
    "src/CHTL JS/CHTLJSNode/AnimateNode.cpp"
)

# 扫描器源文件
set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

# 调度器源文件
set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher.cpp
)

# 创建CHTL核心库
add_library(CHTLCore STATIC ${CHTL_CORE_SOURCES})
target_include_directories(CHTLCore PUBLIC src/CHTL)

# 创建CHTL JS核心库（完全独立）
add_library(CHTLJSCore STATIC ${CHTLJS_CORE_SOURCES})
target_include_directories(CHTLJSCore PUBLIC "src/CHTL JS")

# 创建扫描器库
add_library(CHTLScanner STATIC ${SCANNER_SOURCES})
target_include_directories(CHTLScanner PUBLIC src/Scanner)

# 创建调度器库
add_library(CHTLDispatcher STATIC ${DISPATCHER_SOURCES})
target_include_directories(CHTLDispatcher PUBLIC src/CompilerDispatcher)
target_link_libraries(CHTLDispatcher CHTLCore CHTLJSCore CHTLScanner)

# 主程序可执行文件
add_executable(chtl src/main.cpp)
target_link_libraries(chtl CHTLDispatcher CHTLCore CHTLJSCore CHTLScanner)

# 测试可执行文件
add_executable(chtl_test src/Test/BasicCompilationTest.cpp)
target_link_libraries(chtl_test CHTLDispatcher CHTLCore CHTLJSCore CHTLScanner)

# 简化测试可执行文件
add_executable(chtl_simple_test src/Test/SimpleCompilerTest.cpp)
target_link_libraries(chtl_simple_test CHTLDispatcher CHTLCore CHTLJSCore CHTLScanner)

# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CHTL_DEBUG_MODE)
    if(MSVC)
        add_compile_options(/Zi)
    else()
        add_compile_options(-g)
    endif()
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(TARGETS CHTLCore CHTLJSCore CHTLScanner CHTLDispatcher DESTINATION lib)

# 创建module目录（用于模块管理）
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CMOD)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/module/CJMOD)

# 复制示例文件
configure_file(${CMAKE_SOURCE_DIR}/CHTL语法文档.md ${CMAKE_BINARY_DIR}/CHTL语法文档.md COPYONLY)

# 打印配置信息
message(STATUS "CHTL编译器配置完成")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "UTF-8支持: 已启用")
message(STATUS "架构设计: CHTL和CHTL JS完全分离")

# 自定义目标：运行测试
add_custom_target(run_tests
    COMMAND chtl_test
    DEPENDS chtl_test
    COMMENT "运行CHTL编译器基础测试"
)

# 自定义目标：编译示例
add_custom_target(compile_example
    COMMAND chtl example.chtl example.html
    DEPENDS chtl
    COMMENT "编译CHTL示例文件"
)