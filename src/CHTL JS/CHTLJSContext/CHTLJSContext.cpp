#include "CHTLJSContext.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTLJS {

CHTLJSContextManager::CHTLJSContextManager() 
    : m_VirObjectCounter(0), m_AnimationCounter(0), m_DelegateCounter(0) {
    CreateDefaultCHTLJSGlobalContext();
}

void CHTLJSContextManager::CreateDefaultCHTLJSGlobalContext() {
    m_CurrentContext = std::make_unique<CHTLJSContextInfo>(
        CHTLJSContextType::GLOBAL_CONTEXT, 
        CHTLJSScopeType::FILE_SCOPE, 
        "chtljs_global"
    );
}

bool CHTLJSContextManager::EnterContext(CHTLJSContextType type, CHTLJSScopeType scope, const std::string& name) {
    // 验证CHTL JS上下文转换的有效性
    if (m_CurrentContext && !IsValidCHTLJSContextTransition(m_CurrentContext->Type, type)) {
        return false;
    }
    
    // 保存当前上下文
    if (m_CurrentContext) {
        m_ContextStack.push(m_CurrentContext.release());
    }
    
    // 创建新CHTL JS上下文
    auto newContext = std::make_unique<CHTLJSContextInfo>(type, scope, name);
    
    // 设置父上下文名称
    if (!m_ContextStack.empty()) {
        newContext->ParentName = m_ContextStack.top()->Name;
        
        // 继承父CHTL JS上下文信息
        InheritFromParentCHTLJSContext(m_ContextStack.top(), newContext.get());
    }
    
    // 应用CHTL JS作用域规则
    ApplyCHTLJSScopeRules(newContext.get());
    
    m_CurrentContext = std::move(newContext);
    return true;
}

bool CHTLJSContextManager::ExitContext() {
    if (m_ContextStack.empty()) {
        return false; // 无法退出全局上下文
    }
    
    m_CurrentContext.reset(m_ContextStack.top());
    m_ContextStack.pop();
    return true;
}

CHTLJSContextInfo* CHTLJSContextManager::GetCurrentContext() {
    return m_CurrentContext.get();
}

CHTLJSContextType CHTLJSContextManager::GetCurrentContextType() const {
    return m_CurrentContext ? m_CurrentContext->Type : CHTLJSContextType::GLOBAL_CONTEXT;
}

CHTLJSScopeType CHTLJSContextManager::GetCurrentScopeType() const {
    return m_CurrentContext ? m_CurrentContext->Scope : CHTLJSScopeType::FILE_SCOPE;
}

void CHTLJSContextManager::RegisterVirtualObject(const std::string& name, 
                                                 const std::string& functionContent, 
                                                 bool isGlobal) {
    if (isGlobal) {
        m_GlobalVirtualObjects[name] = functionContent;
    }
    else if (m_CurrentContext) {
        m_CurrentContext->VirtualObjects[name] = functionContent;
        m_CurrentContext->CurrentVirObject = name;
    }
}

VirKeyResolution CHTLJSContextManager::ResolveVirtualObjectKey(const std::string& objectName, 
                                                              const std::string& keyName) {
    // 首先在当前上下文查找
    if (m_CurrentContext) {
        auto it = m_CurrentContext->VirtualObjects.find(objectName);
        if (it != m_CurrentContext->VirtualObjects.end()) {
            // 解析CHTL JS函数中的键
            auto keys = ParseCHTLJSFunctionKeys(it->second);
            auto keyIt = keys.find(keyName);
            if (keyIt != keys.end()) {
                // 判断键的类型
                std::string value = keyIt->second;
                std::string keyType;
                
                if (value.find("function") != std::string::npos || value.find("()") != std::string::npos) {
                    keyType = "function";
                }
                else if (value.front() == '{' && value.back() == '}') {
                    keyType = "object";
                }
                else if (value.front() == '[' && value.back() == ']') {
                    keyType = "array";
                }
                else {
                    keyType = "value";
                }
                
                return VirKeyResolution(value, keyType);
            }
        }
    }
    
    // 在全局虚对象中查找
    auto globalIt = m_GlobalVirtualObjects.find(objectName);
    if (globalIt != m_GlobalVirtualObjects.end()) {
        auto keys = ParseCHTLJSFunctionKeys(globalIt->second);
        auto keyIt = keys.find(keyName);
        if (keyIt != keys.end()) {
            return VirKeyResolution(keyIt->second, "global");
        }
    }
    
    VirKeyResolution result;
    result.IsResolved = false;
    result.ErrorMessage = "Virtual object or key not found: " + objectName + "->" + keyName;
    return result;
}

