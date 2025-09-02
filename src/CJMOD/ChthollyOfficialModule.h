#pragma once

#include "CJMODIntegration.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CJMOD {

/**
 * 珂朵莉官方模块
 * 基于珂朵莉·诺塔·塞尼欧里斯角色设计
 * 来源：《末日时在做什么？有没有空？可不可以来拯救？》
 * 
 * 角色特点：
 * - 妖精兵器，拥有强大的力量
 * - 温柔善良，充满爱心
 * - 为了保护重要的人愿意牺牲一切
 * - 喜欢黄油蛋糕和平静的生活
 * - 拥有美丽的红色长发和翠绿色眼睛
 */
class ChthollyOfficialModule : public CJMODExtension {
private:
    // 珂朵莉特色配置
    struct ChthollyConfig {
        std::string favoriteFood = "黄油蛋糕";
        std::string hairColor = "#FF6B6B";      // 红色长发
        std::string eyeColor = "#4ECDC4";       // 翠绿色眼睛
        std::string weaponType = "妖精兵器";
        std::string personality = "温柔善良";
        std::string mission = "保护重要的人";
    };
    
    ChthollyConfig m_Config;

public:
    /**
     * 构造函数
     */
    ChthollyOfficialModule() : CJMODExtension("chtl::Chtholly", "1.0.0") {
        m_SupportedSyntax = {
            "printMylove",          // 爱的表达
            "iNeverAway",           // 永恒守护
            "util...then",          // 异步链式
            "protectThem",          // 保护重要的人
            "butterCake",           // 黄油蛋糕特效
            "fairyWeapon",          // 妖精兵器
            "gentleTouch",          // 温柔触碰
            "scarletHair",          // 红发特效
            "emeraldEyes",          // 翠绿眼眸
            "lastBattle"            // 最后的战斗
        };
    }
    
    /**
     * 处理代码片段
     */
    std::string ProcessFragment(const std::string& fragment, 
                               const std::unordered_map<std::string, std::string>& context) override;
    
    /**
     * 检查语法支持
     */
    bool SupportsSyntax(const std::string& syntax) override;
    
    /**
     * 获取扩展信息
     */
    std::unordered_map<std::string, std::string> GetExtensionInfo() override;
    
    /**
     * 初始化扩展
     */
    bool Initialize() override;
    
    /**
     * 清理扩展
     */
    void Cleanup() override;

private:
    /**
     * 处理printMylove - 爱的表达
     * 将图片转换为字符像素块，表达对珂朵莉的爱
     */
    std::string ProcessPrintMylove(const std::string& fragment);
    
    /**
     * 处理iNeverAway - 永恒守护
     * 创建永不消失的守护函数，象征永恒的陪伴
     */
    std::string ProcessINeverAway(const std::string& fragment);
    
    /**
     * 处理protectThem - 保护重要的人
     * 创建保护机制，监控和守护重要元素
     */
    std::string ProcessProtectThem(const std::string& fragment);
    
    /**
     * 处理butterCake - 黄油蛋糕特效
     * 创建温馨的视觉效果，象征珂朵莉最爱的食物
     */
    std::string ProcessButterCake(const std::string& fragment);
    
    /**
     * 处理fairyWeapon - 妖精兵器
     * 创建强大的功能增强，象征妖精兵器的力量
     */
    std::string ProcessFairyWeapon(const std::string& fragment);
    
    /**
     * 处理gentleTouch - 温柔触碰
     * 创建柔和的交互效果，体现珂朵莉的温柔
     */
    std::string ProcessGentleTouch(const std::string& fragment);
    
    /**
     * 处理scarletHair - 红发特效
     * 创建红色渐变效果，象征珂朵莉的美丽红发
     */
    std::string ProcessScarletHair(const std::string& fragment);
    
    /**
     * 处理emeraldEyes - 翠绿眼眸
     * 创建翠绿色光效，象征珂朵莉的美丽眼睛
     */
    std::string ProcessEmeraldEyes(const std::string& fragment);
    
    /**
     * 处理lastBattle - 最后的战斗
     * 创建英勇的战斗效果，致敬珂朵莉的牺牲精神
     */
    std::string ProcessLastBattle(const std::string& fragment);
};

/**
 * 由比滨结衣官方模块
 * 基于由比滨结衣角色设计
 * 来源：《我的青春恋爱物语果然有问题》
 * 
 * 角色特点：
 * - 开朗活泼，充满活力
 * - 善于社交，人际关系良好
 * - 温柔体贴，关心他人
 * - 有时会掩饰自己的真实想法
 * - 粉色长发，温暖的笑容
 * - 喜欢时尚和可爱的事物
 */
