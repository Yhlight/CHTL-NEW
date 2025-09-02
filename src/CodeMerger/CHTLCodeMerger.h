#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * 代码片段类型
 */
enum class CodeFragmentType {
    HTML_FRAGMENT,              // HTML片段
    CSS_FRAGMENT,               // CSS片段
    JAVASCRIPT_FRAGMENT,        // JavaScript片段
    CHTL_FRAGMENT,              // CHTL片段
    CHTLJS_FRAGMENT            // CHTL JS片段
};

/**
 * 合并器代码片段信息
 */
struct MergerCodeFragment {
    CodeFragmentType Type;                              // 片段类型
    std::string Content;                                // 片段内容
    std::string SourceFile;                             // 源文件
    size_t LineNumber;                                  // 行号
    size_t ColumnNumber;                                // 列号
    std::string Context;                                // 上下文信息
    std::unordered_map<std::string, std::string> Metadata; // 元数据
    
    MergerCodeFragment() : Type(CodeFragmentType::HTML_FRAGMENT), LineNumber(0), ColumnNumber(0) {}
    MergerCodeFragment(CodeFragmentType type, const std::string& content, 
                const std::string& sourceFile = "", size_t line = 0, size_t column = 0)
        : Type(type), Content(content), SourceFile(sourceFile), LineNumber(line), ColumnNumber(column) {}
};

/**
 * 合并结果
 */
struct CodeMergeResult {
    std::string MergedHTML;                             // 合并后的HTML
    std::string MergedCSS;                              // 合并后的CSS
    std::string MergedJavaScript;                       // 合并后的JavaScript
    std::string FullHTML;                               // 完整的HTML（包含内联CSS/JS）
    std::vector<std::string> Errors;                    // 错误列表
    std::vector<std::string> Warnings;                  // 警告列表
    bool IsSuccess;                                     // 是否成功
    
    CodeMergeResult() : IsSuccess(false) {}
};

/**
 * 合并策略枚举
 */
enum class MergeStrategy {
    INLINE_MERGE,               // 内联合并（CSS/JS嵌入HTML）
    SEPARATE_FILES,             // 分离文件（CSS/JS独立文件）
    HYBRID_MERGE                // 混合合并（关键CSS内联，其他分离）
};

/**
 * 合并选项
 */
struct MergeOptions {
    MergeStrategy Strategy;                             // 合并策略
    bool MinifyCSS;                                     // 是否压缩CSS
    bool MinifyJavaScript;                              // 是否压缩JavaScript
    bool RemoveComments;                                // 是否移除注释
    bool OptimizeOrder;                                 // 是否优化顺序
    bool GenerateSourceMap;                             // 是否生成源映射
    std::string CSSFileName;                            // CSS文件名
    std::string JavaScriptFileName;                     // JavaScript文件名
    
    MergeOptions() : Strategy(MergeStrategy::INLINE_MERGE), MinifyCSS(false), 
                    MinifyJavaScript(false), RemoveComments(false), 
                    OptimizeOrder(true), GenerateSourceMap(false),
                    CSSFileName("styles.css"), JavaScriptFileName("scripts.js") {}
};

/**
 * CHTL代码合并器
 * 负责将CHTL和CHTL JS编译器处理的片段合并为完整代码
 * 然后交给CSS和JavaScript编译器进行最终处理
 */
class CHTLCodeMerger {
private:
    std::vector<MergerCodeFragment> m_HTMLFragments;          // HTML片段列表
    std::vector<MergerCodeFragment> m_CSSFragments;           // CSS片段列表
    std::vector<MergerCodeFragment> m_JavaScriptFragments;    // JavaScript片段列表
    MergeOptions m_Options;                             // 合并选项
    
    // 合并状态
    std::string m_CurrentSourceFile;                    // 当前源文件
    size_t m_MergeCounter;                              // 合并计数器

public:
    /**
     * 构造函数
     */
    CHTLCodeMerger();
    
    /**
     * 析构函数
     */
    ~CHTLCodeMerger() = default;
    
    /**
     * 添加HTML片段
     * @param content 片段内容
     * @param sourceFile 源文件
     * @param line 行号
     * @param column 列号
     */
    void AddHTMLFragment(const std::string& content, const std::string& sourceFile = "", 
                        size_t line = 0, size_t column = 0);
    
    /**
     * 添加CSS片段
     * @param content 片段内容
     * @param sourceFile 源文件
     * @param line 行号
     * @param column 列号
     */
    void AddCSSFragment(const std::string& content, const std::string& sourceFile = "", 
                       size_t line = 0, size_t column = 0);
    
    /**
     * 添加JavaScript片段
     * @param content 片段内容
     * @param sourceFile 源文件
     * @param line 行号
     * @param column 列号
     */
    void AddJavaScriptFragment(const std::string& content, const std::string& sourceFile = "", 
                              size_t line = 0, size_t column = 0);
    
    /**
     * 批量添加片段
     * @param fragments 片段列表
     */
    void AddFragments(const std::vector<MergerCodeFragment>& fragments);
    
    /**
     * 执行代码合并
     * @return 合并结果
     */
    CodeMergeResult MergeCode();
    
