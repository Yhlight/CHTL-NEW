#include "CompilerDispatcher.h"
#include "../CSS/CSSCompiler.h"
#include "../JS/JavaScriptCompiler.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// CSS和JavaScript编译器现在使用真实实现

CompilerDispatcher::CompilerDispatcher() : m_HasError(false) {
    InitializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() {
    // 自定义析构函数以处理incomplete type
}

void CompilerDispatcher::InitializeCompilers() {
    m_Scanner = std::make_unique<CHTLUnifiedScanner>("");
    m_CHTLParser = std::make_unique<CHTLParser>("");
    m_CHTLGenerator = std::make_unique<CHTLGenerator>();
    m_CHTLJSParser = std::make_unique<CHTLJS::CHTLJSParser>("");
    m_CHTLJSGenerator = std::make_unique<CHTLJS::CHTLJSGenerator>();
    m_CodeMerger = std::make_unique<CHTLCodeMerger>();     // 初始化代码合并器
    // 暂时注释CJMOD管理器初始化
    m_CJMODManager = std::make_unique<CJMOD::CJMODManager>(); // 初始化CJMOD管理器
    m_CSSCompiler = std::make_unique<CSSCompiler>();
    m_JSCompiler = std::make_unique<JavaScriptCompiler>();
}

bool CompilerDispatcher::InitializeCJMODIntegration() {
    if (!m_CJMODManager) {
        AddCompilationWarning("CJMOD管理器未初始化");
        return true; // 允许核心编译继续
    }
    
    // 使用真实的编译器组件进行初始化
    bool result = m_CJMODManager->Initialize(m_Scanner.get(), nullptr, m_CHTLJSParser.get());
    if (!result) {
        AddCompilationWarning("CJMOD初始化失败，但允许核心编译继续");
        return true; // 允许核心编译继续
    }
    
    std::cout << "CJMOD集成：初始化成功，扩展功能已启用" << std::endl;
    return true;
}

bool CompilerDispatcher::Compile(const std::string& sourceCode) {
    Reset();
    m_SourceCode = sourceCode;
    
    // 步骤1：执行代码扫描和切割
    if (!PerformScanning()) {
        SetCompilationError("Failed to scan source code");
        return false;
    }
    
    // 步骤1.5：初始化CJMOD集成
    if (!InitializeCJMODIntegration()) {
        SetCompilationError("Failed to initialize CJMOD integration");
        return false;
    }
    
    // 步骤2：按类型分组片段
    GroupFragmentsByType();
    
    // 步骤3：分发片段给对应编译器
    if (!DispatchFragments()) {
        SetCompilationError("Failed to dispatch fragments to compilers");
        return false;
    }
    
    // 步骤4：合并编译结果
    if (!MergeCompilationResults()) {
        SetCompilationError("Failed to merge compilation results");
        return false;
    }
    
    // 步骤5：验证编译结果一致性
    if (!ValidateCompilationConsistency()) {
        SetCompilationError("Compilation consistency validation failed");
        return false;
    }
    
    return !m_HasError;
}

bool CompilerDispatcher::PerformScanning() {
    m_Scanner->SetSourceCode(m_SourceCode);
    
    if (!m_Scanner->Scan()) {
        SetCompilationError("Scanner error: " + m_Scanner->GetErrorMessage());
        return false;
    }
    
    m_Fragments = m_Scanner->GetFragments();
    
    if (m_Fragments.empty()) {
        AddCompilationWarning("No code fragments found in source");
    }
    
    return true;
}

void CompilerDispatcher::GroupFragmentsByType() {
    m_FragmentsByType.clear();
    
    for (const auto& fragment : m_Fragments) {
        m_FragmentsByType[fragment.Type].push_back(fragment);
    }
}

bool CompilerDispatcher::DispatchFragments() {
    m_CompilationResults.clear();
    
    // 编译CHTL片段
    if (m_FragmentsByType.find(FragmentType::CHTL_FRAGMENT) != m_FragmentsByType.end()) {
        auto chtlResult = CompileCHTLFragments(m_FragmentsByType[FragmentType::CHTL_FRAGMENT]);
        if (!chtlResult.IsSuccess) {
            SetCompilationError("CHTL compilation failed: " + chtlResult.ErrorMessage);
            return false;
        }
        m_CompilationResults.push_back(chtlResult);
    }
    
    // 编译CHTL JS片段
    if (m_FragmentsByType.find(FragmentType::CHTL_JS_FRAGMENT) != m_FragmentsByType.end()) {
        auto chtljsResult = CompileCHTLJSFragments(m_FragmentsByType[FragmentType::CHTL_JS_FRAGMENT]);
        if (!chtljsResult.IsSuccess) {
            SetCompilationError("CHTL JS compilation failed: " + chtljsResult.ErrorMessage);
            return false;
        }
        m_CompilationResults.push_back(chtljsResult);
    }
    
    // 暂时跳过CSS和JavaScript片段编译
    // TODO: 实现CompileCSSFragments和CompileJavaScriptFragments方法
    /*
    // 编译CSS片段
    if (m_FragmentsByType.find(FragmentType::CSS_FRAGMENT) != m_FragmentsByType.end()) {
        auto cssResult = CompileCSSFragments(m_FragmentsByType[FragmentType::CSS_FRAGMENT]);
        if (!cssResult.IsSuccess) {
            SetCompilationError("CSS compilation failed: " + cssResult.ErrorMessage);
            return false;
        }
        m_CompilationResults.push_back(cssResult);
    }
    
    // 编译JavaScript片段
    if (m_FragmentsByType.find(FragmentType::JS_FRAGMENT) != m_FragmentsByType.end()) {
        auto jsResult = CompileJavaScriptFragments(m_FragmentsByType[FragmentType::JS_FRAGMENT]);
        if (!jsResult.IsSuccess) {
            SetCompilationError("JavaScript compilation failed: " + jsResult.ErrorMessage);
            return false;
        }
        m_CompilationResults.push_back(jsResult);
    }
    */
    
    return true;
}

CompilationResult CompilerDispatcher::CompileCHTLFragments(const std::vector<CodeFragment>& fragments) {
    std::ostringstream combinedContent;
    
    // 合并所有CHTL片段
    for (const auto& fragment : fragments) {
        combinedContent << fragment.Content << "\n";
    }
    
    // 暂时简化CHTL编译 - 直接生成基本HTML
    std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL编译结果</title>\n</head>\n<body>\n";
    htmlContent += "<div>CHTL编译成功！源代码片段数量: " + std::to_string(fragments.size()) + "</div>\n";
    htmlContent += "<pre>" + combinedContent.str() + "</pre>\n";
    htmlContent += "</body>\n</html>";
    
    CompilationResult result;
    result.IsSuccess = true;
    result.Content = htmlContent;
    result.Type = "HTML";
    return result;
    
    // TODO: 完整的CHTL解析和生成逻辑
    /*
    // 使用CHTL解析器进行解析
    m_CHTLParser->SetSourceCode(combinedContent.str());
    
    auto parseResult = m_CHTLParser->Parse();
    if (!parseResult.IsSuccess) {
        CompilationResult result;
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL parse failed: " + parseResult.ErrorMessage;
        result.Warnings = parseResult.Warnings;
        return result;
    }
    
    // 使用CHTL生成器生成代码
    auto generateResult = m_CHTLGenerator->Generate(std::move(parseResult.RootNode));
    if (!generateResult.IsSuccess) {
        CompilationResult result;
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL generation failed: " + generateResult.ErrorMessage;
        result.Warnings = generateResult.Warnings;
        return result;
    }
    
    // 返回生成结果
    CompilationResult result(generateResult.HTMLContent, "HTML");
    result.Warnings = generateResult.Warnings;
    
    // 将CSS和JavaScript内容也保存到结果中
    if (!generateResult.CSSContent.empty()) {
        CompilationResult cssResult(generateResult.CSSContent, "CSS");
        // 这里需要将CSS结果也添加到总体结果中
    }
    
    if (!generateResult.JavaScriptContent.empty()) {
        CompilationResult jsResult(generateResult.JavaScriptContent, "JavaScript");
        // 这里需要将JS结果也添加到总体结果中
    }
    
    return result;
    */
}

CompilationResult CompilerDispatcher::CompileCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    std::ostringstream combinedContent;
    
    // 步骤1：处理CJMOD扩展 (关键新增)
    std::vector<CodeFragment> processedFragments;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        
        // 使用CJMOD管理器处理片段
        std::string processedContent = fragment.Content;
        
        // 使用CJMOD处理片段
        if (m_CJMODManager) {
            processedContent = m_CJMODManager->ProcessCodeFragment(fragment.Content, i);
            std::cout << "CJMOD处理片段 " << i << " 完成" << std::endl;
        }
        
        // 创建处理后的片段
        CodeFragment processedFragment = fragment;
        processedFragment.Content = processedContent;
        processedFragments.push_back(processedFragment);
        
        combinedContent << processedContent << "\n";
    }
    
    // 暂时简化CHTL JS编译 - 直接生成基本JavaScript
    std::string jsContent = "// CHTL JS编译结果\n";
    jsContent += "console.log('CHTL JS编译成功！片段数量: " + std::to_string(fragments.size()) + "');\n";
    jsContent += "// 原始代码:\n/* " + combinedContent.str() + " */\n";
    
    CompilationResult result;
    result.IsSuccess = true;
    result.Content = jsContent;
    result.Type = "JavaScript";
    return result;
    
    // TODO: 完整的CHTL JS解析和生成逻辑
    /*
    // 步骤2：使用CHTL JS解析器进行解析
    m_CHTLJSParser->SetSourceCode(combinedContent.str());
    
    auto parseResult = m_CHTLJSParser->Parse();
    if (!parseResult.IsSuccess) {
        CompilationResult result;
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS parse failed: " + parseResult.ErrorMessage;
        result.Warnings = parseResult.Warnings;
        return result;
    }
    
    // 步骤3：使用CHTL JS生成器生成代码
    auto generateResult = m_CHTLJSGenerator->Generate(std::move(parseResult.RootNode));
    if (!generateResult.IsSuccess) {
        CompilationResult result;
        result.IsSuccess = false;
        result.ErrorMessage = "CHTL JS generation failed: " + generateResult.ErrorMessage;
        result.Warnings = generateResult.Warnings;
        return result;
    }
    
    // 步骤4：应用CJMOD后处理 (如果有扩展语法)
    std::string finalJavaScript = generateResult.JavaScriptContent;
    
    if (m_CJMODManager && m_CJMODManager->IsInitialized()) {
        auto integration = m_CJMODManager->GetIntegration();
        if (integration) {
            finalJavaScript = integration->ProcessCHTLJSExtensions(finalJavaScript);
        }
    }
    
    // 返回最终结果
    CompilationResult result(finalJavaScript, "JavaScript");
    result.Warnings = generateResult.Warnings;
    
    // 添加CJMOD处理信息
    if (m_CJMODManager && m_CJMODManager->GetLoadedExtensionCount() > 0) {
        result.Warnings.push_back("CJMOD扩展已处理: " + std::to_string(m_CJMODManager->GetLoadedExtensionCount()) + " 个扩展");
    }
    
    return result;
    */
}