class YuigahamaOfficialModule : public CJMODExtension {
private:
    // 由比滨结衣特色配置
    struct YuigahamaConfig {
        std::string hairColor = "#FFB6C1";      // 粉色长发
        std::string personality = "开朗活泼";
        std::string socialSkill = "善于社交";
        std::string catchphrase = "雅哈喽～";
        std::string hobby = "时尚购物";
        std::string strength = "温柔体贴";
    };
    
    YuigahamaConfig m_Config;

public:
    /**
     * 构造函数
     */
    YuigahamaOfficialModule() : CJMODExtension("chtl::Yuigahama", "1.0.0") {
        m_SupportedSyntax = {
            "yahalloo",             // 雅哈喽问候
            "socialConnect",        // 社交连接
            "gentleSupport",        // 温柔支持
            "fashionStyle",         // 时尚样式
            "cheerfulAnimation",    // 开朗动画
            "pinkHair",            // 粉色头发特效
            "warmSmile",           // 温暖笑容
            "friendshipBond",      // 友谊纽带
            "musicPlayer",         // 音乐播放器
            "accordionMenu"        // 手风琴菜单
        };
    }
    
    /**
     * 处理代码片段
     */
    std::string ProcessFragment(const std::string& fragment, 
                               const std::unordered_map<std::string, std::string>& context) override;
    
    /**
     * 检查语法支持
     */
    bool SupportsSyntax(const std::string& syntax) override;
    
    /**
     * 获取扩展信息
     */
    std::unordered_map<std::string, std::string> GetExtensionInfo() override;
    
    /**
     * 初始化扩展
     */
    bool Initialize() override;
    
    /**
     * 清理扩展
     */
    void Cleanup() override;

private:
    /**
     * 处理yahalloo - 雅哈喽问候
     * 创建特色的问候和欢迎效果
     */
    std::string ProcessYahalloo(const std::string& fragment);
    
    /**
     * 处理socialConnect - 社交连接
     * 创建社交网络和连接功能
     */
    std::string ProcessSocialConnect(const std::string& fragment);
    
    /**
     * 处理gentleSupport - 温柔支持
     * 创建温柔的用户支持和帮助系统
     */
    std::string ProcessGentleSupport(const std::string& fragment);
    
    /**
     * 处理fashionStyle - 时尚样式
     * 创建时尚的UI样式和视觉效果
     */
    std::string ProcessFashionStyle(const std::string& fragment);
    
    /**
     * 处理cheerfulAnimation - 开朗动画
     * 创建活泼开朗的动画效果
     */
    std::string ProcessCheerfulAnimation(const std::string& fragment);
    
    /**
     * 处理pinkHair - 粉色头发特效
     * 创建粉色渐变和头发飘动效果
     */
    std::string ProcessPinkHair(const std::string& fragment);
    
    /**
     * 处理warmSmile - 温暖笑容
     * 创建温暖的交互反馈效果
     */
    std::string ProcessWarmSmile(const std::string& fragment);
    
    /**
     * 处理friendshipBond - 友谊纽带
     * 创建友谊连接和社交功能
     */
    std::string ProcessFriendshipBond(const std::string& fragment);
    
    /**
     * 处理musicPlayer - 音乐播放器
     * 创建音乐播放和控制功能
     */
    std::string ProcessMusicPlayer(const std::string& fragment);
    
    /**
     * 处理accordionMenu - 手风琴菜单
     * 创建手风琴式展开菜单
     */
    std::string ProcessAccordionMenu(const std::string& fragment);
};

/**
 * 官方模块注册器
 * 负责注册和管理所有官方CJMOD模块
 */
class OfficialModuleRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<CJMODExtension>> m_OfficialModules;
    bool m_IsInitialized;

public:
    /**
     * 构造函数
     */
    OfficialModuleRegistry() : m_IsInitialized(false) {}
    
    /**
     * 初始化所有官方模块
     * @return 是否成功
     */
    bool InitializeOfficialModules();
    
    /**
     * 获取官方模块
     * @param moduleName 模块名称
     * @return 模块指针
     */
    CJMODExtension* GetOfficialModule(const std::string& moduleName);
    
    /**
     * 注册官方模块到CJMOD管理器
     * @param cjmodManager CJMOD管理器
     * @return 注册的模块数量
     */
    size_t RegisterToManager(CJMODManager& cjmodManager);
    
    /**
     * 获取所有官方模块名称
     * @return 模块名称列表
     */
    std::vector<std::string> GetOfficialModuleNames() const;
    
    /**
     * 检查是否为官方模块
     * @param moduleName 模块名称
     * @return 是否为官方模块
     */
    bool IsOfficialModule(const std::string& moduleName) const;
};

} // namespace CJMOD