bool CHTLJSContextManager::HasVirtualObject(const std::string& name) {
    // 检查当前上下文
    if (m_CurrentContext) {
        if (m_CurrentContext->VirtualObjects.find(name) != m_CurrentContext->VirtualObjects.end()) {
            return true;
        }
    }
    
    // 检查全局虚对象
    return m_GlobalVirtualObjects.find(name) != m_GlobalVirtualObjects.end();
}

void CHTLJSContextManager::RegisterSelectorMapping(const std::string& selector, const std::string& domCode) {
    m_SelectorCache[selector] = domCode;
    
    if (m_CurrentContext) {
        m_CurrentContext->SelectorMappings[selector] = domCode;
        m_CurrentContext->CurrentSelector = selector;
    }
}

std::string CHTLJSContextManager::ResolveEnhancedSelector(const std::string& selector) {
    // 检查缓存
    auto cacheIt = m_SelectorCache.find(selector);
    if (cacheIt != m_SelectorCache.end()) {
        return cacheIt->second;
    }
    
    // 解析增强选择器：{{selector}}
    if (selector.length() < 4 || 
        selector.substr(0, 2) != "{{" || 
        selector.substr(selector.length() - 2) != "}}") {
        return ""; // 无效选择器
    }
    
    std::string content = selector.substr(2, selector.length() - 4);
    std::string domCode = GenerateSelectorPriorityCode(content);
    
    // 缓存结果
    RegisterSelectorMapping(selector, domCode);
    
    return domCode;
}

std::string CHTLJSContextManager::GenerateSelectorPriorityCode(const std::string& content) {
    std::ostringstream code;
    
    if (content.front() == '.') {
        // 类选择器：{{.box}}
        code << "document.querySelector('" << content << "')";
    }
    else if (content.front() == '#') {
        // ID选择器：{{#box}}
        std::string idName = content.substr(1);
        code << "document.getElementById('" << idName << "')";
    }
    else if (content.find(' ') != std::string::npos) {
        // 后代选择器：{{.box button}}
        code << "document.querySelector('" << content << "')";
    }
    else if (content.find('[') != std::string::npos) {
        // 索引访问：{{button[0]}}
        std::regex indexPattern(R"((\w+)\[(\d+)\])");
        std::smatch match;
        if (std::regex_match(content, match, indexPattern)) {
            std::string tag = match[1].str();
            std::string index = match[2].str();
            code << "document.getElementsByTagName('" << tag << "')[" << index << "]";
        }
    }
    else {
        // 标签/类名/ID优先级查找：{{box}}
        // 先判断是否为tag，然后查找类名/id（id优先）
        code << "(function() { ";
        code << "var elem = document.getElementById('" << content << "'); ";
        code << "if (elem) return elem; ";
        code << "elem = document.querySelector('." << content << "'); ";
        code << "if (elem) return elem; ";
        code << "return document.getElementsByTagName('" << content << "'); ";
        code << "})()";
    }
    
    return code.str();
}

void CHTLJSContextManager::RegisterEventHandler(const std::string& selector, 
                                               const std::string& eventType, 
                                               const std::string& handler, 
                                               bool isGlobal) {
    std::string key = selector + "::" + eventType;
    
    if (isGlobal) {
        if (m_GlobalEventDelegates.find(selector) == m_GlobalEventDelegates.end()) {
            m_GlobalEventDelegates[selector] = std::vector<std::string>();
        }
        m_GlobalEventDelegates[selector].push_back(eventType + "::" + handler);
    }
    else if (m_CurrentContext) {
        m_CurrentContext->EventHandlers[key] = handler;
    }
}

void CHTLJSContextManager::RegisterEventDelegate(const std::string& parentSelector, 
                                                const std::vector<std::string>& targetSelectors,
                                                const std::string& eventType, 
                                                const std::string& handler) {
    // 注册事件委托到全局注册表
    if (m_GlobalEventDelegates.find(parentSelector) == m_GlobalEventDelegates.end()) {
        m_GlobalEventDelegates[parentSelector] = std::vector<std::string>();
    }
    
    for (const auto& target : targetSelectors) {
        std::string delegateInfo = target + "::" + eventType + "::" + handler;
        m_GlobalEventDelegates[parentSelector].push_back(delegateInfo);
    }
    
    if (m_CurrentContext) {
        m_CurrentContext->CurrentDelegate = parentSelector;
    }
}

