#ifndef CHTL_CHTL_PARSER_PARSER_H
#define CHTL_CHTL_PARSER_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "CHTL/Core/Token.h"
#include "CHTL/Core/Context.h"
#include "CHTL/AST/ASTNode.h"

namespace CHTL {

/**
 * CHTL语法分析器
 * 负责将Token序列转换为AST
 */
class Parser {
private:
    std::vector<Token> tokens;           // Token列表
    size_t currentIndex;                 // 当前Token索引
    std::shared_ptr<CompileContext> context;  // 编译上下文
    
    /**
     * 获取当前Token
     */
    const Token& CurrentToken() const {
        return currentIndex < tokens.size() ? tokens[currentIndex] : tokens.back();
    }
    
    /**
     * 查看下一个Token
     */
    const Token& PeekToken(size_t offset = 1) const {
        size_t index = currentIndex + offset;
        return index < tokens.size() ? tokens[index] : tokens.back();
    }
    
    /**
     * 消费当前Token
     */
    void ConsumeToken() {
        if (currentIndex < tokens.size() && CurrentToken().type != TokenType::EOF_TOKEN) {
            currentIndex++;
        }
    }
    
    /**
     * 匹配并消费Token
     */
    bool Match(TokenType type);
    
    /**
     * 期望特定类型的Token
     */
    void Expect(TokenType type, const std::string& message);
    
    /**
     * 检查是否为特定Token类型
     */
    bool Check(TokenType type) const {
        return CurrentToken().type == type;
    }
    
    /**
     * 报告语法错误
     */
    void ReportError(const std::string& message);
    
    // 解析方法
    std::shared_ptr<ProgramNode> ParseProgram();
    std::shared_ptr<ASTNode> ParseTopLevelDeclaration();
    std::shared_ptr<ASTNode> ParseUseStatement();
    std::shared_ptr<ASTNode> ParseTemplate();
    std::shared_ptr<ASTNode> ParseCustom();
    std::shared_ptr<ASTNode> ParseOrigin();
    std::shared_ptr<ASTNode> ParseImport();
    std::shared_ptr<ASTNode> ParseNamespace();
    std::shared_ptr<ASTNode> ParseConfiguration();
    std::shared_ptr<ElementNode> ParseElement();
    std::shared_ptr<TextNode> ParseText();
    std::shared_ptr<ASTNode> ParseAttribute();
    std::shared_ptr<ASTNode> ParseLocalStyle();
    std::shared_ptr<ASTNode> ParseLocalScript();
    std::shared_ptr<ASTNode> ParseStyleRule();
    std::shared_ptr<ASTNode> ParseStyleProperty();
    std::shared_ptr<ASTNode> ParseExpression();
    std::shared_ptr<ASTNode> ParseLiteral();
    std::shared_ptr<ASTNode> ParseIdentifier();
    
    // 辅助方法
    bool IsTopLevelKeyword() const;
    bool IsElementStart() const;
    bool IsStylePropertyStart() const;
    std::string ParseStringOrUnquotedLiteral();
    
public:
    explicit Parser(std::shared_ptr<CompileContext> ctx);
    ~Parser() = default;
    
    /**
     * 解析Token序列
     * @param tokenList Token列表
     * @return AST根节点
     */
    std::shared_ptr<ProgramNode> Parse(const std::vector<Token>& tokenList);
    
    /**
     * 重置解析器状态
     */
    void Reset();
};

} // namespace CHTL

#endif // CHTL_CHTL_PARSER_PARSER_H