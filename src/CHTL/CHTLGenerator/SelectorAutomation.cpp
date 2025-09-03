#include "SelectorAutomation.h"
#include <regex>
#include <algorithm>
#include <iostream>

namespace CHTL {

SelectorAutomationEngine::SelectorAutomationEngine(const SelectorAutomationConfig& config)
    : m_Config(config) {
}

void SelectorAutomationEngine::ProcessElementAutomation(ElementNode* element, 
                                                        StyleNode* styleNode, 
                                                        ScriptNode* scriptNode) {
    if (!element) {
        return;
    }
    
    Reset();
    AnalyzeElementAttributes(element);
    
    // 分析样式块选择器
    bool styleTriggered = false;
    if (styleNode) {
        m_StyleSelectors = AnalyzeStyleSelectors(styleNode->GetContent());
        ProcessStyleSelectorAutomation(element, m_StyleSelectors);
        styleTriggered = true;
    }
    
    // 分析脚本块选择器
    if (scriptNode) {
        m_ScriptSelectors = AnalyzeScriptSelectors(scriptNode->GetContent());
        ProcessScriptSelectorAutomation(element, m_ScriptSelectors, styleTriggered);
    }
}

std::vector<SelectorInfo> SelectorAutomationEngine::AnalyzeStyleSelectors(const std::string& styleContent) {
    std::vector<SelectorInfo> selectors;
    size_t order = 0;
    
    // 匹配类选择器 .className
    std::regex classRegex(R"(\.([a-zA-Z_][a-zA-Z0-9_-]*))");
    std::sregex_iterator classIter(styleContent.begin(), styleContent.end(), classRegex);
    std::sregex_iterator classEnd;
    
    for (; classIter != classEnd; ++classIter) {
        const std::smatch& match = *classIter;
        SelectorInfo info;
        info.Name = match[1].str();
        info.FullSelector = match[0].str();
        info.Type = "class";
        info.IsReference = false;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 匹配ID选择器 #idName
    std::regex idRegex(R"(#([a-zA-Z_][a-zA-Z0-9_-]*))");
    std::sregex_iterator idIter(styleContent.begin(), styleContent.end(), idRegex);
    std::sregex_iterator idEnd;
    
    for (; idIter != idEnd; ++idIter) {
        const std::smatch& match = *idIter;
        SelectorInfo info;
        info.Name = match[1].str();
        info.FullSelector = match[0].str();
        info.Type = "id";
        info.IsReference = false;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 匹配引用选择器 &
    std::regex refRegex(R"(&(?::hover|::before|::after|:active|:focus)?)");
    std::sregex_iterator refIter(styleContent.begin(), styleContent.end(), refRegex);
    std::sregex_iterator refEnd;
    
    for (; refIter != refEnd; ++refIter) {
        const std::smatch& match = *refIter;
        SelectorInfo info;
        info.Name = "&";
        info.FullSelector = match[0].str();
        info.Type = "reference";
        info.IsReference = true;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 按出现顺序排序
    std::sort(selectors.begin(), selectors.end(), 
              [](const SelectorInfo& a, const SelectorInfo& b) {
                  return a.Order < b.Order;
              });
    
    return selectors;
}

std::vector<SelectorInfo> SelectorAutomationEngine::AnalyzeScriptSelectors(const std::string& scriptContent) {
    std::vector<SelectorInfo> selectors;
    size_t order = 0;
    
    // 匹配增强选择器 {{.className}}
    std::regex enhancedClassRegex(R"(\{\{\.([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    std::sregex_iterator classIter(scriptContent.begin(), scriptContent.end(), enhancedClassRegex);
    std::sregex_iterator classEnd;
    
    for (; classIter != classEnd; ++classIter) {
        const std::smatch& match = *classIter;
        SelectorInfo info;
        info.Name = match[1].str();
        info.FullSelector = "{{." + match[1].str() + "}}";
        info.Type = "class";
        info.IsReference = false;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 匹配增强ID选择器 {{#idName}}
    std::regex enhancedIdRegex(R"(\{\{#([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    std::sregex_iterator idIter(scriptContent.begin(), scriptContent.end(), enhancedIdRegex);
    std::sregex_iterator idEnd;
    
    for (; idIter != idEnd; ++idIter) {
        const std::smatch& match = *idIter;
        SelectorInfo info;
        info.Name = match[1].str();
        info.FullSelector = "{{#" + match[1].str() + "}}";
        info.Type = "id";
        info.IsReference = false;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 匹配增强引用选择器 {{&}}
    std::regex enhancedRefRegex(R"(\{\{&\}\})");
    std::sregex_iterator refIter(scriptContent.begin(), scriptContent.end(), enhancedRefRegex);
    std::sregex_iterator refEnd;
    
    for (; refIter != refEnd; ++refIter) {
        SelectorInfo info;
        info.Name = "&";
        info.FullSelector = "{{&}}";
        info.Type = "reference";
        info.IsReference = true;
        info.Order = order++;
        selectors.push_back(info);
    }
    
    // 按出现顺序排序
    std::sort(selectors.begin(), selectors.end(), 
              [](const SelectorInfo& a, const SelectorInfo& b) {
                  return a.Order < b.Order;
              });
    
    return selectors;
}

void SelectorAutomationEngine::ProcessStyleSelectorAutomation(ElementNode* element, 
                                                              const std::vector<SelectorInfo>& selectors) {
    if (!element || selectors.empty()) {
        return;
    }
    
    // 处理类选择器自动化
    if (!m_Config.DisableStyleAutoAddClass && !m_HasClassAttribute) {
        std::string firstClass = GetFirstClassSelector(selectors);
        if (!firstClass.empty()) {
            AddClassToElement(element, firstClass);
            std::cout << "🔄 自动添加类名: " << firstClass << " (样式块触发)" << std::endl;
        }
    }
    
    // 处理ID选择器自动化
    if (!m_Config.DisableStyleAutoAddId && !m_HasIdAttribute) {
        std::string firstId = GetFirstIdSelector(selectors);
        if (!firstId.empty()) {
            AddIdToElement(element, firstId);
            std::cout << "🔄 自动添加ID: " << firstId << " (样式块触发)" << std::endl;
        }
    }
    
    // 处理引用选择器
    for (const auto& selector : selectors) {
        if (selector.IsReference) {
            std::string resolvedSelector = ProcessReferenceSelector(element, true);
            std::cout << "🔄 引用选择器解析: " << selector.FullSelector << " → " << resolvedSelector << std::endl;
        }
    }
}

void SelectorAutomationEngine::ProcessScriptSelectorAutomation(ElementNode* element, 
                                                               const std::vector<SelectorInfo>& selectors,
                                                               bool hasStyleTriggered) {
    if (!element || selectors.empty()) {
        return;
    }
    
    // 检查是否有触发自动化的选择器 ({{.box}}, {{#box}})
    bool hasTriggerSelectors = HasTriggerSelectors(selectors);
    
    if (hasTriggerSelectors) {
        // 处理类选择器自动化 (如果样式块没有触发)
        if (m_Config.DisableScriptAutoAddClass && !hasStyleTriggered && !m_HasClassAttribute) {
            std::string firstClass = GetFirstClassSelector(selectors);
            if (!firstClass.empty()) {
                AddClassToElement(element, firstClass);
                std::cout << "🔄 自动添加类名: " << firstClass << " (脚本块{{.box}}触发)" << std::endl;
            }
        }
        
        // 处理ID选择器自动化 (如果样式块没有触发)
        if (m_Config.DisableScriptAutoAddId && !hasStyleTriggered && !m_HasIdAttribute) {
            std::string firstId = GetFirstIdSelector(selectors);
            if (!firstId.empty()) {
                AddIdToElement(element, firstId);
                std::cout << "🔄 自动添加ID: " << firstId << " (脚本块{{#box}}触发)" << std::endl;
            }
        }
    }
    
    // 处理引用选择器 (脚本中优先ID)
    for (const auto& selector : selectors) {
        if (selector.IsReference) {
            std::string resolvedSelector = ProcessReferenceSelector(element, false);
            std::cout << "🔄 引用选择器解析: " << selector.FullSelector << " → " << resolvedSelector << std::endl;
        }
    }
}

std::string SelectorAutomationEngine::ProcessReferenceSelector(ElementNode* element, bool isInStyle) {
    if (!element) {
        return "&";
    }
    
    // 样式块中优先选择class，脚本块中优先选择id
    if (isInStyle) {
        if (m_HasClassAttribute && !m_ExistingClass.empty()) {
            return "." + m_ExistingClass;
        } else if (m_HasIdAttribute && !m_ExistingId.empty()) {
            return "#" + m_ExistingId;
        }
    } else {
        if (m_HasIdAttribute && !m_ExistingId.empty()) {
            return "#" + m_ExistingId;
        } else if (m_HasClassAttribute && !m_ExistingClass.empty()) {
            return "." + m_ExistingClass;
        }
    }
    
    return "&";
}

std::string SelectorAutomationEngine::GetFirstClassSelector(const std::vector<SelectorInfo>& selectors) {
    for (const auto& selector : selectors) {
        if (selector.Type == "class") {
            return selector.Name;
        }
    }
    return "";
}

std::string SelectorAutomationEngine::GetFirstIdSelector(const std::vector<SelectorInfo>& selectors) {
    for (const auto& selector : selectors) {
        if (selector.Type == "id") {
            return selector.Name;
        }
    }
    return "";
}

bool SelectorAutomationEngine::HasTriggerSelectors(const std::vector<SelectorInfo>& selectors) {
    for (const auto& selector : selectors) {
        if (selector.FullSelector.find("{{.") == 0 || selector.FullSelector.find("{{#") == 0) {
            return true;
        }
    }
    return false;
}

void SelectorAutomationEngine::UpdateConfig(const SelectorAutomationConfig& config) {
    m_Config = config;
}

void SelectorAutomationEngine::Reset() {
    m_StyleSelectors.clear();
    m_ScriptSelectors.clear();
    m_HasClassAttribute = false;
    m_HasIdAttribute = false;
    m_ExistingClass.clear();
    m_ExistingId.clear();
}

SelectorInfo SelectorAutomationEngine::ParseSelector(const std::string& selector, size_t order) {
    SelectorInfo info;
    info.Order = order;
    
    if (selector.length() > 0 && selector[0] == '.') {
        info.Type = "class";
        info.Name = selector.substr(1);
        info.FullSelector = selector;
    } else if (selector.length() > 0 && selector[0] == '#') {
        info.Type = "id";
        info.Name = selector.substr(1);
        info.FullSelector = selector;
    } else if (selector == "&") {
        info.Type = "reference";
        info.Name = "&";
        info.FullSelector = "&";
        info.IsReference = true;
    }
    
    return info;
}

void SelectorAutomationEngine::AnalyzeElementAttributes(ElementNode* element) {
    if (!element) {
        return;
    }
    
    // 检查现有的class属性
    if (element->HasAttribute("class")) {
        m_HasClassAttribute = true;
        m_ExistingClass = element->GetAttribute("class");
    }
    
    // 检查现有的id属性
    if (element->HasAttribute("id")) {
        m_HasIdAttribute = true;
        m_ExistingId = element->GetAttribute("id");
    }
}

void SelectorAutomationEngine::AddClassToElement(ElementNode* element, const std::string& className) {
    if (!element || className.empty()) {
        return;
    }
    
    if (m_HasClassAttribute) {
        // 如果已有class，追加新的类名
        std::string existingClasses = element->GetAttribute("class");
        if (existingClasses.find(className) == std::string::npos) {
            element->SetAttribute("class", existingClasses + " " + className);
        }
    } else {
        // 添加新的class属性
        element->SetAttribute("class", className);
        m_HasClassAttribute = true;
        m_ExistingClass = className;
    }
}

void SelectorAutomationEngine::AddIdToElement(ElementNode* element, const std::string& idName) {
    if (!element || idName.empty()) {
        return;
    }
    
    if (!m_HasIdAttribute) {
        // 只有在没有ID的情况下才添加
        element->SetAttribute("id", idName);
        m_HasIdAttribute = true;
        m_ExistingId = idName;
    }
}

} // namespace CHTL