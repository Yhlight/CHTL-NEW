#pragma once

#include "CJMODCore.h"
#include <string>
#include <vector>

namespace CJMOD {

/**
 * CJMOD Syntax类
 * 语法分析类，负责对语法进行解析
 * 严格按照CJMOD.md实现
 */
class Syntax {
public:
    /**
     * 分析语法，返回一个Arg对象
     * @param pattern 语法模式字符串
     * @return 解析出的Arg对象
     */
    static Arg analyze(const std::string& pattern);
    
    /**
     * 判断是否是JS对象
     * @param code 代码字符串
     * @return 是否是JS对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 判断是否是JS函数
     * @param code 代码字符串
     * @return 是否是JS函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 判断是否是JS数组
     * @param code 代码字符串
     * @return 是否是JS数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 判断是否是CHTL JS函数
     * @param code 代码字符串
     * @return 是否是CHTL JS函数
     */
    static bool isCHTLJSFunction(const std::string& code);

private:
    /**
     * 解析语法模式
     * @param pattern 模式字符串
     * @return 解析出的原子参数列表
     */
    static std::vector<AtomArg> parsePattern(const std::string& pattern);
    
    /**
     * 识别占位符类型
     * @param token 令牌字符串
     * @return 占位符类型
     */
    static AtomArgType identifyPlaceholderType(const std::string& token);
};

} // namespace CJMOD