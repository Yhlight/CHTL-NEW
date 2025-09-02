#include "CHTLConstraintValidator.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/StyleNode.h"
#include "../CHTL/CHTLNode/ScriptNode.h"
#include "../CHTL/CHTLNode/TemplateNode.h"
#include "../CHTL/CHTLNode/CustomNode.h"
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLConstraintValidator::CHTLConstraintValidator() {
    InitializeDefaultConstraints();
    InitializeCHTLSyntaxSets();
    InitializeCHTLJSSyntaxSets();
    InitializeLocalScriptConstraints();
}

void CHTLConstraintValidator::InitializeDefaultConstraints() {
    // 根据CHTL语法文档初始化默认约束规则
    
    // 精准约束：模板定义不能出现在局部样式块中
    AddPreciseConstraint("[Template]", "local_style_block", 
                        "模板定义不能出现在局部样式块中");
    
    AddPreciseConstraint("[Template]", "local_script_block", 
                        "模板定义不能出现在局部脚本块中");
    
    AddPreciseConstraint("[Custom]", "local_style_block", 
                        "自定义定义不能出现在局部样式块中");
    
    AddPreciseConstraint("[Custom]", "local_script_block", 
                        "自定义定义不能出现在局部脚本块中");
    
    // 类型约束：HTML元素只能出现在特定上下文中
    AddTypeConstraint("html_element", "document_root_or_element_content", 
                     "HTML元素只能出现在文档根部或其他元素内容中");
    
    AddTypeConstraint("text_node", "element_content", 
                     "文本节点只能出现在元素内容中");
    
    AddTypeConstraint("local_style", "element_content", 
                     "局部样式块只能出现在元素内容中");
    
    AddTypeConstraint("local_script", "element_content", 
                     "局部脚本块只能出现在元素内容中");
    
    // 全局约束：use语句必须在文件开头
    AddGlobalConstraint("use", "file_beginning", 
                       "use语句必须出现在文件开头");
    
    AddGlobalConstraint("namespace", "global_scope", 
                       "命名空间定义必须在全局作用域");
    
    AddGlobalConstraint("configuration", "global_scope", 
                       "配置块必须在全局作用域");
}

void CHTLConstraintValidator::InitializeCHTLSyntaxSets() {
    // 仅CHTL语法（不能在CHTL JS中使用）
    m_CHTLOnlySyntax = {
        // HTML相关
        "html", "head", "body", "div", "span", "p", "a", "img", "ul", "ol", "li",
        "table", "tr", "td", "th", "h1", "h2", "h3", "h4", "h5", "h6",
        "header", "footer", "nav", "section", "article", "aside", "main",
        "figure", "figcaption", "button", "input", "form",
        
        // CHTL特有语法
        "text", "style", "script", // 局部块
        "[Template]", "[Custom]", "[Origin]", "[Configuration]", "[Import]", "[Namespace]",
        "@Style", "@Element", "@Var", "@Config", "@Origin",
        "use", "html5", "inherit", "delete", "insert", "except",
        
        // 选择器自动化
        "&", "class", "id", "tag", "pseudo-class", "pseudo-element"
    };
}

void CHTLConstraintValidator::InitializeCHTLJSSyntaxSets() {
    // 仅CHTL JS语法（不能在CHTL中使用）
    m_CHTLJSOnlySyntax = {
        // CHTL JS特有语法
        "module", "load", "vir", "listen", "delegate", "animate",
        "->", "&->", "{{", "}}", "begin", "end", "when", "target",
        
        // 虚对象相关
        "vir_key", "function_key", "object_key", "array_key", "value_key",
        
        // 事件相关
        "event_mapping", "event_handler", "event_delegate",
        
        // 动画相关
        "animation_keyframe", "easing_function", "animation_target"
    };
    
    // 共享语法（两种语言都可以使用）
    m_SharedSyntax = {
        // 基础语法
        "comment", "single_line_comment", "multi_line_comment", "generator_comment",
        "string_literal", "single_quote_string", "unquoted_literal",
        "identifier", "number", "boolean",
        
        // 基础操作符
        ":", "=", ";", ",", "{", "}", "(", ")", "[", "]",
        
        // 变量和引用
        "variable_reference", "variable_specialization"
    };
}

