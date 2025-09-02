#include "CHTLJSGenerator.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTLJS {

CHTLJSGenerator::CHTLJSGenerator() 
    : m_HasError(false), m_GenerateSourceMaps(false), m_MinifyJavaScript(false), 
      m_AddCHTLJSComments(true), m_OptimizeSelectors(true), m_MergeEventDelegates(true) {
}

CHTLJSGenerationResult CHTLJSGenerator::Generate(std::unique_ptr<CHTLJSBaseNode> rootNode) {
    CHTLJSGenerationResult result;
    
    if (!rootNode) {
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS root node is null";
        return result;
    }
    
    m_RootNode = std::move(rootNode);
    Reset();
    
    try {
        // CHTL JS预处理阶段：收集CHTL JS定义
        CollectCHTLJSDefinitions(m_RootNode.get());
        
        // 处理虚对象解析
        ProcessVirtualObjectResolution();
        
        // 构建事件委托全局注册表
        BuildEventDelegateRegistry();
        
        // 优化增强选择器查询
        OptimizeEnhancedSelectorQueries();
        
        // 生成CHTL JS JavaScript代码
        GenerateCHTLJSJavaScript(m_RootNode.get());
        
        // 验证生成的CHTL JS代码
        if (!ValidateCHTLJSGeneratedCode()) {
            result.IsSuccess = false;
            result.ErrorMessage = "CHTL JS generated code validation failed";
            return result;
        }
        
        // 优化生成的代码
        if (m_OptimizeSelectors) {
            ProcessSelectorCacheOptimization();
        }
        
        if (m_MergeEventDelegates) {
            MergeRedundantEventDelegates();
        }
        
        if (m_MinifyJavaScript) {
            OptimizeCHTLJSGeneratedCode();
        }
        
        // 组装最终CHTL JS结果
        result.JavaScriptContent = m_GeneratedJavaScript;
        result.ModuleLoadCode = m_ModuleLoadCode;
        result.VirtualObjectCode = m_VirtualObjectCode;
        result.EventDelegateCode = m_EventDelegateCode;
        result.AnimationCode = m_AnimationCode;
        result.EnhancedSelectorCode = m_EnhancedSelectorCode;
        result.IsSuccess = true;
        result.Warnings = m_Warnings;
        
    }
    catch (const std::exception& e) {
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS generation exception: " + std::string(e.what());
    }
    
    return result;
}

void CHTLJSGenerator::CollectCHTLJSDefinitions(CHTLJSBaseNode* node) {
    if (!node) {
        return;
    }
    
    // 收集CHTL JS特有的定义
    switch (node->GetType()) {
        case CHTLJSNodeType::VIR_DECLARATION_NODE:
            {
                VirObjectNode* virNode = static_cast<VirObjectNode*>(node);
                m_VirtualObjects[virNode->GetVirName()] = virNode;
            }
            break;
            
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE:
            {
                EnhancedSelectorNode* selectorNode = static_cast<EnhancedSelectorNode*>(node);
                m_EnhancedSelectors[selectorNode->GetOriginalSelector()] = selectorNode;
            }
            break;
            
        case CHTLJSNodeType::ANIMATE_NODE:
            {
                AnimateNode* animateNode = static_cast<AnimateNode*>(node);
                m_Animations[animateNode->GetAnimationName()] = animateNode;
            }
            break;
            
        case CHTLJSNodeType::LOAD_NODE:
            {
                m_ModulePaths.push_back(node->GetContent());
            }
            break;
            
        default:
            break;
    }
    
    // 递归处理子节点
    for (const auto& child : node->GetChildren()) {
        CollectCHTLJSDefinitions(child.get());
    }
}

void CHTLJSGenerator::ProcessVirtualObjectResolution() {
    // 处理虚对象解析
    for (auto& virPair : m_VirtualObjects) {
        VirObjectNode* virNode = virPair.second;
        
        if (!virNode->IsResolved()) {
            // 解析虚对象的CHTL JS函数内容
            const std::string& functionContent = virNode->GetCHTLJSFunctionContent();
            virNode->ParseCHTLJSFunction(functionContent);
        }
    }
}

