#pragma once

#include <string>

namespace Chtholly {

/**
 * 珂朵莉模块CJMOD部分 - printMylove功能
 * 展示CJMOD API的强大能力
 * 严格按照CJMOD.md实现
 */
class printMylove {
public:
    /**
     * 注册CJMOD API
     * 使用Syntax::analyze、Arg::bind、CJMODScanner::scan等强大功能
     */
    static void registerCJMODAPI();
    
    /**
     * 演示CJMOD强大功能
     * 展示语法分析、扫描、转换、导出等能力
     */
    static void demonstrateCJMODPower();
    
    /**
     * 生成爱的消息
     * @param url 目标URL
     * @param mode 模式
     * @return 爱的消息
     */
    static std::string generateLoveMessage(const std::string& url, const std::string& mode = "");
};

} // namespace Chtholly