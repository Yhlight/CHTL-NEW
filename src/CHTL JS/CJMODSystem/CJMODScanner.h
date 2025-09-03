#pragma once

#include "CJMODSyntax.h"
#include <string>
#include <vector>
#include <memory>

namespace CJMOD {

/**
 * CJMOD扫描结果
 */
struct CJMODScanResult {
    std::vector<std::string> Tokens;                    // 扫描到的令牌
    std::vector<std::unique_ptr<AtomArg>> PlaceholderArgs; // 占位符参数
    std::string ProcessedCode;                          // 处理后的代码
    bool IsSuccess;                                     // 是否成功
    std::string ErrorMessage;                           // 错误信息
    
    CJMODScanResult() : IsSuccess(false) {}
};

/**
 * CJMOD扫描器接口
 * 提供CJMOD语法扫描和占位符处理功能
 */
class CJMODScanner {
private:
    std::string m_SourceCode;                           // 源代码
    std::vector<std::string> m_Tokens;                  // 令牌列表
    std::vector<std::unique_ptr<AtomArg>> m_PlaceholderArgs; // 占位符参数
    size_t m_CurrentPosition;                           // 当前扫描位置

public:
    /**
     * 构造函数
     * @param sourceCode 源代码
     */
    explicit CJMODScanner(const std::string& sourceCode);
    
    /**
     * 析构函数
     */
    virtual ~CJMODScanner() = default;
    
    /**
     * 静态方法 - scan() 扫描语法片段
     * @param args Arg对象 (包含语法模式)
     * @param keyword 扫描的关键字
     * @return 扫描结果Arg对象
     */
    static Arg scan(const Arg& args, const std::string& keyword);
    
    /**
     * 实例方法 - 扫描CJMOD语法
     * @param code 代码片段
     * @return 扫描结果
     */
    virtual CJMODScanResult scan(const std::string& code);
    
    /**
     * 扫描占位符
     * @param code 代码片段
     * @return 占位符列表
     */
    std::vector<std::unique_ptr<AtomArg>> scanPlaceholders(const std::string& code);
    
    /**
     * 处理占位符替换
     * @param code 原始代码
     * @param args 参数映射
     * @return 处理后的代码
     */
    std::string processPlaceholderReplacement(const std::string& code, 
                                             const std::unordered_map<std::string, std::string>& args);
    
    /**
     * 扫描CHTL JS语法扩展
     * @param code 代码片段
     * @return 扩展语法列表
     */
    std::vector<std::string> scanCHTLJSExtensions(const std::string& code);
    
    /**
     * 验证CJMOD语法
     * @param code 代码片段
     * @return 是否有效
     */
    bool validateCJMODSyntax(const std::string& code);
    
    /**
     * 获取扫描到的令牌
     * @return 令牌列表
     */
    const std::vector<std::string>& getTokens() const { return m_Tokens; }
    
    /**
     * 获取占位符参数
     * @return 占位符参数列表
     */
    const std::vector<std::unique_ptr<AtomArg>>& getPlaceholderArgs() const { return m_PlaceholderArgs; }
    
    /**
     * 重置扫描器
     */
    virtual void reset();

protected:
    /**
     * 扫描下一个令牌
     * @return 令牌字符串
     */
    virtual std::string scanNextToken();
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 检查是否到达末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * 获取当前字符
     * @return 当前字符
     */
    char currentChar() const;
    
    /**
     * 前进一个字符
     * @return 前进后的字符
     */
    char advance();
    
    /**
     * 窥视下一个字符
     * @param offset 偏移量
     * @return 字符
     */
    char peek(size_t offset = 1) const;

private:
    /**
     * 扫描占位符令牌
     * @return 占位符令牌
     */
    std::string scanPlaceholderToken();
    
    /**
     * 扫描标识符令牌
     * @return 标识符令牌
     */
    std::string scanIdentifierToken();
    
    /**
     * 扫描字符串字面量
     * @param quote 引号字符
     * @return 字符串字面量
     */
    std::string scanStringLiteral(char quote);
    
    /**
     * 扫描操作符令牌
     * @return 操作符令牌
     */
    std::string scanOperatorToken();
};

} // namespace CJMOD