void CHTLJSGenerator::BuildEventDelegateRegistry() {
    // 构建事件委托全局注册表，避免创建多个相同的事件委托
    
    if (m_AddCHTLJSComments) {
        m_EventDelegateCode += AddCHTLJSSourceComment("CHTL JS Event Delegate Global Registry");
    }
    
    m_EventDelegateCode += "(function() {\n";
    m_EventDelegateCode += "  var chtljs_delegate_registry = {};\n";
    m_EventDelegateCode += "  \n";
    m_EventDelegateCode += "  window.CHTLJSEventDelegate = {\n";
    m_EventDelegateCode += "    register: function(parent, targets, eventType, handler) {\n";
    m_EventDelegateCode += "      var key = parent + '::' + eventType;\n";
    m_EventDelegateCode += "      if (!chtljs_delegate_registry[key]) {\n";
    m_EventDelegateCode += "        chtljs_delegate_registry[key] = {\n";
    m_EventDelegateCode += "          parent: parent,\n";
    m_EventDelegateCode += "          targets: [],\n";
    m_EventDelegateCode += "          handlers: []\n";
    m_EventDelegateCode += "        };\n";
    m_EventDelegateCode += "      }\n";
    m_EventDelegateCode += "      \n";
    m_EventDelegateCode += "      var registry = chtljs_delegate_registry[key];\n";
    m_EventDelegateCode += "      registry.targets = registry.targets.concat(targets);\n";
    m_EventDelegateCode += "      registry.handlers.push(handler);\n";
    m_EventDelegateCode += "    },\n";
    m_EventDelegateCode += "    \n";
    m_EventDelegateCode += "    activate: function() {\n";
    m_EventDelegateCode += "      for (var key in chtljs_delegate_registry) {\n";
    m_EventDelegateCode += "        var reg = chtljs_delegate_registry[key];\n";
    m_EventDelegateCode += "        var parentElement = document.querySelector(reg.parent);\n";
    m_EventDelegateCode += "        if (parentElement) {\n";
    m_EventDelegateCode += "          parentElement.addEventListener(key.split('::')[1], function(e) {\n";
    m_EventDelegateCode += "            for (var i = 0; i < reg.targets.length; i++) {\n";
    m_EventDelegateCode += "              if (e.target.matches(reg.targets[i])) {\n";
    m_EventDelegateCode += "                for (var j = 0; j < reg.handlers.length; j++) {\n";
    m_EventDelegateCode += "                  reg.handlers[j](e);\n";
    m_EventDelegateCode += "                }\n";
    m_EventDelegateCode += "              }\n";
    m_EventDelegateCode += "            }\n";
    m_EventDelegateCode += "          });\n";
    m_EventDelegateCode += "        }\n";
    m_EventDelegateCode += "      }\n";
    m_EventDelegateCode += "    }\n";
    m_EventDelegateCode += "  };\n";
    m_EventDelegateCode += "})();\n\n";
}

