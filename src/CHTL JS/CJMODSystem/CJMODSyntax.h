#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace CJMOD {

// 前向声明
class Arg;
class AtomArg;

/**
 * CJMOD语法分析结果
 */
struct SyntaxAnalysisResult {
    bool IsObject;                      // 是否为对象
    bool IsFunction;                    // 是否为函数
    bool IsArray;                       // 是否为数组
    bool IsCHTLJSFunction;              // 是否为CHTL JS函数
    std::string Type;                   // 类型名称
    std::string Content;                // 内容
    std::vector<std::string> Parameters; // 参数列表
    
    SyntaxAnalysisResult() : IsObject(false), IsFunction(false), IsArray(false), IsCHTLJSFunction(false) {}
};

/**
 * AtomArg类 - 原子参数
 * CJMOD API的基础参数单元，支持$和$?占位符
 */
class AtomArg {
private:
    std::string m_Pattern;                      // 参数模式
    std::string m_Value;                        // 参数值
    bool m_IsOptional;                          // 是否可选 ($?)
    bool m_HasValue;                            // 是否已设置值
    std::function<std::string(const std::string&)> m_BindFunction; // 绑定的处理函数

public:
    /**
     * 构造函数
     * @param pattern 参数模式 ("$" 或 "$?")
     */
    explicit AtomArg(const std::string& pattern);
    
    /**
     * 析构函数
     */
    ~AtomArg() = default;
    
    /**
     * 绑定处理函数
     * @param func 处理函数
     */
    void bind(std::function<std::string(const std::string&)> func);
    
    /**
     * 设置值
     * @param value 值
     */
    void setValue(const std::string& value);
    
    /**
     * 获取值
     * @return 值
     */
    std::string getValue() const { return m_Value; }
    
    /**
     * 是否是可选参数
     * @return 是否可选
     */
    bool isOptional() const { return m_IsOptional; }
    
    /**
     * 是否已有值
     * @return 是否已有值
     */
    bool hasValue() const { return m_HasValue; }
    
    /**
     * 处理值 (应用绑定函数)
     * @param inputValue 输入值
     * @return 处理后的值
     */
    std::string process(const std::string& inputValue);
    
    /**
     * 从字符串解析
     * @param str 输入字符串
     */
    void ParseFromString(const std::string& str);
};

/**
 * Arg类 - 参数列表类
 * CJMOD API的核心类，包含解析出的参数列表
 */
class Arg {
private:
    std::vector<std::unique_ptr<AtomArg>> m_Args;       // 参数列表
    std::string m_Pattern;                              // 语法模式
    std::string m_TransformResult;                      // 转换结果

public:
    /**
     * 构造函数
     */
    Arg() = default;
    
    /**
     * 构造函数
     * @param pattern 语法模式
     */
    explicit Arg(const std::string& pattern);
    
    /**
     * 构造函数
     * @param values 值列表
     */
    explicit Arg(const std::vector<std::string>& values);
    
    /**
     * 析构函数
     */
    ~Arg() = default;
    
    /**
     * 绑定处理函数
     * @param pattern 参数模式
     * @param func 处理函数
     */
    void bind(const std::string& pattern, std::function<std::string(const std::string&)> func);
    
    /**
     * 填充参数值
     * @param result 扫描结果
     */
    void fillValue(const Arg& result) const;
    
    /**
     * 转换为最终代码
     * @param transformExpression 转换表达式
     */
    void transform(const std::string& transformExpression);
    
    /**
     * 打印参数列表
     */
    void print() const;
    
    /**
     * 获取参数数量
     * @return 参数数量
     */
    size_t size() const { return m_Args.size(); }
    
    /**
     * 获取指定索引的参数
     * @param index 索引
     * @return 参数引用
     */
    AtomArg& operator[](size_t index);
    
    /**
     * 获取指定索引的参数 (const版本)
     * @param index 索引
     * @return 参数引用
     */
    const AtomArg& operator[](size_t index) const;
    
    /**
     * 获取转换结果
     * @return 转换结果
     */
    std::string getTransformResult() const { return m_TransformResult; }
    