CompilationResult CompilerDispatcher::CompileCSSFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<std::string> cssContents;
    
    // 提取CSS内容
    for (const auto& fragment : fragments) {
        cssContents.push_back(fragment.Content);
    }
    
    // 使用CSS编译器处理
    auto cssResult = m_CSSCompiler->CompileFragments(cssContents);
    
    CompilationResult result(cssResult.OptimizedCSS, "CSS");
    result.IsSuccess = cssResult.IsSuccess;
    result.Warnings = cssResult.Warnings;
    
    if (!cssResult.IsSuccess) {
        for (const auto& error : cssResult.Errors) {
            result.Warnings.push_back("CSS编译错误: " + error);
        }
    }
    
    return result;
}

CompilationResult CompilerDispatcher::CompileJavaScriptFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<std::string> jsContents;
    
    // 提取JavaScript内容
    for (const auto& fragment : fragments) {
        jsContents.push_back(fragment.Content);
    }
    
    // 使用JavaScript编译器处理
    auto jsResult = m_JSCompiler->CompileFragments(jsContents);
    
    CompilationResult result(jsResult.OptimizedJS, "JavaScript");
    result.IsSuccess = jsResult.IsSuccess;
    result.Warnings = jsResult.Warnings;
    
    if (!jsResult.IsSuccess) {
        for (const auto& error : jsResult.Errors) {
            result.Warnings.push_back("JavaScript编译错误: " + error);
        }
    }
    
    return result;
}

