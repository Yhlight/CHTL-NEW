#pragma once

#include <string>

namespace Chtholly {

/**
 * 珂朵莉模块CJMOD部分 - printMylove功能
 * 使用真正的CJMOD API展示极致强大的爱意传递功能
 * 严格按照CJMOD.md规范，体现CJMOD作为CHTL极为强大特征的能力
 */
class printMylove {
public:
    /**
     * 注册CJMOD API
     * 使用Syntax::analyze、Arg::bind、CJMODScanner::scan等强大功能
     * 创建天然支持vir虚对象的CHTL JS函数
     */
    static void registerCJMODAPI();
    
    /**
     * 演示CJMOD强大功能
     * 展示语法分析、双指针扫描、前置截取、转换、导出等完整能力
     */
    static void demonstrateCJMODPower();
    
    /**
     * 演示所有占位符类型
     * 展示$, $?, $!, $_, $!_, ...等所有占位符的强大能力
     */
    static void demonstrateAllPlaceholderTypes();
    
    /**
     * 演示扫描方法
     * 展示双指针扫描法和前置截取法的强大能力
     */
    static void demonstrateScanningMethods();
    
    /**
     * 生成爱的消息
     * @param url 目标URL
     * @param mode 模式
     * @return 使用CJMOD API生成的爱意消息
     */
    static std::string generateLoveMessage(const std::string& url, const std::string& mode = "");
    
    /**
     * 创建高级爱意表达
     * @param target 爱意目标
     * @param emotion 情感类型
     * @param method 表达方式
     * @return 高级爱意表达代码
     */
    static std::string createAdvancedLoveExpression(const std::string& target, const std::string& emotion, const std::string& method = "");
    
    /**
     * 展示高级特性
     * 演示CJMOD API的高级组合使用
     */
    static void showcaseAdvancedFeatures();
};

} // namespace Chtholly