#include "CJMODIntegration.h"
#include <regex>
#include <sstream>

namespace CJMOD {

// CJMODCompilerIntegration实现
CJMODCompilerIntegration::CJMODCompilerIntegration() 
    : m_Scanner(nullptr), m_CHTLJSLexer(nullptr), m_CHTLJSParser(nullptr), m_IsProcessing(false) {
}

void CJMODCompilerIntegration::SetCompilerReferences(CHTL::CHTLUnifiedScanner* scanner,
                                                     CHTLJS::CHTLJSLexer* lexer,
                                                     CHTLJS::CHTLJSParser* parser) {
    m_Scanner = scanner;
    m_CHTLJSLexer = lexer;
    m_CHTLJSParser = parser;
}

void CJMODCompilerIntegration::RegisterExtension(const std::string& extensionName, 
                                                std::unique_ptr<CJMODExtension> extension) {
    if (extension && extension->Initialize()) {
        m_Extensions[extensionName] = std::move(extension);
    }
}

std::string CJMODCompilerIntegration::ProcessCodeFragment(const std::string& fragment, 
                                                         const std::string& fragmentType) {
    if (!RequiresCJMODProcessing(fragment)) {
        return fragment; // 不需要CJMOD处理，直接返回
    }
    
    // 分析代码片段中的CJMOD语法
    auto cjmodSyntaxList = AnalyzeCJMODSyntax(fragment);
    
    if (cjmodSyntaxList.empty()) {
        return fragment; // 没有CJMOD语法，直接返回
    }
    
    // 应用CJMOD扩展处理
    std::string processedFragment = ApplyCJMODExtensions(fragment, cjmodSyntaxList);
    
    return processedFragment;
}

bool CJMODCompilerIntegration::RequiresCJMODProcessing(const std::string& fragment) {
    // 检查是否包含CJMOD特殊语法
    std::vector<std::regex> cjmodPatterns = {
        std::regex(R"(printMylove\s*\{)"),      // printMylove语法
        std::regex(R"(iNeverAway\s*\{)"),       // iNeverAway语法
        std::regex(R"(util\s+.*\s*->\s*then)"), // util...then语法
        std::regex(R"(\$[?!_]?)"),              // 占位符语法
        std::regex(R"(\.\.\.)"),                // 可变参数语法
    };
    
    for (const auto& pattern : cjmodPatterns) {
        if (std::regex_search(fragment, pattern)) {
            return true;
        }
    }
    
    return false;
}

std::string CJMODCompilerIntegration::GetRealCodeFragment(size_t fragmentIndex) {
    if (!m_Scanner) {
        return "";
    }
    
    // 从统一扫描器获取真实的代码片段
    auto fragments = m_Scanner->GetFragments();
    
    if (fragmentIndex >= fragments.size()) {
        return "";
    }
    
    // 返回指定索引的代码片段内容
    return fragments[fragmentIndex].Content;
}

bool CJMODCompilerIntegration::ReturnProcessedFragment(size_t fragmentIndex, 
                                                      const std::string& processedFragment) {
    if (!m_Scanner) {
        return false;
    }
    
    // 这里需要修改扫描器的片段内容
    // 注意：这需要扫描器提供修改片段的接口
    return m_Scanner->UpdateFragmentContent(fragmentIndex, processedFragment);
}

std::string CJMODCompilerIntegration::ProcessCHTLJSExtensions(const std::string& chtljsCode) {
    std::string processedCode = chtljsCode;
    
    // 处理所有注册的扩展
    for (const auto& extensionPair : m_Extensions) {
        const auto& extension = extensionPair.second;
        
        // 检查扩展是否支持当前代码
        if (extension->SupportsSyntax(processedCode)) {
            std::unordered_map<std::string, std::string> context;
            context["fragmentType"] = "CHTLJS";
            context["extensionName"] = extensionPair.first;
            
            processedCode = extension->ProcessFragment(processedCode, context);
        }
    }
    
    return processedCode;
}

void CJMODCompilerIntegration::Reset() {
    m_IsProcessing = false;
    m_ProcessedFragments.clear();
}

std::vector<std::string> CJMODCompilerIntegration::AnalyzeCJMODSyntax(const std::string& fragment) {
    std::vector<std::string> syntaxList;
    
    // 查找printMylove语法
    std::regex printMylovePattern(R"(printMylove\s*\{[^}]*\})");
    std::sregex_iterator iter(fragment.begin(), fragment.end(), printMylovePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        syntaxList.push_back("printMylove");
    }
    
    // 查找iNeverAway语法
    std::regex iNeverAwayPattern(R"(iNeverAway\s*\{[^}]*\})");
    iter = std::sregex_iterator(fragment.begin(), fragment.end(), iNeverAwayPattern);
    
    for (; iter != end; ++iter) {
        syntaxList.push_back("iNeverAway");
    }
    
    // 查找util...then语法
    std::regex utilThenPattern(R"(util\s+.*\s*->\s*then\s*\{[^}]*\})");
    iter = std::sregex_iterator(fragment.begin(), fragment.end(), utilThenPattern);
    
    for (; iter != end; ++iter) {
        syntaxList.push_back("util...then");
    }
    
    return syntaxList;
}

std::string CJMODCompilerIntegration::ApplyCJMODExtensions(const std::string& fragment, 
                                                          const std::vector<std::string>& extensions) {
    std::string result = fragment;
    
    for (const auto& syntaxName : extensions) {
        // 查找支持该语法的扩展
        for (const auto& extensionPair : m_Extensions) {
            const auto& extension = extensionPair.second;
            
            if (extension->SupportsSyntax(syntaxName)) {
                std::unordered_map<std::string, std::string> context;
                context["syntax"] = syntaxName;
                context["originalFragment"] = fragment;
                
                result = extension->ProcessFragment(result, context);
                break; // 找到第一个支持的扩展就处理
            }
        }
    }
    
    return result;
}

// ChthollyExtension实现
std::string ChthollyExtension::ProcessFragment(const std::string& fragment, 
                                              const std::unordered_map<std::string, std::string>& context) {
    std::string result = fragment;
    
    auto syntaxIt = context.find("syntax");
    if (syntaxIt != context.end()) {
        const std::string& syntax = syntaxIt->second;
        
        if (syntax == "printMylove") {
            result = ProcessPrintMylove(fragment);
        }
        else if (syntax == "iNeverAway") {
            result = ProcessINeverAway(fragment);
        }
        else if (syntax == "util...then") {
            result = ProcessUtilThen(fragment);
        }
    }
    
    return result;
}

bool ChthollyExtension::SupportsSyntax(const std::string& syntax) {
    return std::find(m_SupportedSyntax.begin(), m_SupportedSyntax.end(), syntax) != m_SupportedSyntax.end() ||
           syntax.find("printMylove") != std::string::npos ||
           syntax.find("iNeverAway") != std::string::npos ||
           syntax.find("util") != std::string::npos;
}

std::unordered_map<std::string, std::string> ChthollyExtension::GetExtensionInfo() {
    return {
        {"name", "Chtholly"},
        {"version", "1.0.0"},
        {"description", "珂朵莉CJMOD扩展 - 世界上最幸福的女孩"},
        {"author", "CHTL Team"},
        {"features", "printMylove, iNeverAway, util...then"}
    };
}

bool ChthollyExtension::Initialize() {
    // 初始化珂朵莉扩展
    return true;
}

void ChthollyExtension::Cleanup() {
    // 清理资源
}

std::string ChthollyExtension::ProcessPrintMylove(const std::string& fragment) {
    // 将printMylove语法转换为JavaScript代码
    std::regex printMylovePattern(R"(printMylove\s*\{([^}]*)\})");
    
    return std::regex_replace(fragment, printMylovePattern, 
        R"((function() {
            const config = {$1};
            console.log('💖 珂朵莉永远在我心中');
            // 这里可以实现图片转字符像素的逻辑
            return 'Chtholly Love: ' + JSON.stringify(config);
        })())");
}

