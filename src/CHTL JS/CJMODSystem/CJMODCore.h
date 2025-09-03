#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

/**
 * CJMOD核心API定义
 * 这是CHTL极为强大的特征之一，提供C++ API扩展能力
 */

namespace CJMOD {

/**
 * 原子参数类型枚举
 * 按照CJMOD.md规范定义
 */
enum class AtomArgType {
    PLACEHOLDER,                    // $ 占位符
    OPTIONAL_PLACEHOLDER,           // $? 可选占位符
    REQUIRED_PLACEHOLDER,           // $! 必须占位符
    UNORDERED_PLACEHOLDER,          // $_ 无序占位符
    REQUIRED_UNORDERED_PLACEHOLDER, // $!_ 必须无序占位符
    VARIADIC_PLACEHOLDER,           // ... 不定参数占位符
    OPERATOR,                       // 操作符（如 **）
    LITERAL                         // 字面量
};

// 前向声明
class AtomArg;
class Arg;
class Syntax;
class CJMODScanner;
class CJMODGenerator;

/**
 * CJMOD原子参数类
 * 处理单个占位符参数
 */
class AtomArg {
private:
    std::string m_Pattern;                      // 参数模式
    std::string m_Value;                        // 参数值
    AtomArgType m_Type;                         // 参数类型
    bool m_IsOptional;                          // 是否可选 ($?)
    bool m_HasValue;                            // 是否已设置值
    std::function<std::string(const std::string&)> m_BindFunction; // 绑定的处理函数

public:
    /**
     * 构造函数
     * @param pattern 参数模式 ("$" 或 "$?" 或 "$!" 或 "$_" 或 "...")
     */
    explicit AtomArg(const std::string& pattern = "$");
    
    /**
     * 构造函数（带类型）
     * @param pattern 参数模式
     * @param type 参数类型
     */
    AtomArg(const std::string& pattern, AtomArgType type);
    
    /**
     * 拷贝构造函数
     */
    AtomArg(const AtomArg& other);
    
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
     * 获取参数类型
     * @return 参数类型
     */
    AtomArgType getType() const { return m_Type; }
    
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
    
    /**
     * 静态工厂方法：解析占位符
     * @param placeholder 占位符字符串
     * @return AtomArg智能指针
     */
    static std::unique_ptr<AtomArg> parsePlaceholder(const std::string& placeholder);
};

/**
 * CJMOD参数类
 * 包含解析出的参数列表
 */
class Arg {
private:
    std::string m_Pattern;                      // 参数模式
    std::vector<std::unique_ptr<AtomArg>> m_Args; // 原子参数列表
    std::string m_TransformResult;              // 转换结果
    std::string m_TransformExpression;          // 转换表达式
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> m_BindFunctions; // 绑定函数映射

public:
    /**
     * 默认构造函数
     */
    Arg() = default;
    
    /**
     * 构造函数
     * @param pattern 参数模式
     */
    explicit Arg(const std::string& pattern);
    
    /**
     * 构造函数（多值）
     * @param values 值列表
     */
    explicit Arg(const std::vector<std::string>& values);
    
    /**
     * 拷贝构造函数
     */
    Arg(const Arg& other);
    
    /**
     * 移动构造函数
     */
    Arg(Arg&& other) noexcept;
    
    /**
     * 拷贝赋值操作符
     */
    Arg& operator=(const Arg& other);
    
    /**
     * 移动赋值操作符
     */
    Arg& operator=(Arg&& other) noexcept;
    
    /**
     * 构造函数（完整参数）
     * @param name 参数名
     * @param value 参数值
     * @param type 参数类型
     */
    Arg(const std::string& name, const std::string& value, const std::string& type);
    
    /**
     * 析构函数
     */
    ~Arg() = default;
    
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
     * 绑定获取值的函数
     * @param placeholder 占位符
     * @param bindFunc 绑定函数
     */
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> bindFunc);
    
    /**
     * 打印参数列表（调试用）
     */
    void print() const;
    
    /**
     * 检查是否有转换
     * @return 是否有转换
     */
    bool hasTransformation() const;
    
    /**
     * 获取转换表达式
     * @return 转换表达式
     */
    std::string getTransformation() const;
    
    /**
     * 添加原子参数
     * @param atom 原子参数
     */
    void addAtomArg(const AtomArg& atom);
    
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
    
    /**
     * 绑定值（用于测试）
     * @param value 值
     * @return 是否成功
     */
    bool bind(const std::string& value);

    // 类型检查属性（用于向后兼容）
    std::string Type;
    bool IsObject = false;
    bool IsFunction = false;
    bool IsCHTLJSFunction = false;

private:
    /**
     * 解析语法模式
     * @param pattern 语法模式
     */
    void parsePattern(const std::string& pattern);
};

/**
 * CJMOD语法分析器
 * 提供语法分析和类型检测
 */
