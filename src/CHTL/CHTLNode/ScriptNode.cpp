#include "ScriptNode.h"
#include "Visitor.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTL {

ScriptNode::ScriptNode(const std::string& scriptContent, size_t line, size_t column)
    : CHTLBaseNode(CHTLNodeType::SCRIPT_NODE, "script", line, column),
      m_ScriptContent(scriptContent), m_HasCHTLJSSyntax(false), 
      m_HasContextReference(false), m_HasTemplateUsage(false), m_HasCustomUsage(false) {
    
    SetContent(scriptContent);
    if (!scriptContent.empty()) {
        ParseScriptContent(scriptContent);
    }
}

void ScriptNode::SetScriptContent(const std::string& content) {
    m_ScriptContent = content;
    SetContent(content);
    
    // 重新解析内容
    m_TemplateVariableUsages.clear();
    m_CustomVariableUsages.clear();
    m_VariableSpecializations.clear();
    m_NamespaceFromUsages.clear();
    m_ContextReferences.clear();
    m_CHTLJSSyntaxBlocks.clear();
    
    m_HasCHTLJSSyntax = false;
    m_HasContextReference = false;
    m_HasTemplateUsage = false;
    m_HasCustomUsage = false;
    
    ParseScriptContent(content);
}

void ScriptNode::ParseScriptContent(const std::string& scriptContent) {
    // 分离CHTL语法和CHTL JS语法
    SeparateCHTLAndCHTLJSSyntax(scriptContent);
    
    // 提取CHTL语法元素
    ExtractCHTLSyntaxElements(scriptContent);
    
    // 提取CHTL JS语法块
    ExtractCHTLJSSyntaxBlocks(scriptContent);
}

void ScriptNode::SeparateCHTLAndCHTLJSSyntax(const std::string& content) {
    // 检测是否包含CHTL JS语法
    m_HasCHTLJSSyntax = DetectCHTLJSSyntax(content);
}

bool ScriptNode::DetectCHTLJSSyntax(const std::string& content) {
    // 检测CHTL JS语法特征
    std::regex chtljsPatterns[] = {
        std::regex(R"(\{\{[^}]+\}\})"),              // 增强选择器 {{selector}}
        std::regex(R"(->(?:listen|delegate|textContent|addEventListener))"), // ->操作符
        std::regex(R"(&->\s*\w+)"),                 // 事件绑定操作符 &->
        std::regex(R"(vir\s+\w+\s*=)"),            // 虚对象 vir
        std::regex(R"(module\s*\{)"),              // 模块导入 module{}
        std::regex(R"(listen\s*\{)"),              // 增强监听器 listen{}
        std::regex(R"(delegate\s*\{)"),            // 事件委托 delegate{}
        std::regex(R"(animate\s*\{)")              // 动画 animate{}
    };
    
    for (const auto& pattern : chtljsPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

void ScriptNode::ExtractCHTLSyntaxElements(const std::string& content) {
    // 提取局部script允许的CHTL语法元素
    
    // 1. 模板变量（@Var）
    std::regex templateVarPattern(R"(@Var\s+([a-zA-Z][a-zA-Z0-9_]*))");
    std::sregex_iterator templateIter(content.begin(), content.end(), templateVarPattern);
    std::sregex_iterator end;
    
    for (; templateIter != end; ++templateIter) {
        std::smatch match = *templateIter;
        std::string templateVar = match[0].str();
        AddTemplateVariableUsage(templateVar);
    }
    
    // 2. 自定义变量组
    std::regex customVarPattern(R"(\[Custom\]\s*@Var\s+([a-zA-Z][a-zA-Z0-9_]*))");
    std::sregex_iterator customIter(content.begin(), content.end(), customVarPattern);
    
    for (; customIter != end; ++customIter) {
        std::smatch match = *customIter;
        std::string customVar = match[0].str();
        AddCustomVariableUsage(customVar);
    }
    
    // 3. 变量组特例化（ThemeColor(tableColor = newValue)）
    std::regex varSpecPattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\(([a-zA-Z][a-zA-Z0-9_]*)\s*=\s*([^)]+)\))");
    std::sregex_iterator varSpecIter(content.begin(), content.end(), varSpecPattern);
    
    for (; varSpecIter != end; ++varSpecIter) {
        std::smatch match = *varSpecIter;
        std::string specialization = match[0].str();
        AddVariableSpecialization(specialization);
    }
    
    // 4. 命名空间from使用
    std::regex fromPattern(R"(from\s+([a-zA-Z][a-zA-Z0-9_.]*))");
    std::sregex_iterator fromIter(content.begin(), content.end(), fromPattern);
    
    for (; fromIter != end; ++fromIter) {
        std::smatch match = *fromIter;
        std::string fromUsage = match[0].str();
        AddNamespaceFromUsage(fromUsage);
    }
    
    // 5. 上下文引用（{{&}}特供语法）
    std::regex contextPattern(R"(\{\{&\}\})");
    if (std::regex_search(content, contextPattern)) {
        AddContextReference("{{&}}");
    }
    
    // 更高级的上下文引用：{{&}}->method
    std::regex advancedContextPattern(R"(\{\{&\}\}->([a-zA-Z][a-zA-Z0-9_]*))");
    std::sregex_iterator advContextIter(content.begin(), content.end(), advancedContextPattern);
    
    for (; advContextIter != end; ++advContextIter) {
        std::smatch match = *advContextIter;
        std::string contextRef = match[0].str();
        AddContextReference(contextRef);
    }
}