void CHTLJSGenerator::OptimizeEnhancedSelectorQueries() {
    // 优化增强选择器查询，缓存常用选择器
    
    if (m_AddCHTLJSComments) {
        m_EnhancedSelectorCode += AddCHTLJSSourceComment("CHTL JS Enhanced Selector Query Cache");
    }
    
    m_EnhancedSelectorCode += "(function() {\n";
    m_EnhancedSelectorCode += "  var chtljs_selector_cache = {};\n";
    m_EnhancedSelectorCode += "  \n";
    m_EnhancedSelectorCode += "  window.CHTLJSSelector = {\n";
    m_EnhancedSelectorCode += "    query: function(selector) {\n";
    m_EnhancedSelectorCode += "      if (chtljs_selector_cache[selector]) {\n";
    m_EnhancedSelectorCode += "        return chtljs_selector_cache[selector];\n";
    m_EnhancedSelectorCode += "      }\n";
    m_EnhancedSelectorCode += "      \n";
    m_EnhancedSelectorCode += "      var element = null;\n";
    m_EnhancedSelectorCode += "      \n";
    
    // 生成选择器优先级查找逻辑
    for (const auto& selectorPair : m_EnhancedSelectors) {
        EnhancedSelectorNode* selectorNode = selectorPair.second;
        std::string optimizedQuery = GenerateOptimizedDOMQuery(selectorNode->GetSelectorContent());
        
        m_EnhancedSelectorCode += "      // Selector: " + selectorNode->GetOriginalSelector() + "\n";
        m_EnhancedSelectorCode += "      if (selector === '" + selectorNode->GetOriginalSelector() + "') {\n";
        m_EnhancedSelectorCode += "        element = " + optimizedQuery + ";\n";
        m_EnhancedSelectorCode += "      }\n";
    }
    
    m_EnhancedSelectorCode += "      \n";
    m_EnhancedSelectorCode += "      if (element) {\n";
    m_EnhancedSelectorCode += "        chtljs_selector_cache[selector] = element;\n";
    m_EnhancedSelectorCode += "      }\n";
    m_EnhancedSelectorCode += "      \n";
    m_EnhancedSelectorCode += "      return element;\n";
    m_EnhancedSelectorCode += "    }\n";
    m_EnhancedSelectorCode += "  };\n";
    m_EnhancedSelectorCode += "})();\n\n";
}

void CHTLJSGenerator::GenerateCHTLJSJavaScript(CHTLJSBaseNode* node) {
    if (!node) {
        return;
    }
    
    // 根据节点类型生成相应的JavaScript代码
    switch (node->GetType()) {
        case CHTLJSNodeType::MODULE_NODE:
            GenerateModuleLoadJavaScript(m_ModulePaths);
            break;
            
        case CHTLJSNodeType::VIR_DECLARATION_NODE:
            {
                VirObjectNode* virNode = static_cast<VirObjectNode*>(node);
                GenerateVirtualObjectJavaScript(virNode);
            }
            break;
            
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE:
            {
                EnhancedSelectorNode* selectorNode = static_cast<EnhancedSelectorNode*>(node);
                GenerateEnhancedSelectorJavaScript(selectorNode);
            }
            break;
            
        case CHTLJSNodeType::ANIMATE_NODE:
            {
                AnimateNode* animateNode = static_cast<AnimateNode*>(node);
                GenerateAnimationJavaScript(animateNode);
            }
            break;
            
        default:
            break;
    }
    
    // 递归处理子节点
    for (const auto& child : node->GetChildren()) {
        GenerateCHTLJSJavaScript(child.get());
    }
}

void CHTLJSGenerator::GenerateModuleLoadJavaScript(const std::vector<std::string>& modulePaths) {
    if (modulePaths.empty()) {
        return;
    }
    
    if (m_AddCHTLJSComments) {
        m_ModuleLoadCode += AddCHTLJSSourceComment("CHTL JS AMD Module Loader");
    }
    
    m_ModuleLoadCode += GenerateAMDModuleLoader(modulePaths);
}

std::string CHTLJSGenerator::GenerateAMDModuleLoader(const std::vector<std::string>& paths) {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  // CHTL JS AMD-style Module Loader\n";
    code << "  var loadedModules = {};\n";
    code << "  var loadQueue = [];\n";
    code << "  var modulePaths = [\n";
    
    for (size_t i = 0; i < paths.size(); ++i) {
        code << "    '" << paths[i] << "'";
        if (i < paths.size() - 1) {
            code << ",";
        }
        code << "\n";
    }
    
    code << "  ];\n";
    code << "  \n";
    code << "  function loadModule(path, callback) {\n";
    code << "    if (loadedModules[path]) {\n";
    code << "      if (callback) callback();\n";
    code << "      return;\n";
    code << "    }\n";
    code << "    \n";
    code << "    var script = document.createElement('script');\n";
    code << "    script.src = path;\n";
    code << "    script.onload = function() {\n";
    code << "      loadedModules[path] = true;\n";
    code << "      console.log('CHTL JS module loaded:', path);\n";
    code << "      if (callback) callback();\n";
    code << "    };\n";
    code << "    script.onerror = function() {\n";
    code << "      console.error('Failed to load CHTL JS module:', path);\n";
    code << "    };\n";
    code << "    document.head.appendChild(script);\n";
    code << "  }\n";
    code << "  \n";
    code << "  function loadNext(index) {\n";
    code << "    if (index >= modulePaths.length) {\n";
    code << "      console.log('All CHTL JS modules loaded');\n";
    code << "      return;\n";
    code << "    }\n";
    code << "    \n";
    code << "    loadModule(modulePaths[index], function() {\n";
    code << "      loadNext(index + 1);\n";
    code << "    });\n";
    code << "  }\n";
    code << "  \n";
    code << "  // Start loading modules\n";
    code << "  loadNext(0);\n";
    code << "})();\n\n";
    
    return code.str();
}

