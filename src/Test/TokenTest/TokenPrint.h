#pragma once

#include "../../CHTL/CHTLLexer/CHTLToken.h"
#include "../../CHTL JS/CHTLJSLexer/CHTLJSToken.h"
#include <vector>
#include <string>
#include <iostream>

namespace CHTL {
namespace Test {

/**
 * Token打印工具类
 * 用于调试和测试Token流
 */
class TokenPrint {
public:
    /**
     * 打印CHTL Token流
     * @param tokens Token列表
     * @param title 标题
     */
    static void PrintCHTLTokens(const std::vector<CHTLToken>& tokens, const std::string& title = "CHTL Tokens");
    
    /**
     * 打印CHTL JS Token流
     * @param tokens Token列表
     * @param title 标题
     */
    static void PrintCHTLJSTokens(const std::vector<CHTLJS::CHTLJSToken>& tokens, const std::string& title = "CHTL JS Tokens");
    
    /**
     * 打印单个CHTL Token
     * @param token Token对象
     * @param index Token索引
     */
    static void PrintCHTLToken(const CHTLToken& token, size_t index);
    
    /**
     * 打印单个CHTL JS Token
     * @param token Token对象
     * @param index Token索引
     */
    static void PrintCHTLJSToken(const CHTLJS::CHTLJSToken& token, size_t index);
    
    /**
     * 生成Token统计报告
     * @param tokens CHTL Token列表
     * @return 统计报告字符串
     */
    static std::string GenerateCHTLTokenStatistics(const std::vector<CHTLToken>& tokens);
    
    /**
     * 生成CHTL JS Token统计报告
     * @param tokens CHTL JS Token列表
     * @return 统计报告字符串
     */
    static std::string GenerateCHTLJSTokenStatistics(const std::vector<CHTLJS::CHTLJSToken>& tokens);
    
    /**
     * 将Token流导出为表格格式
     * @param tokens CHTL Token列表
     * @return 表格字符串
     */
    static std::string ExportCHTLTokensAsTable(const std::vector<CHTLToken>& tokens);
    
    /**
     * 将CHTL JS Token流导出为表格格式
     * @param tokens CHTL JS Token列表
     * @return 表格字符串
     */
    static std::string ExportCHTLJSTokensAsTable(const std::vector<CHTLJS::CHTLJSToken>& tokens);

private:
    /**
     * 格式化Token类型字符串
     * @param type Token类型
     * @return 格式化的类型字符串
     */
    static std::string FormatCHTLTokenType(CHTLTokenType type);
    
    /**
     * 格式化CHTL JS Token类型字符串
     * @param type Token类型
     * @return 格式化的类型字符串
     */
    static std::string FormatCHTLJSTokenType(CHTLJS::CHTLJSTokenType type);
    
    /**
     * 截断字符串用于显示
     * @param str 原字符串
     * @param maxLength 最大长度
     * @return 截断后的字符串
     */
    static std::string TruncateString(const std::string& str, size_t maxLength = 30);
};

} // namespace Test
} // namespace CHTL