class Syntax {
public:
    /**
     * 分析代码语法
     * @param code 代码
     * @return 分析结果
     */
    static Arg analyzeCode(const std::string& code);
    
    /**
     * 分析语法模式（CJMOD.md要求）
     * @param pattern 语法模式
     * @return 解析出的Arg对象
     */
    static Arg analyze(const std::string& pattern);
    
    /**
     * 检查是否为对象
     * @param code 代码
     * @return 是否为对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 检查是否为函数
     * @param code 代码
     * @return 是否为函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 检查是否为数组
     * @param code 代码
     * @return 是否为数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 检查是否为CHTL JS函数
     * @param code 代码
     * @return 是否为CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);

private:
    /**
     * 实例方法：检查是否为对象
     */
    bool checkIsObject(const std::string& code);
    
    /**
     * 实例方法：检查是否为函数
     */
    bool checkIsFunction(const std::string& code);
    
    /**
     * 实例方法：检查是否为数组
     */
    bool checkIsArray(const std::string& code);
    
    /**
     * 实例方法：检查是否为CHTL JS函数
     */
    bool checkIsCHTLJSFunction(const std::string& code);
};

/**
 * CJMOD扫描器
 * 提供扫描和占位符处理能力
 */
class CJMODScanner {
public:
    /**
     * 扫描占位符
     * @param code 代码
     * @return 占位符列表
     */
    static std::vector<std::unique_ptr<AtomArg>> scanPlaceholders(const std::string& code);
    
    /**
     * 静态扫描方法
     * @param code 代码
     * @param pattern 模式
     * @return 扫描结果
     */
    static Arg scan(const std::string& code, const std::string& pattern);
    
    /**
     * 扫描语法片段（CJMOD.md要求）
     * @param args 参数对象
     * @param keyword 扫描的关键字
     * @return 扫描结果
     */
    static Arg scan(const Arg& args, const std::string& keyword = "");
    
    /**
     * 处理占位符替换
     * @param code 代码
     * @param placeholders 占位符映射
     * @return 替换后的代码
     */
    static std::string processPlaceholderReplacement(const std::string& code, 
                                                   const std::unordered_map<std::string, std::string>& placeholders);
};

/**
 * CJMOD函数信息结构
 */
struct CJMODFunctionInfo {
    std::string FunctionType;                   // 函数类型
    std::string FunctionName;                   // 函数名称
    std::string FunctionBody;                   // 函数体
    std::unordered_map<std::string, std::string> Metadata; // 元数据
};

/**
 * CJMOD生成器
 * 提供代码生成能力
 */
class CJMODGenerator {
public:
    /**
     * 导出JavaScript代码
     * @return JavaScript代码
     */
    static std::string exportJavaScript();
    
    /**
     * 静态导出结果方法
     * @param result 结果
     * @return 导出的代码
     */
    static std::string exportResult(const std::string& result);
    
    /**
     * 导出最终的JS代码（CJMOD.md要求）
     * @param args 参数对象
     * @return 导出的JavaScript代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * 处理占位符绑定
     * @param template_str 模板字符串
     * @param args 参数映射
     * @return 绑定后的代码
     */
    static std::string processPlaceholderBinding(const std::string& template_str,
                                                const std::unordered_map<std::string, Arg>& args);
    
    /**
     * 生成虚拟对象绑定
     * @param virName 虚拟对象名
     * @param functionInfo 函数信息
     * @return 绑定代码
     */
    static std::string generateVirtualObjectBinding(const std::string& virName, const CJMODFunctionInfo& functionInfo);
};

/**
 * CJMOD函数生成器
 * 专门用于创建和管理CHTL JS函数
 */
class CJMODFunctionGenerator {
public:
    /**
     * 创建CHTL JS函数
     * @param functionType 函数类型（listen/delegate/animate）
     * @param functionName 函数名称
     * @param functionBody 函数体
     * @return CHTL JS函数信息
     */
    static CJMODFunctionInfo CreateCHTLJSFunction(const std::string& functionType,
                                                 const std::string& functionName,
                                                 const std::string& functionBody);
    
    /**
     * 绑定虚拟对象
     * @param virName 虚拟对象名
     * @param functionInfo 函数信息
     * @return 绑定代码
     */
    static std::string bindVirtualObject(const std::string& virName, const CJMODFunctionInfo& functionInfo);
    
    /**
     * 生成函数包装器
     * @param functionInfo 函数信息
     * @return 包装器代码
     */
    static std::string generateFunctionWrapper(const CJMODFunctionInfo& functionInfo);
    
    /**
     * 验证CHTL JS函数
     * @param functionCode 函数代码
     * @return 是否有效
     */
    static bool validateCHTLJSFunction(const std::string& functionCode);

private:
    /**
     * 检测值类型
     * @param value 值
     * @return 类型字符串
     */
    static std::string detectValueType(const std::string& value);
};

} // namespace CJMOD