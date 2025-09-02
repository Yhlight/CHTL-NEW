#include <iostream>
#include "CHTL/Lexer/Lexer.h"

using namespace CHTL;

int main() {
    std::string code = "[Template] @Style button {\n    background: blue;\n}";
    
    Lexer lexer(code);
    lexer.Tokenize();
    
    auto tokens = lexer.GetTokens();
    
    std::cout << "SEMICOLON enum value: " << static_cast<int>(TokenType::SEMICOLON) << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << "Token: " << static_cast<int>(token.type) 
                  << " Value: '" << token.value << "'" 
                  << " Line: " << token.line 
                  << " Column: " << token.column << std::endl;
    }
    
    return 0;
}