void ScriptNode::ExtractCHTLJSSyntaxBlocks(const std::string& content) {
    if (!m_HasCHTLJSSyntax) {
        return;
    }
    
    // 提取CHTL JS语法块
    std::vector<std::regex> chtljsBlockPatterns = {
        std::regex(R"(module\s*\{[^}]*\})"),        // module块
        std::regex(R"(listen\s*\{[^}]*\})"),        // listen块
        std::regex(R"(delegate\s*\{[^}]*\})"),      // delegate块
        std::regex(R"(animate\s*\{[^}]*\})"),       // animate块
        std::regex(R"(vir\s+\w+\s*=\s*[^;]+;?)"),  // vir声明
        std::regex(R"(\{\{[^}]+\}\}->[^;]+;?)"),    // 增强选择器操作
        std::regex(R"(\{\{[^}]+\}\}\s*&->\s*\w+\s*\{[^}]*\})") // 事件绑定操作符
    };
    
    for (const auto& pattern : chtljsBlockPatterns) {
        std::sregex_iterator iter(content.begin(), content.end(), pattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            AddCHTLJSSyntaxBlock(match.str());
        }
    }
}

void ScriptNode::AddTemplateVariableUsage(const std::string& templateVar) {
    if (IsAllowedCHTLSyntax(templateVar)) {
        m_TemplateVariableUsages.push_back(templateVar);
        m_HasTemplateUsage = true;
    }
}

void ScriptNode::AddCustomVariableUsage(const std::string& customVar) {
    if (IsAllowedCHTLSyntax(customVar)) {
        m_CustomVariableUsages.push_back(customVar);
        m_HasCustomUsage = true;
    }
}

void ScriptNode::AddVariableSpecialization(const std::string& specialization) {
    if (IsAllowedCHTLSyntax(specialization)) {
        m_VariableSpecializations.push_back(specialization);
    }
}

void ScriptNode::AddNamespaceFromUsage(const std::string& fromUsage) {
    if (IsAllowedCHTLSyntax(fromUsage)) {
        m_NamespaceFromUsages.push_back(fromUsage);
    }
}

void ScriptNode::AddContextReference(const std::string& contextRef) {
    // {{&}}等特供语法属于CHTL本身功能，始终允许
    m_ContextReferences.push_back(contextRef);
    m_HasContextReference = true;
}

void ScriptNode::AddCHTLJSSyntaxBlock(const std::string& chtljsBlock) {
    m_CHTLJSSyntaxBlocks.push_back(chtljsBlock);
    m_HasCHTLJSSyntax = true;
}

