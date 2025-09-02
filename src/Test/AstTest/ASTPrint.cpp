#include "ASTPrint.h"
#include <iomanip>
#include <sstream>

namespace CHTL {
namespace Test {

void ASTPrint::PrintCHTLAST(const std::shared_ptr<CHTLBaseNode>& rootNode, const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::setw(88) << std::left << title << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    if (!rootNode) {
        std::cout << "  📝 AST为空" << std::endl;
        return;
    }
    
    std::cout << "\n🌳 AST结构:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    std::cout << GenerateCHTLASTText(rootNode) << std::endl;
    std::cout << "\n" << GenerateCHTLASTStatistics(rootNode) << std::endl;
}

void ASTPrint::PrintCHTLJSAST(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode, const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::setw(88) << std::left << title << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    if (!rootNode) {
        std::cout << "  📝 AST为空" << std::endl;
        return;
    }
    
    std::cout << "\n🌳 AST结构:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    std::cout << GenerateCHTLJSASTText(rootNode) << std::endl;
    std::cout << "\n" << GenerateCHTLJSASTStatistics(rootNode) << std::endl;
}

std::string ASTPrint::GenerateCHTLASTText(const std::shared_ptr<CHTLBaseNode>& rootNode, int indent) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream oss;
    
    // 打印当前节点
    oss << GenerateIndent(indent) << "📄 " << rootNode->GetTypeString();
    
    // 添加节点值信息
    if (!rootNode->GetValue().empty()) {
        oss << " [值: \"" << rootNode->GetValue().substr(0, 30);
        if (rootNode->GetValue().length() > 30) {
            oss << "...";
        }
        oss << "\"]";
    }
    
    oss << std::endl;
    
    // 递归打印子节点
    const auto& children = rootNode->GetChildren();
    for (const auto& child : children) {
        oss << GenerateCHTLASTText(child, indent + 1);
    }
    
    return oss.str();
}

std::string ASTPrint::GenerateCHTLJSASTText(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode, int indent) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream oss;
    
    // 打印当前节点
    oss << GenerateIndent(indent) << "📄 " << rootNode->GetTypeString();
    
    // 添加节点值信息
    if (!rootNode->GetValue().empty()) {
        oss << " [值: \"" << rootNode->GetValue().substr(0, 30);
        if (rootNode->GetValue().length() > 30) {
            oss << "...";
        }
        oss << "\"]";
    }
    
    oss << std::endl;
    
    // 递归打印子节点
    const auto& children = rootNode->GetChildren();
    for (const auto& child : children) {
        oss << GenerateCHTLJSASTText(child, indent + 1);
    }
    
    return oss.str();
}

std::string ASTPrint::GenerateCHTLASTStatistics(const std::shared_ptr<CHTLBaseNode>& rootNode) {
    if (!rootNode) {
        return "📊 AST统计: 空AST";
    }
    
    size_t nodeCount = CountCHTLASTNodes(rootNode);
    
    std::ostringstream oss;
    oss << "📊 AST统计:" << std::endl;
    oss << "  节点总数: " << nodeCount << std::endl;
    oss << "  根节点类型: " << rootNode->GetTypeString() << std::endl;
    oss << "  直接子节点数: " << rootNode->GetChildren().size() << std::endl;
    oss << "  AST深度: " << "TODO" << std::endl; // TODO: 实现深度计算
    oss << "  结构完整性: " << (ValidateCHTLASTStructure(rootNode) ? "✅ 完整" : "❌ 不完整") << std::endl;
    
    return oss.str();
}

std::string ASTPrint::GenerateCHTLJSASTStatistics(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode) {
    if (!rootNode) {
        return "📊 AST统计: 空AST";
    }
    
    size_t nodeCount = CountCHTLJSASTNodes(rootNode);
    
    std::ostringstream oss;
    oss << "📊 AST统计:" << std::endl;
    oss << "  节点总数: " << nodeCount << std::endl;
    oss << "  根节点类型: " << rootNode->GetTypeString() << std::endl;
    oss << "  直接子节点数: " << rootNode->GetChildren().size() << std::endl;
    oss << "  AST深度: " << "TODO" << std::endl; // TODO: 实现深度计算
    oss << "  结构完整性: " << (ValidateCHTLJSASTStructure(rootNode) ? "✅ 完整" : "❌ 不完整") << std::endl;
    
    return oss.str();
}

std::string ASTPrint::ExportCHTLASTAsDOT(const std::shared_ptr<CHTLBaseNode>& rootNode) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "digraph CHTL_AST {" << std::endl;
    oss << "  rankdir=TB;" << std::endl;
    oss << "  node [shape=box, style=rounded];" << std::endl;
    
    // TODO: 实现DOT格式导出
    oss << "  \"Root\" [label=\"" << EscapeDOTString(rootNode->GetTypeString()) << "\"];" << std::endl;
    
    oss << "}" << std::endl;
    
    return oss.str();
}

std::string ASTPrint::ExportCHTLJSASTAsDOT(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "digraph CHTLJS_AST {" << std::endl;
    oss << "  rankdir=TB;" << std::endl;
    oss << "  node [shape=box, style=rounded];" << std::endl;
    
    // TODO: 实现DOT格式导出
    oss << "  \"Root\" [label=\"" << EscapeDOTString(rootNode->GetTypeString()) << "\"];" << std::endl;
    
    oss << "}" << std::endl;
    
    return oss.str();
}

bool ASTPrint::ValidateCHTLASTStructure(const std::shared_ptr<CHTLBaseNode>& rootNode) {
    if (!rootNode) {
        return false;
    }
    
    // 基本验证：检查节点类型是否有效
    if (rootNode->GetTypeString().empty()) {
        return false;
    }
    
    // 递归验证子节点
    const auto& children = rootNode->GetChildren();
    for (const auto& child : children) {
        if (!ValidateCHTLASTStructure(child)) {
            return false;
        }
    }
    
    return true;
}

bool ASTPrint::ValidateCHTLJSASTStructure(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode) {
    if (!rootNode) {
        return false;
    }
    
    // 基本验证：检查节点类型是否有效
    if (rootNode->GetTypeString().empty()) {
        return false;
    }
    
    // 递归验证子节点
    const auto& children = rootNode->GetChildren();
    for (const auto& child : children) {
        if (!ValidateCHTLJSASTStructure(child)) {
            return false;
        }
    }
    
    return true;
}

size_t ASTPrint::CountCHTLASTNodes(const std::shared_ptr<CHTLBaseNode>& node) {
    if (!node) {
        return 0;
    }
    
    size_t count = 1; // 当前节点
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        count += CountCHTLASTNodes(child);
    }
    
    return count;
}

size_t ASTPrint::CountCHTLJSASTNodes(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& node) {
    if (!node) {
        return 0;
    }
    
    size_t count = 1; // 当前节点
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        count += CountCHTLJSASTNodes(child);
    }
    
    return count;
}

std::string ASTPrint::GenerateIndent(int level) {
    return std::string(level * 2, ' ');
}

std::string ASTPrint::EscapeDOTString(const std::string& str) {
    std::string result = str;
    
    // 转义DOT格式特殊字符
    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    return result;
}

} // namespace Test
} // namespace CHTL