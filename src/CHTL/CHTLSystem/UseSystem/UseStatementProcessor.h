#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * use语句类型
 */
enum class UseStatementType {
    HTML5_DECLARATION,      // use html5;
    CONFIG_USAGE,           // use @Config Name;
    CONFIG_FULL_USAGE,      // use [Configuration] @Config Name;
    TEMPLATE_USAGE,         // use [Template] @Style Name;
    CUSTOM_USAGE,           // use [Custom] @Element Name;
    ORIGIN_USAGE            // use [Origin] @Html Name;
};

/**
 * use语句信息
 */
struct UseStatementInfo {
    UseStatementType Type;              // use语句类型
    std::string Target;                 // 目标名称
    std::string FullTarget;             // 完整目标 (包含类型前缀)
    std::string SourceFile;             // 来源文件
    bool IsProcessed = false;           // 是否已处理
    std::string ProcessedResult;        // 处理结果
};

/**
 * use语句处理器
 * 实现完整的CHTL use语法
 * 支持HTML5声明、配置组使用、全缀名使用等
 */
class UseStatementProcessor {
private:
    // use语句存储
    std::vector<UseStatementInfo> m_UseStatements;
    
    // 当前文件状态
    std::string m_CurrentFile;
    bool m_HasUseStatement = false;
    UseStatementInfo m_ActiveUseStatement;
    
    // HTML5模式
    bool m_IsHTML5Mode = false;

public:
    /**
     * 构造函数
     */
    UseStatementProcessor();
    
    /**
     * 析构函数
     */
    ~UseStatementProcessor() = default;
    
    /**
     * 处理use语句
     * @param useStatement use语句内容
     * @param sourceFile 来源文件
     * @return 是否成功处理
     */
    bool ProcessUseStatement(const std::string& useStatement, const std::string& sourceFile = "");
    
    /**
     * 解析use语句类型
     * @param useContent use语句内容
     * @return use语句信息
     */
    UseStatementInfo ParseUseStatement(const std::string& useContent);
    
    /**
     * 处理HTML5声明
     * @return 生成的HTML5声明
     */
    std::string ProcessHTML5Declaration();
    
    /**
     * 处理配置组使用
     * @param configName 配置组名称
     * @param isFullName 是否是全缀名
     * @return 是否成功处理
     */
    bool ProcessConfigUsage(const std::string& configName, bool isFullName = false);
    
    /**
     * 处理模板使用
     * @param templateName 模板名称
     * @param templateType 模板类型
     * @return 是否成功处理
     */
    bool ProcessTemplateUsage(const std::string& templateName, const std::string& templateType);
    
    /**
     * 处理自定义使用
     * @param customName 自定义名称
     * @param customType 自定义类型
     * @return 是否成功处理
     */
    bool ProcessCustomUsage(const std::string& customName, const std::string& customType);
    
    /**
     * 处理原始嵌入使用
     * @param originName 原始嵌入名称
     * @param originType 原始嵌入类型
     * @return 是否成功处理
     */
    bool ProcessOriginUsage(const std::string& originName, const std::string& originType);
    
    /**
     * 验证use语句位置 (必须在文件开头)
     * @param currentLine 当前行号
     * @param hasNonCommentContent 是否已有非注释内容
     * @return 是否位置正确
     */
    bool ValidateUseStatementPosition(size_t currentLine, bool hasNonCommentContent);
    
    /**
     * 验证use语句唯一性 (每个文件只能有一个配置组use)
     * @param newUseType use语句类型
     * @return 是否允许添加
     */
    bool ValidateUseStatementUniqueness(UseStatementType newUseType);
    
    /**
     * 检查是否是HTML5模式
     * @return 是否是HTML5模式
     */
    bool IsHTML5Mode() const { return m_IsHTML5Mode; }
    
    /**
     * 获取激活的use语句
     * @return use语句信息
     */
    const UseStatementInfo& GetActiveUseStatement() const { return m_ActiveUseStatement; }
    
    /**
     * 获取所有use语句
     * @return use语句列表
     */
    const std::vector<UseStatementInfo>& GetAllUseStatements() const { return m_UseStatements; }
    
    /**
     * 获取use语句统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, size_t> GetUseStatementStatistics();
    
    /**
     * 验证所有use语句
     * @return 验证结果
     */
    bool ValidateAllUseStatements();
    
    /**
     * 重置处理器状态
     */
    void Reset();
    
    /**
     * 设置当前文件
     * @param fileName 文件名
     */
    void SetCurrentFile(const std::string& fileName);

private:
    /**
     * 解析配置组名称
     * @param configExpression 配置表达式
     * @return 解析后的配置名称和是否是全缀名
     */
    std::pair<std::string, bool> ParseConfigName(const std::string& configExpression);
    
    /**
     * 解析模板/自定义表达式
     * @param expression 表达式
     * @return 解析后的类型和名称
     */
    std::pair<std::string, std::string> ParseTemplateCustomExpression(const std::string& expression);
    
    /**
     * 验证use语句语法
     * @param useContent use语句内容
     * @return 是否语法正确
     */
    bool ValidateUseStatementSyntax(const std::string& useContent);
    
    /**
     * 生成use语句的处理结果
     * @param useInfo use语句信息
     * @return 处理结果
     */
    std::string GenerateUseStatementResult(const UseStatementInfo& useInfo);
};

} // namespace CHTL