bool ScriptNode::IsAllowedCHTLSyntax(const std::string& syntax) const {
    // 局部script允许的CHTL语法元素：
    // - 模板变量
    // - 自定义变量组  
    // - 变量组特例化
    // - 命名空间from
    // - 注释（任意位置允许）
    // - 任意类型原始嵌入（任意位置允许）
    // - {{&}}等特供语法（CHTL本身功能）
    
    // 检查是否为允许的语法
    if (syntax.find("@Var") != std::string::npos ||           // 模板变量
        syntax.find("[Custom] @Var") != std::string::npos ||  // 自定义变量组
        syntax.find("from") != std::string::npos ||           // 命名空间from
        syntax.find("{{&}}") != std::string::npos ||          // 上下文引用特供语法
        syntax.find("//") != std::string::npos ||             // 注释
        syntax.find("/*") != std::string::npos ||             // 多行注释
        syntax.find("--") != std::string::npos ||             // 生成器注释
        syntax.find("[Origin]") != std::string::npos) {       // 原始嵌入
        return true;
    }
    
    // 检查变量组特例化模式
    std::regex varSpecPattern(R"([a-zA-Z][a-zA-Z0-9_]*\([a-zA-Z][a-zA-Z0-9_]*\s*=\s*[^)]+\))");
    if (std::regex_search(syntax, varSpecPattern)) {
        return true;
    }
    
    return false;
}

std::string ScriptNode::GenerateJavaScript(const std::string& contextClass, const std::string& contextId) const {
    std::ostringstream js;
    
    // 处理CHTL语法元素的JavaScript生成
    
    // 1. 处理上下文引用{{&}}
    std::string processedContent = m_ScriptContent;
    if (m_HasContextReference) {
        std::string contextSelector = ResolveContextReference(contextClass, contextId);
        
        if (!contextSelector.empty()) {
            // 替换{{&}}为实际的DOM查询
            std::regex contextRefPattern(R"(\{\{&\}\})");
            std::string domQuery = "document.querySelector('" + contextSelector + "')";
            processedContent = std::regex_replace(processedContent, contextRefPattern, domQuery);
        }
    }
    
    // 2. 处理变量引用
    for (const auto& varRef : m_VariableReferences) {
        std::string resolvedValue = ResolveVariableReference(varRef);
        if (!resolvedValue.empty()) {
            // 替换变量引用为实际值
            size_t pos = processedContent.find(varRef);
            if (pos != std::string::npos) {
                processedContent.replace(pos, varRef.length(), resolvedValue);
            }
        }
    }
    
    // 3. 生成最终JavaScript代码
    js << "(function() {\n";
    js << "  // CHTL局部脚本块\n";
    
    // 添加处理后的脚本内容
    std::istringstream contentStream(processedContent);
    std::string line;
    while (std::getline(contentStream, line)) {
        if (!line.empty()) {
            js << "  " << line << "\n";
        }
    }
    
    js << "})();\n";
    
    return js.str();
}

std::string ScriptNode::ResolveContextReference(const std::string& contextClass, const std::string& contextId) const {
    // 局部script优先选择id
    if (!contextId.empty()) {
        return "#" + contextId;
    }
    else if (!contextClass.empty()) {
        return "." + contextClass;
    }
    
    return ""; // 无法解析
}

std::string ScriptNode::ResolveVariableReference(const std::string& variableRef) const {
    // 解析变量引用：ThemeColor(tableColor) 或 ThemeColor(tableColor = newValue)
    std::regex varPattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\(([a-zA-Z][a-zA-Z0-9_]*)\s*(?:=\s*([^)]+))?\))");
    std::smatch match;
    
    if (std::regex_match(variableRef, match, varPattern)) {
        std::string varGroup = match[1].str();
        std::string varName = match[2].str();
        std::string newValue = match[3].str();
        
        // 这里应该从GlobalMap中查找变量值
        // 暂时返回简化实现
        if (!newValue.empty()) {
            return newValue;
        }
        else {
            return "/* Variable: " + varGroup + "(" + varName + ") */";
        }
    }
    
    return variableRef; // 无法解析，返回原值
}