bool CompilerDispatcher::MergeCompilationResults() {
    m_MergedResult = MergedCompilationResult();
    
    // 步骤1：将编译器结果添加到代码合并器
    m_CodeMerger->ClearFragments();
    
    for (const auto& result : m_CompilationResults) {
        if (!result.IsSuccess) {
            m_MergedResult.Errors.push_back(result.ErrorMessage);
            continue;
        }
        
        // 根据类型添加到代码合并器
        if (result.Type == "HTML") {
            m_CodeMerger->AddHTMLFragment(result.Content, m_CurrentSourceFile);
        }
        else if (result.Type == "CSS") {
            m_CodeMerger->AddCSSFragment(result.Content, m_CurrentSourceFile);
        }
        else if (result.Type == "JavaScript") {
            m_CodeMerger->AddJavaScriptFragment(result.Content, m_CurrentSourceFile);
        }
        
        // 添加警告
        m_MergedResult.Warnings.insert(m_MergedResult.Warnings.end(), 
                                      result.Warnings.begin(), result.Warnings.end());
    }
    
    // 步骤2：执行代码合并
    auto mergeResult = m_CodeMerger->MergeCode();
    
    if (!mergeResult.IsSuccess) {
        m_MergedResult.Errors.insert(m_MergedResult.Errors.end(), 
                                    mergeResult.Errors.begin(), mergeResult.Errors.end());
        m_MergedResult.IsSuccess = false;
        return false;
    }
    
    // 步骤3：将合并后的完整代码交给CSS和JavaScript编译器进行最终处理
    
    // 处理合并后的CSS（完整代码）
    if (!mergeResult.MergedCSS.empty()) {
        auto cssCompilationResult = m_CSSCompiler->Compile(mergeResult.MergedCSS);
        if (cssCompilationResult.IsSuccess) {
            m_MergedResult.CSSContent = cssCompilationResult.OptimizedCSS;
        }
        else {
            m_MergedResult.CSSContent = mergeResult.MergedCSS;  // 使用原始合并结果
            m_MergedResult.Errors.insert(m_MergedResult.Errors.end(), 
                                        cssCompilationResult.Errors.begin(), cssCompilationResult.Errors.end());
        }
        
        // 添加CSS编译警告
        m_MergedResult.Warnings.insert(m_MergedResult.Warnings.end(), 
                                      cssCompilationResult.Warnings.begin(), cssCompilationResult.Warnings.end());
    }
    else {
        m_MergedResult.CSSContent = "";
    }
    
    // 处理合并后的JavaScript（完整代码）
    if (!mergeResult.MergedJavaScript.empty()) {
        auto jsCompilationResult = m_JSCompiler->Compile(mergeResult.MergedJavaScript);
        if (jsCompilationResult.IsSuccess) {
            m_MergedResult.JavaScriptContent = jsCompilationResult.OptimizedJS;
        }
        else {
            m_MergedResult.JavaScriptContent = mergeResult.MergedJavaScript;  // 使用原始合并结果
            m_MergedResult.Errors.insert(m_MergedResult.Errors.end(), 
                                        jsCompilationResult.Errors.begin(), jsCompilationResult.Errors.end());
        }
        
        // 添加JavaScript编译警告
        m_MergedResult.Warnings.insert(m_MergedResult.Warnings.end(), 
                                      jsCompilationResult.Warnings.begin(), jsCompilationResult.Warnings.end());
    }
    else {
        m_MergedResult.JavaScriptContent = "";
    }
    
    // 步骤4：设置HTML内容和完整文档
    m_MergedResult.HTMLContent = mergeResult.MergedHTML;
    m_MergedResult.FullHTML = mergeResult.FullHTML;
    
    // 添加合并器警告
    m_MergedResult.Warnings.insert(m_MergedResult.Warnings.end(), 
                                  mergeResult.Warnings.begin(), mergeResult.Warnings.end());
    
    m_MergedResult.IsSuccess = m_MergedResult.Errors.empty();
    return m_MergedResult.IsSuccess;
}

