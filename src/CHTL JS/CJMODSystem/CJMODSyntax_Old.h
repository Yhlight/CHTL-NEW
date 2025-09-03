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
    
    /**
     * 检查是否为对象 (实例方法)
     * @param code 代码片段
     * @return 是否为对象
     */
    bool checkIsObject(const std::string& code);
    
    /**
     * 检查是否为函数 (实例方法)
     * @param code 代码片段
     * @return 是否为函数
     */
    bool checkIsFunction(const std::string& code);
    
    /**
     * 检查是否为数组 (实例方法)
     * @param code 代码片段
     * @return 是否为数组
     */
    bool checkIsArray(const std::string& code);
    
    /**
     * 检查是否为CHTL JS函数 (实例方法)
     * @param code 代码片段
     * @return 是否为CHTL JS函数
     */
    bool checkIsCHTLJSFunction(const std::string& code);
    
    /**
     * 获取类型名称
     * @param code 代码片段
     * @return 类型名称
     */
    std::string getType(const std::string& code);
    
    /**
     * 设置类型映射
     * @param pattern 模式
     * @param type 类型
     */
    void setTypeMapping(const std::string& pattern, const std::string& type);
    
    /**
     * 获取分析结果
     * @return 分析结果
     */
    const SyntaxAnalysisResult& getAnalysisResult() const { return m_AnalysisResult; }

private:
    /**
     * 初始化默认类型映射
     */
    void initializeDefaultTypeMappings();
    
    /**
     * 分析对象语法
     * @param code 代码
     * @return 是否为对象
     */
    bool analyzeObjectSyntax(const std::string& code);
    
    /**
     * 分析函数语法
     * @param code 代码
     * @return 是否为函数
     */
    bool analyzeFunctionSyntax(const std::string& code);
    
    /**
     * 分析数组语法
     * @param code 代码
     * @return 是否为数组
     */
    bool analyzeArraySyntax(const std::string& code);
    
    /**
     * 分析CHTL JS函数语法
     * @param code 代码
     * @return 是否为CHTL JS函数
     */
    bool analyzeCHTLJSFunctionSyntax(const std::string& code);
};

/**
 * 占位符类型枚举
 */
enum class PlaceholderType {
    REQUIRED_PLACEHOLDER,       // $ - 必需占位符
    OPTIONAL_PLACEHOLDER,       // $? - 可选占位符
    BOOLEAN_PLACEHOLDER,        // $! - 布尔占位符
    IGNORE_PLACEHOLDER,         // $_ - 忽略占位符
    VARIADIC_PLACEHOLDER        // ... - 可变参数占位符
};

/**
 * 原子参数类
 * 表示CJMOD中的占位符参数
 */
class AtomArg {
private:
    PlaceholderType m_Type;                             // 占位符类型
    std::string m_Name;                                 // 参数名称
    std::string m_DefaultValue;                         // 默认值
    bool m_IsResolved;                                  // 是否已解析

public:
    /**
     * 构造函数
     * @param type 占位符类型
     * @param name 参数名称
     * @param defaultValue 默认值
     */
    AtomArg(PlaceholderType type, const std::string& name, const std::string& defaultValue = "");
    
    /**
     * 析构函数
     */
    ~AtomArg() = default;
    
    /**
     * 获取占位符类型
     * @return 占位符类型
     */
    PlaceholderType getType() const { return m_Type; }
    
    /**
     * 获取参数名称
     * @return 参数名称
     */
    const std::string& getName() const { return m_Name; }
    
    /**
     * 获取默认值
     * @return 默认值
     */
    const std::string& getDefaultValue() const { return m_DefaultValue; }
    
    /**
     * 检查是否已解析
     * @return 是否已解析
     */
    bool isResolved() const { return m_IsResolved; }
    
    /**
     * 设置解析状态
     * @param resolved 是否已解析
     */
    void setResolved(bool resolved) { m_IsResolved = resolved; }
    
    /**
     * 获取占位符字符串表示
     * @return 占位符字符串
     */
    std::string getPlaceholderString() const;
    
    /**
     * 解析占位符
     * @param placeholderStr 占位符字符串
     * @return 原子参数
     */
    static std::unique_ptr<AtomArg> parsePlaceholder(const std::string& placeholderStr);
};

/**
 * 参数类
 * 表示CJMOD中的参数绑定和转换
 */
class Arg {
private:
    std::string m_Name;                                 // 参数名称
    std::string m_Value;                                // 参数值
    std::string m_Type;                                 // 参数类型
    std::vector<std::unique_ptr<AtomArg>> m_AtomArgs;   // 原子参数列表
    std::function<std::string(const std::string&)> m_Transform; // 转换函数

public:
    /**
     * 构造函数
     * @param name 参数名称
     * @param value 参数值
     * @param type 参数类型
     */
    Arg(const std::string& name, const std::string& value, const std::string& type = "string");
    
    /**
     * 析构函数
     */
    ~Arg() = default;
    
    /**
     * 绑定参数值
     * @param value 参数值
     * @return 是否成功
     */
    bool bind(const std::string& value);
    
    /**
     * 填充值到占位符
     * @param template_str 模板字符串
     * @return 填充后的字符串
     */
    std::string fillValue(const std::string& template_str) const;
    
    /**
     * 转换参数值
     * @param transformer 转换函数
     * @return 转换后的值
     */
    std::string transform(std::function<std::string(const std::string&)> transformer);
    
    /**
     * 添加原子参数
     * @param atomArg 原子参数
     */
    void addAtomArg(std::unique_ptr<AtomArg> atomArg);
    
    /**
     * 获取参数名称
     * @return 参数名称
     */
    const std::string& getName() const { return m_Name; }
    
    /**
     * 获取参数值
     * @return 参数值
     */
    const std::string& getValue() const { return m_Value; }
    
    /**
     * 获取参数类型
     * @return 参数类型
     */
    const std::string& getType() const { return m_Type; }
    
    /**
     * 获取原子参数列表
     * @return 原子参数列表
     */
    const std::vector<std::unique_ptr<AtomArg>>& getAtomArgs() const { return m_AtomArgs; }

private:
    /**
     * 解析模板字符串中的占位符
     * @param template_str 模板字符串
     * @return 占位符列表
     */
    std::vector<std::string> parsePlaceholders(const std::string& template_str) const;
    
    /**
     * 应用转换函数
     * @param value 原始值
     * @return 转换后的值
     */
    std::string applyTransform(const std::string& value);
};

} // namespace CJMOD
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
    void fillValue(const Arg& result);
    
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

private:
    /**
     * 解析语法模式
     * @param pattern 语法模式
     */
    void parsePattern(const std::string& pattern);
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
    std::string getValue() const;
    
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
    
    /**
     * 生成JavaScript函数引用
     * @param key 键名
     * @return JavaScript代码
     */
    std::string generateFunctionReference(const std::string& key) const;
    
    /**
     * 生成JavaScript对象引用
     * @param key 键名
     * @return JavaScript代码
     */
    std::string generateObjectReference(const std::string& key) const;

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