void ScriptNode::Accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->VisitScriptNode(this);
    }
}

std::unique_ptr<CHTLBaseNode> ScriptNode::Clone() const {
    auto cloned = std::make_unique<ScriptNode>(m_ScriptContent, GetLine(), GetColumn());
    
    // 复制基础属性
    cloned->SetNamespace(GetNamespace());
    
    // 复制CHTL语法元素
    cloned->m_TemplateVariableUsages = m_TemplateVariableUsages;
    cloned->m_CustomVariableUsages = m_CustomVariableUsages;
    cloned->m_VariableSpecializations = m_VariableSpecializations;
    cloned->m_NamespaceFromUsages = m_NamespaceFromUsages;
    cloned->m_ContextReferences = m_ContextReferences;
    cloned->m_CHTLJSSyntaxBlocks = m_CHTLJSSyntaxBlocks;
    
    // 复制状态标记
    cloned->m_HasCHTLJSSyntax = m_HasCHTLJSSyntax;
    cloned->m_HasContextReference = m_HasContextReference;
    cloned->m_HasTemplateUsage = m_HasTemplateUsage;
    cloned->m_HasCustomUsage = m_HasCustomUsage;
    
    return cloned;
}

std::string ScriptNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "ScriptNode [局部script - 属于CHTL]";
    
    if (m_HasCHTLJSSyntax) {
        oss << " [CHTL JS: " << m_CHTLJSSyntaxBlocks.size() << " blocks]";
    }
    
    if (m_HasContextReference) {
        oss << " [Context: " << m_ContextReferences.size() << " refs]";
    }
    
    if (m_HasTemplateUsage) {
        oss << " [Templates: " << m_TemplateVariableUsages.size() << "]";
    }
    
    if (m_HasCustomUsage) {
        oss << " [Customs: " << m_CustomVariableUsages.size() << "]";
    }
    
    if (!m_VariableSpecializations.empty()) {
        oss << " [VarSpec: " << m_VariableSpecializations.size() << "]";
    }
    
    if (!m_NamespaceFromUsages.empty()) {
        oss << " [From: " << m_NamespaceFromUsages.size() << "]";
    }
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出CHTL语法元素详情
    if (m_HasTemplateUsage) {
        oss << indentStr << "  Template Variables:\n";
        for (const auto& templateVar : m_TemplateVariableUsages) {
            oss << indentStr << "    " << templateVar << "\n";
        }
    }
    
    if (m_HasCustomUsage) {
        oss << indentStr << "  Custom Variables:\n";
        for (const auto& customVar : m_CustomVariableUsages) {
            oss << indentStr << "    " << customVar << "\n";
        }
    }
    
    if (!m_VariableSpecializations.empty()) {
        oss << indentStr << "  Variable Specializations:\n";
        for (const auto& varSpec : m_VariableSpecializations) {
            oss << indentStr << "    " << varSpec << "\n";
        }
    }
    
    if (m_HasContextReference) {
        oss << indentStr << "  Context References:\n";
        for (const auto& contextRef : m_ContextReferences) {
            oss << indentStr << "    " << contextRef << "\n";
        }
    }
    
    if (m_HasCHTLJSSyntax) {
        oss << indentStr << "  CHTL JS Syntax Blocks:\n";
        for (const auto& chtljsBlock : m_CHTLJSSyntaxBlocks) {
            oss << indentStr << "    " << chtljsBlock.substr(0, 50);
            if (chtljsBlock.length() > 50) oss << "...";
            oss << "\n";
        }
    }
    
    // 输出脚本内容预览
    if (!m_ScriptContent.empty()) {
        oss << indentStr << "  Content Preview: ";
        std::string preview = m_ScriptContent.substr(0, 80);
        std::replace(preview.begin(), preview.end(), '\n', ' ');
        oss << preview;
        if (m_ScriptContent.length() > 80) oss << "...";
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace CHTL