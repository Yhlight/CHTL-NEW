#include "CHTLJSGlobalMap.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTLJS {

CHTLJSGlobalMap::CHTLJSGlobalMap() 
    : m_VirtualObjectCounter(0), m_AnimationCounter(0), m_DelegateCounter(0) {
    
    // 注册CHTL JS内置函数
    RegisterCHTLJSFunction("listen", true);
    RegisterCHTLJSFunction("delegate", true);
    RegisterCHTLJSFunction("animate", true);
    RegisterCHTLJSFunction("module", false); // module不支持vir
}

bool CHTLJSGlobalMap::RegisterVirtualObject(std::unique_ptr<VirtualObjectDefinition> definition) {
    if (!definition || definition->Name.empty()) {
        return false;
    }
    
    // 检查是否已存在
    if (m_VirtualObjects.find(definition->Name) != m_VirtualObjects.end()) {
        return false; // 虚对象已存在
    }
    
    m_VirtualObjects[definition->Name] = std::move(definition);
    return true;
}

VirtualObjectDefinition* CHTLJSGlobalMap::GetVirtualObject(const std::string& name) {
    auto it = m_VirtualObjects.find(name);
    return (it != m_VirtualObjects.end()) ? it->second.get() : nullptr;
}

std::string CHTLJSGlobalMap::ResolveVirtualObjectKey(const std::string& objectName, const std::string& keyName) {
    auto virObj = GetVirtualObject(objectName);
    if (!virObj) {
        return ""; // 虚对象不存在
    }
    
    // 检查键类型
    auto typeIt = virObj->KeyTypes.find(keyName);
    if (typeIt != virObj->KeyTypes.end()) {
        std::string keyType = typeIt->second;
        
        if (keyType == "function") {
            // 返回函数引用
            auto funcIt = virObj->FunctionRefs.find(keyName);
            if (funcIt != virObj->FunctionRefs.end()) {
                return funcIt->second;
            }
        }
        else if (keyType == "object" || keyType == "array") {
            // 返回对象或数组值
            auto valueIt = virObj->KeyValues.find(keyName);
            if (valueIt != virObj->KeyValues.end()) {
                return valueIt->second;
            }
        }
    }
    
    return ""; // 键不存在
}

bool CHTLJSGlobalMap::RegisterEventDelegate(std::unique_ptr<EventDelegateDefinition> definition) {
    if (!definition || definition->ParentSelector.empty()) {
        return false;
    }
    
    std::string key = definition->ParentSelector;
    
    // 检查是否已存在相同的父选择器
    auto it = m_EventDelegates.find(key);
    if (it != m_EventDelegates.end()) {
        // 合并到现有的事件委托中
        auto& existing = it->second;
        
        // 合并目标选择器
        for (const auto& target : definition->TargetSelectors) {
            if (std::find(existing->TargetSelectors.begin(), existing->TargetSelectors.end(), target) 
                == existing->TargetSelectors.end()) {
                existing->TargetSelectors.push_back(target);
            }
        }
        
        // 合并事件处理器
        for (const auto& handler : definition->EventHandlers) {
            existing->EventHandlers[handler.first] = handler.second;
        }
        
        return true;
    }
    
    m_EventDelegates[key] = std::move(definition);
    return true;
}

EventDelegateDefinition* CHTLJSGlobalMap::GetEventDelegate(const std::string& parentSelector) {
    auto it = m_EventDelegates.find(parentSelector);
    return (it != m_EventDelegates.end()) ? it->second.get() : nullptr;
}

bool CHTLJSGlobalMap::MergeEventDelegate(const std::string& parentSelector, 
                                        const std::string& targetSelector,
                                        const std::string& eventType, 
                                        const std::string& handler) {
    auto delegate = GetEventDelegate(parentSelector);
    if (!delegate) {
        // 创建新的事件委托
        auto newDelegate = std::make_unique<EventDelegateDefinition>(parentSelector);
        newDelegate->TargetSelectors.push_back(targetSelector);
        newDelegate->EventHandlers[eventType] = handler;
        
        return RegisterEventDelegate(std::move(newDelegate));
    }
    
    // 合并到现有委托
    if (std::find(delegate->TargetSelectors.begin(), delegate->TargetSelectors.end(), targetSelector) 
        == delegate->TargetSelectors.end()) {
        delegate->TargetSelectors.push_back(targetSelector);
    }
    
    delegate->EventHandlers[eventType] = handler;
    return true;
}

bool CHTLJSGlobalMap::RegisterAnimation(std::unique_ptr<AnimationDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    if (definition->Name.empty()) {
        definition->Name = GenerateAnimationName();
    }
    
    m_Animations[definition->Name] = std::move(definition);
    return true;
}

