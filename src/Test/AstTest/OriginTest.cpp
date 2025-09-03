#include <iostream>
#include <memory>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/OriginNode.h"

void TestBasicOriginNodes() {
    std::cout << "\n=== 基本原始嵌入节点测试 ===" << std::endl;
    
    // 测试HTML原始嵌入
    auto htmlOrigin = CHTL::CHTLOriginNodeFactory::CreateHtmlOriginNode();
    htmlOrigin->SetRawContent("<div class=\"raw-html\">这是原始HTML代码</div>");
    
    std::cout << "HTML原始嵌入: " << htmlOrigin->ToString() << std::endl;
    std::cout << "是否为基本类型: " << (htmlOrigin->IsBasicType() ? "是" : "否") << std::endl;
    std::cout << "是否为自定义类型: " << (htmlOrigin->IsCustomType() ? "是" : "否") << std::endl;
    std::cout << "直接输出: " << htmlOrigin->GenerateDirectOutput() << std::endl;
    
    // 测试CSS原始嵌入
    auto styleOrigin = CHTL::CHTLOriginNodeFactory::CreateStyleOriginNode();
    styleOrigin->SetRawContent(".raw-css { color: red; background: blue; }");
    
    std::cout << "\nCSS原始嵌入: " << styleOrigin->ToString() << std::endl;
    std::cout << "内容长度: " << styleOrigin->GetContentLength() << std::endl;
    std::cout << "直接输出: " << styleOrigin->GenerateDirectOutput() << std::endl;
    
    // 测试JavaScript原始嵌入
    auto jsOrigin = CHTL::CHTLOriginNodeFactory::CreateJavaScriptOriginNode();
    jsOrigin->SetRawContent("console.log('这是原始JavaScript代码');");
    
    std::cout << "\nJS原始嵌入: " << jsOrigin->ToString() << std::endl;
    std::cout << "是否为空: " << (jsOrigin->IsEmpty() ? "是" : "否") << std::endl;
    std::cout << "直接输出: " << jsOrigin->GenerateDirectOutput() << std::endl;
}

void TestNamedOriginNodes() {
    std::cout << "\n=== 带名原始嵌入节点测试 ===" << std::endl;
    
    // 测试带名HTML原始嵌入
    auto namedHtml = CHTL::CHTLOriginNodeFactory::CreateHtmlOriginNode("box");
    namedHtml->SetRawContent("<section id=\"box-section\">带名HTML内容</section>");
    
    std::cout << "带名HTML原始嵌入: " << namedHtml->ToString() << std::endl;
    std::cout << "是否为带名: " << (namedHtml->IsNamed() ? "是" : "否") << std::endl;
    std::cout << "原始嵌入名称: " << namedHtml->GetOriginName() << std::endl;
    std::cout << "直接输出: " << namedHtml->GenerateDirectOutput() << std::endl;
    
    // 测试带名原始嵌入的使用
    auto htmlUsage = CHTL::CHTLOriginNodeFactory::CreateHtmlUsageNode("box");
    
    std::cout << "\n带名HTML使用: " << htmlUsage->ToString() << std::endl;
    std::cout << "类型名称: " << htmlUsage->GetTypeName() << std::endl;
    std::cout << "原始嵌入名称: " << htmlUsage->GetOriginName() << std::endl;
}

void TestCustomOriginTypes() {
    std::cout << "\n=== 自定义类型原始嵌入测试 ===" << std::endl;
    
    // 测试自定义类型@Vue
    auto vueOrigin = CHTL::CHTLOriginNodeFactory::CreateCustomOriginNode("Vue", "component");
    vueOrigin->SetRawContent("<template><div>Vue组件内容</div></template>");
    
    std::cout << "自定义Vue原始嵌入: " << vueOrigin->ToString() << std::endl;
    std::cout << "类型名称: " << vueOrigin->GetTypeName() << std::endl;
    std::cout << "是否为自定义类型: " << (vueOrigin->IsCustomType() ? "是" : "否") << std::endl;
    std::cout << "会创建隐式配置: " << (vueOrigin->WillCreateImplicitConfig() ? "是" : "否") << std::endl;
    std::cout << "直接输出: " << vueOrigin->GenerateDirectOutput() << std::endl;
    
    // 测试自定义类型@Markdown
    auto markdownOrigin = CHTL::CHTLOriginNodeFactory::CreateCustomOriginNode("Markdown", "readme");
    markdownOrigin->SetRawContent("# 标题\n\n这是Markdown内容\n\n- 列表项1\n- 列表项2");
    
    std::cout << "\n自定义Markdown原始嵌入: " << markdownOrigin->ToString() << std::endl;
    std::cout << "类型名称: " << markdownOrigin->GetTypeName() << std::endl;
    std::cout << "直接输出: " << markdownOrigin->GenerateDirectOutput() << std::endl;
    
    // 测试自定义类型使用
    auto vueUsage = CHTL::CHTLOriginNodeFactory::CreateCustomUsageNode("Vue", "component");
    std::cout << "\n自定义Vue使用: " << vueUsage->ToString() << std::endl;
}

