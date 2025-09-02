#pragma once

#include "../../CHTL/CHTLNode/CHTLBaseNode.h"
#include "../../CHTL JS/CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <iostream>
#include <memory>

namespace CHTL {
namespace Test {

/**
 * AST打印工具类
 * 用于调试和可视化AST结构
 */
class ASTPrint {
public:
    /**
     * 打印CHTL AST树
     * @param rootNode 根节点
     * @param title 标题
     */
    static void PrintCHTLAST(const std::shared_ptr<CHTLBaseNode>& rootNode, const std::string& title = "CHTL AST");
    
    /**
     * 打印CHTL JS AST树
     * @param rootNode 根节点
     * @param title 标题
     */
    static void PrintCHTLJSAST(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode, const std::string& title = "CHTL JS AST");
    
    /**
     * 生成CHTL AST的文本表示
     * @param rootNode 根节点
     * @param indent 缩进级别
     * @return AST文本表示
     */
    static std::string GenerateCHTLASTText(const std::shared_ptr<CHTLBaseNode>& rootNode, int indent = 0);
    
    /**
     * 生成CHTL JS AST的文本表示
     * @param rootNode 根节点
     * @param indent 缩进级别
     * @return AST文本表示
     */
    static std::string GenerateCHTLJSASTText(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode, int indent = 0);
    
    /**
     * 生成AST统计信息
     * @param rootNode 根节点
     * @return 统计信息字符串
     */
    static std::string GenerateCHTLASTStatistics(const std::shared_ptr<CHTLBaseNode>& rootNode);
    
    /**
     * 生成CHTL JS AST统计信息
     * @param rootNode 根节点
     * @return 统计信息字符串
     */
    static std::string GenerateCHTLJSASTStatistics(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode);
    
    /**
     * 导出AST为DOT格式（用于Graphviz）
     * @param rootNode 根节点
     * @return DOT格式字符串
     */
    static std::string ExportCHTLASTAsDOT(const std::shared_ptr<CHTLBaseNode>& rootNode);
    
    /**
     * 导出CHTL JS AST为DOT格式
     * @param rootNode 根节点
     * @return DOT格式字符串
     */
    static std::string ExportCHTLJSASTAsDOT(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode);
    
    /**
     * 验证AST结构完整性
     * @param rootNode 根节点
     * @return 验证结果
     */
    static bool ValidateCHTLASTStructure(const std::shared_ptr<CHTLBaseNode>& rootNode);
    
    /**
     * 验证CHTL JS AST结构完整性
     * @param rootNode 根节点
     * @return 验证结果
     */
    static bool ValidateCHTLJSASTStructure(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& rootNode);

private:
    /**
     * 递归计算AST节点数量
     * @param node 节点
     * @return 节点数量
     */
    static size_t CountCHTLASTNodes(const std::shared_ptr<CHTLBaseNode>& node);
    
    /**
     * 递归计算CHTL JS AST节点数量
     * @param node 节点
     * @return 节点数量
     */
    static size_t CountCHTLJSASTNodes(const std::shared_ptr<CHTLJS::CHTLJSBaseNode>& node);
    
    /**
     * 生成缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    static std::string GenerateIndent(int level);
    
    /**
     * 转义DOT格式特殊字符
     * @param str 原字符串
     * @return 转义后的字符串
     */
    static std::string EscapeDOTString(const std::string& str);
};

} // namespace Test
} // namespace CHTL