void CHTLJSGenerator::GenerateEnhancedSelectorJavaScript(EnhancedSelectorNode* selectorNode) {
    if (!selectorNode) {
        return;
    }
    
    std::string domQuery = selectorNode->GenerateDOMQuery();
    
    if (!domQuery.empty()) {
        if (m_AddCHTLJSComments) {
            m_EnhancedSelectorCode += AddCHTLJSSourceComment("Enhanced Selector: " + selectorNode->GetOriginalSelector());
        }
        
        m_EnhancedSelectorCode += "var element_" + std::to_string(selectorNode->GetLine()) + 
                                 "_" + std::to_string(selectorNode->GetColumn()) + " = " + domQuery + ";\n";
    }
}

void CHTLJSGenerator::GenerateVirtualObjectJavaScript(VirObjectNode* virNode) {
    if (!virNode) {
        return;
    }
    
    if (m_AddCHTLJSComments) {
        m_VirtualObjectCode += AddCHTLJSSourceComment("Virtual Object: " + virNode->GetVirName());
    }
    
    // 生成虚对象代码
    m_VirtualObjectCode += virNode->GenerateVirObjectCode();
    
    // 生成键引用映射
    m_VirtualObjectCode += GenerateVirKeyReferenceMap(virNode);
    
    // 生成函数定义
    m_VirtualObjectCode += GenerateVirFunctionDefinitions(virNode);
}

void CHTLJSGenerator::GenerateAnimationJavaScript(AnimateNode* animateNode) {
    if (!animateNode) {
        return;
    }
    
    if (m_AddCHTLJSComments) {
        m_AnimationCode += AddCHTLJSSourceComment("CHTL JS Animation: " + animateNode->GetAnimationName());
    }
    
    // 生成requestAnimationFrame封装的动画代码
    std::string animationLogic = animateNode->GenerateAnimationCode();
    m_AnimationCode += GenerateRequestAnimationFrameWrapper(animateNode->GetAnimationName(), animationLogic);
}

std::string CHTLJSGenerator::GenerateOptimizedDOMQuery(const std::string& selector) {
    // 生成优化的DOM查询代码
    
    if (selector.front() == '.') {
        // 类选择器
        return "document.querySelector('" + selector + "')";
    }
    else if (selector.front() == '#') {
        // ID选择器
        std::string idName = selector.substr(1);
        return "document.getElementById('" + idName + "')";
    }
    else if (selector.find(' ') != std::string::npos) {
        // 后代选择器
        return "document.querySelector('" + selector + "')";
    }
    else if (selector.find('[') != std::string::npos) {
        // 索引访问
        std::regex indexPattern(R"((\w+)\[(\d+)\])");
        std::smatch match;
        if (std::regex_match(selector, match, indexPattern)) {
            std::string tag = match[1].str();
            std::string index = match[2].str();
            return "document.getElementsByTagName('" + tag + "')[" + index + "]";
        }
    }
    else {
        // 自动检测选择器（优先级：id > class > tag）
        return GenerateSelectorPriorityCode(selector);
    }
    
    return "document.querySelector('" + selector + "')";
}

