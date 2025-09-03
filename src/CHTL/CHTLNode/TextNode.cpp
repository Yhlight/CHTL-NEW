#include "TextNode.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

    CHTLTextNode::CHTLTextNode(const std::string& content) 
        : CHTLBaseNode(CHTLNodeType::TEXT, "text", content), 
          textContent(content), isUnquoted(false), hasVariableReference(false) {
        
        // 检查是否包含变量引用
        std::regex varRegex(R"(\$\{[^}]+\})"); // 简单的变量引用模式
        std::smatch matches;
        
        if (std::regex_search(content, matches, varRegex)) {
            hasVariableReference = true;
            
            // 提取所有变量引用
            std::sregex_iterator iter(content.begin(), content.end(), varRegex);
            std::sregex_iterator end;
            
            for (; iter != end; ++iter) {
                std::string varRef = iter->str();
                // 移除${}包装
                if (varRef.length() > 3) {
                    std::string varName = varRef.substr(2, varRef.length() - 3);
                    variableReferences.push_back(varName);
                }
            }
        }
    }

    // 文本内容管理实现
    const std::string& CHTLTextNode::GetTextContent() const {
        return textContent;
    }

    void CHTLTextNode::SetTextContent(const std::string& content) {
        textContent = content;
        SetNodeValue(content);
        
        // 重新检查变量引用
        variableReferences.clear();
        hasVariableReference = false;
        
        std::regex varRegex(R"(\$\{[^}]+\})");
        if (std::regex_search(content, varRegex)) {
            hasVariableReference = true;
            
            std::sregex_iterator iter(content.begin(), content.end(), varRegex);
            std::sregex_iterator end;
            
            for (; iter != end; ++iter) {
                std::string varRef = iter->str();
                if (varRef.length() > 3) {
                    std::string varName = varRef.substr(2, varRef.length() - 3);
                    variableReferences.push_back(varName);
                }
            }
        }
    }

    void CHTLTextNode::AppendText(const std::string& text) {
        textContent += text;
        SetNodeValue(textContent);
    }

    void CHTLTextNode::ClearText() {
        textContent.clear();
        SetNodeValue("");
        variableReferences.clear();
        hasVariableReference = false;
    }

    // 字面量类型管理实现
    bool CHTLTextNode::IsUnquoted() const {
        return isUnquoted;
    }

    void CHTLTextNode::SetUnquoted(bool unquoted) {
        isUnquoted = unquoted;
    }

    // 变量引用管理实现
    bool CHTLTextNode::HasVariableReference() const {
        return hasVariableReference;
    }

    const std::vector<std::string>& CHTLTextNode::GetVariableReferences() const {
        return variableReferences;
    }

    void CHTLTextNode::AddVariableReference(const std::string& varName) {
        if (std::find(variableReferences.begin(), variableReferences.end(), varName) == variableReferences.end()) {
            variableReferences.push_back(varName);
            hasVariableReference = true;
        }
    }

    void CHTLTextNode::RemoveVariableReference(const std::string& varName) {
        auto it = std::find(variableReferences.begin(), variableReferences.end(), varName);
        if (it != variableReferences.end()) {
            variableReferences.erase(it);
            hasVariableReference = !variableReferences.empty();
        }
    }

    void CHTLTextNode::ClearVariableReferences() {
        variableReferences.clear();
        hasVariableReference = false;
    }

    // 文本处理方法实现
    std::string CHTLTextNode::ProcessVariableReferences() const {
        if (!hasVariableReference) {
            return textContent;
        }
        
        std::string result = textContent;
        
        // 简单的变量替换（实际实现需要结合GlobalMap）
        for (const auto& varName : variableReferences) {
            std::string placeholder = "${" + varName + "}";
            std::string replacement = "[VAR:" + varName + "]"; // 临时替换
            
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), replacement);
                pos += replacement.length();
            }
        }
        
        return result;
    }

    std::string CHTLTextNode::EscapeHTML() const {
        std::string result = textContent;
        
        // HTML实体转义
        std::vector<std::pair<std::string, std::string>> escapes = {
            {"&", "&amp;"},
            {"<", "&lt;"},
            {">", "&gt;"},
            {"\"", "&quot;"},
            {"'", "&#39;"}
        };
        
        for (const auto& escape : escapes) {
            size_t pos = 0;
            while ((pos = result.find(escape.first, pos)) != std::string::npos) {
                result.replace(pos, escape.first.length(), escape.second);
                pos += escape.second.length();
            }
        }
        
        return result;
    }

    // 重写虚方法实现
    std::string CHTLTextNode::ToString() const {
        std::stringstream ss;
        ss << "Text[\"" << textContent.substr(0, 30);
        if (textContent.length() > 30) ss << "...";
        ss << "\"]";
        if (isUnquoted) ss << " (unquoted)";
        if (hasVariableReference) ss << " (has vars)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLTextNode::Clone() const {
        auto cloned = std::make_shared<CHTLTextNode>(textContent);
        cloned->isUnquoted = isUnquoted;
        cloned->hasVariableReference = hasVariableReference;
        cloned->variableReferences = variableReferences;
        
        return cloned;
    }

    bool CHTLTextNode::Validate() const {
        // 文本节点验证：内容不能为空（除非是特殊情况）
        return !textContent.empty() || GetChildCount() == 0;
    }

    // 文本节点特有方法实现
    bool CHTLTextNode::IsEmpty() const {
        return textContent.empty();
    }

    size_t CHTLTextNode::GetTextLength() const {
        return textContent.length();
    }

    bool CHTLTextNode::ContainsOnlyWhitespace() const {
        return std::all_of(textContent.begin(), textContent.end(), 
                          [](char c) { return std::isspace(c); });
    }

    void CHTLTextNode::TrimWhitespace() {
        // 去除首尾空白
        size_t start = textContent.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            textContent.clear();
            return;
        }
        
        size_t end = textContent.find_last_not_of(" \t\n\r");
        textContent = textContent.substr(start, end - start + 1);
        SetNodeValue(textContent);
    }

    void CHTLTextNode::NormalizeWhitespace() {
        // 标准化空白字符
        std::regex whitespaceRegex(R"(\s+)");
        textContent = std::regex_replace(textContent, whitespaceRegex, " ");
        SetNodeValue(textContent);
    }

    // 静态工具方法实现
    bool CHTLTextNode::IsValidTextContent(const std::string& content) {
        // 文本内容验证（基础版本）
        return true; // 文本内容基本都是有效的
    }

    std::string CHTLTextNode::SanitizeText(const std::string& content) {
        // 清理文本内容
        std::string result = content;
        
        // 移除危险字符或序列
        std::regex dangerousRegex(R"(<script[^>]*>.*?</script>)");
        result = std::regex_replace(result, dangerousRegex, "");
        
        return result;
    }

} // namespace CHTL