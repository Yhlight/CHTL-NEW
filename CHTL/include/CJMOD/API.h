#ifndef CHTL_CJMOD_API_H
#define CHTL_CJMOD_API_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace CHTL {
namespace CJMOD {

/**
 * 原子参数类型
 */
enum class AtomArgType {
    Placeholder,        // $ $? $! $_ 等占位符
    Operator,          // 操作符
    Literal,           // 字面量
    Spread             // ... 扩展运算符
};

/**
 * 原子参数
 */
struct AtomArg {
    AtomArgType type;
    std::string value;
    std::string originalValue;  // 原始值（扫描前）
    
    AtomArg(AtomArgType t, const std::string& v) 
        : type(t), value(v), originalValue(v) {}
};

/**
 * 参数列表类
 */
class Arg {
private:
    std::vector<AtomArg> atoms;
    std::map<std::string, std::function<std::string(const std::string&)>> bindings;
    
public:
    Arg() = default;
    
    /**
     * 添加原子参数
     */
    void AddAtom(const AtomArg& atom) {
        atoms.push_back(atom);
    }
    
    /**
     * 绑定占位符的获取函数
     */
    void bind(const std::string& placeholder, 
              std::function<std::string(const std::string&)> getter) {
        bindings[placeholder] = getter;
    }
    
    /**
     * 填充值
     */
    void fillValue(const Arg& scannedArg);
    
    /**
     * 转换为目标代码
     */
    std::string transform(const std::string& pattern) const;
    
    /**
     * 打印参数列表
     */
    void print() const;
    
    /**
     * 获取原子参数
     */
    const std::vector<AtomArg>& GetAtoms() const { return atoms; }
    AtomArg& operator[](size_t index) { return atoms[index]; }
    const AtomArg& operator[](size_t index) const { return atoms[index]; }
    
    size_t size() const { return atoms.size(); }
};

/**
 * 语法分析类
 */
class Syntax {
public:
    /**
     * 分析语法，返回参数列表
     */
    static Arg analyze(const std::string& pattern);
    
    /**
     * 判断是否是JS对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 判断是否是JS函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 判断是否是JS数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 判断是否是CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);
};

/**
 * CJMOD扫描器
 */
class CJMODScanner {
public:
    /**
     * 扫描代码，返回匹配的参数
     */
    static Arg scan(const Arg& pattern, const std::string& code);
};

/**
 * CJMOD生成器
 */
class CJMODGenerator {
public:
    /**
     * 导出结果
     */
    static void exportResult(const Arg& args);
};

/**
 * CHTL JS函数类
 */
class CHTLJSFunction {
private:
    std::string name;
    std::map<std::string, std::string> properties;
    std::string body;
    
public:
    CHTLJSFunction(const std::string& funcName) : name(funcName) {}
    
    /**
     * 添加属性
     */
    void AddProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }
    
    /**
     * 设置函数体
     */
    void SetBody(const std::string& funcBody) {
        body = funcBody;
    }
    
    /**
     * 获取函数名
     */
    const std::string& GetName() const { return name; }
    
    /**
     * 获取属性
     */
    const std::map<std::string, std::string>& GetProperties() const { return properties; }
    
    /**
     * 获取函数体
     */
    const std::string& GetBody() const { return body; }
    
    /**
     * 绑定虚拟对象
     */
    void bindVirtualObject(const std::string& virName, 
                          std::function<std::string(const std::string&)> handler);
};

/**
 * 创建CHTL JS函数
 */
std::shared_ptr<CHTLJSFunction> CreateCHTLJSFunction(const std::string& code);

/**
 * CJMOD模块接口
 * 所有CJMOD模块必须实现这个接口
 */
class ICJMODModule {
public:
    virtual ~ICJMODModule() = default;
    
    /**
     * 获取模块名称
     */
    virtual std::string GetModuleName() const = 0;
    
    /**
     * 获取模块版本
     */
    virtual std::string GetModuleVersion() const = 0;
    
    /**
     * 初始化模块
     */
    virtual bool Initialize() = 0;
    
    /**
     * 清理模块
     */
    virtual void Cleanup() = 0;
    
    /**
     * 注册语法扩展
     */
    virtual void RegisterSyntaxExtensions() = 0;
};

/**
 * CJMOD模块工厂函数类型
 */
typedef ICJMODModule* (*CreateModuleFunc)();
typedef void (*DestroyModuleFunc)(ICJMODModule*);

} // namespace CJMOD
} // namespace CHTL

// 导出宏（用于模块开发）
#ifdef _WIN32
    #define CJMOD_EXPORT extern "C" __declspec(dllexport)
#else
    #define CJMOD_EXPORT extern "C" __attribute__((visibility("default")))
#endif

// 模块定义宏
#define CJMOD_MODULE(ModuleClass) \
    CJMOD_EXPORT CHTL::CJMOD::ICJMODModule* CreateModule() { \
        return new ModuleClass(); \
    } \
    CJMOD_EXPORT void DestroyModule(CHTL::CJMOD::ICJMODModule* module) { \
        delete module; \
    }

#endif // CHTL_CJMOD_API_H