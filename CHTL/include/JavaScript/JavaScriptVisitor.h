#ifndef CHTL_JAVASCRIPT_JAVASCRIPTVISITOR_H
#define CHTL_JAVASCRIPT_JAVASCRIPTVISITOR_H

#include "JavaScriptParserBaseListener.h"
#include <string>
#include <sstream>
#include <vector>
#include <stack>

namespace CHTL {

/**
 * JavaScript访问器
 * 遍历JavaScript解析树并处理代码
 */
class JavaScriptVisitor : public JavaScriptParserBaseListener {
private:
    std::stringstream output;
    int indentLevel;
    bool prettyPrint;
    bool minify;
    std::stack<bool> inFunction;
    
    /**
     * 获取缩进字符串
     */
    std::string GetIndent() const {
        if (!prettyPrint || minify) return "";
        return std::string(indentLevel * 2, ' ');
    }
    
    /**
     * 获取空格（用于美化输出）
     */
    std::string GetSpace() const {
        return (minify ? "" : " ");
    }
    
    /**
     * 获取换行（用于美化输出）
     */
    std::string GetNewLine() const {
        return (minify ? "" : "\n");
    }
    
public:
    JavaScriptVisitor(bool pretty = true, bool mini = false) 
        : indentLevel(0), prettyPrint(pretty), minify(mini) {
        inFunction.push(false);
    }
    
    std::string GetOutput() const { return output.str(); }
    void ClearOutput() { 
        output.str(""); 
        output.clear(); 
        indentLevel = 0;
        while (!inFunction.empty()) inFunction.pop();
        inFunction.push(false);
    }
    
    // 程序入口
    void enterProgram(JavaScriptParser::ProgramContext* ctx) override {
        // 开始处理JavaScript程序
    }
    
    void exitProgram(JavaScriptParser::ProgramContext* ctx) override {
        // 确保输出以换行结束
        if (!minify && output.tellp() > 0) {
            std::string current = output.str();
            if (!current.empty() && current.back() != '\n') {
                output << "\n";
            }
        }
    }
    
    // 函数声明
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override {
        output << GetIndent() << "function";
        if (ctx->identifier()) {
            output << " " << ctx->identifier()->getText();
        }
        output << "(";
        
        // 处理参数
        if (ctx->formalParameterList()) {
            output << ctx->formalParameterList()->getText();
        }
        
        output << ")" << GetSpace() << "{" << GetNewLine();
        indentLevel++;
        inFunction.push(true);
    }
    
    void exitFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext* ctx) override {
        indentLevel--;
        output << GetIndent() << "}" << GetNewLine();
        inFunction.pop();
    }
    
    // 变量声明列表
    void enterVariableDeclarationList(JavaScriptParser::VariableDeclarationListContext* ctx) override {
        if (ctx->varModifier()) {
            output << ctx->varModifier()->getText() << " ";
        }
    }
    
    // 变量声明
    void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext* ctx) override {
        // 变量名和初始化会由子节点处理
        output << ctx->getText();
    }
    
    // 变量语句
    void enterVariableStatement(JavaScriptParser::VariableStatementContext* ctx) override {
        output << GetIndent();
    }
    
    void exitVariableStatement(JavaScriptParser::VariableStatementContext* ctx) override {
        output << ";" << GetNewLine();
    }
    
    // 表达式语句
    void enterExpressionStatement(JavaScriptParser::ExpressionStatementContext* ctx) override {
        output << GetIndent();
    }
    
    void exitExpressionStatement(JavaScriptParser::ExpressionStatementContext* ctx) override {
        output << ";" << GetNewLine();
    }
    
    // if语句
    void enterIfStatement(JavaScriptParser::IfStatementContext* ctx) override {
        output << GetIndent() << "if" << GetSpace() << "(";
        if (ctx->expressionSequence()) {
            output << ctx->expressionSequence()->getText();
        }
        output << ")" << GetSpace();
        
        // 检查是否有块语句
        if (ctx->statement().size() > 0 && ctx->statement(0)->block()) {
            // 已经有块，不需要额外处理
        } else {
            output << GetNewLine();
            indentLevel++;
        }
    }
    
    void exitIfStatement(JavaScriptParser::IfStatementContext* ctx) override {
        if (ctx->statement().size() > 0 && !ctx->statement(0)->block()) {
            indentLevel--;
        }
        
        // 处理else部分
        if (ctx->Else()) {
            output << GetIndent() << "else" << GetSpace();
            if (ctx->statement().size() > 1) {
                if (!ctx->statement(1)->block()) {
                    output << GetNewLine();
                }
            }
        }
    }
    
    // 块语句
    void enterBlock(JavaScriptParser::BlockContext* ctx) override {
        output << "{" << GetNewLine();
        indentLevel++;
    }
    
    void exitBlock(JavaScriptParser::BlockContext* ctx) override {
        indentLevel--;
        output << GetIndent() << "}" << GetNewLine();
    }
    
    // for循环
    void enterForStatement(JavaScriptParser::ForStatementContext* ctx) override {
        output << GetIndent() << "for" << GetSpace() << "(";
    }
    
    void exitForStatement(JavaScriptParser::ForStatementContext* ctx) override {
        // for语句的处理由子节点完成
    }
    
    // while循环
    void enterWhileStatement(JavaScriptParser::WhileStatementContext* ctx) override {
        output << GetIndent() << "while" << GetSpace() << "(";
        if (ctx->expressionSequence()) {
            output << ctx->expressionSequence()->getText();
        }
        output << ")" << GetSpace();
    }
    
    // return语句
    void enterReturnStatement(JavaScriptParser::ReturnStatementContext* ctx) override {
        output << GetIndent() << "return";
        if (ctx->expressionSequence()) {
            output << " ";
        }
    }
    
    void exitReturnStatement(JavaScriptParser::ReturnStatementContext* ctx) override {
        output << ";" << GetNewLine();
    }
    
    // 赋值表达式
    void enterAssignmentExpression(JavaScriptParser::AssignmentExpressionContext* ctx) override {
        // 赋值表达式的文本会被自动处理
        output << ctx->getText();
    }
    
    // 字面量
    void enterLiteral(JavaScriptParser::LiteralContext* ctx) override {
        output << ctx->getText();
    }
    
    // 标识符
    void enterIdentifier(JavaScriptParser::IdentifierContext* ctx) override {
        output << ctx->getText();
    }
    
    // 处理注释（如果需要保留）
    void visitTerminal(antlr4::tree::TerminalNode* node) override {
        // 可以在这里处理注释等终端节点
    }
};

} // namespace CHTL

#endif // CHTL_JAVASCRIPT_JAVASCRIPTVISITOR_H