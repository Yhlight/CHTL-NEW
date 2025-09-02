#ifndef CHTL_JAVASCRIPT_JAVASCRIPTERRORLISTENER_H
#define CHTL_JAVASCRIPT_JAVASCRIPTERRORLISTENER_H

#include "antlr4-runtime.h"
#include <string>
#include <vector>

namespace CHTL {

/**
 * JavaScript错误监听器
 * 捕获JavaScript解析过程中的语法错误
 */
class JavaScriptErrorListener : public antlr4::BaseErrorListener {
private:
    std::vector<std::string> errors;
    bool hasErrors;
    
public:
    JavaScriptErrorListener() : hasErrors(false) {}
    
    void syntaxError(antlr4::Recognizer* recognizer,
                    antlr4::Token* offendingSymbol,
                    size_t line,
                    size_t charPositionInLine,
                    const std::string& msg,
                    std::exception_ptr e) override {
        hasErrors = true;
        std::string error = "JavaScript语法错误 [行 " + std::to_string(line) + 
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

#endif // CHTL_JAVASCRIPT_JAVASCRIPTERRORLISTENER_H