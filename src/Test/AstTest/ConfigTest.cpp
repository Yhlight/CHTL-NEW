#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ConfigNode.h"

void TestBasicConfigNode() {
    std::cout << "\n=== 基础配置节点测试 ===" << std::endl;
    
    // 创建基础配置节点
    auto basicConfig = CHTL::CHTLConfigNodeFactory::CreateBasicConfigNode();
    
    std::cout << "基础配置: " << basicConfig->ToString() << std::endl;
    std::cout << "是否为命名配置: " << (basicConfig->IsNamed() ? "是" : "否") << std::endl;
    std::cout << "是否为活跃配置: " << (basicConfig->IsActive() ? "是" : "否") << std::endl;
    
    // 查看默认配置项
    std::cout << "INDEX_INITIAL_COUNT: " << basicConfig->GetConfigItem("INDEX_INITIAL_COUNT") << std::endl;
    std::cout << "DEBUG_MODE: " << basicConfig->GetConfigItem("DEBUG_MODE") << std::endl;
    std::cout << "DISABLE_NAME_GROUP: " << basicConfig->GetConfigItem("DISABLE_NAME_GROUP") << std::endl;
    
    // 添加自定义配置项
    basicConfig->AddConfigItem("CUSTOM_SETTING", "enabled", CHTL::CHTLConfigItemType::STRING_CONFIG);
    
    std::cout << "添加自定义配置后: " << basicConfig->ToString() << std::endl;
    std::cout << "CUSTOM_SETTING: " << basicConfig->GetConfigItem("CUSTOM_SETTING") << std::endl;
}

void TestNamedConfigNode() {
    std::cout << "\n=== 命名配置组测试 ===" << std::endl;
    
    // 创建命名配置组
    auto namedConfig = CHTL::CHTLConfigNodeFactory::CreateNamedConfigNode("Basic");
    
    std::cout << "命名配置组: " << namedConfig->ToString() << std::endl;
    std::cout << "配置名称: " << namedConfig->GetConfigName() << std::endl;
    std::cout << "是否为命名配置: " << (namedConfig->IsNamed() ? "是" : "否") << std::endl;
    std::cout << "是否为活跃配置: " << (namedConfig->IsActive() ? "是" : "否") << std::endl;
    
    // 添加配置项
    namedConfig->AddConfigItem("THEME_COLOR", "blue", CHTL::CHTLConfigItemType::STRING_CONFIG);
    namedConfig->AddConfigItem("ENABLE_ANIMATIONS", "true", CHTL::CHTLConfigItemType::BOOLEAN_CONFIG);
    
    std::cout << "添加配置项后: " << namedConfig->ToString() << std::endl;
    
    // 创建另一个命名配置组
    auto stdConfig = CHTL::CHTLConfigNodeFactory::CreateNamedConfigNode("Std");
    std::cout << "\n标准配置组: " << stdConfig->ToString() << std::endl;
}

void TestNameConfigNode() {
    std::cout << "\n=== Name配置块测试 ===" << std::endl;
    
    // 创建Name配置节点
    auto nameConfig = CHTL::CHTLConfigNodeFactory::CreateNameConfigNode();
    
    std::cout << "Name配置: " << nameConfig->ToString() << std::endl;
    std::cout << "选项数量限制: " << nameConfig->GetOptionCountLimit() << std::endl;
    
    // 测试关键字映射
    std::cout << "KEYWORD_INHERIT映射: " << nameConfig->GetKeywordMapping("KEYWORD_INHERIT") << std::endl;
    std::cout << "KEYWORD_DELETE映射: " << nameConfig->GetKeywordMapping("KEYWORD_DELETE") << std::endl;
    std::cout << "KEYWORD_TEXT映射: " << nameConfig->GetKeywordMapping("KEYWORD_TEXT") << std::endl;
    
    // 测试组选项
    auto customStyleOptions = nameConfig->GetGroupOptions("CUSTOM_STYLE");
    std::cout << "CUSTOM_STYLE组选项数量: " << customStyleOptions.size() << std::endl;
    for (const auto& option : customStyleOptions) {
        std::cout << "  选项: " << option << std::endl;
    }
    
    // 添加新的关键字映射
    nameConfig->AddKeywordMapping("CUSTOM_KEYWORD", "customValue");
    std::cout << "添加自定义关键字: " << nameConfig->GetKeywordMapping("CUSTOM_KEYWORD") << std::endl;
    
    // 测试选项限制
    std::cout << "CUSTOM_STYLE是否在限制内: " << (nameConfig->IsWithinOptionLimit("CUSTOM_STYLE") ? "是" : "否") << std::endl;
}

