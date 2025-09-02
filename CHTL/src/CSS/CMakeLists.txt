# CSS编译器库
add_library(CSSCompiler STATIC
    CSSCompiler.cpp
)

target_include_directories(CSSCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# TODO: 添加ANTLR依赖
# target_link_libraries(CSSCompiler PUBLIC
#     ${ANTLR_LIBRARIES}
# )