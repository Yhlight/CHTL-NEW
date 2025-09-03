#include "CommentNode.h"
#include <sstream>

namespace CHTL {

    CHTLCommentNode::CHTLCommentNode(CHTLCommentType type, const std::string& content) 
        : CHTLBaseNode(CHTLNodeType::COMMENT, "comment", content), 
          commentType(type), commentContent(content) {
        
        // 根据注释类型确定是否应该生成
        shouldGenerate = (type == CHTLCommentType::GENERATOR);
    }

    // 注释类型管理实现
    CHTLCommentType CHTLCommentNode::GetCommentType() const {
        return commentType;
    }

    void CHTLCommentNode::SetCommentType(CHTLCommentType type) {
        commentType = type;
        shouldGenerate = (type == CHTLCommentType::GENERATOR);
    }

    // 注释内容管理实现
    const std::string& CHTLCommentNode::GetCommentContent() const {
        return commentContent;
    }

    void CHTLCommentNode::SetCommentContent(const std::string& content) {
        commentContent = content;
        SetNodeValue(content);
    }

    void CHTLCommentNode::AppendComment(const std::string& text) {
        commentContent += text;
        SetNodeValue(commentContent);
    }

    // 生成控制实现
    bool CHTLCommentNode::ShouldGenerate() const {
        return shouldGenerate;
    }

    void CHTLCommentNode::SetShouldGenerate(bool generate) {
        shouldGenerate = generate;
    }

    // 注释转换方法实现
    std::string CHTLCommentNode::ToHTMLComment() const {
        if (!shouldGenerate) return "";
        return "<!-- " + commentContent + " -->";
    }

    std::string CHTLCommentNode::ToCSSComment() const {
        if (!shouldGenerate) return "";
        
        if (commentType == CHTLCommentType::SINGLE_LINE) {
            return "/* " + commentContent + " */";
        } else {
            return "/* " + commentContent + " */";
        }
    }

    std::string CHTLCommentNode::ToJSComment() const {
        if (!shouldGenerate) return "";
        
        if (commentType == CHTLCommentType::SINGLE_LINE) {
            return "// " + commentContent;
        } else {
            return "/* " + commentContent + " */";
        }
    }

    // 重写虚方法实现
    std::string CHTLCommentNode::ToString() const {
        std::stringstream ss;
        ss << "Comment[" << CommentTypeToString(commentType) << "]";
        ss << " \"" << commentContent.substr(0, 20);
        if (commentContent.length() > 20) ss << "...";
        ss << "\"";
        if (shouldGenerate) ss << " (generate)";
        return ss.str();
    }

    std::shared_ptr<CHTLBaseNode> CHTLCommentNode::Clone() const {
        auto cloned = std::make_shared<CHTLCommentNode>(commentType, commentContent);
        cloned->shouldGenerate = shouldGenerate;
        
        return cloned;
    }

    bool CHTLCommentNode::Validate() const {
        // 注释节点验证
        return !commentContent.empty() && CHTLBaseNode::Validate();
    }

    // 注释处理方法实现
    void CHTLCommentNode::TrimContent() {
        size_t start = commentContent.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            commentContent.clear();
            SetNodeValue("");
            return;
        }
        
        size_t end = commentContent.find_last_not_of(" \t\n\r");
        commentContent = commentContent.substr(start, end - start + 1);
        SetNodeValue(commentContent);
    }

    bool CHTLCommentNode::IsEmpty() const {
        return commentContent.empty();
    }

    size_t CHTLCommentNode::GetContentLength() const {
        return commentContent.length();
    }

    // 静态工具方法实现
    std::string CHTLCommentNode::CommentTypeToString(CHTLCommentType type) {
        switch (type) {
            case CHTLCommentType::SINGLE_LINE: return "SINGLE_LINE";
            case CHTLCommentType::MULTI_LINE: return "MULTI_LINE";
            case CHTLCommentType::GENERATOR: return "GENERATOR";
            default: return "UNKNOWN";
        }
    }

    CHTLCommentType CHTLCommentNode::ParseCommentType(const std::string& commentText) {
        if (commentText.substr(0, 2) == "//") {
            return CHTLCommentType::SINGLE_LINE;
        } else if (commentText.substr(0, 2) == "/*" && commentText.substr(commentText.length() - 2) == "*/") {
            return CHTLCommentType::MULTI_LINE;
        } else if (commentText.substr(0, 2) == "--") {
            return CHTLCommentType::GENERATOR;
        }
        
        return CHTLCommentType::SINGLE_LINE; // 默认类型
    }

    bool CHTLCommentNode::IsValidCommentContent(const std::string& content) {
        // 注释内容验证（基础版本）
        // 检查是否包含不安全内容
        return content.find("</script>") == std::string::npos &&
               content.find("</style>") == std::string::npos;
    }

} // namespace CHTL