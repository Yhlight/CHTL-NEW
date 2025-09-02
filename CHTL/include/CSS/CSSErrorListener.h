#ifndef CHTL_CSS_CSSERRORLISTENER_H
#define CHTL_CSS_CSSERRORLISTENER_H

#include "antlr4-runtime.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * CSS错误监听器
 * 捕获CSS解析过程中的语法错误
 */
class CSSErrorListener : public antlr4::BaseErrorListener {
private:
    std::vector<std::string> errors;
    bool hasErrors;
    
public:
    CSSErrorListener() : hasErrors(false) {}
    
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override {
        hasErrors = true;
        std::string error = "CSS语法错误 [行 " + std::to_string(line) + 
                          ", 列 " + std::to_string(charPositionInLine) + "]: " + msg;
        errors.push_back(error);
    }
    
    bool HasErrors() const { return hasErrors; }
    const std::vector<std::string>& GetErrors() const { return errors; }
    void ClearErrors() { 
        errors.clear(); 
        hasErrors = false;
    }
};

} // namespace CHTL

#endif // CHTL_CSS_CSSERRORLISTENER_H