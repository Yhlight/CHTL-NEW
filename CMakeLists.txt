cmake_minimum_required(VERSION 3.16)

# 项目设置
project(CHTL_Compiler LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    # Visual Studio编译器设置
    add_compile_options(/utf-8)  # 支持UTF-8编码
    add_compile_options(/W4)     # 高警告级别
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    # GCC/Clang编译器设置
    add_compile_options(-finput-charset=UTF-8)  # 输入UTF-8编码
    add_compile_options(-fexec-charset=UTF-8)   # 执行UTF-8编码
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/ThirdParty/ANTLR4/include/antlr4-runtime)

# 链接目录
link_directories(${CMAKE_SOURCE_DIR}/src/ThirdParty/ANTLR4/lib)

# CHTL编译器库
set(CHTL_SOURCES
    src/CHTL/CHTLLexer/CHTLToken.cpp
    src/CHTL/CHTLLexer/CHTLGlobalMap.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
)

add_library(CHTL_Lib STATIC ${CHTL_SOURCES})
target_include_directories(CHTL_Lib PUBLIC src/CHTL)

# CHTL JS编译器库
set(CHTLJS_SOURCES
    "src/CHTL JS/CHTLJSLexer/CHTLJSToken.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSGlobalMap.cpp"
    "src/CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "src/CHTL JS/CHTLJSState/CHTLJSState.cpp"
)

add_library(CHTLJS_Lib STATIC ${CHTLJS_SOURCES})
target_include_directories(CHTLJS_Lib PUBLIC "src/CHTL JS")

# Scanner库
set(SCANNER_SOURCES
    src/Scanner/CHTLUnifiedScanner.cpp
)

add_library(Scanner_Lib STATIC ${SCANNER_SOURCES})
target_include_directories(Scanner_Lib PUBLIC src/Scanner)

# CompilerDispatcher库
set(DISPATCHER_SOURCES
    src/CompilerDispatcher/CompilerDispatcher.cpp
)

add_library(Dispatcher_Lib STATIC ${DISPATCHER_SOURCES})
target_include_directories(Dispatcher_Lib PUBLIC src/CompilerDispatcher)

# 主程序
add_executable(chtl_compiler
    src/main.cpp
)

# 链接库
target_link_libraries(chtl_compiler 
    CHTL_Lib 
    CHTLJS_Lib 
    Scanner_Lib
    Dispatcher_Lib
)

# 链接ANTLR4库
if(WIN32)
    target_link_libraries(chtl_compiler antlr4-runtime-static)
else()
    target_link_libraries(chtl_compiler antlr4-runtime)
endif()

# 测试目标
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Token测试
    add_executable(token_test
        src/Test/TokenTest/TokenTest.cpp
        src/CHTL/CHTLLexer/CHTLToken.cpp
        src/CHTL/CHTLLexer/CHTLGlobalMap.cpp
        src/CHTL/CHTLLexer/CHTLLexer.cpp
        "src/CHTL JS/CHTLJSLexer/CHTLJSToken.cpp"
        "src/CHTL JS/CHTLJSLexer/CHTLJSGlobalMap.cpp"
        "src/CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    )
    
    target_include_directories(token_test PRIVATE 
        src/CHTL 
        "src/CHTL JS"
        src/Test/TokenTest
    )
    
    # AST测试
    add_executable(ast_test
        src/Test/AstTest/ASTTest.cpp
    )
    
    target_link_libraries(ast_test CHTL_Lib)
    target_include_directories(ast_test PRIVATE src/CHTL src/Test/AstTest)
    
    # 样式测试
    add_executable(style_test
        src/Test/AstTest/StyleTest.cpp
    )
    
    target_link_libraries(style_test CHTL_Lib)
    target_include_directories(style_test PRIVATE src/CHTL src/Test/AstTest)
    
    # 模板测试
    add_executable(template_test
        src/Test/AstTest/TemplateTest.cpp
    )
    
    target_link_libraries(template_test CHTL_Lib)
    target_include_directories(template_test PRIVATE src/CHTL src/Test/AstTest)
endif()

# 安装规则
install(TARGETS chtl_compiler
    RUNTIME DESTINATION bin
)

# 设置项目属性
set_target_properties(chtl_compiler PROPERTIES
    OUTPUT_NAME "chtl"
    DEBUG_POSTFIX "_debug"
)

# 平台特定设置
if(WIN32)
    set_target_properties(chtl_compiler PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 编译后操作
add_custom_command(TARGET chtl_compiler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "CHTL编译器构建完成！"
    COMMAND ${CMAKE_COMMAND} -E echo "输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)