# JavaScript编译器库
add_library(JavaScriptCompiler STATIC
    JavaScriptCompiler.cpp
)

target_include_directories(JavaScriptCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# TODO: 添加ANTLR依赖
# target_link_libraries(JavaScriptCompiler PUBLIC
#     ${ANTLR_LIBRARIES}
# )