void TestOriginTypeConfigNode() {
    std::cout << "\n=== OriginType配置块测试 ===" << std::endl;
    
    // 创建OriginType配置节点
    auto originTypeConfig = CHTL::CHTLConfigNodeFactory::CreateOriginTypeConfigNode();
    
    std::cout << "OriginType配置: " << originTypeConfig->ToString() << std::endl;
    
    // 添加原始嵌入类型映射
    originTypeConfig->AddOriginTypeMapping("ORIGINTYPE_VUE", "@Vue");
    originTypeConfig->AddOriginTypeMapping("ORIGINTYPE_MARKDOWN", "@Markdown");
    originTypeConfig->AddOriginTypeMapping("ORIGINTYPE_REACT", "@React");
    
    std::cout << "添加类型映射后: " << originTypeConfig->ToString() << std::endl;
    
    // 测试类型映射获取
    std::cout << "ORIGINTYPE_VUE映射: " << originTypeConfig->GetOriginTypeMapping("ORIGINTYPE_VUE") << std::endl;
    std::cout << "ORIGINTYPE_MARKDOWN映射: " << originTypeConfig->GetOriginTypeMapping("ORIGINTYPE_MARKDOWN") << std::endl;
    
    // 测试命名约定验证
    bool vueValid = originTypeConfig->FollowsNamingConvention("ORIGINTYPE_VUE", "@Vue");
    bool markdownValid = originTypeConfig->FollowsNamingConvention("ORIGINTYPE_MARKDOWN", "@Markdown");
    bool invalidValid = originTypeConfig->FollowsNamingConvention("INVALID_NAME", "@Vue");
    
    std::cout << "ORIGINTYPE_VUE命名约定: " << (vueValid ? "正确" : "错误") << std::endl;
    std::cout << "ORIGINTYPE_MARKDOWN命名约定: " << (markdownValid ? "正确" : "错误") << std::endl;
    std::cout << "INVALID_NAME命名约定: " << (invalidValid ? "正确" : "错误") << std::endl;
}

void TestConfigConflictDetector() {
    std::cout << "\n=== 配置冲突检测器测试 ===" << std::endl;
    
    // 清空之前的配置
    CHTL::CHTLConfigConflictDetector::ClearAllConfigs();
    
    // 创建无名配置（应该成为活跃配置）
    auto unnamedConfig1 = CHTL::CHTLConfigNodeFactory::CreateBasicConfigNode();
    std::cout << "创建第一个无名配置" << std::endl;
    
    auto activeConfig = CHTL::CHTLConfigConflictDetector::GetActiveConfig();
    if (activeConfig) {
        std::cout << "当前活跃配置: " << activeConfig->ToString() << std::endl;
    }
    
    // 创建命名配置（不应该成为活跃配置）
    auto namedConfig = CHTL::CHTLConfigNodeFactory::CreateNamedConfigNode("TestConfig");
    std::cout << "\n创建命名配置: " << namedConfig->ToString() << std::endl;
    std::cout << "命名配置是否活跃: " << (namedConfig->IsActive() ? "是" : "否") << std::endl;
    
    // 创建第二个无名配置（应该产生冲突）
    auto unnamedConfig2 = CHTL::CHTLConfigNodeFactory::CreateBasicConfigNode();
    std::cout << "\n创建第二个无名配置" << std::endl;
    
    // 检测冲突
    bool hasConflict = CHTL::CHTLConfigConflictDetector::HasConfigConflict();
    std::cout << "是否有配置冲突: " << (hasConflict ? "是" : "否") << std::endl;
    
    bool hasMultipleUnnamed = CHTL::CHTLConfigConflictDetector::HasMultipleUnnamedConfigs();
    std::cout << "是否有多个无名配置: " << (hasMultipleUnnamed ? "是" : "否") << std::endl;
    
    // 获取冲突配置
    auto conflictingConfigs = CHTL::CHTLConfigConflictDetector::GetConflictingConfigs();
    std::cout << "冲突配置数量: " << conflictingConfigs.size() << std::endl;
    
    // 验证配置设置
    bool isValid = CHTL::CHTLConfigConflictDetector::ValidateConfigSetup();
    std::cout << "配置设置是否有效: " << (isValid ? "是" : "否") << std::endl;
}

