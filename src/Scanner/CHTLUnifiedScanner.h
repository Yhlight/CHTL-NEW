#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <codecvt>
#include <locale>

// 前向声明
class CJMODScannerPlugin;

namespace CHTL {

/**
 * 片段类型枚举
 * 定义扫描器识别的四种代码片段类型
 */
enum class FragmentType {
    CHTL_FRAGMENT,      // CHTL语法片段
    CHTL_JS_FRAGMENT,   // CHTL JS语法片段  
    CSS_FRAGMENT,       // CSS语法片段
    JS_FRAGMENT         // JavaScript语法片段
};

/**
 * 代码片段结构
 * 存储切割后的代码片段信息
 */
struct CodeFragment {
    FragmentType Type;              // 片段类型
    std::string Content;            // 片段内容
    size_t StartPosition;           // 在原始代码中的起始位置
    size_t EndPosition;             // 在原始代码中的结束位置
    size_t LineNumber;              // 行号
    size_t ColumnNumber;            // 列号
    bool IsComplete;                // 是否为完整片段
    
    CodeFragment() = default;
    CodeFragment(FragmentType type, const std::string& content, 
                size_t start, size_t end, size_t line, size_t column, bool complete = true)
        : Type(type), Content(content), StartPosition(start), EndPosition(end), 
          LineNumber(line), ColumnNumber(column), IsComplete(complete) {}
};

/**
 * 语法单元结构
 * 存储最小语法单元信息，用于精确切割
 */
struct SyntaxUnit {
    std::string Content;            // 单元内容
    size_t StartPosition;           // 起始位置
    size_t EndPosition;             // 结束位置
    bool IsCHTLJSSyntax;           // 是否为CHTL JS语法
    
    SyntaxUnit() = default;
    SyntaxUnit(const std::string& content, size_t start, size_t end, bool isCHTLJS = false)
        : Content(content), StartPosition(start), EndPosition(end), IsCHTLJSSyntax(isCHTLJS) {}
};

/**
 * CHTLUnifiedScanner - 精准代码切割器
 * 
 * 核心功能：
 * 1. 可变长度切片机制
 * 2. 最小语法单元切割
 * 3. 片段完整性验证
 * 4. UTF-8字符支持
 * 5. 上下文保持避免过度细分
 */
class CHTLUnifiedScanner {
private:
    std::string m_SourceCode;                           // 源代码
    size_t m_CurrentPosition;                           // 当前扫描位置
    size_t m_CurrentLine;                              // 当前行号
    size_t m_CurrentColumn;                            // 当前列号
    std::vector<CodeFragment> m_Fragments;             // 切割后的片段
    std::wstring_convert<std::codecvt_utf8<wchar_t>> m_UTF8Converter; // UTF-8转换器
    
    // CHTL语法关键字映射表
    std::unordered_map<std::string, bool> m_CHTLKeywords;
    
    // CHTL JS语法特征映射表
    std::unordered_map<std::string, bool> m_CHTLJSKeywords;
    
    // CJMOD扫描挂件（只有Import CJMOD后才启用）
    bool m_CJMODEnabled;                                    // 是否启用CJMOD扫描
    std::unique_ptr<CJMODScannerPlugin> m_CJMODPlugin; // CJMOD扫描插件

public:
    /**
     * 构造函数
     * @param sourceCode 源代码字符串
     */
    explicit CHTLUnifiedScanner(const std::string& sourceCode);
    
    /**
     * 默认构造函数
     */
    CHTLUnifiedScanner();
    
    /**
     * 析构函数
     */
    ~CHTLUnifiedScanner() = default;
    
    /**
     * 执行代码扫描和切割
     * @return 切割是否成功
     */
    bool Scan();
    
    /**
     * 获取切割后的片段列表
     * @return 代码片段向量
     */
    const std::vector<CodeFragment>& GetFragments() const;
    
    /**
     * 重置扫描器状态
     */
    void Reset();
    
