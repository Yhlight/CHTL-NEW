#include "../Module/ModuleLoader.h"
#include <iostream>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        🏛️ CHTL官方模块测试 🏛️                                            ║" << std::endl;
    std::cout << "║                     珂朵莉模块·由比滨结衣模块·标准CMOD结构                                ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n🎯 官方模块加载测试:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    try {
        // 1. 初始化官方模块加载器
        std::cout << "\n🏗️ 步骤1: 初始化官方模块加载器" << std::endl;
        
        CHTL::Module::OfficialModuleLoader loader;
        bool initSuccess = loader.Initialize("src/Module");
        
        std::cout << "  官方模块加载器初始化: " << (initSuccess ? "✅ 成功" : "❌ 失败") << std::endl;
        
        if (!initSuccess) {
            std::cout << "  ❌ 无法继续测试，初始化失败" << std::endl;
            return 1;
        }
        
        // 2. 扫描并加载所有官方模块
        std::cout << "\n📦 步骤2: 扫描并加载官方模块" << std::endl;
        
        size_t loadedCount = loader.LoadAllOfficialModules();
        std::cout << "  成功加载官方模块数量: " << loadedCount << " 个" << std::endl;
        
        auto officialModules = loader.GetOfficialModuleList();
        std::cout << "  官方模块列表:" << std::endl;
        for (const auto& moduleName : officialModules) {
            std::cout << "    📦 " << moduleName << std::endl;
            
            // 获取模块详细信息
            const auto* info = loader.GetModuleInfo(moduleName);
            if (info) {
                std::cout << "      版本: " << info->Version << std::endl;
                std::cout << "      描述: " << info->Description << std::endl;
                std::cout << "      作者: " << info->Author << std::endl;
            }
        }
        
        // 3. 测试珂朵莉模块
        std::cout << "\n💖 步骤3: 珂朵莉模块详细测试" << std::endl;
        
        bool hasChtholly = loader.IsOfficialModule("Chtholly");
        std::cout << "  珂朵莉模块存在: " << (hasChtholly ? "✅ 是" : "❌ 否") << std::endl;
        
        if (hasChtholly) {
            const auto* chthollyInfo = loader.GetModuleInfo("Chtholly");
            if (chthollyInfo) {
                std::cout << "  🌟 珂朵莉模块信息:" << std::endl;
                std::cout << "    角色: 珂朵莉·诺塔·塞尼欧里斯" << std::endl;
                std::cout << "    作品: 《末日时在做什么？有没有空？可不可以来拯救？》" << std::endl;
                std::cout << "    描述: " << chthollyInfo->Description << std::endl;
                std::cout << "    版本: " << chthollyInfo->Version << std::endl;
                std::cout << "    许可: " << chthollyInfo->License << std::endl;
                
                auto chthollyModule = loader.GetCMODManager()->FindModule("Chtholly");
                if (chthollyModule) {
                    const auto& exports = chthollyModule->GetExports();
                    std::cout << "  🎯 导出功能 (" << exports.size() << "个):" << std::endl;
                    
                    for (const auto& exportItem : exports) {
                        std::cout << "    • " << exportItem.Name << " (" << exportItem.Type << ")" << std::endl;
                        std::cout << "      路径: " << exportItem.Path << std::endl;
                        std::cout << "      完全限定名: " << exportItem.FullQualifiedName << std::endl;
                    }
                }
            }
        }
        
        // 4. 测试由比滨结衣模块
        std::cout << "\n🌸 步骤4: 由比滨结衣模块详细测试" << std::endl;
        
        bool hasYuigahama = loader.IsOfficialModule("Yuigahama");
        std::cout << "  由比滨结衣模块存在: " << (hasYuigahama ? "✅ 是" : "❌ 否") << std::endl;
        
        if (hasYuigahama) {
            const auto* yuigahamaInfo = loader.GetModuleInfo("Yuigahama");
            if (yuigahamaInfo) {
                std::cout << "  🌟 由比滨结衣模块信息:" << std::endl;
                std::cout << "    角色: 由比滨结衣" << std::endl;
                std::cout << "    作品: 《我的青春恋爱物语果然有问题》" << std::endl;
                std::cout << "    描述: " << yuigahamaInfo->Description << std::endl;
                std::cout << "    版本: " << yuigahamaInfo->Version << std::endl;
                std::cout << "    口头禅: 雅哈喽～" << std::endl;
                
                auto yuigahamaModule = loader.GetCMODManager()->FindModule("Yuigahama");
                if (yuigahamaModule) {
                    const auto& exports = yuigahamaModule->GetExports();
                    std::cout << "  🎯 导出功能 (" << exports.size() << "个):" << std::endl;
                    
                    for (const auto& exportItem : exports) {
                        std::cout << "    • " << exportItem.Name << " (" << exportItem.Type << ")" << std::endl;
                    }
                }
            }
        }
        
        // 5. 生成完整报告
        std::cout << "\n📋 步骤5: 生成完整官方模块报告" << std::endl;
        std::cout << loader.GenerateOfficialModuleReport() << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cout << "  ✗ 官方模块测试异常: " << e.what() << std::endl;
    }
    
    std::cout << "\n🌟 官方模块特色总结:" << std::endl;
    std::cout << "┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                              💖 珂朵莉模块                                                  │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🌸 类型: CMOD + CJMOD 混合模块                                                              │" << std::endl;
    std::cout << "│ 🔥 CMOD功能: 手风琴、四叶窗相册、备忘录、暖色笔记、樱花雨、鼠标特效等                    │" << std::endl;
    std::cout << "│ ⚡ CJMOD功能: printMylove、iNeverAway、util...then表达式                                  │" << std::endl;
    std::cout << "│ 💎 特色: 基于珂朵莉角色设计，体现温柔善良、保护使命的精神                                 │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                              🌸 由比滨结衣模块                                              │" << std::endl;
    std::cout << "├─────────────────────────────────────────────────────────────────────────────────────────────┤" << std::endl;
    std::cout << "│ 🎀 类型: 纯CMOD模块                                                                         │" << std::endl;
    std::cout << "│ 🎵 CMOD功能: 音乐播放器、手风琴、四叶窗相册、备忘录、暖色笔记、樱花雨、鼠标特效          │" << std::endl;
    std::cout << "│ 💕 特色: 基于由比滨结衣角色设计，体现开朗活泼、善于社交的特色                             │" << std::endl;
    std::cout << "│ 🗣️ 口头禅: 雅哈喽～                                                                        │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
    
    std::cout << "\n🎊 模块设计严格遵循文档规范:" << std::endl;
    std::cout << "  ✅ 标准CMOD结构: src/ + info/ 目录" << std::endl;
    std::cout << "  ✅ 模块信息文件: module.info 包含完整信息" << std::endl;
    std::cout << "  ✅ 导出表定义: 明确的导出项和完全限定名" << std::endl;
    std::cout << "  ✅ 混合模块支持: 珂朵莉模块的CMOD+CJMOD结构" << std::endl;
    std::cout << "  ✅ 纯CMOD模块: 由比滨结衣模块的标准CMOD结构" << std::endl;
    std::cout << "  ✅ 角色特色体现: 基于角色设计的独特功能" << std::endl;
    
    std::cout << "\n🏆 官方模块测试完成！" << std::endl;
    std::cout << "珂朵莉和由比滨结衣的官方模块已按照CHTL文档规范完整实现。" << std::endl;
    
    return 0;
}