void CHTLConstraintValidator::InitializeLocalScriptConstraints() {
    // 局部script允许的CHTL语法（严格限制）
    m_LocalScriptAllowedCHTLSyntax = {
        // 模板变量
        "template_variable", "@Var", "variable_reference",
        
        // 自定义变量组
        "custom_variable", "variable_specialization",
        
        // 命名空间from
        "namespace_from", "from",
        
        // 注释
        "comment", "single_line_comment", "multi_line_comment", "generator_comment",
        
        // 原始嵌入
        "@Origin", "origin_reference"
    };
    
    // 局部script允许的CHTL JS语法（特供）
    m_LocalScriptAllowedCHTLJSSyntax = {
        // {{&}}特供语法
        "{{&}}", "context_reference_enhanced",
        
        // 基础JavaScript语法（作为脚本内容）
        "javascript_expression", "javascript_statement", "javascript_function"
    };
}

ConstraintValidationResult CHTLConstraintValidator::ValidateCHTLNode(CHTLBaseNode* node, const std::string& context) {
    ConstraintValidationResult result;
    
    if (!node) {
        result.IsValid = false;
        result.Violations.push_back("节点为空");
        return result;
    }
    
    std::string nodeContext = context.empty() ? AnalyzeNodeContext(node) : context;
    std::string syntaxName = node->GetTypeString();
    
    // 检查所有约束规则
    for (const auto& rule : m_PreciseConstraints) {
        if (rule.IsEnabled && CheckConstraintRule(rule, syntaxName, nodeContext)) {
            result.IsValid = false;
            result.Violations.push_back(FormatConstraintViolation(rule, syntaxName, nodeContext));
        }
    }
    
    for (const auto& rule : m_TypeConstraints) {
        if (rule.IsEnabled && CheckConstraintRule(rule, syntaxName, nodeContext)) {
            result.IsValid = false;
            result.Violations.push_back(FormatConstraintViolation(rule, syntaxName, nodeContext));
        }
    }
    
    for (const auto& rule : m_GlobalConstraints) {
        if (rule.IsEnabled && CheckConstraintRule(rule, syntaxName, nodeContext)) {
            result.IsValid = false;
            result.Violations.push_back(FormatConstraintViolation(rule, syntaxName, nodeContext));
        }
    }
    
    // 特殊处理：局部script约束
    if (node->GetType() == CHTLNodeType::SCRIPT_NODE) {
        ScriptNode* scriptNode = static_cast<ScriptNode*>(node);
        auto scriptResult = ValidateLocalScriptConstraints(scriptNode->GetScriptContent());
        
        if (!scriptResult.IsValid) {
            result.IsValid = false;
            result.Violations.insert(result.Violations.end(), 
                                   scriptResult.Violations.begin(), 
                                   scriptResult.Violations.end());
        }
        
        result.Warnings.insert(result.Warnings.end(), 
                              scriptResult.Warnings.begin(), 
                              scriptResult.Warnings.end());
    }
    
    // 递归验证子节点
    for (const auto& child : node->GetChildren()) {
        auto childResult = ValidateCHTLNode(child.get(), nodeContext);
        
        if (!childResult.IsValid) {
            result.IsValid = false;
            result.Violations.insert(result.Violations.end(), 
                                   childResult.Violations.begin(), 
                                   childResult.Violations.end());
        }
        
        result.Warnings.insert(result.Warnings.end(), 
                              childResult.Warnings.begin(), 
                              childResult.Warnings.end());
    }
    
    return result;
}