std::string CHTLJSContextManager::GetEventDelegateCode(const std::string& parentSelector) {
    auto it = m_GlobalEventDelegates.find(parentSelector);
    if (it == m_GlobalEventDelegates.end()) {
        return "";
    }
    
    std::ostringstream code;
    std::string parentDOMCode = ResolveEnhancedSelector(parentSelector);
    
    code << "(function() {\n";
    code << "  var parentElement = " << parentDOMCode << ";\n";
    code << "  if (parentElement) {\n";
    
    // 按事件类型分组
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> eventGroups;
    
    for (const auto& delegateInfo : it->second) {
        std::stringstream ss(delegateInfo);
        std::string target, eventType, handler;
        std::getline(ss, target, ':');
        std::getline(ss, eventType, ':');
        std::getline(ss, handler);
        
        eventGroups[eventType].push_back({target, handler});
    }
    
    // 为每种事件类型生成委托代码
    for (const auto& eventGroup : eventGroups) {
        code << "    parentElement.addEventListener('" << eventGroup.first << "', function(e) {\n";
        
        for (const auto& targetHandler : eventGroup.second) {
            std::string target = targetHandler.first;
            std::string handler = targetHandler.second;
            
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
            code << "        (" << handler << ")(e);\n";
            code << "      }\n";
        }
        
        code << "    });\n";
    }
    
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

void CHTLJSContextManager::RegisterAnimation(const std::string& name, 
                                           const std::vector<std::string>& targets,
                                           const std::unordered_map<std::string, std::string>& properties, 
                                           bool isGlobal) {
    std::string animName = name.empty() ? GenerateUniqueName("chtljs_anim", m_AnimationCounter) : name;
    
    if (isGlobal) {
        // 序列化动画信息到全局映射
        std::ostringstream oss;
        oss << "targets:[";
        for (size_t i = 0; i < targets.size(); ++i) {
            oss << targets[i];
            if (i < targets.size() - 1) oss << ",";
        }
        oss << "];properties:{";
        for (const auto& prop : properties) {
            oss << prop.first << ":" << prop.second << ";";
        }
        oss << "}";
        
        m_GlobalAnimations[animName] = oss.str();
    }
    else if (m_CurrentContext) {
        // 存储到当前上下文
        for (const auto& prop : properties) {
            m_CurrentContext->AnimationStates[prop.first] = prop.second;
        }
        m_CurrentContext->CurrentAnimation = animName;
    }
}

std::string CHTLJSContextManager::GenerateAnimationCode(const std::string& animationName) {
    // 从全局动画或当前上下文生成动画代码
    auto globalIt = m_GlobalAnimations.find(animationName);
    if (globalIt != m_GlobalAnimations.end()) {
        // 解析全局动画信息并生成代码
        return "/* Animation: " + animationName + " - " + globalIt->second + " */";
    }
    
    if (m_CurrentContext && m_CurrentContext->CurrentAnimation == animationName) {
        std::ostringstream code;
        code << "/* Current Animation: " << animationName << " */\n";
        code << "requestAnimationFrame(function() {\n";
        
        for (const auto& state : m_CurrentContext->AnimationStates) {
            code << "  // " << state.first << ": " << state.second << "\n";
        }
        
        code << "});\n";
        return code.str();
    }
    
    return "";
}

void CHTLJSContextManager::RegisterModulePath(const std::string& path, bool isGlobal) {
    if (isGlobal) {
        if (m_GlobalModules.find("global") == m_GlobalModules.end()) {
            m_GlobalModules["global"] = std::vector<std::string>();
        }
        m_GlobalModules["global"].push_back(path);
    }
    else if (m_CurrentContext) {
        m_CurrentContext->ModulePaths.push_back(path);
    }
}

std::vector<std::string> CHTLJSContextManager::GetAllModulePaths() {
    std::vector<std::string> allPaths;
    
    // 添加全局模块路径
    auto globalIt = m_GlobalModules.find("global");
    if (globalIt != m_GlobalModules.end()) {
        allPaths.insert(allPaths.end(), globalIt->second.begin(), globalIt->second.end());
    }
    
    // 添加当前上下文模块路径
    if (m_CurrentContext) {
        allPaths.insert(allPaths.end(), 
                       m_CurrentContext->ModulePaths.begin(), 
                       m_CurrentContext->ModulePaths.end());
    }
    
    return allPaths;
}

std::string CHTLJSContextManager::GenerateModuleLoadCode() {
    auto allPaths = GetAllModulePaths();
    if (allPaths.empty()) {
        return "";
    }
    
    std::ostringstream code;
    
    // 生成AMD风格的模块加载代码
    code << "(function() {\n";
    code << "  var loadedModules = {};\n";
    code << "  var modulePaths = [\n";
    
    for (size_t i = 0; i < allPaths.size(); ++i) {
        code << "    '" << allPaths[i] << "'";
        if (i < allPaths.size() - 1) {
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

std::string CHTLJSContextManager::GenerateListenCode(const std::string& selector, 
                                                     const std::unordered_map<std::string, std::string>& events) {
    std::ostringstream code;
    std::string domElement = ResolveEnhancedSelector(selector);
    
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

std::string CHTLJSContextManager::GenerateEventBindCode(const std::string& selector, 
                                                       const std::string& eventType, 
                                                       const std::string& handler) {
    std::ostringstream code;
    std::string domElement = ResolveEnhancedSelector(selector);
    
    code << "(function() {\n";
    code << "  var element = " << domElement << ";\n";
    code << "  if (element) {\n";
    code << "    element.addEventListener('" << eventType << "', " << handler << ");\n";
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

std::vector<std::string> CHTLJSContextManager::BuildCHTLJSScopeChain() {
    std::vector<std::string> chain;
    
    // 添加当前CHTL JS上下文
    if (m_CurrentContext) {
        chain.push_back(m_CurrentContext->Name);
    }
    
    // 添加父CHTL JS上下文
    std::stack<CHTLJSContextInfo*> tempStack = m_ContextStack;
    std::vector<std::string> parentChain;
    
    while (!tempStack.empty()) {
        parentChain.push_back(tempStack.top()->Name);
        tempStack.pop();
    }
    
    // 反转父链（从根到当前）
    std::reverse(parentChain.begin(), parentChain.end());
    chain.insert(chain.begin(), parentChain.begin(), parentChain.end());
    
    return chain;
}

std::string CHTLJSContextManager::GetFullCHTLJSContextPath() {
    auto scopeChain = BuildCHTLJSScopeChain();
    std::string path;
    
    for (size_t i = 0; i < scopeChain.size(); ++i) {
        if (i > 0) {
            path += ".";
        }
        path += scopeChain[i];
    }
    
    return path;
}

bool CHTLJSContextManager::IsAllowedCHTLJSSyntax(const std::string& syntaxType) {
    CHTLJSContextType currentContext = GetCurrentContextType();
    
    // CHTL JS语法在相应的上下文中都是允许的
    switch (currentContext) {
        case CHTLJSContextType::GLOBAL_CONTEXT:
            // 全局上下文允许所有CHTL JS语法
            return true;
            
        case CHTLJSContextType::MODULE_CONTEXT:
            // 模块上下文只允许load语法
            return syntaxType == "LOAD";
            
        case CHTLJSContextType::LISTEN_CONTEXT:
            // listen上下文允许事件映射
            return syntaxType == "EVENT_MAPPING";
            
        case CHTLJSContextType::DELEGATE_CONTEXT:
            // delegate上下文允许target和事件映射
            return syntaxType == "TARGET" || syntaxType == "EVENT_MAPPING";
            
        case CHTLJSContextType::ANIMATE_CONTEXT:
            // animate上下文允许所有动画属性
            return true;
            
        case CHTLJSContextType::VIR_CONTEXT:
            // vir上下文允许键值对定义
            return syntaxType == "KEY_VALUE_PAIR";
            
        case CHTLJSContextType::SELECTOR_CONTEXT:
            // 选择器上下文允许->操作
            return syntaxType == "ARROW_OPERATION";
            
        default:
            return true;
    }
}

bool CHTLJSContextManager::ValidateEnhancedSelector(const std::string& selector) {
    // 验证增强选择器格式
    std::regex selectorPattern(R"(\{\{[^}]+\}\})");
    return std::regex_match(selector, selectorPattern);
}

bool CHTLJSContextManager::ValidateCHTLJSFunction(const std::string& functionContent) {
    // 验证CHTL JS函数格式
    std::regex functionPatterns[] = {
        std::regex(R"(listen\s*\{[^}]*\})"),
        std::regex(R"(delegate\s*\{[^}]*\})"),
        std::regex(R"(animate\s*\{[^}]*\})"),
        std::regex(R"(module\s*\{[^}]*\})")
    };
    
    for (const auto& pattern : functionPatterns) {
        if (std::regex_search(functionContent, pattern)) {
            return true;
        }
    }
    
    return false;
}

std::string CHTLJSContextManager::GetCHTLJSContextStatistics() const {
    std::ostringstream oss;
    oss << "CHTL JS Context Statistics:\n";
    oss << "Context Stack Depth: " << m_ContextStack.size() << "\n";
    oss << "Global Virtual Objects: " << m_GlobalVirtualObjects.size() << "\n";
    oss << "Global Event Delegates: " << m_GlobalEventDelegates.size() << "\n";
    oss << "Global Animations: " << m_GlobalAnimations.size() << "\n";
    oss << "Global Modules: " << m_GlobalModules.size() << "\n";
    oss << "Selector Cache: " << m_SelectorCache.size() << "\n";
    
    if (m_CurrentContext) {
        oss << "Current Context: " << m_CurrentContext->Name << "\n";
        oss << "Current Virtual Objects: " << m_CurrentContext->VirtualObjects.size() << "\n";
        oss << "Current Selector Mappings: " << m_CurrentContext->SelectorMappings.size() << "\n";
        oss << "Current Event Handlers: " << m_CurrentContext->EventHandlers.size() << "\n";
        oss << "Current Animation States: " << m_CurrentContext->AnimationStates.size() << "\n";
        oss << "Current Module Paths: " << m_CurrentContext->ModulePaths.size() << "\n";
    }
    
    return oss.str();
}

std::string CHTLJSContextManager::PrintCHTLJSContextStack() const {
    std::ostringstream oss;
    oss << "CHTL JS Context Stack (top to bottom):\n";
    
    if (m_CurrentContext) {
        oss << "  [Current] " << m_CurrentContext->Name 
            << " (" << static_cast<int>(m_CurrentContext->Type) << ")\n";
    }
    
    std::stack<CHTLJSContextInfo*> tempStack = m_ContextStack;
    int level = 0;
    
    while (!tempStack.empty()) {
        auto context = tempStack.top();
        oss << "  [" << level << "] " << context->Name 
            << " (" << static_cast<int>(context->Type) << ")\n";
        tempStack.pop();
        level++;
    }
    
    return oss.str();
}

void CHTLJSContextManager::Reset() {
    while (!m_ContextStack.empty()) {
        m_ContextStack.pop();
    }
    
    m_GlobalVirtualObjects.clear();
    m_GlobalEventDelegates.clear();
    m_GlobalAnimations.clear();
    m_GlobalModules.clear();
    m_SelectorCache.clear();
    
    m_VirObjectCounter = 0;
    m_AnimationCounter = 0;
    m_DelegateCounter = 0;
    
    CreateDefaultCHTLJSGlobalContext();
}

bool CHTLJSContextManager::IsValidCHTLJSContextTransition(CHTLJSContextType fromType, CHTLJSContextType toType) {
    // 定义CHTL JS有效的上下文转换规则
    switch (fromType) {
        case CHTLJSContextType::GLOBAL_CONTEXT:
            return true; // 全局上下文可以转换到任何CHTL JS上下文
            
        case CHTLJSContextType::MODULE_CONTEXT:
            return toType == CHTLJSContextType::GLOBAL_CONTEXT;
            
        case CHTLJSContextType::LISTEN_CONTEXT:
        case CHTLJSContextType::DELEGATE_CONTEXT:
        case CHTLJSContextType::ANIMATE_CONTEXT:
            return toType == CHTLJSContextType::GLOBAL_CONTEXT ||
                   toType == CHTLJSContextType::EVENT_CONTEXT;
                   
        case CHTLJSContextType::VIR_CONTEXT:
            return toType == CHTLJSContextType::LISTEN_CONTEXT ||
                   toType == CHTLJSContextType::DELEGATE_CONTEXT ||
                   toType == CHTLJSContextType::ANIMATE_CONTEXT ||
                   toType == CHTLJSContextType::GLOBAL_CONTEXT;
                   
        case CHTLJSContextType::SELECTOR_CONTEXT:
            return toType == CHTLJSContextType::EVENT_CONTEXT ||
                   toType == CHTLJSContextType::GLOBAL_CONTEXT;
                   
        default:
            return false;
    }
}

void CHTLJSContextManager::InheritFromParentCHTLJSContext(CHTLJSContextInfo* parentContext, 
                                                         CHTLJSContextInfo* childContext) {
    if (!parentContext || !childContext) {
        return;
    }
    
    // 继承父CHTL JS上下文的虚对象
    for (const auto& virObj : parentContext->VirtualObjects) {
        if (childContext->VirtualObjects.find(virObj.first) == childContext->VirtualObjects.end()) {
            childContext->VirtualObjects[virObj.first] = virObj.second;
        }
    }
    
    // 继承选择器映射
    for (const auto& selector : parentContext->SelectorMappings) {
        if (childContext->SelectorMappings.find(selector.first) == childContext->SelectorMappings.end()) {
            childContext->SelectorMappings[selector.first] = selector.second;
        }
    }
}

void CHTLJSContextManager::ApplyCHTLJSScopeRules(CHTLJSContextInfo* context) {
    if (!context) {
        return;
    }
    
    // 根据CHTL JS作用域类型应用特定规则
    switch (context->Scope) {
        case CHTLJSScopeType::MODULE_SCOPE:
            // 模块作用域只允许load语法
            break;
            
        case CHTLJSScopeType::FUNCTION_SCOPE:
            // CHTL JS函数作用域允许所有CHTL JS语法
            break;
            
        case CHTLJSScopeType::VIR_SCOPE:
            // 虚对象作用域允许键值对定义
            break;
            
        case CHTLJSScopeType::EVENT_SCOPE:
            // 事件作用域允许事件处理相关语法
            break;
            
        default:
            break;
    }
}

std::unordered_map<std::string, std::string> CHTLJSContextManager::ParseCHTLJSFunctionKeys(const std::string& functionContent) {
    std::unordered_map<std::string, std::string> keys;
    
    // 解析CHTL JS函数中的键值对
    // 例如：listen { click: () => {}, mouseenter: handler }
    std::regex keyValuePattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(functionContent.begin(), functionContent.end(), keyValuePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // 清理值中的多余空白
        value.erase(std::remove_if(value.begin(), value.end(), 
                   [](char c) { return std::isspace(c) && c != ' '; }), value.end());
        
        keys[key] = value;
    }
    
    return keys;
}

std::string CHTLJSContextManager::GenerateUniqueName(const std::string& prefix, int& counter) {
    return prefix + "_" + std::to_string(++counter);
}

// RAII CHTL JS上下文管理器实现
CHTLJSContextGuard::CHTLJSContextGuard(CHTLJSContextManager* contextManager, 
                                      CHTLJSContextType type, 
                                      CHTLJSScopeType scope, 
                                      const std::string& name)
    : m_ContextManager(contextManager), m_ShouldExit(false) {
    
    if (m_ContextManager && m_ContextManager->EnterContext(type, scope, name)) {
        m_ShouldExit = true;
    }
}

CHTLJSContextGuard::~CHTLJSContextGuard() {
    if (m_ShouldExit && m_ContextManager) {
        m_ContextManager->ExitContext();
    }
}

CHTLJSContextGuard::CHTLJSContextGuard(CHTLJSContextGuard&& other) noexcept
    : m_ContextManager(other.m_ContextManager), m_ShouldExit(other.m_ShouldExit) {
    other.m_ShouldExit = false;
}

CHTLJSContextGuard& CHTLJSContextGuard::operator=(CHTLJSContextGuard&& other) noexcept {
    if (this != &other) {
        if (m_ShouldExit && m_ContextManager) {
            m_ContextManager->ExitContext();
        }
        
        m_ContextManager = other.m_ContextManager;
        m_ShouldExit = other.m_ShouldExit;
        other.m_ShouldExit = false;
    }
    return *this;
}

void CHTLJSContextGuard::Release() {
    m_ShouldExit = false;
}

} // namespace CHTLJS