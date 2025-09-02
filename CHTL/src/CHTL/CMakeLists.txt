# CHTL编译器库
add_library(CHTLCompiler STATIC
    # Core
    Core/Token.cpp
    Core/State.cpp
    Core/Context.cpp
    Core/GlobalMap.cpp
    
    # Lexer
    Lexer/Lexer.cpp
    
    # Parser
    Parser/Parser.cpp
    
    # AST
    AST/ASTNode.cpp
    AST/ASTVisitor.cpp
    AST/TemplateNodes.cpp
    AST/CustomNodes.cpp
    AST/StyleNodes.cpp
    
    # Generator
    Generator/Generator.cpp
    
    # Utils
    Utils/FileUtils.cpp
    Utils/StringUtils.cpp
    
    # 主编译器
    CHTLCompiler.cpp
)

target_include_directories(CHTLCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 依赖库
target_link_libraries(CHTLCompiler PUBLIC
    # 如果需要其他库，在这里添加
)