std::string CHTLJSGenerator::GenerateSelectorPriorityCode(const std::string& content) {
    std::ostringstream code;
    
    // 生成优先级查找代码：先判断是否为tag，然后查找类名/id（id优先）
    code << "(function() { ";
    code << "var elem = document.getElementById('" << content << "'); ";
    code << "if (elem) return elem; ";
    code << "elem = document.querySelector('." << content << "'); ";
    code << "if (elem) return elem; ";
    code << "return document.getElementsByTagName('" << content << "'); ";
    code << "})()";
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateVirKeyReferenceMap(VirObjectNode* virNode) {
    if (!virNode) {
        return "";
    }
    
    std::ostringstream code;
    
    code << "// Virtual Object Key Reference Map: " << virNode->GetVirName() << "\n";
    code << "var " << virNode->GetVirName() << "_keyMap = {\n";
    
    const auto& virKeys = virNode->GetVirKeys();
    bool first = true;
    
    for (const auto& keyPair : virKeys) {
        const VirKey& key = keyPair.second;
        
        if (!first) {
            code << ",\n";
        }
        
        code << "  " << key.Name << ": ";
        
        switch (key.Type) {
            case VirKeyType::FUNCTION_KEY:
                code << "function() { return " << virNode->GetVirName() << "_" << key.Name << "; }";
                break;
            case VirKeyType::OBJECT_KEY:
            case VirKeyType::ARRAY_KEY:
                code << key.Value;
                break;
            case VirKeyType::VALUE_KEY:
                code << "'" << key.Value << "'";
                break;
        }
        
        first = false;
    }
    
    code << "\n};\n\n";
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateVirFunctionDefinitions(VirObjectNode* virNode) {
    if (!virNode) {
        return "";
    }
    
    std::ostringstream code;
    
    const auto& virKeys = virNode->GetVirKeys();
    
    for (const auto& keyPair : virKeys) {
        const VirKey& key = keyPair.second;
        
        if (key.Type == VirKeyType::FUNCTION_KEY) {
            code << "// Virtual function: " << virNode->GetVirName() << "->" << key.Name << "\n";
            code << "function " << virNode->GetVirName() << "_" << key.Name << key.FunctionSignature;
            code << " " << key.Value << "\n\n";
        }
    }
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateRequestAnimationFrameWrapper(const std::string& animationName,
                                                                 const std::string& animationLogic) {
    std::ostringstream code;
    
    code << "// CHTL JS Animation: " << animationName << "\n";
    code << "(function() {\n";
    code << "  function " << animationName << "() {\n";
    code << animationLogic;
    code << "  }\n";
    code << "  \n";
    code << "  // Start animation\n";
    code << "  " << animationName << "();\n";
    code << "})();\n\n";
    
    return code.str();
}

std::string CHTLJSGenerator::AddCHTLJSSourceComment(const std::string& comment) {
    if (!m_AddCHTLJSComments) {
        return "";
    }
    
    return "// " + comment + "\n";
}

void CHTLJSGenerator::SetCHTLJSGenerationOptions(bool sourceMaps, bool minifyJS, 
                                                 bool chtljsComments, bool optimizeSelectors, 
                                                 bool mergeEventDelegates) {
    m_GenerateSourceMaps = sourceMaps;
    m_MinifyJavaScript = minifyJS;
    m_AddCHTLJSComments = chtljsComments;
    m_OptimizeSelectors = optimizeSelectors;
    m_MergeEventDelegates = mergeEventDelegates;
}

void CHTLJSGenerator::Reset() {
    m_GeneratedJavaScript.clear();
    m_ModuleLoadCode.clear();
    m_VirtualObjectCode.clear();
    m_EventDelegateCode.clear();
    m_AnimationCode.clear();
    m_EnhancedSelectorCode.clear();
    
    m_VirtualObjects.clear();
    m_EnhancedSelectors.clear();
    m_Animations.clear();
    m_ModulePaths.clear();
    m_EventDelegateRegistry.clear();
    m_SelectorQueryCache.clear();
    
    m_HasError = false;
    m_ErrorMessage.clear();
    m_Warnings.clear();
}

// 占位实现（需要后续完善）
std::string CHTLJSGenerator::GenerateListenJavaScript(const std::string& selector, 
                                                     const std::unordered_map<std::string, std::string>& events) {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  var element = CHTLJSSelector.query('" << selector << "');\n";
    code << "  if (element) {\n";
    
    for (const auto& event : events) {
        code << "    element.addEventListener('" << event.first << "', " << event.second << ");\n";
    }
    
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateDelegateJavaScript(const std::string& parentSelector,
                                                       const std::vector<std::string>& targetSelectors,
                                                       const std::unordered_map<std::string, std::string>& events) {
    std::ostringstream code;
    
    code << "// Event Delegate: " << parentSelector << "\n";
    
    for (const auto& event : events) {
        code << "CHTLJSEventDelegate.register('" << parentSelector << "', [";
        
        for (size_t i = 0; i < targetSelectors.size(); ++i) {
            code << "'" << targetSelectors[i] << "'";
            if (i < targetSelectors.size() - 1) {
                code << ", ";
            }
        }
        
        code << "], '" << event.first << "', " << event.second << ");\n";
    }
    
    return code.str();
}

std::string CHTLJSGenerator::GenerateEventBindJavaScript(const std::string& selector,
                                                        const std::string& eventType,
                                                        const std::string& handler) {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  var element = CHTLJSSelector.query('" << selector << "');\n";
    code << "  if (element) {\n";
    code << "    element.addEventListener('" << eventType << "', " << handler << ");\n";
    code << "  }\n";
    code << "})();\n";
    
    return code.str();
}

void CHTLJSGenerator::OptimizeCHTLJSGeneratedCode() {
    if (m_MinifyJavaScript) {
        m_GeneratedJavaScript = MinifyJavaScriptCode(m_GeneratedJavaScript);
        m_ModuleLoadCode = MinifyJavaScriptCode(m_ModuleLoadCode);
        m_VirtualObjectCode = MinifyJavaScriptCode(m_VirtualObjectCode);
        m_EventDelegateCode = MinifyJavaScriptCode(m_EventDelegateCode);
        m_AnimationCode = MinifyJavaScriptCode(m_AnimationCode);
        m_EnhancedSelectorCode = MinifyJavaScriptCode(m_EnhancedSelectorCode);
    }
}

std::string CHTLJSGenerator::MinifyJavaScriptCode(const std::string& jsCode) {
    std::string result = jsCode;
    
    // 移除注释
    if (!m_AddCHTLJSComments) {
        std::regex commentPattern(R"(//.*?$)");
        result = std::regex_replace(result, commentPattern, "");
        
        std::regex multiCommentPattern(R"(/\*.*?\*/)");
        result = std::regex_replace(result, multiCommentPattern, "");
    }
    
    // 压缩空白
    std::regex whitespacePattern(R"(\s+)");
    result = std::regex_replace(result, whitespacePattern, " ");
    
    // 移除多余的换行
    std::regex newlinePattern(R"(\n\s*)");
    result = std::regex_replace(result, newlinePattern, "");
    
    return result;
}

bool CHTLJSGenerator::ValidateCHTLJSGeneratedCode() {
    // 验证生成的CHTL JS代码有效性
    return !m_GeneratedJavaScript.empty() || 
           !m_ModuleLoadCode.empty() || 
           !m_VirtualObjectCode.empty() || 
           !m_EventDelegateCode.empty() || 
           !m_AnimationCode.empty() || 
           !m_EnhancedSelectorCode.empty();
}

void CHTLJSGenerator::ProcessSelectorCacheOptimization() {
    // 处理选择器缓存优化
    // 将常用选择器缓存起来以提高性能
}

void CHTLJSGenerator::MergeRedundantEventDelegates() {
    // 合并重复的事件委托，避免创建多个相同的事件委托
}

void CHTLJSGenerator::SetCHTLJSGenerationError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = message;
}

void CHTLJSGenerator::AddCHTLJSGenerationWarning(const std::string& message) {
    m_Warnings.push_back(message);
}

std::string CHTLJSGenerator::GenerateErrorHandlingCode(const std::string& context) {
    return "console.error('CHTL JS Error in " + context + "');";
}

} // namespace CHTLJS