    /**
     * 合并CSS片段为完整CSS
     * @return 完整CSS代码
     */
    std::string MergeCSSFragments();
    
    /**
     * 合并JavaScript片段为完整JavaScript
     * @return 完整JavaScript代码
     */
    std::string MergeJavaScriptFragments();
    
    /**
     * 合并HTML片段
     * @return 完整HTML代码
     */
    std::string MergeHTMLFragments();
    
    /**
     * 生成完整HTML文档
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JavaScript内容
     * @return 完整HTML文档
     */
    std::string GenerateCompleteHTML(const std::string& htmlContent, 
                                    const std::string& cssContent, 
                                    const std::string& jsContent);
    
    /**
     * 设置合并选项
     * @param options 合并选项
     */
    void SetMergeOptions(const MergeOptions& options);
    
    /**
     * 获取合并选项
     * @return 合并选项
     */
    const MergeOptions& GetMergeOptions() const { return m_Options; }
    
    /**
     * 设置当前源文件
     * @param sourceFile 源文件路径
     */
    void SetCurrentSourceFile(const std::string& sourceFile);
    
    /**
     * 获取片段统计
     * @return 片段数量统计
     */
    std::unordered_map<std::string, size_t> GetFragmentStatistics() const;
    
    /**
     * 清除所有片段
     */
    void ClearFragments();
    
    /**
     * 重置合并器
     */
    void Reset();
    
    /**
     * 验证片段完整性
     * @return 是否完整
     */
    bool ValidateFragments() const;

private:
    /**
     * 按类型分组片段
     * @param fragments 片段列表
     * @return 分组后的片段
     */
    std::unordered_map<CodeFragmentType, std::vector<MergerCodeFragment>> 
        GroupFragmentsByType(const std::vector<MergerCodeFragment>& fragments);
    
    /**
     * 优化片段顺序
     * @param fragments 片段列表
     * @return 优化后的片段列表
     */
    std::vector<MergerCodeFragment> OptimizeFragmentOrder(const std::vector<MergerCodeFragment>& fragments);
    
    /**
     * 合并同类型片段
     * @param fragments 同类型片段列表
     * @return 合并后的内容
     */
    std::string MergeSameTypeFragments(const std::vector<MergerCodeFragment>& fragments);
    
    /**
     * 生成源映射
     * @param originalFragments 原始片段
     * @param mergedContent 合并后内容
     * @return 源映射字符串
     */
    std::string GenerateSourceMap(const std::vector<MergerCodeFragment>& originalFragments, 
                                 const std::string& mergedContent);
    
    /**
     * 内联合并策略
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JavaScript内容
     * @return 合并结果
     */
    CodeMergeResult InlineMergeStrategy(const std::string& htmlContent, 
                                       const std::string& cssContent, 
                                       const std::string& jsContent);
    
    /**
     * 分离文件合并策略
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JavaScript内容
     * @return 合并结果
     */
    CodeMergeResult SeparateFilesMergeStrategy(const std::string& htmlContent, 
                                              const std::string& cssContent, 
                                              const std::string& jsContent);
    
    /**
     * 混合合并策略
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JavaScript内容
     * @return 合并结果
     */
    CodeMergeResult HybridMergeStrategy(const std::string& htmlContent, 
                                       const std::string& cssContent, 
                                       const std::string& jsContent);
    
    /**
     * 添加片段元数据
     * @param fragment 片段引用
     * @param key 元数据键
     * @param value 元数据值
     */
    void AddFragmentMetadata(MergerCodeFragment& fragment, const std::string& key, const std::string& value);
    
    /**
     * 验证CSS片段
     * @param cssContent CSS内容
     * @return 是否有效
     */
    bool ValidateCSSFragment(const std::string& cssContent) const;
    
    /**
     * 验证JavaScript片段
     * @param jsContent JavaScript内容
     * @return 是否有效
     */
    bool ValidateJavaScriptFragment(const std::string& jsContent) const;
};

/**
 * 代码合并器工厂
 * 用于创建不同配置的代码合并器
 */
class CodeMergerFactory {
public:
    /**
     * 创建内联合并器
     * @return 代码合并器
     */
    static std::unique_ptr<CHTLCodeMerger> CreateInlineMerger();
    
    /**
     * 创建分离文件合并器
     * @param cssFileName CSS文件名
     * @param jsFileName JavaScript文件名
     * @return 代码合并器
     */
    static std::unique_ptr<CHTLCodeMerger> CreateSeparateFilesMerger(
        const std::string& cssFileName = "styles.css",
        const std::string& jsFileName = "scripts.js");
    
    /**
     * 创建混合合并器
     * @param criticalCSSThreshold 关键CSS阈值
     * @return 代码合并器
     */
    static std::unique_ptr<CHTLCodeMerger> CreateHybridMerger(size_t criticalCSSThreshold = 1024);
    
    /**
     * 创建生产环境合并器
     * @return 代码合并器
     */
    static std::unique_ptr<CHTLCodeMerger> CreateProductionMerger();
    
    /**
     * 创建开发环境合并器
     * @return 代码合并器
     */
    static std::unique_ptr<CHTLCodeMerger> CreateDevelopmentMerger();
};

} // namespace CHTL