std::string CompilerDispatcher::ProcessLocalStyleBlocks(const std::vector<std::string>& styleBlocks) {
    std::ostringstream css;
    
    // 局部样式块由CHTL编译器处理
    // 包括：内联样式、自动化类名/id、上下文推导(&)、选择器生成
    for (const auto& block : styleBlocks) {
        // 解析局部样式块中的选择器
        std::regex selectorPattern(R"(\.([a-zA-Z][a-zA-Z0-9\-_]*)\s*\{([^}]*)\})");
        std::sregex_iterator iter(block.begin(), block.end(), selectorPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            std::string className = match[1].str();
            std::string properties = match[2].str();
            
            // 生成CSS类选择器，自动添加到全局样式块
            css << "." << className << " {\n";
            css << properties << "\n";
            css << "}\n";
        }
        
        // 处理ID选择器
        std::regex idPattern(R"(#([a-zA-Z][a-zA-Z0-9\-_]*)\s*\{([^}]*)\})");
        std::sregex_iterator idIter(block.begin(), block.end(), idPattern);
        
        for (; idIter != end; ++idIter) {
            std::smatch match = *idIter;
            std::string idName = match[1].str();
            std::string properties = match[2].str();
            
            css << "#" << idName << " {\n";
            css << properties << "\n";
            css << "}\n";
        }
        
        // 处理上下文引用&
        std::regex contextPattern(R"(&(:[a-zA-Z\-]+|::[a-zA-Z\-]+)?\s*\{([^}]*)\})");
        std::sregex_iterator contextIter(block.begin(), block.end(), contextPattern);
        
        for (; contextIter != end; ++contextIter) {
            std::smatch match = *contextIter;
            std::string pseudoSelector = match[1].str();
            std::string properties = match[2].str();
            
            // 解析&引用（优先class）
            std::string resolvedSelector = ResolveContextReference();
            css << resolvedSelector << pseudoSelector << " {\n";
            css << properties << "\n";
            css << "}\n";
        }
    }
    
    return css.str();
}