    /**
     * 启用CJMOD扫描挂件
     * 只有Import CJMOD后才会调用此方法
     */
    void EnableCJMODScanning();
    
    /**
     * 禁用CJMOD扫描挂件
     */
    void DisableCJMODScanning();
    
    /**
     * 检查是否启用了CJMOD扫描
     * @return 是否启用CJMOD
     */
    bool IsCJMODEnabled() const { return m_CJMODEnabled; }
    
    /**
     * 设置新的源代码
     * @param sourceCode 新的源代码
     */
    void SetSourceCode(const std::string& sourceCode);
    
    /**
     * 获取错误信息
     * @return 错误信息字符串
     */
    std::string GetErrorMessage() const;
    
    /**
     * 更新片段内容 (供CJMOD使用)
     * @param fragmentIndex 片段索引
     * @param newContent 新内容
     * @return 是否成功
     */
    bool UpdateFragmentContent(size_t fragmentIndex, const std::string& newContent);
    
    /**
     * 获取片段数量
     * @return 片段数量
     */
    size_t GetFragmentCount() const;

private:
    /**
     * 初始化关键字映射表
     */
    void InitializeKeywordMaps();
    
    /**
     * 识别片段类型
     * @param content 片段内容
     * @return 片段类型
     */
    FragmentType IdentifyFragmentType(const std::string& content);
    
    /**
     * 检查片段完整性
     * @param currentFragment 当前片段
     * @param nextFragment 下一个片段起始部分
     * @return 是否需要扩增切片
     */
    bool CheckFragmentCompleteness(const std::string& currentFragment, 
                                  const std::string& nextFragment);
    
    /**
     * 扩增切片范围
     * @param additionalLength 需要扩增的长度
     * @return 扩增后的片段内容
     */
    std::string ExpandSlice(size_t additionalLength);
    
    /**
     * 对片段进行最小语法单元二次切割
     * @param fragment 原始片段
     * @return 最小语法单元向量
     */
    std::vector<SyntaxUnit> PerformMinimalUnitCutting(const CodeFragment& fragment);
    
    /**
     * 检查是否为CHTL语法
     * @param content 内容
     * @return 是否为CHTL语法
     */
    bool IsCHTLSyntax(const std::string& content);
    
    /**
     * 检查是否为CHTL JS语法
     * @param content 内容
     * @return 是否为CHTL JS语法
     */
    bool IsCHTLJSSyntax(const std::string& content);
    
    /**
     * 检查是否为CSS语法
     * @param content 内容
     * @return 是否为CSS语法
     */
    bool IsCSSSyntax(const std::string& content);
    
    /**
     * 检查是否为JavaScript语法
     * @param content 内容
     * @return 是否为JavaScript语法
     */
    bool IsJavaScriptSyntax(const std::string& content);
    
    /**
     * 跳过空白字符和注释
     */
    void SkipWhitespaceAndComments();
    
    /**
     * 读取下一个字符（UTF-8支持）
     * @return 下一个字符，如果到达文件末尾返回'\0'
     */
    char GetNextChar();
    
    /**
     * 窥视下一个字符（不移动位置）
     * @param offset 偏移量，默认为1
     * @return 指定偏移位置的字符
     */
    char PeekChar(size_t offset = 1);
    
    /**
     * 读取指定长度的字符串
     * @param length 长度
     * @return 读取的字符串
     */
    std::string ReadString(size_t length);
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达末尾
     */
    bool IsAtEnd() const;
    
    /**
     * 更新行号和列号
     * @param ch 当前字符
     */
    void UpdateLineColumn(char ch);
    
    /**
     * 验证UTF-8字符编码
     * @param str 待验证字符串
     * @return 是否为有效UTF-8编码
     */
    bool ValidateUTF8(const std::string& str);
    
    /**
     * 处理CHTL JS增强选择器切割
     * 例如：{{box}}->click 切割为 {{box}}-> 和 click
     * @param content 包含增强选择器的内容
     * @return 切割后的语法单元
     */
    std::vector<SyntaxUnit> CutCHTLJSEnhancedSelector(const std::string& content);
    