ConstraintValidationResult CHTLConstraintValidator::ValidateCHTLJSNode(CHTLJS::CHTLJSBaseNode* node, const std::string& context) {
    ConstraintValidationResult result;
    
    if (!node) {
        result.IsValid = false;
        result.Violations.push_back("CHTL JS节点为空");
        return result;
    }
    
    std::string nodeContext = context.empty() ? AnalyzeCHTLJSNodeContext(node) : context;
    std::string syntaxName = node->GetTypeString();
    
    // 验证CHTL JS语法边界
    if (!ValidateSyntaxBoundary(syntaxName, "CHTL_JS", nodeContext)) {
        result.IsValid = false;
        result.Violations.push_back("语法 '" + syntaxName + "' 不属于CHTL JS，不能在CHTL JS上下文中使用");
    }
    
    // 检查CHTL JS特有约束
    if (syntaxName == "vir" || syntaxName == "virtual_object") {
        // 虚对象约束：必须有有效的函数内容
        if (node->GetContent().empty()) {
            result.Warnings.push_back("虚对象缺少函数内容");
        }
    }
    
    if (syntaxName == "enhanced_selector") {
        // 增强选择器约束：必须符合{{...}}格式
        std::string selector = node->GetContent();
        if (selector.length() < 4 || 
            selector.substr(0, 2) != "{{" || 
            selector.substr(selector.length() - 2) != "}}") {
            result.IsValid = false;
            result.Violations.push_back("增强选择器格式错误，必须使用{{...}}格式");
        }
    }
    
    // 递归验证子节点
    for (const auto& child : node->GetChildren()) {
        auto childResult = ValidateCHTLJSNode(child.get(), nodeContext);
        
        if (!childResult.IsValid) {
            result.IsValid = false;
            result.Violations.insert(result.Violations.end(), 
                                   childResult.Violations.begin(), 
                                   childResult.Violations.end());
        }
        
        result.Warnings.insert(result.Warnings.end(), 
                              childResult.Warnings.begin(), 
                              childResult.Warnings.end());
    }
    
    return result;
}

bool CHTLConstraintValidator::ValidateSyntaxBoundary(const std::string& syntax, const std::string& targetLanguage, const std::string& context) {
    // 检查语法是否可以在目标语言中使用
    
    if (targetLanguage == "CHTL") {
        // 在CHTL中使用
        if (m_CHTLJSOnlySyntax.find(syntax) != m_CHTLJSOnlySyntax.end()) {
            // 这是CHTL JS专有语法，不能在CHTL中使用
            return false;
        }
        
        return m_CHTLOnlySyntax.find(syntax) != m_CHTLOnlySyntax.end() || 
               m_SharedSyntax.find(syntax) != m_SharedSyntax.end();
    }
    else if (targetLanguage == "CHTL_JS") {
        // 在CHTL JS中使用
        if (m_CHTLOnlySyntax.find(syntax) != m_CHTLOnlySyntax.end()) {
            // 这是CHTL专有语法，不能在CHTL JS中使用
            return false;
        }
        
        return m_CHTLJSOnlySyntax.find(syntax) != m_CHTLJSOnlySyntax.end() || 
               m_SharedSyntax.find(syntax) != m_SharedSyntax.end();
    }
    else if (targetLanguage == "LOCAL_SCRIPT") {
        // 局部script特殊处理（属于CHTL但支持部分CHTL JS语法）
        return m_LocalScriptAllowedCHTLSyntax.find(syntax) != m_LocalScriptAllowedCHTLSyntax.end() ||
               m_LocalScriptAllowedCHTLJSSyntax.find(syntax) != m_LocalScriptAllowedCHTLJSSyntax.end() ||
               m_SharedSyntax.find(syntax) != m_SharedSyntax.end();
    }
    
    return false;
}