AnimationDefinition* CHTLJSGlobalMap::GetAnimation(const std::string& name) {
    auto it = m_Animations.find(name);
    return (it != m_Animations.end()) ? it->second.get() : nullptr;
}

std::string CHTLJSGlobalMap::GenerateAnimationName() {
    return "chtljs_anim_" + std::to_string(++m_AnimationCounter);
}

bool CHTLJSGlobalMap::RegisterModuleLoad(std::unique_ptr<ModuleLoadDefinition> definition) {
    if (!definition) {
        return false;
    }
    
    std::string key = "module_" + std::to_string(m_ModuleLoads.size());
    m_ModuleLoads[key] = std::move(definition);
    return true;
}

ModuleLoadDefinition* CHTLJSGlobalMap::GetModuleLoad(const std::string& contextName) {
    auto it = m_ModuleLoads.find(contextName);
    return (it != m_ModuleLoads.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> CHTLJSGlobalMap::AnalyzeDependencies(const std::vector<std::string>& loadPaths) {
    // 实现AMD风格的依赖分析
    std::vector<std::string> sortedPaths = loadPaths;
    
    // 简单的拓扑排序（这里可以根据需要实现更复杂的依赖分析）
    std::sort(sortedPaths.begin(), sortedPaths.end());
    
    return sortedPaths;
}

bool CHTLJSGlobalMap::CacheSelector(std::unique_ptr<EnhancedSelectorCache> cache) {
    if (!cache || cache->OriginalSelector.empty()) {
        return false;
    }
    
    m_SelectorCache[cache->OriginalSelector] = std::move(cache);
    return true;
}

EnhancedSelectorCache* CHTLJSGlobalMap::GetSelectorCache(const std::string& selector) {
    auto it = m_SelectorCache.find(selector);
    return (it != m_SelectorCache.end()) ? it->second.get() : nullptr;
}

std::string CHTLJSGlobalMap::ParseEnhancedSelector(const std::string& selector) {
    // 检查缓存
    auto cached = GetSelectorCache(selector);
    if (cached && cached->IsValidSelector) {
        return cached->ParsedSelector;
    }
    
    // 解析增强选择器：{{selector}}
    if (selector.length() < 4 || 
        selector.substr(0, 2) != "{{" || 
        selector.substr(selector.length() - 2) != "}}") {
        return ""; // 无效选择器
    }
    
    std::string content = selector.substr(2, selector.length() - 4);
    std::string parsed;
    std::string selectorType;
    
    // 解析选择器内容
    if (content.front() == '.') {
        // 类选择器
        parsed = "document.querySelector('" + content + "')";
        selectorType = "class";
    }
    else if (content.front() == '#') {
        // ID选择器
        parsed = "document.getElementById('" + content.substr(1) + "')";
        selectorType = "id";
    }
    else if (content.find(' ') != std::string::npos) {
        // 后代选择器
        parsed = "document.querySelector('" + content + "')";
        selectorType = "descendant";
    }
    else if (content.find('[') != std::string::npos) {
        // 索引访问：button[0]
        std::regex indexPattern(R"((\w+)\[(\d+)\])");
        std::smatch match;
        if (std::regex_match(content, match, indexPattern)) {
            std::string tag = match[1].str();
            std::string index = match[2].str();
            parsed = "document.getElementsByTagName('" + tag + "')[" + index + "]";
            selectorType = "index";
        }
    }
    else {
        // 先判断是否为标签，然后查找类名/id（id优先）
        parsed = "(function() { "
                "var elem = document.getElementById('" + content + "'); "
                "if (elem) return elem; "
                "elem = document.querySelector('." + content + "'); "
                "if (elem) return elem; "
                "return document.getElementsByTagName('" + content + "'); "
                "})()";
        selectorType = "auto";
    }
    
    // 缓存解析结果
    auto cache = std::make_unique<EnhancedSelectorCache>(selector, parsed, selectorType);
    CacheSelector(std::move(cache));
    
    return parsed;
}

std::string CHTLJSGlobalMap::GenerateDOMQueryCode(const std::string& selector, const std::string& method) {
    std::string baseCode = ParseEnhancedSelector(selector);
    
    if (baseCode.empty()) {
        return "";
    }
    
    if (method.empty()) {
        return baseCode;
    }
    
    // 添加方法调用
    if (method == "textContent" || method == "innerHTML" || method == "addEventListener") {
        return "(" + baseCode + ")." + method;
    }
    
    return baseCode + "." + method;
}

void CHTLJSGlobalMap::RegisterCHTLJSFunction(const std::string& functionName, bool supportVir) {
    m_CHTLJSFunctions[functionName] = supportVir;
}

bool CHTLJSGlobalMap::IsCHTLJSFunction(const std::string& functionName) {
    return m_CHTLJSFunctions.find(functionName) != m_CHTLJSFunctions.end();
}

std::vector<std::string> CHTLJSGlobalMap::GetAllCHTLJSFunctions() const {
    std::vector<std::string> functions;
    for (const auto& pair : m_CHTLJSFunctions) {
        functions.push_back(pair.first);
    }
    return functions;
}

std::string CHTLJSGlobalMap::GenerateListenerCode(const std::string& selector, 
                                                  const std::unordered_map<std::string, std::string>& events) {
    std::ostringstream code;
    std::string domElement = ParseEnhancedSelector(selector);
    
    code << "(function() {\n";
    code << "  var element = " << domElement << ";\n";
    code << "  if (element) {\n";
    
    for (const auto& event : events) {
        code << "    element.addEventListener('" << event.first << "', " << event.second << ");\n";
    }
    
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSGlobalMap::GenerateDelegateCode(const EventDelegateDefinition& delegate) {
    std::ostringstream code;
    std::string parentElement = ParseEnhancedSelector(delegate.ParentSelector);
    
    code << "(function() {\n";
    code << "  var parentElement = " << parentElement << ";\n";
    code << "  if (parentElement) {\n";
    
    for (const auto& event : delegate.EventHandlers) {
        code << "    parentElement.addEventListener('" << event.first << "', function(e) {\n";
        
        // 检查目标选择器
        for (const auto& target : delegate.TargetSelectors) {
            std::string targetCheck;
            if (target.front() == '.') {
                targetCheck = "e.target.classList.contains('" + target.substr(1) + "')";
            }
            else if (target.front() == '#') {
                targetCheck = "e.target.id === '" + target.substr(1) + "'";
            }
            else {
                targetCheck = "e.target.tagName.toLowerCase() === '" + target + "'";
            }
            
            code << "      if (" << targetCheck << ") {\n";
            code << "        (" << event.second << ")(e);\n";
            code << "      }\n";
        }
        
        code << "    });\n";
    }
    
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSGlobalMap::GenerateAnimationCode(const AnimationDefinition& animation) {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  var targets = [];\n";
    
    // 处理目标元素
    for (const auto& target : animation.Targets) {
        std::string targetElement = ParseEnhancedSelector(target);
        code << "  var target = " << targetElement << ";\n";
        code << "  if (target) targets.push(target);\n";
    }
    
    code << "  \n";
    code << "  function animate() {\n";
    code << "    var startTime = performance.now();\n";
    code << "    var duration = " << animation.Duration << ";\n";
    code << "    \n";
    
    // 设置起始状态
    if (!animation.BeginState.empty()) {
        code << "    targets.forEach(function(target) {\n";
        for (const auto& prop : animation.BeginState) {
            code << "      target.style." << prop.first << " = '" << prop.second << "';\n";
        }
        code << "    });\n";
    }
    
    code << "    \n";
    code << "    function frame(currentTime) {\n";
    code << "      var elapsed = currentTime - startTime;\n";
    code << "      var progress = Math.min(elapsed / duration, 1);\n";
    code << "      \n";
    
    // 处理关键帧
    if (!animation.Keyframes.empty()) {
        for (const auto& keyframe : animation.Keyframes) {
            double at = keyframe.first;
            code << "      if (progress >= " << at << ") {\n";
            code << "        targets.forEach(function(target) {\n";
            for (const auto& prop : keyframe.second) {
                code << "          target.style." << prop.first << " = '" << prop.second << "';\n";
            }
            code << "        });\n";
            code << "      }\n";
        }
    }
    
    // 设置结束状态
    code << "      if (progress >= 1) {\n";
    if (!animation.EndState.empty()) {
        code << "        targets.forEach(function(target) {\n";
        for (const auto& prop : animation.EndState) {
            code << "          target.style." << prop.first << " = '" << prop.second << "';\n";
        }
        code << "        });\n";
    }
    
    // 处理回调
    if (!animation.Callback.empty()) {
        code << "        (" << animation.Callback << ")();\n";
    }
    
    // 处理循环
    if (animation.Loop > 1 || animation.Loop == -1) {
        code << "        // TODO: 实现循环逻辑\n";
    }
    
    code << "      } else {\n";
    code << "        requestAnimationFrame(frame);\n";
    code << "      }\n";
    code << "    }\n";
    code << "    \n";
    
    // 处理延迟
    if (animation.Delay > 0) {
        code << "    setTimeout(function() {\n";
        code << "      requestAnimationFrame(frame);\n";
        code << "    }, " << animation.Delay << ");\n";
    }
    else {
        code << "    requestAnimationFrame(frame);\n";
    }
    
    code << "  }\n";
    code << "  \n";
    code << "  animate();\n";
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSGlobalMap::GenerateModuleLoadCode(const ModuleLoadDefinition& moduleLoad) {
    std::ostringstream code;
    
    // 生成AMD风格的模块加载代码
    code << "(function() {\n";
    code << "  var loadedModules = {};\n";
    code << "  var loadQueue = [];\n";
    code << "  \n";
    
    // 分析依赖顺序
    auto sortedPaths = AnalyzeDependencies(moduleLoad.LoadPaths);
    
    code << "  var modulePaths = [\n";
    for (size_t i = 0; i < sortedPaths.size(); ++i) {
        code << "    '" << sortedPaths[i] << "'";
        if (i < sortedPaths.size() - 1) {
            code << ",";
        }
        code << "\n";
    }
    code << "  ];\n";
    code << "  \n";
    
    code << "  function loadModule(path, callback) {\n";
    code << "    if (loadedModules[path]) {\n";
    code << "      callback();\n";
    code << "      return;\n";
    code << "    }\n";
    code << "    \n";
    code << "    var script = document.createElement('script');\n";
    code << "    script.src = path;\n";
    code << "    script.onload = function() {\n";
    code << "      loadedModules[path] = true;\n";
    code << "      callback();\n";
    code << "    };\n";
    code << "    script.onerror = function() {\n";
    code << "      console.error('Failed to load module: ' + path);\n";
    code << "    };\n";
    code << "    document.head.appendChild(script);\n";
    code << "  }\n";
    code << "  \n";
    
    code << "  function loadNext(index) {\n";
    code << "    if (index >= modulePaths.length) return;\n";
    code << "    loadModule(modulePaths[index], function() {\n";
    code << "      loadNext(index + 1);\n";
    code << "    });\n";
    code << "  }\n";
    code << "  \n";
    code << "  loadNext(0);\n";
    code << "})();\n";
    
    return code.str();
}

void CHTLJSGlobalMap::Clear() {
    m_VirtualObjects.clear();
    m_EventDelegates.clear();
    m_Animations.clear();
    m_ModuleLoads.clear();
    m_SelectorCache.clear();
    m_CHTLJSFunctions.clear();
    
    m_VirtualObjectCounter = 0;
    m_AnimationCounter = 0;
    m_DelegateCounter = 0;
}

std::string CHTLJSGlobalMap::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL JS GlobalMap Statistics:\n";
    oss << "Virtual Objects: " << m_VirtualObjects.size() << "\n";
    oss << "Event Delegates: " << m_EventDelegates.size() << "\n";
    oss << "Animations: " << m_Animations.size() << "\n";
    oss << "Module Loads: " << m_ModuleLoads.size() << "\n";
    oss << "Selector Cache: " << m_SelectorCache.size() << "\n";
    oss << "CHTL JS Functions: " << m_CHTLJSFunctions.size() << "\n";
    return oss.str();
}

std::string CHTLJSGlobalMap::GenerateUniqueName(const std::string& prefix, int& counter) {
    return prefix + "_" + std::to_string(++counter);
}

bool CHTLJSGlobalMap::ValidateSelector(const std::string& selector) {
    // 验证选择器格式
    if (selector.empty()) {
        return false;
    }
    
    // 检查是否为增强选择器格式
    if (selector.substr(0, 2) == "{{" && selector.substr(selector.length() - 2) == "}}") {
        std::string content = selector.substr(2, selector.length() - 4);
        
        // 验证选择器内容
        std::regex validSelectorPattern(R"([.#]?[a-zA-Z][a-zA-Z0-9\-_]*(?:\s+[a-zA-Z][a-zA-Z0-9\-_]*)*(?:\[\d+\])?)");
        return std::regex_match(content, validSelectorPattern);
    }
    
    return false;
}

void CHTLJSGlobalMap::OptimizeEventDelegateMerging(const std::string& parentSelector) {
    auto delegate = GetEventDelegate(parentSelector);
    if (!delegate) {
        return;
    }
    
    // 优化事件委托，合并相同的事件处理器
    std::unordered_map<std::string, std::vector<std::string>> eventTargetMap;
    
    for (const auto& target : delegate->TargetSelectors) {
        for (const auto& event : delegate->EventHandlers) {
            eventTargetMap[event.first].push_back(target);
        }
    }
    
    // 重构事件处理器以提高效率
    delegate->IsRegistered = true;
}

} // namespace CHTLJS