#include "EnhancedSelectorEngine.h"
#include <regex>
#include <iostream>
#include <sstream>
#include <unordered_set>

namespace CHTLJS {

EnhancedSelectorEngine::EnhancedSelectorEngine() {
}

void EnhancedSelectorEngine::SetCurrentElementContext(const std::string& elementClass,
                                                     const std::string& elementId,
                                                     const std::string& elementTag) {
    m_CurrentElementClass = elementClass;
    m_CurrentElementId = elementId;
    m_CurrentElementTag = elementTag;
    
    std::cout << "🎯 设置元素上下文: tag=" << elementTag 
              << ", class=" << elementClass 
              << ", id=" << elementId << std::endl;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ParseEnhancedSelector(const std::string& expression) {
    EnhancedSelectorInfo info;
    info.OriginalExpression = expression;
    
    // 检查缓存
    auto cacheIt = m_SelectorCache.find(expression);
    if (cacheIt != m_SelectorCache.end()) {
        return cacheIt->second;
    }
    
    // 验证语法
    if (!ValidateSelectorSyntax(expression)) {
        std::cerr << "❌ 无效的增强选择器语法: " << expression << std::endl;
        return info;
    }
    
    // 提取选择器内容 (去掉 {{ 和 }})
    if (expression.length() < 4 || 
        expression.substr(0, 2) != "{{" || 
        expression.substr(expression.length() - 2) != "}}") {
        std::cerr << "❌ 增强选择器格式错误: " << expression << std::endl;
        return info;
    }
    
    std::string selectorContent = expression.substr(2, expression.length() - 4);
    info.SelectorContent = selectorContent;
    
    // 确定选择器类型并处理
    info.Type = DetermineSelectorType(selectorContent);
    
    switch (info.Type) {
        case EnhancedSelectorType::BASIC_SELECTOR:
            info = ProcessIntelligentSelector(selectorContent);
            break;
        case EnhancedSelectorType::CLASS_SELECTOR:
            info = ProcessClassSelector(selectorContent.substr(1)); // 去掉 .
            break;
        case EnhancedSelectorType::ID_SELECTOR:
            info = ProcessIdSelector(selectorContent.substr(1)); // 去掉 #
            break;
        case EnhancedSelectorType::TAG_SELECTOR:
            info = ProcessTagSelector(selectorContent);
            break;
        case EnhancedSelectorType::COMPOUND_SELECTOR:
            info = ProcessCompoundSelector(selectorContent);
            break;
        case EnhancedSelectorType::INDEX_SELECTOR:
            info = ProcessIndexSelector(selectorContent);
            break;
        case EnhancedSelectorType::CONTEXT_REFERENCE:
            info = ProcessContextReference();
            break;
    }
    
    // 生成JavaScript表达式
    info.JSExpression = GenerateJavaScriptSelector(info);
    
    // 缓存结果
    m_SelectorCache[expression] = info;
    
    return info;
}

EnhancedSelectorType EnhancedSelectorEngine::DetermineSelectorType(const std::string& content) {
    if (content == "&") {
        return EnhancedSelectorType::CONTEXT_REFERENCE;
    }
    
    if (content.length() > 0 && content[0] == '.') {
        return EnhancedSelectorType::CLASS_SELECTOR;
    }
    
    if (content.length() > 0 && content[0] == '#') {
        return EnhancedSelectorType::ID_SELECTOR;
    }
    
    if (content.find("[") != std::string::npos && content.find("]") != std::string::npos) {
        return EnhancedSelectorType::INDEX_SELECTOR;
    }
    
    if (content.find(" ") != std::string::npos) {
        return EnhancedSelectorType::COMPOUND_SELECTOR;
    }
    
    if (IsValidHTMLTag(content)) {
        return EnhancedSelectorType::TAG_SELECTOR;
    }
    
    return EnhancedSelectorType::BASIC_SELECTOR;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessIntelligentSelector(const std::string& selectorContent) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::BASIC_SELECTOR;
    info.SelectorContent = selectorContent;
    info.OriginalExpression = "{{" + selectorContent + "}}";
    
    // 智能推导：先判断是否为tag，然后查找类名/id=box(id优先)的元素
    if (IsValidHTMLTag(selectorContent)) {
        // 是HTML标签
        info.ResolvedSelector = selectorContent;
        info.IsArray = true; // 标签选择器返回数组
        std::cout << "🎯 智能推导: " << selectorContent << " → HTML标签 (数组)" << std::endl;
    } else {
        // 不是HTML标签，查找class/id (id优先)
        if (!m_CurrentElementId.empty() && m_CurrentElementId == selectorContent) {
            info.ResolvedSelector = "#" + selectorContent;
            std::cout << "🎯 智能推导: " << selectorContent << " → ID选择器" << std::endl;
        } else if (!m_CurrentElementClass.empty() && m_CurrentElementClass == selectorContent) {
            info.ResolvedSelector = "." + selectorContent;
            std::cout << "🎯 智能推导: " << selectorContent << " → 类选择器" << std::endl;
        } else {
            // 默认作为类选择器
            info.ResolvedSelector = "." + selectorContent;
            std::cout << "🎯 智能推导: " << selectorContent << " → 默认类选择器" << std::endl;
        }
    }
    
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessClassSelector(const std::string& className) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::CLASS_SELECTOR;
    info.SelectorContent = "." + className;
    info.OriginalExpression = "{{." + className + "}}";
    info.ResolvedSelector = "." + className;
    
    std::cout << "🎯 类选择器: {{." << className << "}} → ." << className << std::endl;
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessIdSelector(const std::string& idName) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::ID_SELECTOR;
    info.SelectorContent = "#" + idName;
    info.OriginalExpression = "{{#" + idName + "}}";
    info.ResolvedSelector = "#" + idName;
    
    std::cout << "🎯 ID选择器: {{#" << idName << "}} → #" << idName << std::endl;
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessTagSelector(const std::string& tagName) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::TAG_SELECTOR;
    info.SelectorContent = tagName;
    info.OriginalExpression = "{{" + tagName + "}}";
    info.ResolvedSelector = tagName;
    info.IsArray = true; // 标签选择器返回数组
    
    std::cout << "🎯 标签选择器: {{" << tagName << "}} → " << tagName << " (数组)" << std::endl;
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessCompoundSelector(const std::string& compoundSelector) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::COMPOUND_SELECTOR;
    info.SelectorContent = compoundSelector;
    info.OriginalExpression = "{{" + compoundSelector + "}}";
    info.ResolvedSelector = compoundSelector;
    info.IsArray = true; // 复合选择器返回数组
    
    std::cout << "🎯 复合选择器: {{" << compoundSelector << "}} → " << compoundSelector << " (数组)" << std::endl;
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessIndexSelector(const std::string& indexSelector) {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::INDEX_SELECTOR;
    info.SelectorContent = indexSelector;
    info.OriginalExpression = "{{" + indexSelector + "}}";
    
    auto [tagName, index] = ExtractIndexValue(indexSelector);
    info.ResolvedSelector = tagName;
    info.Index = index;
    
    std::cout << "🎯 索引选择器: {{" << indexSelector << "}} → " << tagName << "[" << index << "]" << std::endl;
    return info;
}

EnhancedSelectorInfo EnhancedSelectorEngine::ProcessContextReference() {
    EnhancedSelectorInfo info;
    info.Type = EnhancedSelectorType::CONTEXT_REFERENCE;
    info.SelectorContent = "&";
    info.OriginalExpression = "{{&}}";
    
    // 上下文引用：脚本中优先ID，其次class
    if (!m_CurrentElementId.empty()) {
        info.ResolvedSelector = "#" + m_CurrentElementId;
        std::cout << "🎯 上下文引用: {{&}} → #" << m_CurrentElementId << " (ID优先)" << std::endl;
    } else if (!m_CurrentElementClass.empty()) {
        info.ResolvedSelector = "." + m_CurrentElementClass;
        std::cout << "🎯 上下文引用: {{&}} → ." << m_CurrentElementClass << " (class备选)" << std::endl;
    } else {
        info.ResolvedSelector = m_CurrentElementTag;
        std::cout << "🎯 上下文引用: {{&}} → " << m_CurrentElementTag << " (tag备选)" << std::endl;
    }
    
    return info;
}

std::string EnhancedSelectorEngine::GenerateJavaScriptSelector(const EnhancedSelectorInfo& selectorInfo) {
    if (selectorInfo.Type == EnhancedSelectorType::INDEX_SELECTOR) {
        return GenerateIndexAccess(selectorInfo.ResolvedSelector, selectorInfo.Index);
    }
    
    return GenerateQuerySelector(selectorInfo.ResolvedSelector, selectorInfo.IsArray);
}

std::string EnhancedSelectorEngine::GenerateQuerySelector(const std::string& cssSelector, bool isArray) {
    if (isArray) {
        return "document.querySelectorAll('" + cssSelector + "')";
    } else {
        return "document.querySelector('" + cssSelector + "')";
    }
}

std::string EnhancedSelectorEngine::GenerateIndexAccess(const std::string& baseSelector, size_t index) {
    return "document.querySelectorAll('" + baseSelector + "')[" + std::to_string(index) + "]";
}

std::pair<std::string, size_t> EnhancedSelectorEngine::ExtractIndexValue(const std::string& indexExpression) {
    std::regex indexRegex(R"(([a-zA-Z][a-zA-Z0-9]*)\[(\d+)\])");
    std::smatch matches;
    
    if (std::regex_search(indexExpression, matches, indexRegex)) {
        std::string tagName = matches[1].str();
        size_t index = std::stoull(matches[2].str());
        return {tagName, index};
    }
    
    return {indexExpression, 0};
}

bool EnhancedSelectorEngine::ValidateSelectorSyntax(const std::string& expression) {
    // 基础格式检查
    if (expression.length() < 4) {
        return false;
    }
    
    if (expression.substr(0, 2) != "{{" || expression.substr(expression.length() - 2) != "}}") {
        return false;
    }
    
    std::string content = expression.substr(2, expression.length() - 4);
    
    // 检查是否为空
    if (content.empty()) {
        return false;
    }
    
    // 检查是否包含不支持的复杂选择器
    if (content.find(".") != std::string::npos && content.find("#") != std::string::npos) {
        // 不支持 .class#id 这种交集选择器
        return false;
    }
    
    return true;
}

bool EnhancedSelectorEngine::IsValidHTMLTag(const std::string& tagName) {
    // HTML5标签列表
    static const std::unordered_set<std::string> htmlTags = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "a", "img", "br", "hr", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "dl", "dt", "dd", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "button", "textarea", "select", "option", "label", "fieldset", "legend",
        "section", "article", "header", "footer", "nav", "aside", "main", "figure", "figcaption",
        "details", "summary", "dialog", "canvas", "svg", "audio", "video", "source", "track",
        "embed", "object", "param", "iframe", "area", "map", "base", "col", "colgroup",
        "caption", "optgroup", "datalist", "output", "progress", "meter", "time", "mark",
        "ruby", "rt", "rp", "bdi", "bdo", "kbd", "samp", "var", "sub", "sup", "small",
        "strong", "em", "b", "i", "u", "s", "code", "pre", "blockquote", "cite", "q",
        "abbr", "dfn", "del", "ins", "wbr"
    };
    
    return htmlTags.find(tagName) != htmlTags.end();
}

std::string EnhancedSelectorEngine::OptimizeSelector(const std::string& selector) {
    // 优化选择器性能
    std::string optimized = selector;
    
    // 移除多余的空格
    std::regex spaceRegex(R"(\s+)");
    optimized = std::regex_replace(optimized, spaceRegex, " ");
    
    // 去除前后空白
    optimized.erase(0, optimized.find_first_not_of(" \t"));
    optimized.erase(optimized.find_last_not_of(" \t") + 1);
    
    return optimized;
}

void EnhancedSelectorEngine::ClearCache() {
    m_SelectorCache.clear();
    std::cout << "🧹 清理增强选择器缓存" << std::endl;
}

std::unordered_map<std::string, size_t> EnhancedSelectorEngine::GetSelectorStatistics() {
    std::unordered_map<std::string, size_t> stats;
    
    for (const auto& [expression, info] : m_SelectorCache) {
        std::string typeStr;
        switch (info.Type) {
            case EnhancedSelectorType::BASIC_SELECTOR: typeStr = "Basic"; break;
            case EnhancedSelectorType::CLASS_SELECTOR: typeStr = "Class"; break;
            case EnhancedSelectorType::ID_SELECTOR: typeStr = "ID"; break;
            case EnhancedSelectorType::TAG_SELECTOR: typeStr = "Tag"; break;
            case EnhancedSelectorType::COMPOUND_SELECTOR: typeStr = "Compound"; break;
            case EnhancedSelectorType::INDEX_SELECTOR: typeStr = "Index"; break;
            case EnhancedSelectorType::CONTEXT_REFERENCE: typeStr = "Context"; break;
        }
        stats[typeStr]++;
    }
    
    stats["Total"] = m_SelectorCache.size();
    return stats;
}

std::vector<std::string> EnhancedSelectorEngine::AnalyzeCompoundSelector(const std::string& compoundSelector) {
    std::vector<std::string> components;
    std::istringstream stream(compoundSelector);
    std::string component;
    
    while (stream >> component) {
        components.push_back(component);
    }
    
    return components;
}

} // namespace CHTLJS