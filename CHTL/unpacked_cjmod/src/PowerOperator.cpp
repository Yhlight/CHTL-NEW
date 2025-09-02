#include "CJMOD/API.h"
#include <cmath>
#include <sstream>

using namespace CHTL::CJMOD;

/**
 * 示例CJMOD模块：实现幂运算符 **
 */
class PowerOperatorModule : public ICJMODModule {
public:
    std::string GetModuleName() const override {
        return "PowerOperator";
    }
    
    std::string GetModuleVersion() const override {
        return "1.0.0";
    }
    
    bool Initialize() override {
        // 初始化模块
        return true;
    }
    
    void Cleanup() override {
        // 清理资源
    }
    
    void RegisterSyntaxExtensions() override {
        // 注册 ** 运算符的语法扩展
        // 在实际实现中，这里应该注册到CHTL JS编译器
        
        // 示例：将 a ** b 转换为 Math.pow(a, b)
    }
    
    /**
     * 转换幂运算符
     */
    static std::string TransformPowerOperator(const std::string& code) {
        // 简单的实现：查找 ** 并替换为 Math.pow
        std::string result = code;
        size_t pos = 0;
        
        while ((pos = result.find("**", pos)) != std::string::npos) {
            // 找到左操作数
            size_t leftStart = pos;
            while (leftStart > 0 && std::isalnum(result[leftStart - 1])) {
                leftStart--;
            }
            std::string leftOperand = result.substr(leftStart, pos - leftStart);
            
            // 找到右操作数
            size_t rightEnd = pos + 2;
            while (rightEnd < result.length() && std::isalnum(result[rightEnd])) {
                rightEnd++;
            }
            std::string rightOperand = result.substr(pos + 2, rightEnd - pos - 2);
            
            // 构建Math.pow调用
            std::string powCall = "Math.pow(" + leftOperand + ", " + rightOperand + ")";
            
            // 替换
            result.replace(leftStart, rightEnd - leftStart, powCall);
            
            pos = leftStart + powCall.length();
        }
        
        return result;
    }
};

// 导出模块
CJMOD_MODULE(PowerOperatorModule)