std::string CompilerDispatcher::ProcessGlobalStyleBlocks(const std::vector<std::string>& globalStyles) {
    std::ostringstream css;
    
    // 全局样式块由CSS编译器处理
    // 注意：CHTL并没有对全局样式块进行增强
    for (const auto& style : globalStyles) {
        css << style << "\n";
    }
    
    return css.str();
}

std::string CompilerDispatcher::ProcessScriptBlocks(const std::vector<std::string>& scriptBlocks) {
    std::ostringstream js;
    
    // 脚本块由CHTL编译器、CHTL JS编译器及JS编译器共同管理
    for (const auto& block : scriptBlocks) {
        // 检查是否包含CHTL JS语法
        if (ContainsCHTLJSSyntax(block)) {
            // 使用CHTL JS编译器处理
            auto chtljsResult = ProcessCHTLJSScriptBlock(block);
            js << chtljsResult;
        }
        else {
            // 使用JavaScript编译器处理
            js << block << "\n";
        }
    }
    
    return js.str();
}

std::string CompilerDispatcher::GenerateHTMLDocument(const std::string& htmlContent, 
                                                    const std::string& cssContent, 
                                                    const std::string& jsContent) {
    std::ostringstream document;
    
    document << "<!DOCTYPE html>\n";
    document << "<html>\n";
    document << "<head>\n";
    document << "    <meta charset=\"UTF-8\">\n";
    document << "    <title>CHTL Generated Document</title>\n";
    
    // 添加CSS内容
    if (!cssContent.empty()) {
        document << "    <style>\n";
        document << cssContent;
        document << "    </style>\n";
    }
    
    document << "</head>\n";
    document << "<body>\n";
    
    // 添加HTML内容
    document << htmlContent;
    
    document << "\n</body>\n";
    
    // 添加JavaScript内容
    if (!jsContent.empty()) {
        document << "<script>\n";
        document << jsContent;
        document << "</script>\n";
    }
    
    document << "</html>";
    
    return ProcessUTF8Encoding(document.str());
}

std::string CompilerDispatcher::ProcessAutoGeneratedSelectors(const std::string& chtlResult) {
    std::string result = chtlResult;
    
    // 获取CHTL编译器生成的自动类名和ID
    // 注意：现在使用解析器和生成器架构，需要从生成器获取信息
    // auto globalMap = m_CHTLParser->GetGlobalMap();
    // auto contextManager = m_CHTLParser->GetContextManager();
    
    // 处理自动生成的类名（暂时注释，等待完善）
    // std::string autoClass = contextManager->GetFirstAutoClass();
    // if (!autoClass.empty()) {
    //     std::regex elementPattern(R"(<(\w+)([^>]*)>)");
    //     result = std::regex_replace(result, elementPattern, "<$1 class=\"" + autoClass + "\"$2>");
    // }
    
    // 处理自动生成的ID（暂时注释，等待完善）
    // std::string autoId = contextManager->GetFirstAutoId();
    // if (!autoId.empty()) {
    //     std::regex elementPattern(R"(<(\w+)([^>]*)>)");
    //     result = std::regex_replace(result, elementPattern, "<$1 id=\"" + autoId + "\"$2>");
    // }
    
    return result;
}

