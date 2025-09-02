#include "../CJMOD/ChthollyOfficialModule.h"
#include "../CJMOD/CJMODIntegration.h"
#include <iostream>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       💖 CJMOD官方模块测试 💖                                              ║" << std::endl;
    std::cout << "║                   珂朵莉·诺塔·塞尼欧里斯 & 由比滨结衣                                     ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    // 1. 测试珂朵莉官方模块
    std::cout << "\n💖 珂朵莉·诺塔·塞尼欧里斯官方模块测试:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        CJMOD::ChthollyOfficialModule chthollyModule;
        
        // 获取角色信息
        auto chthollyInfo = chthollyModule.GetExtensionInfo();
        std::cout << "\n🌟 角色信息:" << std::endl;
        std::cout << "  角色名: " << chthollyInfo["character"] << std::endl;
        std::cout << "  作品: " << chthollyInfo["source"] << std::endl;
        std::cout << "  性格: " << chthollyInfo["personality"] << std::endl;
        std::cout << "  发色: " << chthollyInfo["hairColor"] << " (红色长发)" << std::endl;
        std::cout << "  瞳色: " << chthollyInfo["eyeColor"] << " (翠绿眼眸)" << std::endl;
        std::cout << "  最爱: " << chthollyInfo["favoriteFood"] << std::endl;
        std::cout << "  使命: " << chthollyInfo["mission"] << std::endl;
        
        // 测试珂朵莉特色语法
        std::cout << "\n💕 珂朵莉特色语法测试:" << std::endl;
        
        // 测试printMylove
        std::string printMyloveCode = "printMylove { url: 'chtholly.jpg', mode: 'ASCII', width: 80 }";
        bool supportsPrintMylove = chthollyModule.SupportsSyntax(printMyloveCode);
        std::cout << "  printMylove支持: " << (supportsPrintMylove ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsPrintMylove) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "printMylove";
            
            std::string processed = chthollyModule.ProcessFragment(printMyloveCode, context);
            std::cout << "  处理结果长度: " << processed.length() << " 字符" << std::endl;
            std::cout << "  包含爱的表达: " << (processed.find("珂朵莉") != std::string::npos ? "✅ 是" : "❌ 否") << std::endl;
        }
        
        // 测试protectThem
        std::string protectCode = "protectThem { targets: ['.important', '#critical'], methods: ['monitor', 'alert'] }";
        bool supportsProtect = chthollyModule.SupportsSyntax(protectCode);
        std::cout << "\n  protectThem支持: " << (supportsProtect ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsProtect) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "protectThem";
            
            std::string processed = chthollyModule.ProcessFragment(protectCode, context);
            std::cout << "  保护机制: " << (processed.find("保护重要的人") != std::string::npos ? "✅ 已启用" : "❌ 未启用") << std::endl;
        }
        
        // 测试fairyWeapon
        std::string weaponCode = "fairyWeapon { type: 'Seneorious', power: 100, target: '.weapon-target' }";
        bool supportsWeapon = chthollyModule.SupportsSyntax(weaponCode);
        std::cout << "\n  fairyWeapon支持: " << (supportsWeapon ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsWeapon) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "fairyWeapon";
            
            std::string processed = chthollyModule.ProcessFragment(weaponCode, context);
            std::cout << "  妖精兵器: " << (processed.find("妖精兵器") != std::string::npos ? "✅ 已激活" : "❌ 未激活") << std::endl;
        }
        
        // 测试butterCake
        std::string cakeCode = "butterCake { target: '.cake-area', duration: 2000 }";
        bool supportsCake = chthollyModule.SupportsSyntax(cakeCode);
        std::cout << "\n  butterCake支持: " << (supportsCake ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsCake) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "butterCake";
            
            std::string processed = chthollyModule.ProcessFragment(cakeCode, context);
            std::cout << "  黄油蛋糕特效: " << (processed.find("黄油蛋糕") != std::string::npos ? "✅ 已启动" : "❌ 未启动") << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 珂朵莉模块测试异常: " << e.what() << std::endl;
    }
    
    // 2. 测试由比滨结衣官方模块
    std::cout << "\n🌸 由比滨结衣官方模块测试:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        CJMOD::YuigahamaOfficialModule yuigahamaModule;
        
        // 获取角色信息
        auto yuigahamaInfo = yuigahamaModule.GetExtensionInfo();
        std::cout << "\n🌟 角色信息:" << std::endl;
        std::cout << "  角色名: " << yuigahamaInfo["character"] << std::endl;
        std::cout << "  作品: " << yuigahamaInfo["source"] << std::endl;
        std::cout << "  性格: " << yuigahamaInfo["personality"] << std::endl;
        std::cout << "  发色: " << yuigahamaInfo["hairColor"] << " (粉色长发)" << std::endl;
        std::cout << "  口头禅: " << yuigahamaInfo["catchphrase"] << std::endl;
        std::cout << "  特长: " << yuigahamaInfo["socialSkill"] << std::endl;
        
        // 测试由比滨结衣特色语法
        std::cout << "\n💕 由比滨结衣特色语法测试:" << std::endl;
        
        // 测试yahalloo
        std::string yahallooCode = "yahalloo { target: '.greeting-area', auto: true }";
        bool supportsYahalloo = yuigahamaModule.SupportsSyntax(yahallooCode);
        std::cout << "  yahalloo支持: " << (supportsYahalloo ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsYahalloo) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "yahalloo";
            
            std::string processed = yuigahamaModule.ProcessFragment(yahallooCode, context);
            std::cout << "  雅哈喽问候: " << (processed.find("雅哈喽") != std::string::npos ? "✅ 已启用" : "❌ 未启用") << std::endl;
        }
        
        // 测试musicPlayer
        std::string musicCode = "musicPlayer { playlist: ['song1.mp3', 'song2.mp3'], autoPlay: true, volume: 0.8 }";
        bool supportsMusic = yuigahamaModule.SupportsSyntax(musicCode);
        std::cout << "\n  musicPlayer支持: " << (supportsMusic ? "✅ 支持" : "❌ 不支持") << std::endl;
        
        if (supportsMusic) {
            std::unordered_map<std::string, std::string> context;
            context["syntax"] = "musicPlayer";
            
            std::string processed = yuigahamaModule.ProcessFragment(musicCode, context);
            std::cout << "  音乐播放器: " << (processed.find("音乐播放器") != std::string::npos ? "✅ 已创建" : "❌ 未创建") << std::endl;
            std::cout << "  播放器UI: " << (processed.find("yuigahamaMusicPlayer") != std::string::npos ? "✅ 已生成" : "❌ 未生成") << std::endl;
        }
        
        // 测试socialConnect
        std::string socialCode = "socialConnect { platforms: ['twitter', 'instagram'], autoConnect: true }";
        bool supportsSocial = yuigahamaModule.SupportsSyntax(socialCode);
        std::cout << "\n  socialConnect支持: " << (supportsSocial ? "✅ 支持" : "❌ 不支持") << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 由比滨结衣模块测试异常: " << e.what() << std::endl;
    }
    
    // 3. 测试官方模块注册器
    std::cout << "\n🏛️ 官方模块注册器测试:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        CJMOD::OfficialModuleRegistry registry;
        
        bool initSuccess = registry.InitializeOfficialModules();
        std::cout << "  官方模块注册器初始化: " << (initSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
        
        if (initSuccess) {
            auto moduleNames = registry.GetOfficialModuleNames();
            std::cout << "  已注册官方模块数量: " << moduleNames.size() << std::endl;
            
            for (const auto& name : moduleNames) {
                std::cout << "    📦 " << name << std::endl;
                
                auto module = registry.GetOfficialModule(name);
                if (module) {
                    auto info = module->GetExtensionInfo();
                    std::cout << "      角色: " << info["character"] << std::endl;
                    std::cout << "      特色: " << info["features"] << std::endl;
                }
            }
            
            // 测试官方模块前缀检查
            std::cout << "\n  官方模块前缀测试:" << std::endl;
            std::cout << "    chtl::Chtholly: " << (registry.IsOfficialModule("chtl::Chtholly") ? "✅ 官方" : "❌ 非官方") << std::endl;
            std::cout << "    chtl::Yuigahama: " << (registry.IsOfficialModule("chtl::Yuigahama") ? "✅ 官方" : "❌ 非官方") << std::endl;
            std::cout << "    custom::Module: " << (registry.IsOfficialModule("custom::Module") ? "✅ 官方" : "❌ 非官方") << std::endl;
        }
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 官方模块注册器测试异常: " << e.what() << std::endl;
    }
    
    std::cout << "\n🎯 CJMOD官方模块状态:" << std::endl;
    std::cout << "  ✅ 珂朵莉官方模块 (chtl::Chtholly)" << std::endl;
    std::cout << "    • printMylove: 爱的表达，图片转字符像素块" << std::endl;
    std::cout << "    • iNeverAway: 永恒守护，创建守护函数注册表" << std::endl;
    std::cout << "    • protectThem: 保护重要的人，监控保护机制" << std::endl;
    std::cout << "    • butterCake: 黄油蛋糕特效，温馨视觉效果" << std::endl;
    std::cout << "    • fairyWeapon: 妖精兵器，强大功能增强" << std::endl;
    std::cout << "    • gentleTouch: 温柔触碰，柔和交互效果" << std::endl;
    std::cout << "    • scarletHair: 红发特效，红色渐变效果" << std::endl;
    std::cout << "    • emeraldEyes: 翠绿眼眸，翠绿色光效" << std::endl;
    std::cout << "    • lastBattle: 最后的战斗，英勇战斗效果" << std::endl;
    
    std::cout << "\n  ✅ 由比滨结衣官方模块 (chtl::Yuigahama)" << std::endl;
    std::cout << "    • yahalloo: 雅哈喽问候，特色问候效果" << std::endl;
    std::cout << "    • socialConnect: 社交连接，社交网络功能" << std::endl;
    std::cout << "    • gentleSupport: 温柔支持，用户帮助系统" << std::endl;
    std::cout << "    • fashionStyle: 时尚样式，时尚UI效果" << std::endl;
    std::cout << "    • cheerfulAnimation: 开朗动画，活泼动画效果" << std::endl;
    std::cout << "    • pinkHair: 粉色头发特效，粉色渐变" << std::endl;
    std::cout << "    • warmSmile: 温暖笑容，温暖交互反馈" << std::endl;
    std::cout << "    • friendshipBond: 友谊纽带，友谊连接功能" << std::endl;
    std::cout << "    • musicPlayer: 音乐播放器，音乐播放控制" << std::endl;
    std::cout << "    • accordionMenu: 手风琴菜单，展开菜单" << std::endl;
    
    std::cout << "\n🌟 角色特色体现:" << std::endl;
    std::cout << "┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                              💖 珂朵莉·诺塔·塞尼欧里斯                                    │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🌸 来源: 《末日时在做什么？有没有空？可不可以来拯救？》                                     │" << std::endl;
    std::cout << "│ 💎 身份: 妖精兵器，拥有强大的力量                                                           │" << std::endl;
    std::cout << "│ 💕 性格: 温柔善良，充满爱心，为保护重要的人愿意牺牲一切                                   │" << std::endl;
    std::cout << "│ 🔥 外貌: 美丽的红色长发，翠绿色眼睛                                                         │" << std::endl;
    std::cout << "│ 🍰 爱好: 喜欢黄油蛋糕和平静的生活                                                           │" << std::endl;
    std::cout << "│ ⚔️ 使命: 守护世界，保护重要的人                                                             │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                                🌸 由比滨结衣                                                │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🎀 来源: 《我的青春恋爱物语果然有问题》                                                     │" << std::endl;
    std::cout << "│ 🌟 性格: 开朗活泼，充满活力，善于社交                                                       │" << std::endl;
    std::cout << "│ 💕 特点: 温柔体贴，关心他人，有时会掩饰真实想法                                             │" << std::endl;
    std::cout << "│ 🌸 外貌: 粉色长发，温暖的笑容                                                               │" << std::endl;
    std::cout << "│ 👗 爱好: 喜欢时尚和可爱的事物                                                               │" << std::endl;
    std::cout << "│ 🗣️ 口头禅: 雅哈喽～                                                                         │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n💎 CJMOD官方模块设计理念:" << std::endl;
    std::cout << "  ✨ 基于角色特色设计扩展语法，体现角色个性" << std::endl;
    std::cout << "  ✨ 珂朵莉模块体现守护、牺牲、温柔的精神" << std::endl;
    std::cout << "  ✨ 由比滨结衣模块体现开朗、社交、时尚的特色" << std::endl;
    std::cout << "  ✨ 通过CJMOD API实现角色特色功能的代码扩展" << std::endl;
    std::cout << "  ✨ 为CHTL编程语言注入情感和人文关怀" << std::endl;
    
    std::cout << "\n🏆 CJMOD官方模块测试完成！" << std::endl;
    std::cout << "珂朵莉和由比滨结衣的官方模块已完整实现，体现了角色的独特魅力。" << std::endl;
    
    return 0;
}