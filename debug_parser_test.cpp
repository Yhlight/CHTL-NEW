#include "src/CHTL/CHTLParser/CHTLParser.h"
#include <iostream>

using namespace CHTL;

int main() {
    std::string testCode = R"(
html
{
    head
    {
        title
        {
            text
            {
                Simple Test
            }
        }
    }
    
    body
    {
        div
        {
            text
            {
                Hello CHTL!
            }
        }
    }
}
)";

    std::cout << "🔍 调试CHTL解析器" << std::endl;
    std::cout << "📝 测试代码长度: " << testCode.length() << " 字符" << std::endl;
    
    CHTLParser parser(testCode);
    auto result = parser.Parse();
    
    std::cout << "📋 解析结果:" << std::endl;
    std::cout << "   成功: " << (result.IsSuccess ? "是" : "否") << std::endl;
    
    if (!result.IsSuccess) {
        std::cout << "   错误: " << result.ErrorMessage << std::endl;
    }
    
    if (result.RootNode) {
        std::cout << "   根节点: " << result.RootNode->GetContent() << std::endl;
        std::cout << "   子节点数量: " << result.RootNode->GetChildren().size() << std::endl;
    } else {
        std::cout << "   根节点: nullptr" << std::endl;
    }
    
    return 0;
}