ConstraintValidationResult CHTLConstraintValidator::ValidateLocalScriptConstraints(const std::string& scriptContent) {
    ConstraintValidationResult result;
    
    // 局部script特殊约束验证
    // 根据CHTL语法文档：局部script允许：模板变量、自定义变量组、变量组特例化、命名空间from、注释、原始嵌入、{{&}}特供语法
    
    // 检查不允许的CHTL语法
    std::vector<std::string> forbiddenCHTLSyntax = {
        "[Template] @Element", "[Template] @Style",
        "[Custom] @Element", "[Custom] @Style",
        "html", "head", "body", "div", "span", // HTML元素
        "text {", "style {" // 其他局部块
    };
    
    for (const auto& forbidden : forbiddenCHTLSyntax) {
        if (scriptContent.find(forbidden) != std::string::npos) {
            result.IsValid = false;
            result.Violations.push_back("局部script中不允许使用语法: " + forbidden);
        }
    }
    
    // 检查不允许的CHTL JS语法（除了特供的{{&}}）
    std::vector<std::string> forbiddenCHTLJSSyntax = {
        "module {", "load:", "vir ", "listen {", "delegate {", "animate {"
    };
    
    for (const auto& forbidden : forbiddenCHTLJSSyntax) {
        if (scriptContent.find(forbidden) != std::string::npos) {
            result.IsValid = false;
            result.Violations.push_back("局部script中不允许使用CHTL JS语法: " + forbidden);
        }
    }
    
    // 允许的特供语法检查
    if (scriptContent.find("{{&}}") != std::string::npos) {
        result.Warnings.push_back("检测到{{&}}特供语法，这是局部script中允许的CHTL JS语法");
    }
    
    // 检查允许的CHTL语法
    std::regex varTemplatePattern(R"(@Var\s+\w+)");
    if (std::regex_search(scriptContent, varTemplatePattern)) {
        result.Warnings.push_back("检测到模板变量使用，符合局部script约束");
    }
    
    std::regex customVarPattern(R"(\w+\(\w+\))");
    if (std::regex_search(scriptContent, customVarPattern)) {
        result.Warnings.push_back("检测到自定义变量组使用，符合局部script约束");
    }
    
    return result;
}

bool CHTLConstraintValidator::IsCHTLSyntaxValid(const std::string& syntax, const std::string& context) {
    return ValidateSyntaxBoundary(syntax, "CHTL", context);
}

bool CHTLConstraintValidator::IsCHTLJSSyntaxValid(const std::string& syntax, const std::string& context) {
    return ValidateSyntaxBoundary(syntax, "CHTL_JS", context);
}

std::string CHTLConstraintValidator::GetSyntaxOwnership(const std::string& syntax) {
    if (m_CHTLOnlySyntax.find(syntax) != m_CHTLOnlySyntax.end()) {
        return "CHTL";
    }
    else if (m_CHTLJSOnlySyntax.find(syntax) != m_CHTLJSOnlySyntax.end()) {
        return "CHTL_JS";
    }
    else if (m_SharedSyntax.find(syntax) != m_SharedSyntax.end()) {
        return "SHARED";
    }
    else {
        return "UNKNOWN";
    }
}

void CHTLConstraintValidator::AddPreciseConstraint(const std::string& sourceSyntax, const std::string& targetContext, const std::string& description) {
    m_PreciseConstraints.emplace_back(ConstraintType::PRECISE_CONSTRAINT, sourceSyntax, targetContext, description);
}

void CHTLConstraintValidator::AddTypeConstraint(const std::string& syntaxType, const std::string& constraint, const std::string& description) {
    m_TypeConstraints.emplace_back(ConstraintType::TYPE_CONSTRAINT, syntaxType, constraint, description);
}

void CHTLConstraintValidator::AddGlobalConstraint(const std::string& syntax, const std::string& constraint, const std::string& description) {
    m_GlobalConstraints.emplace_back(ConstraintType::GLOBAL_CONSTRAINT, syntax, constraint, description);
}

bool CHTLConstraintValidator::CheckConstraintRule(const ConstraintRule& rule, const std::string& syntax, const std::string& context) {
    switch (rule.Type) {
        case ConstraintType::PRECISE_CONSTRAINT:
            // 精准约束：检查特定语法是否出现在特定上下文中
            return syntax == rule.SourceSyntax && context == rule.TargetContext;
            
        case ConstraintType::TYPE_CONSTRAINT:
            // 类型约束：检查语法类型是否符合约束
            return syntax.find(rule.SourceSyntax) != std::string::npos && 
                   context != rule.TargetContext;
            
        case ConstraintType::GLOBAL_CONSTRAINT:
            // 全局约束：检查全局范围的约束
            return syntax == rule.SourceSyntax && context != rule.TargetContext;
            
        default:
            return false;
    }
}