void TestDefaultConfigs() {
    std::cout << "\n=== 默认配置测试 ===" << std::endl;
    
    // 创建默认配置
    auto defaultConfig = CHTL::CHTLConfigNodeFactory::CreateDefaultConfig();
    
    std::cout << "默认配置: " << defaultConfig->ToString() << std::endl;
    
    // 查看选择器自动化配置
    std::cout << "DISABLE_STYLE_AUTO_ADD_CLASS: " << defaultConfig->GetConfigItem("DISABLE_STYLE_AUTO_ADD_CLASS") << std::endl;
    std::cout << "DISABLE_SCRIPT_AUTO_ADD_CLASS: " << defaultConfig->GetConfigItem("DISABLE_SCRIPT_AUTO_ADD_CLASS") << std::endl;
    std::cout << "DISABLE_DEFAULT_NAMESPACE: " << defaultConfig->GetConfigItem("DISABLE_DEFAULT_NAMESPACE") << std::endl;
    
    // 创建默认Name配置
    auto defaultNameConfig = CHTL::CHTLConfigNodeFactory::CreateDefaultNameConfig();
    
    std::cout << "\n默认Name配置: " << defaultNameConfig->ToString() << std::endl;
    
    // 查看组选项
    auto customStyleOptions = defaultNameConfig->GetGroupOptions("CUSTOM_STYLE");
    std::cout << "CUSTOM_STYLE组选项: ";
    for (size_t i = 0; i < customStyleOptions.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << customStyleOptions[i];
    }
    std::cout << std::endl;
}

void TestConfigValidation() {
    std::cout << "\n=== 配置验证测试 ===" << std::endl;
    
    // 测试配置名称验证
    std::vector<std::string> configNames = {"BasicConfig", "_ValidConfig", "123Invalid", "", "valid_config_123"};
    
    for (const auto& name : configNames) {
        bool isValid = CHTL::CHTLConfigNodeFactory::IsValidConfigName(name);
        std::cout << "配置名称 \"" << name << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
    
    // 测试配置项验证
    std::vector<std::pair<std::string, std::string>> configItems = {
        {"VALID_ITEM", "true"},
        {"", "invalid"},           // 空键
        {"VALID_KEY", ""},         // 空值
        {"DEBUG_MODE", "false"}
    };
    
    for (const auto& item : configItems) {
        bool isValid = CHTL::CHTLConfigNodeFactory::IsValidConfigItem(item.first, item.second);
        std::cout << "配置项 \"" << item.first << "\" = \"" << item.second << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
    
    // 测试组选项验证
    std::vector<std::vector<std::string>> groupOptions = {
        {"@Style", "@style", "@CSS"},  // 有效组选项
        {},                            // 空组选项（无效）
        {"@Element"},                  // 单个选项（有效）
        {"", "@Style"}                 // 包含空选项（无效）
    };
    
    for (size_t i = 0; i < groupOptions.size(); i++) {
        bool isValid = CHTL::CHTLConfigNodeFactory::IsValidGroupOption(groupOptions[i]);
        std::cout << "组选项 " << i << " (大小:" << groupOptions[i].size() << "): " << (isValid ? "有效" : "无效") << std::endl;
    }
}

int main() {
    try {
        std::cout << "CHTL第七阶段配置系统测试开始..." << std::endl;
        
        // 执行第七阶段配置系统测试
        TestBasicConfigNode();
        TestNamedConfigNode();
        TestNameConfigNode();
        TestOriginTypeConfigNode();
        TestConfigConflictDetector();
        TestDefaultConfigs();
        TestConfigValidation();
        
        std::cout << "\n第七阶段配置系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}