std::string ChthollyExtension::ProcessINeverAway(const std::string& fragment) {
    // 将iNeverAway语法转换为JavaScript代码
    std::regex iNeverAwayPattern(R"(iNeverAway\s*\{([^}]*)\})");
    
    return std::regex_replace(fragment, iNeverAwayPattern, 
        R"((function() {
            const functions = {$1};
            console.log('💫 我永远不会离开');
            // 创建全局函数注册表
            window.iNeverAwayFunctions = window.iNeverAwayFunctions || {};
            Object.assign(window.iNeverAwayFunctions, functions);
            return functions;
        })())");
}

std::string ChthollyExtension::ProcessUtilThen(const std::string& fragment) {
    // 将util...then语法转换为Promise链式调用
    std::regex utilThenPattern(R"(util\s+(.*?)\s*->\s*change\s*\{([^}]*)\}\s*->\s*then\s*\{([^}]*)\})");
    
    return std::regex_replace(fragment, utilThenPattern, 
        R"((function() {
            const condition = () => $1;
            const onChange = () => {$2};
            const onThen = () => {$3};
            
            let lastValue = condition();
            const checkChange = () => {
                const currentValue = condition();
                if (currentValue !== lastValue) {
                    lastValue = currentValue;
                    onChange();
                    if (currentValue) {
                        onThen();
                    }
                }
            };
            
            setInterval(checkChange, 100); // 检查变化
            return { condition, onChange, onThen };
        })())");
}