    /**
     * 转换为字符串
     * @return 字符串表示
     */
    std::string toString() const;

private:
    /**
     * 解析语法模式
     * @param pattern 语法模式
     */
    void parsePattern(const std::string& pattern);
};

/**
 * CJMOD Syntax类
 * 提供语法分析和类型检测功能
 * 用于CJMOD扩展中的语法解析
 */
class Syntax {
private:
    std::string m_SourceCode;                           // 源代码
    SyntaxAnalysisResult m_AnalysisResult;              // 分析结果
    std::unordered_map<std::string, std::string> m_TypeMappings; // 类型映射

public:
    /**
     * 构造函数
     * @param sourceCode 源代码
     */
    explicit Syntax(const std::string& sourceCode);
    
    /**
     * 析构函数
     */
    ~Syntax() = default;
    
    /**
     * 静态方法 - analyze() 语法分析
     * @param syntaxPattern 语法模式 (如: "$ ** $")
     * @return Arg对象，包含解析出的参数列表
     */
    static Arg analyze(const std::string& syntaxPattern);
    
    /**
     * 静态方法 - isObject() 判断是否是JS对象
     * @param code 代码片段
     * @return 是否是对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 静态方法 - isFunction() 判断是否是JS函数
     * @param code 代码片段
     * @return 是否是函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 静态方法 - isArray() 判断是否是JS数组
     * @param code 代码片段
     * @return 是否是数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 静态方法 - isCHTLJSFunction() 判断是否是CHTL JS函数
     * @param code 代码片段
     * @return 是否是CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);
    
    /**
     * 实例方法 - 分析代码片段
     * @param code 代码片段
     * @return 分析结果
     */
    SyntaxAnalysisResult analyzeCode(const std::string& code);

private:
    /**
     * 分析对象语法
     * @param code 代码片段
     * @return 是否为对象
     */
    bool analyzeObjectSyntax(const std::string& code);
    
    /**
     * 分析函数语法
     * @param code 代码片段
     * @return 是否为函数
     */
    bool analyzeFunctionSyntax(const std::string& code);
    
    /**
     * 分析数组语法
     * @param code 代码片段
     * @return 是否为数组
     */
    bool analyzeArraySyntax(const std::string& code);
    
    /**
     * 分析CHTL JS函数语法
     * @param code 代码片段
     * @return 是否为CHTL JS函数
     */
    bool analyzeCHTLJSFunctionSyntax(const std::string& code);
};

/**
 * CHTLJSFunction类 - CHTL JS函数封装
 * 用于CJMOD中处理CHTL JS函数的元信息
 */
class CHTLJSFunction {
private:
    std::string m_FunctionName;                                 // 函数名称
    std::string m_FunctionContent;                              // 函数内容
    std::unordered_map<std::string, std::string> m_KeyValues;   // 键值对
    std::vector<std::string> m_Keys;                            // 键列表
    bool m_IsValid;                                             // 是否有效

public:
    /**
     * 构造函数
     * @param functionContent CHTL JS函数内容
     */
    explicit CHTLJSFunction(const std::string& functionContent);
    
    /**
     * 析构函数
     */
    ~CHTLJSFunction() = default;
    
    /**
     * 获取函数名称
     * @return 函数名称
     */
    std::string getFunctionName() const { return m_FunctionName; }
    
    /**
     * 获取键列表
     * @return 键列表
     */
    std::vector<std::string> getKeys() const { return m_Keys; }
    
    /**
     * 获取键值
     * @param key 键名
     * @return 键值
     */
    std::string getKeyValue(const std::string& key) const;
    
    /**
     * 检查键是否存在
     * @param key 键名
     * @return 是否存在
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * 获取键的类型
     * @param key 键名
     * @return 类型 ("function", "object", "array", "literal")
     */
    std::string getKeyType(const std::string& key) const;
    
    /**
     * 是否是有效的CHTL JS函数
     * @return 是否有效
     */
    bool isValid() const { return m_IsValid; }

private:
    /**
     * 解析CHTL JS函数内容
     * @param content 函数内容
     */
    void parseCHTLJSFunction(const std::string& content);
    
    /**
     * 检测值的类型
     * @param value 值
     * @return 类型
     */
    std::string detectValueType(const std::string& value) const;
};

} // namespace CJMOD