std::string CHTLConstraintValidator::AnalyzeNodeContext(CHTLBaseNode* node) {
    if (!node) {
        return "unknown";
    }
    
    switch (node->GetType()) {
        case CHTLNodeType::ELEMENT_NODE:
            return "element_content";
        case CHTLNodeType::TEXT_NODE:
            return "text_content";
        case CHTLNodeType::STYLE_NODE:
            return "local_style_block";
        case CHTLNodeType::SCRIPT_NODE:
            return "local_script_block";
        case CHTLNodeType::TEMPLATE_STYLE_NODE:
        case CHTLNodeType::TEMPLATE_ELEMENT_NODE:
        case CHTLNodeType::TEMPLATE_VAR_NODE:
            return "template_definition";
        case CHTLNodeType::CUSTOM_STYLE_NODE:
        case CHTLNodeType::CUSTOM_ELEMENT_NODE:
        case CHTLNodeType::CUSTOM_VAR_NODE:
            return "custom_definition";
        default:
            return "global_scope";
    }
}

std::string CHTLConstraintValidator::AnalyzeCHTLJSNodeContext(CHTLJS::CHTLJSBaseNode* node) {
    if (!node) {
        return "unknown";
    }
    
    using namespace CHTLJS;
    
    switch (node->GetType()) {
        case CHTLJSNodeType::MODULE_NODE:
            return "module_block";
        case CHTLJSNodeType::VIR_DECLARATION_NODE:
            return "virtual_object_declaration";
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE:
            return "enhanced_selector_usage";
        case CHTLJSNodeType::LISTEN_NODE:
        case CHTLJSNodeType::DELEGATE_NODE:
        case CHTLJSNodeType::ANIMATE_NODE:
            return "chtljs_function_block";
        default:
            return "chtljs_global_scope";
    }
}

std::string CHTLConstraintValidator::FormatConstraintViolation(const ConstraintRule& rule, const std::string& syntax, const std::string& context) {
    return "约束违反 [" + syntax + " in " + context + "]: " + rule.Description;
}

void CHTLConstraintValidator::Reset() {
    m_PreciseConstraints.clear();
    m_TypeConstraints.clear();
    m_GlobalConstraints.clear();
    m_ContextConstraints.clear();
    
    InitializeDefaultConstraints();
}

std::vector<ConstraintRule> CHTLConstraintValidator::GetAllConstraints() const {
    std::vector<ConstraintRule> allConstraints;
    
    allConstraints.insert(allConstraints.end(), m_PreciseConstraints.begin(), m_PreciseConstraints.end());
    allConstraints.insert(allConstraints.end(), m_TypeConstraints.begin(), m_TypeConstraints.end());
    allConstraints.insert(allConstraints.end(), m_GlobalConstraints.begin(), m_GlobalConstraints.end());
    
    return allConstraints;
}

// 占位实现（需要后续完善）
bool CHTLConstraintValidator::CheckTemplateConstraints(const std::string& templateType, const std::string& content) {
    // 检查模板约束
    (void)templateType;
    (void)content;
    return true;
}

bool CHTLConstraintValidator::CheckCustomConstraints(const std::string& customType, const std::string& content) {
    // 检查自定义约束
    (void)customType;
    (void)content;
    return true;
}

bool CHTLConstraintValidator::CheckImportConstraints(const std::string& importType, const std::string& importPath) {
    // 检查导入约束
    (void)importType;
    (void)importPath;
    return true;
}

bool CHTLConstraintValidator::CheckNamespaceConstraints(const std::string& namespaceName, const std::string& context) {
    // 检查命名空间约束
    (void)namespaceName;
    (void)context;
    return true;
}

} // namespace CHTL