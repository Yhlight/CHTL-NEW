# CSS编译器库
add_library(CSSCompiler STATIC
    CSSCompiler.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/CSS/css3Lexer.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/CSS/css3Parser.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/CSS/css3ParserBaseListener.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/CSS/css3ParserListener.cpp
)

target_include_directories(CSSCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/CSS
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/include
)

# 添加ANTLR依赖
if(WIN32)
    target_link_libraries(CSSCompiler PUBLIC
        ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/lib/antlr4-runtime-static.lib
    )
else()
    # Linux下使用静态库
    target_link_libraries(CSSCompiler PUBLIC
        ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/lib/libantlr4-runtime-static.a
    )
endif()