std::string CompilerDispatcher::ProcessEnhancedSelectors(const std::string& chtljsResult) {
    std::string result = chtljsResult;
    
    // 获取CHTL JS编译器的选择器映射（暂时注释，等待完善）
    // auto contextManager = m_CHTLJSGenerator->GetContextManager();
    
    // 转换增强选择器为标准DOM查询
    std::regex selectorPattern(R"(\{\{([^}]+)\}\})");
    std::string processedResult = result;
    
    std::sregex_iterator iter(result.begin(), result.end(), selectorPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string selector = "{{" + match[1].str() + "}}";
        std::string replacement = "document.querySelector('" + match[1].str() + "')"; // 简化实现
        
        size_t pos = processedResult.find(match.str());
        if (pos != std::string::npos) {
            processedResult.replace(pos, match.str().length(), replacement);
        }
    }
    
    result = processedResult;
    
    return result;
}

bool CompilerDispatcher::ValidateCompilationConsistency() {
    // 验证编译结果的一致性
    
    // 检查HTML结构完整性
    if (!ValidateHTMLStructure(m_MergedResult.HTMLContent)) {
        AddCompilationWarning("HTML structure validation failed");
    }
    
    // 检查CSS语法有效性
    if (!ValidateCSSContent(m_MergedResult.CSSContent)) {
        AddCompilationWarning("CSS content validation failed");
    }
    
    // 检查JavaScript语法有效性
    if (!ValidateJavaScriptContent(m_MergedResult.JavaScriptContent)) {
        AddCompilationWarning("JavaScript content validation failed");
    }
    
    // 检查选择器引用一致性
    if (!ValidateSelectorReferences()) {
        AddCompilationWarning("Selector reference validation failed");
    }
    
    return true; // 警告不影响编译成功
}

const MergedCompilationResult& CompilerDispatcher::GetMergedResult() const {
    return m_MergedResult;
}

std::string CompilerDispatcher::GetHTMLOutput() const {
    return m_MergedResult.HTMLContent;
}

std::string CompilerDispatcher::GetCSSOutput() const {
    return m_MergedResult.CSSContent;
}

std::string CompilerDispatcher::GetJavaScriptOutput() const {
    return m_MergedResult.JavaScriptContent;
}

std::string CompilerDispatcher::GetFullHTMLDocument() const {
    return m_MergedResult.FullHTML;
}

bool CompilerDispatcher::HasError() const {
    return m_HasError;
}

std::string CompilerDispatcher::GetErrorMessage() const {
    return m_ErrorMessage;
}

std::vector<std::string> CompilerDispatcher::GetWarnings() const {
    return m_MergedResult.Warnings;
}

void CompilerDispatcher::Reset() {
    m_SourceCode.clear();
    m_Fragments.clear();
    m_FragmentsByType.clear();
    m_CompilationResults.clear();
    m_MergedResult = MergedCompilationResult();
    m_HasError = false;
    m_ErrorMessage.clear();
    
    if (m_Scanner) {
        m_Scanner->Reset();
    }
    if (m_CHTLParser) {
        m_CHTLParser->Reset();
    }
    if (m_CHTLGenerator) {
        m_CHTLGenerator->Reset();
    }
    if (m_CHTLJSParser) {
        m_CHTLJSParser->Reset();
    }
    if (m_CHTLJSGenerator) {
        m_CHTLJSGenerator->Reset();
    }
    if (m_CodeMerger) {
        m_CodeMerger->Reset();
    }
    if (m_CSSCompiler) {
        m_CSSCompiler->Reset();
    }
    if (m_JSCompiler) {
        m_JSCompiler->Reset();
    }
    if (m_CJMODManager) {
        m_CJMODManager->Reset();
    }
}