    /**
     * 处理CHTL模板语法切割
     * @param content 包含模板语法的内容
     * @return 切割后的语法单元
     */
    std::vector<SyntaxUnit> CutCHTLTemplateSyntax(const std::string& content);
    
    /**
     * 检查是否为完整的CHTL块结构
     * @param content 内容
     * @return 是否为完整块结构
     */
    bool IsCompleteCHTLBlock(const std::string& content);
    
    /**
     * 检查是否为完整的CHTL JS函数
     * @param content 内容
     * @return 是否为完整CHTL JS函数
     */
    bool IsCompleteCHTLJSFunction(const std::string& content);
    
    /**
     * 检查括号是否平衡
     * @param content 内容
     * @return 是否平衡
     */
    bool IsBracketBalanced(const std::string& content);
    
    /**
     * 检查是否为不完整的字符串字面量
     * @param content 内容
     * @return 是否不完整
     */
    bool IsIncompleteStringLiteral(const std::string& content);

private:
    /**
     * 读取初始片段
     * @return 初始片段内容
     */
    std::string ReadInitialFragment();
    
    /**
     * 确保片段完整性
     * @param initialFragment 初始片段
     * @return 完整片段
     */
    std::string EnsureFragmentCompleteness(const std::string& initialFragment);
    
    /**
     * 读取CHTL块语法
     * @return 块内容
     */
    std::string ReadCHTLBlockSyntax();
    
    /**
     * 读取CHTL JS增强选择器
     * @return 选择器内容
     */
    std::string ReadCHTLJSEnhancedSelector();
    
    /**
     * 读取标识符或关键字
     * @return 内容
     */
    std::string ReadIdentifierOrKeyword();
    
    /**
     * 读取单行注释
     * @return 注释内容
     */
    std::string ReadSingleLineComment();
    
    /**
     * 读取多行注释
     * @return 注释内容
     */
    std::string ReadMultiLineComment();
    
    /**
     * 读取生成器注释
     * @return 注释内容
     */
    std::string ReadGeneratorComment();
    
    /**
     * 读取字符串字面量
     * @param quote 引号字符
     * @return 字符串内容
     */
    std::string ReadStringLiteral(char quote);
    
    /**
     * 读取块结构
     * @return 块内容
     */
    std::string ReadBlock();
    
    /**
     * 读取到分隔符
     * @return 内容
     */
    std::string ReadUntilDelimiter();
    
    /**
     * 读取字符串字面量内容
     * @param quote 引号字符
     * @return 内容
     */
    std::string ReadStringLiteralContent(char quote);
    
    /**
     * 跳过空白字符
     */
    void SkipWhitespace();
    
    /**
     * 检查是否为不完整的CHTL块
     * @param current 当前内容
     * @param next 下一内容
     * @return 是否不完整
     */
    bool IsIncompleteCHTLBlock(const std::string& current, const std::string& next);
    
    /**
     * 检查是否为不完整的CHTL JS函数
     * @param current 当前内容
     * @param next 下一内容
     * @return 是否不完整
     */
    bool IsIncompleteCHTLJSFunction(const std::string& current, const std::string& next);
    
    /**
     * 计算扩增长度
     * @param current 当前内容
     * @param next 下一内容
     * @return 扩增长度
     */
    size_t CalculateExpandLength(const std::string& current, const std::string& next);
    
    /**
     * 是否应该聚合单元
     * @param units 语法单元列表
     * @return 是否应该聚合
     */
    bool ShouldAggregateUnits(const std::vector<SyntaxUnit>& units);
    
    /**
     * 聚合语法单元
     * @param units 语法单元列表
     * @return 聚合后的内容
     */
    std::string AggregateUnits(const std::vector<SyntaxUnit>& units);

private:
    std::string m_ErrorMessage;                        // 错误信息
    bool m_HasError;                                   // 是否有错误
};

} // namespace CHTL