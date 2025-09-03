#pragma once

#include "BaseNode.h"

namespace CHTL {

    // 注释类型枚举
    enum class CHTLCommentType {
        SINGLE_LINE,            // 单行注释 //
        MULTI_LINE,             // 多行注释 /**/
        GENERATOR               // 生成器注释 --
    };

    // 注释节点类
    // 专门处理CHTL注释语法的AST节点
    class CHTLCommentNode : public CHTLBaseNode {
    private:
        CHTLCommentType commentType;                // 注释类型
        std::string commentContent;                 // 注释内容
        bool shouldGenerate;                        // 是否应该生成到输出中

    public:
        // 构造函数
        CHTLCommentNode(CHTLCommentType type, const std::string& content);
        ~CHTLCommentNode() override = default;

        // 注释类型管理
        CHTLCommentType GetCommentType() const;                 // 获取注释类型
        void SetCommentType(CHTLCommentType type);              // 设置注释类型
        
        // 注释内容管理
        const std::string& GetCommentContent() const;          // 获取注释内容
        void SetCommentContent(const std::string& content);    // 设置注释内容
        void AppendComment(const std::string& text);           // 追加注释内容
        
        // 生成控制
        bool ShouldGenerate() const;                           // 是否应该生成
        void SetShouldGenerate(bool generate);                 // 设置是否生成
        
        // 注释转换方法
        std::string ToHTMLComment() const;                     // 转换为HTML注释
        std::string ToCSSComment() const;                      // 转换为CSS注释
        std::string ToJSComment() const;                       // 转换为JavaScript注释
        
        // 重写虚方法
        std::string ToString() const override;                 // 转换为字符串表示
        std::shared_ptr<CHTLBaseNode> Clone() const override;  // 克隆节点
        bool Validate() const override;                        // 验证节点有效性
        
        // 注释处理方法
        void TrimContent();                                    // 去除内容首尾空白
        bool IsEmpty() const;                                  // 是否为空注释
        size_t GetContentLength() const;                       // 获取内容长度
        
        // 静态工具方法
        static std::string CommentTypeToString(CHTLCommentType type); // 注释类型转字符串
        static CHTLCommentType ParseCommentType(const std::string& commentText); // 解析注释类型
        static bool IsValidCommentContent(const std::string& content); // 验证注释内容是否有效
    };

} // namespace CHTL