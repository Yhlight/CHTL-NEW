# JavaScript编译器库
add_library(JavaScriptCompiler STATIC
    JavaScriptCompiler.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptLexer.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptLexerBase.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptParser.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptParserBase.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptParserBaseListener.cpp
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS/JavaScriptParserListener.cpp
)

target_include_directories(JavaScriptCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/JS
    ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/include
)

# 添加ANTLR依赖
if(WIN32)
    target_link_libraries(JavaScriptCompiler PUBLIC
        ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/lib/antlr4-runtime-static.lib
    )
else()
    # Linux下使用静态库
    target_link_libraries(JavaScriptCompiler PUBLIC
        ${CMAKE_SOURCE_DIR}/../src/ThirdParty/ANTLR4/lib/libantlr4-runtime-static.a
    )
endif()