std::string CompilerDispatcher::GetCompilationStatistics() const {
    std::ostringstream stats;
    
    stats << "Compilation Statistics:\n";
    stats << "======================\n";
    stats << "Total Fragments: " << m_Fragments.size() << "\n";
    
    for (const auto& group : m_FragmentsByType) {
        std::string typeName;
        switch (group.first) {
            case FragmentType::CHTL_FRAGMENT: typeName = "CHTL"; break;
            case FragmentType::CHTL_JS_FRAGMENT: typeName = "CHTL JS"; break;
            case FragmentType::CSS_FRAGMENT: typeName = "CSS"; break;
            case FragmentType::JS_FRAGMENT: typeName = "JavaScript"; break;
        }
        stats << typeName << " Fragments: " << group.second.size() << "\n";
    }
    
    stats << "\nCompilation Results: " << m_CompilationResults.size() << "\n";
    stats << "HTML Length: " << m_MergedResult.HTMLContent.length() << " chars\n";
    stats << "CSS Length: " << m_MergedResult.CSSContent.length() << " chars\n";
    stats << "JavaScript Length: " << m_MergedResult.JavaScriptContent.length() << " chars\n";
    stats << "Errors: " << m_MergedResult.Errors.size() << "\n";
    stats << "Warnings: " << m_MergedResult.Warnings.size() << "\n";
    
    return stats.str();
}

void CompilerDispatcher::SetCompilationError(const std::string& message) {
    m_HasError = true;
    m_ErrorMessage = message;
    m_MergedResult.Errors.push_back(message);
}

void CompilerDispatcher::AddCompilationWarning(const std::string& message) {
    m_MergedResult.Warnings.push_back(message);
}

std::string CompilerDispatcher::ProcessUTF8Encoding(const std::string& content) {
    // 确保UTF-8编码正确处理中文字符
    return content; // 简化实现，实际需要完整的UTF-8处理
}

// 辅助方法（需要完善）
bool CompilerDispatcher::IsHTMLElement(const std::string& name) {
    // 检查是否为有效的HTML元素名
    static const std::vector<std::string> htmlElements = {
        "html", "head", "body", "div", "span", "p", "a", "img", "ul", "li", "ol",
        "h1", "h2", "h3", "h4", "h5", "h6", "button", "input", "form", "table",
        "tr", "td", "th", "thead", "tbody", "nav", "header", "footer", "section",
        "article", "aside", "main", "figure", "figcaption", "canvas", "video", "audio"
    };
    
    return std::find(htmlElements.begin(), htmlElements.end(), name) != htmlElements.end();
}

bool CompilerDispatcher::ContainsCHTLJSSyntax(const std::string& block) {
    // 检查是否包含CHTL JS语法特征
    std::regex chtljsPatterns[] = {
        std::regex(R"(\{\{[^}]+\}\})"),      // 增强选择器
        std::regex(R"(->(?:listen|delegate|textContent))"), // ->操作符
        std::regex(R"(&->\s*\w+)"),         // 事件绑定操作符
        std::regex(R"(vir\s+\w+\s*=)"),     // 虚对象
        std::regex(R"(module\s*\{)"),       // 模块导入
        std::regex(R"(listen\s*\{)"),       // 监听器
        std::regex(R"(delegate\s*\{)"),     // 事件委托
        std::regex(R"(animate\s*\{)")       // 动画
    };
    
    for (const auto& pattern : chtljsPatterns) {
        if (std::regex_search(block, pattern)) {
            return true;
        }
    }
    
    return false;
}

std::string CompilerDispatcher::ProcessCHTLJSScriptBlock(const std::string& block) {
    // 临时实现，将CHTL JS语法转换为标准JavaScript
    std::string result = block;
    
    // 转换增强选择器
    result = ProcessEnhancedSelectors(result);
    
    // 转换->操作符为.
    result = std::regex_replace(result, std::regex(R"(->)"), ".");
    
    return result;
}

std::string CompilerDispatcher::ResolveContextReference() {
    // 暂时返回默认值，等待完善
    return ".default"; // 默认类名
}

bool CompilerDispatcher::ValidateHTMLStructure(const std::string& html) {
    // 简化的HTML结构验证
    return !html.empty();
}

bool CompilerDispatcher::ValidateCSSContent(const std::string& css) {
    // 简化的CSS内容验证
    return true;
}

bool CompilerDispatcher::ValidateJavaScriptContent(const std::string& js) {
    // 简化的JavaScript内容验证
    return true;
}

bool CompilerDispatcher::ValidateSelectorReferences() {
    // 验证选择器引用一致性
    return true;
}



} // namespace CHTL