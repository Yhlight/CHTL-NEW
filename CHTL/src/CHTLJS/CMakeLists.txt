# CHTL JS编译器库
add_library(CHTLJSCompiler STATIC
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
    AST/ModuleNode.cpp
    AST/SelectorNode.cpp
    AST/ListenerNode.cpp
    AST/AnimateNode.cpp
    AST/VirtualObjectNode.cpp
    
    # Generator
    Generator/Generator.cpp
    Generator/SelectorTransformer.cpp
    Generator/ModuleGenerator.cpp
    
    # Utils
    Utils/SelectorCache.cpp
    Utils/EventDelegationManager.cpp
    
    # 主编译器
    CHTLJSCompiler.cpp
)

target_include_directories(CHTLJSCompiler PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 依赖库
target_link_libraries(CHTLJSCompiler PUBLIC
    # 如果需要其他库，在这里添加
)