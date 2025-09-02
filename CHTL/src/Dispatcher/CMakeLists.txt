# Dispatcher库
add_library(CompilerDispatcher STATIC
    CompilerDispatcher.cpp
)

target_include_directories(CompilerDispatcher PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 链接其他编译器库（稍后添加）
target_link_libraries(CompilerDispatcher PUBLIC
    CHTLCompiler
    CHTLJSCompiler
    CSSCompiler
    JavaScriptCompiler
)