void TestImplicitConfigManager() {
    std::cout << "\n=== 隐式配置管理器测试 ===" << std::endl;
    
    // 查看已注册的自定义类型
    const auto& registeredTypes = CHTL::CHTLImplicitConfigManager::GetRegisteredTypes();
    std::cout << "已注册的自定义类型数量: " << registeredTypes.size() << std::endl;
    
    for (const auto& type : registeredTypes) {
        std::cout << "注册类型: " << type << std::endl;
        
        // 获取隐式配置
        std::string config = CHTL::CHTLImplicitConfigManager::GetTypeConfig(type);
        if (!config.empty()) {
            std::cout << "隐式配置:\n" << config << std::endl;
        }
    }
    
    // 测试新类型注册
    CHTL::CHTLImplicitConfigManager::RegisterCustomType("@React");
    std::cout << "@React是否已注册: " << (CHTL::CHTLImplicitConfigManager::IsCustomTypeRegistered("@React") ? "是" : "否") << std::endl;
    
    // 获取所有隐式配置
    auto allConfigs = CHTL::CHTLImplicitConfigManager::GetAllImplicitConfigs();
    std::cout << "隐式配置总数: " << allConfigs.size() << std::endl;
}

void TestOriginValidation() {
    std::cout << "\n=== 原始嵌入验证测试 ===" << std::endl;
    
    // 测试类型名称验证
    std::vector<std::string> typeNames = {"@Html", "@Vue", "@Markdown", "Html", "@123", "@"};
    
    for (const auto& typeName : typeNames) {
        bool isValid = CHTL::CHTLOriginNode::IsValidTypeName(typeName);
        std::cout << "类型名称 \"" << typeName << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
    
    // 测试原始嵌入名称验证
    std::vector<std::string> originNames = {"box", "component", "123invalid", "_valid", "", "valid-name"};
    
    for (const auto& originName : originNames) {
        bool isValid = CHTL::CHTLOriginNode::IsValidOriginName(originName);
        std::cout << "原始嵌入名称 \"" << originName << "\": " << (isValid ? "有效" : "无效") << std::endl;
    }
}

void TestOriginDirectOutput() {
    std::cout << "\n=== 原始嵌入直接输出测试 ===" << std::endl;
    
    // 测试复杂的原始内容直接输出
    auto complexOrigin = CHTL::CHTLOriginNodeFactory::CreateCustomOriginNode("Vue", "complexComponent");
    
    std::string complexContent = R"(<template>
  <div class="complex-component">
    <h1>{{ title }}</h1>
    <p v-for="item in items" :key="item.id">
      {{ item.text }}
    </p>
    <script>
      export default {
        data() {
          return {
            title: 'Vue组件标题',
            items: [
              { id: 1, text: '项目1' },
              { id: 2, text: '项目2' }
            ]
          }
        }
      }
    </script>
  </div>
</template>)";
    
    complexOrigin->SetRawContent(complexContent);
    
    std::cout << "复杂原始嵌入: " << complexOrigin->ToString() << std::endl;
    std::cout << "内容长度: " << complexOrigin->GetContentLength() << " 字符" << std::endl;
    std::cout << "直接输出（前100字符）: " << complexOrigin->GenerateDirectOutput().substr(0, 100) << "..." << std::endl;
    
    // 验证直接输出的核心特性：绝对不会进行处理
    std::cout << "\n验证直接输出特性:" << std::endl;
    std::cout << "原始内容 == 直接输出: " << (complexOrigin->GetRawContent() == complexOrigin->GenerateDirectOutput() ? "是" : "否") << std::endl;
}

int main() {
    try {
        std::cout << "CHTL第五阶段原始嵌入系统测试开始..." << std::endl;
        
        // 执行第五阶段原始嵌入系统测试
        TestBasicOriginNodes();
        TestNamedOriginNodes();
        TestCustomOriginTypes();
        TestImplicitConfigManager();
        TestOriginValidation();
        TestOriginDirectOutput();
        
        std::cout << "\n第五阶段原始嵌入系统测试完成！" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试错误: " << e.what() << std::endl;
        return 1;
    }
}