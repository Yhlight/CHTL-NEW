#include "ConfigNode.h"
#include "Visitor.h"
#include "../CHTLLexer/CHTLGlobalMap.h"
#include <sstream>
#include <regex>

namespace CHTL {

ConfigNode::ConfigNode(ConfigType configType, const std::string& configName, 
                      size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::CONFIGURATION_NODE, configName, line, column),
      m_ConfigType(configType), m_ConfigName(configName),
      m_IsGlobalConfig(true), m_IsNamedConfig(!configName.empty()) {
    
    InitializeDefaultConfigs();
    
    switch (configType) {
        case ConfigType::NAME_CONFIG:
            InitializeNameConfig();
            break;
        case ConfigType::ORIGIN_TYPE_CONFIG:
            InitializeOriginTypeConfig();
            break;
        default:
            break;
    }
}

void ConfigNode::InitializeDefaultConfigs() {
    // 初始化默认的基础配置项
    AddBasicConfig("INDEX_INITIAL_COUNT", "0", "int", false);
    AddBasicConfig("DEBUG_MODE", "false", "bool", false);
    AddBasicConfig("DISABLE_STYLE_AUTO_ADD_CLASS", "false", "bool", false);
    AddBasicConfig("DISABLE_STYLE_AUTO_ADD_ID", "false", "bool", false);
    AddBasicConfig("DISABLE_SCRIPT_AUTO_ADD_CLASS", "false", "bool", false);
    AddBasicConfig("DISABLE_SCRIPT_AUTO_ADD_ID", "false", "bool", false);
    AddBasicConfig("DISABLE_DEFAULT_NAMESPACE", "false", "bool", false);
}

void ConfigNode::InitializeNameConfig() {
    // 初始化Name配置块的默认关键字映射
    // 用户可以自定义CHTL关键字
    
    AddNameConfig("use", "use", true);           // use → use（默认不变）
    AddNameConfig("html", "html", true);         // html → html（默认不变）
    AddNameConfig("text", "text", true);         // text → text（默认不变）
    AddNameConfig("style", "style", true);       // style → style（默认不变）
    AddNameConfig("script", "script", true);     // script → script（默认不变）
    AddNameConfig("inherit", "inherit", true);   // inherit → inherit（默认不变）
    AddNameConfig("except", "except", true);     // except → except（默认不变）
    
    // 示例自定义关键字
    // AddNameConfig("text", "文本", true);      // text → 文本
    // AddNameConfig("style", "样式", true);     // style → 样式
    // AddNameConfig("script", "脚本", true);    // script → 脚本
}

void ConfigNode::InitializeOriginTypeConfig() {
    // 初始化OriginType配置块的默认类型
    
    // 内置类型
    AddOriginTypeConfig("Html", ".html", "text/html", "html_processor");
    AddOriginTypeConfig("Style", ".css", "text/css", "css_processor");
    AddOriginTypeConfig("JavaScript", ".js", "application/javascript", "js_processor");
    
    // 示例自定义类型
    AddOriginTypeConfig("Vue", ".vue", "text/x-vue", "vue_processor");
    AddOriginTypeConfig("React", ".jsx", "text/jsx", "react_processor");
    AddOriginTypeConfig("Angular", ".component.html", "text/x-angular", "angular_processor");
    AddOriginTypeConfig("Svelte", ".svelte", "text/x-svelte", "svelte_processor");
}

void ConfigNode::SetConfigName(const std::string& name) {
    m_ConfigName = name;
    m_IsNamedConfig = !name.empty();
    // SetValue(name); // 暂时注释，等待CHTLBaseNode方法确认
}

void ConfigNode::AddBasicConfig(const std::string& key, const std::string& value, 
                               const std::string& type, bool required) {
    m_BasicConfigs[key] = ConfigItem(key, value, type, required);
}

std::string ConfigNode::GetBasicConfig(const std::string& key) const {
    auto it = m_BasicConfigs.find(key);
    return (it != m_BasicConfigs.end()) ? it->second.Value : "";
}

bool ConfigNode::HasBasicConfig(const std::string& key) const {
    return m_BasicConfigs.find(key) != m_BasicConfigs.end();
}

void ConfigNode::AddNameConfig(const std::string& originalKeyword, const std::string& customKeyword, bool enabled) {
    m_NameConfigs[originalKeyword] = NameConfigItem(originalKeyword, customKeyword, enabled);
}

std::string ConfigNode::GetCustomKeyword(const std::string& originalKeyword) const {
    auto it = m_NameConfigs.find(originalKeyword);
    if (it != m_NameConfigs.end() && it->second.IsEnabled) {
        return it->second.CustomKeyword;
    }
    return originalKeyword; // 返回原始关键字
}

bool ConfigNode::IsKeywordCustomized(const std::string& originalKeyword) const {
    auto it = m_NameConfigs.find(originalKeyword);
    return (it != m_NameConfigs.end()) && it->second.IsEnabled && 
           (it->second.CustomKeyword != originalKeyword);
}

void ConfigNode::DisableNameConfig(bool disable) {
    // 禁用所有Name配置
    for (auto& config : m_NameConfigs) {
        config.second.IsEnabled = !disable;
    }
}

void ConfigNode::AddOriginTypeConfig(const std::string& typeName, const std::string& fileExtension,
                                    const std::string& mimeType, const std::string& processor) {
    m_OriginTypeConfigs[typeName] = OriginTypeConfigItem(typeName, fileExtension, mimeType, processor, false);
}

OriginTypeConfigItem ConfigNode::GetOriginTypeConfig(const std::string& typeName) const {
    auto it = m_OriginTypeConfigs.find(typeName);
    return (it != m_OriginTypeConfigs.end()) ? it->second : OriginTypeConfigItem();
}

bool ConfigNode::HasOriginTypeConfig(const std::string& typeName) const {
    return m_OriginTypeConfigs.find(typeName) != m_OriginTypeConfigs.end();
}

void ConfigNode::RegisterCustomOriginType(const std::string& typeName, const std::string& fileExtension, 
                                         const std::string& processor) {
    // 注册新的自定义原始嵌入类型
    OriginTypeConfigItem customType;
    customType.TypeName = typeName;
    customType.FileExtension = fileExtension;
    customType.Processor = processor;
    customType.IsBuiltIn = false;
    
    // 自动推断MIME类型
    if (fileExtension == ".vue") {
        customType.MimeType = "text/x-vue";
    }
    else if (fileExtension == ".jsx" || fileExtension == ".tsx") {
        customType.MimeType = "text/jsx";
    }
    else if (fileExtension == ".svelte") {
        customType.MimeType = "text/x-svelte";
    }
    else {
        customType.MimeType = "text/plain";
    }
    
    m_OriginTypeConfigs[typeName] = customType;
}

void ConfigNode::ApplyConfigToGlobal(CHTLGlobalMap* globalMap) {
    if (!globalMap) {
        return;
    }
    
    // 暂时简化实现，等待CHTLGlobalMap方法完善
    // TODO: 实现配置应用到全局映射表
}

bool ConfigNode::ValidateConfig() const {
    // 验证所有配置项
    
    // 验证基础配置
    for (const auto& config : m_BasicConfigs) {
        if (config.second.IsRequired && config.second.Value.empty()) {
            return false; // 必需配置项为空
        }
        
        // 验证类型
        if (config.second.Type == "int") {
            try {
                std::stoi(config.second.Value);
            }
            catch (const std::exception&) {
                return false; // 整数类型验证失败
            }
        }
        else if (config.second.Type == "bool") {
            if (config.second.Value != "true" && config.second.Value != "false") {
                return false; // 布尔类型验证失败
            }
        }
    }
    
    // 验证Name配置
    for (const auto& nameConfig : m_NameConfigs) {
        if (!ValidateNameConfigItem(nameConfig.second)) {
            return false;
        }
    }
    
    // 验证OriginType配置
    for (const auto& originConfig : m_OriginTypeConfigs) {
        if (!ValidateOriginTypeConfigItem(originConfig.second)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigNode::ValidateNameConfigItem(const NameConfigItem& item) const {
    // 验证Name配置项
    
    // 检查关键字不能为空
    if (item.OriginalKeyword.empty() || item.CustomKeyword.empty()) {
        return false;
    }
    
    // 检查自定义关键字格式（必须是有效标识符）
    std::regex identifierPattern(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    if (!std::regex_match(item.CustomKeyword, identifierPattern)) {
        return false;
    }
    
    return true;
}

bool ConfigNode::ValidateOriginTypeConfigItem(const OriginTypeConfigItem& item) const {
    // 验证OriginType配置项
    
    // 检查类型名称不能为空
    if (item.TypeName.empty()) {
        return false;
    }
    
    // 检查文件扩展名格式
    if (!item.FileExtension.empty() && item.FileExtension.front() != '.') {
        return false; // 扩展名必须以.开头
    }
    
    return true;
}

std::string ConfigNode::GenerateHTML() const {
    std::ostringstream html;
    
    html << "<!-- CHTL Configuration: " << m_ConfigName << " -->\n";
    html << "<script type=\"application/json\" class=\"chtl-config\">\n";
    html << "{\n";
    html << "  \"configType\": \"" << GetTypeString() << "\",\n";
    html << "  \"configName\": \"" << m_ConfigName << "\",\n";
    
    // 输出基础配置
    if (!m_BasicConfigs.empty()) {
        html << "  \"basicConfigs\": {\n";
        bool first = true;
        for (const auto& config : m_BasicConfigs) {
            if (!first) html << ",\n";
            html << "    \"" << config.first << "\": \"" << config.second.Value << "\"";
            first = false;
        }
        html << "\n  }";
    }
    
    // 输出Name配置
    if (!m_NameConfigs.empty()) {
        if (!m_BasicConfigs.empty()) html << ",\n";
        html << "  \"nameConfigs\": {\n";
        bool first = true;
        for (const auto& nameConfig : m_NameConfigs) {
            if (nameConfig.second.IsEnabled) {
                if (!first) html << ",\n";
                html << "    \"" << nameConfig.first << "\": \"" << nameConfig.second.CustomKeyword << "\"";
                first = false;
            }
        }
        html << "\n  }";
    }
    
    // 输出OriginType配置
    if (!m_OriginTypeConfigs.empty()) {
        if (!m_BasicConfigs.empty() || !m_NameConfigs.empty()) html << ",\n";
        html << "  \"originTypeConfigs\": {\n";
        bool first = true;
        for (const auto& originConfig : m_OriginTypeConfigs) {
            if (!first) html << ",\n";
            html << "    \"" << originConfig.first << "\": {\n";
            html << "      \"extension\": \"" << originConfig.second.FileExtension << "\",\n";
            html << "      \"mimeType\": \"" << originConfig.second.MimeType << "\",\n";
            html << "      \"processor\": \"" << originConfig.second.Processor << "\"\n";
            html << "    }";
            first = false;
        }
        html << "\n  }";
    }
    
    html << "\n}\n";
    html << "</script>\n";
    html << "<!-- End CHTL Configuration -->";
    
    return html.str();
}

std::string ConfigNode::GetTypeString() const {
    switch (m_ConfigType) {
        case ConfigType::BASIC_CONFIG:
            return "BasicConfig";
        case ConfigType::NAME_CONFIG:
            return "NameConfig";
        case ConfigType::ORIGIN_TYPE_CONFIG:
            return "OriginTypeConfig";
        case ConfigType::NAMED_CONFIG:
            return "NamedConfig";
        case ConfigType::SELECTOR_CONFIG:
            return "SelectorConfig";
        default:
            return "UnknownConfig";
    }
}

std::unique_ptr<CHTLBaseNode> ConfigNode::Clone() const {
    auto cloned = std::make_unique<ConfigNode>(m_ConfigType, m_ConfigName, GetLine(), GetColumn());
    
    // 复制所有配置项
    cloned->m_BasicConfigs = m_BasicConfigs;
    cloned->m_NameConfigs = m_NameConfigs;
    cloned->m_OriginTypeConfigs = m_OriginTypeConfigs;
    cloned->m_IsGlobalConfig = m_IsGlobalConfig;
    cloned->m_IsNamedConfig = m_IsNamedConfig;
    
    return std::move(cloned);
}

void ConfigNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitConfigNode(this);
    }
}

// ConfigNodeFactory实现
std::unique_ptr<ConfigNode> ConfigNodeFactory::CreateBasicConfig(const std::string& configName,
                                                                 size_t line, size_t column) {
    return std::make_unique<ConfigNode>(ConfigType::BASIC_CONFIG, configName, line, column);
}

std::unique_ptr<ConfigNode> ConfigNodeFactory::CreateNameConfig(size_t line, size_t column) {
    auto config = std::make_unique<ConfigNode>(ConfigType::NAME_CONFIG, "Name", line, column);
    
    // 可以在这里添加更多Name配置的初始化
    // 例如：支持中文关键字
    config->AddNameConfig("text", "文本", false);        // 默认禁用，用户可启用
    config->AddNameConfig("style", "样式", false);       // 默认禁用，用户可启用
    config->AddNameConfig("script", "脚本", false);      // 默认禁用，用户可启用
    config->AddNameConfig("template", "模板", false);    // 默认禁用，用户可启用
    config->AddNameConfig("custom", "自定义", false);    // 默认禁用，用户可启用
    
    return config;
}

std::unique_ptr<ConfigNode> ConfigNodeFactory::CreateOriginTypeConfig(size_t line, size_t column) {
    auto config = std::make_unique<ConfigNode>(ConfigType::ORIGIN_TYPE_CONFIG, "OriginType", line, column);
    
    // 可以在这里添加更多自定义类型
    config->RegisterCustomOriginType("TypeScript", ".ts", "typescript_processor");
    config->RegisterCustomOriginType("Sass", ".scss", "sass_processor");
    config->RegisterCustomOriginType("Less", ".less", "less_processor");
    config->RegisterCustomOriginType("Stylus", ".styl", "stylus_processor");
    config->RegisterCustomOriginType("CoffeeScript", ".coffee", "coffeescript_processor");
    
    return config;
}

std::unique_ptr<ConfigNode> ConfigNodeFactory::CreateNamedConfig(const std::string& configName,
                                                                size_t line, size_t column) {
    return std::make_unique<ConfigNode>(ConfigType::NAMED_CONFIG, configName, line, column);
}

} // namespace CHTL