// CJMODManager实现
CJMODManager::CJMODManager() : m_IsInitialized(false) {
    m_Integration = std::make_unique<CJMODCompilerIntegration>();
}

bool CJMODManager::Initialize(CHTL::CHTLUnifiedScanner* scanner,
                             CHTLJS::CHTLJSLexer* lexer,
                             CHTLJS::CHTLJSParser* parser) {
    if (!scanner || !lexer || !parser) {
        return false;
    }
    
    // 设置编译器引用
    m_Integration->SetCompilerReferences(scanner, lexer, parser);
    
    // 注册内置扩展
    RegisterBuiltinExtensions();
    
    m_IsInitialized = true;
    return true;
}

bool CJMODManager::LoadExtension(const std::string& extensionPath) {
    // TODO: 实现动态加载CJMOD扩展
    // 这里需要加载.cjmod文件并实例化扩展
    return false;
}

void CJMODManager::RegisterBuiltinExtensions() {
    // 注册珂朵莉扩展
    auto chthollyExtension = std::make_unique<ChthollyExtension>();
    m_Integration->RegisterExtension("Chtholly", std::move(chthollyExtension));
}

std::string CJMODManager::ProcessCodeFragment(const std::string& fragment, size_t fragmentIndex) {
    if (!m_IsInitialized || !m_Integration) {
        return fragment;
    }
    
    // 获取真实的代码片段
    std::string realFragment = m_Integration->GetRealCodeFragment(fragmentIndex);
    
    if (realFragment.empty()) {
        realFragment = fragment; // 使用传入的片段作为备选
    }
    
    // 处理代码片段
    std::string processedFragment = m_Integration->ProcessCodeFragment(realFragment, "CHTLJS");
    
    // 如果片段被修改，更新扫描器中的片段
    if (processedFragment != realFragment) {
        m_Integration->ReturnProcessedFragment(fragmentIndex, processedFragment);
    }
    
    return processedFragment;
}

void CJMODManager::Reset() {
    if (m_Integration) {
        m_Integration->Reset();
    }
    
    // 清理所有扩展
    for (auto& extension : m_LoadedExtensions) {
        if (extension) {
            extension->Cleanup();
        }
    }
    
    m_IsInitialized = false;
}

} // namespace CJMOD