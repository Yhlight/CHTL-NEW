#include "Common/SyntaxConstraint.h"
#include "Common/Logger.h"
#include <sstream>

namespace CHTL {

SyntaxConstraint::SyntaxConstraint(bool strict) 
    : currentContext(Context::CHTL), strictMode(strict) {
    contextStack.push_back(currentContext);
    UpdateAllowedFeatures();
}

void SyntaxConstraint::EnterContext(Context ctx) {
    contextStack.push_back(ctx);
    currentContext = ctx;
    UpdateAllowedFeatures();
    
    LOG_DEBUG("进入语法上下文: " + GetContextName(ctx));
}

void SyntaxConstraint::ExitContext() {
    if (contextStack.size() > 1) {
        contextStack.pop_back();
        currentContext = contextStack.back();
        UpdateAllowedFeatures();
        
        LOG_DEBUG("退出语法上下文，返回到: " + GetContextName(currentContext));
    }
}

void SyntaxConstraint::UpdateAllowedFeatures() {
    allowedFeatures.clear();
    
    switch (currentContext) {
        case Context::CHTL:
            // CHTL特有功能
            allowedFeatures.insert(Feature::UseStatement);
            allowedFeatures.insert(Feature::TemplateDeclaration);
            allowedFeatures.insert(Feature::CustomDeclaration);
            allowedFeatures.insert(Feature::OriginEmbed);
            allowedFeatures.insert(Feature::ImportStatement);
            allowedFeatures.insert(Feature::NamespaceDeclaration);
            allowedFeatures.insert(Feature::ConfigurationGroup);
            allowedFeatures.insert(Feature::ExceptConstraint);
            allowedFeatures.insert(Feature::LocalStyleBlock);
            allowedFeatures.insert(Feature::LocalScriptBlock);
            allowedFeatures.insert(Feature::CEEquivalence);
            allowedFeatures.insert(Feature::UnquotedLiteral);
            
            // 共享功能
            allowedFeatures.insert(Feature::ElementDeclaration);
            allowedFeatures.insert(Feature::AttributeAssignment);
            allowedFeatures.insert(Feature::TextNode);
            allowedFeatures.insert(Feature::Comments);
            allowedFeatures.insert(Feature::StringLiteral);
            allowedFeatures.insert(Feature::NumberLiteral);
            allowedFeatures.insert(Feature::VariableReference);
            break;
            
        case Context::CHTLJS:
            // CHTL JS特有功能
            allowedFeatures.insert(Feature::EnhancedSelector);
            allowedFeatures.insert(Feature::ArrowOperator);
            allowedFeatures.insert(Feature::ListenBlock);
            allowedFeatures.insert(Feature::DelegateBlock);
            allowedFeatures.insert(Feature::AnimateBlock);
            allowedFeatures.insert(Feature::VirtualObject);
            allowedFeatures.insert(Feature::EventBinding);
            allowedFeatures.insert(Feature::ModuleImport);
            allowedFeatures.insert(Feature::CJMODExtension);
            
            // 共享功能
            allowedFeatures.insert(Feature::ElementDeclaration);
            allowedFeatures.insert(Feature::AttributeAssignment);
            allowedFeatures.insert(Feature::TextNode);
            allowedFeatures.insert(Feature::Comments);
            allowedFeatures.insert(Feature::StringLiteral);
            allowedFeatures.insert(Feature::NumberLiteral);
            allowedFeatures.insert(Feature::VariableReference);
            break;
            
        case Context::Script:
            // 局部script块中可以使用增强选择器和引用
            allowedFeatures.insert(Feature::EnhancedSelector);
            allowedFeatures.insert(Feature::VariableReference);
            allowedFeatures.insert(Feature::Comments);
            // 注意：script块中的内容主要是JavaScript，由JS编译器处理
            break;
            
        case Context::Style:
            // 局部style块中可以使用变量引用
            allowedFeatures.insert(Feature::VariableReference);
            allowedFeatures.insert(Feature::Comments);
            // 注意：style块中的内容主要是CSS，由CSS编译器处理
            break;
            
        case Context::Module:
            // 模块导入块
            allowedFeatures.insert(Feature::ModuleImport);
            allowedFeatures.insert(Feature::Comments);
            break;
    }
}

bool SyntaxConstraint::IsFeatureAllowed(Feature feature) const {
    return allowedFeatures.find(feature) != allowedFeatures.end();
}

std::string SyntaxConstraint::GetFeatureError(Feature feature) const {
    std::string featureName = GetFeatureName(feature);
    std::string contextName = GetContextName(currentContext);
    
    if (IsFeatureAllowed(feature)) {
        return "";
    }
    
    // 生成详细的错误信息
    std::string error = "语法错误: " + featureName + " 不能在 " + contextName + " 上下文中使用";
    
    // 添加建议
    switch (feature) {
        case Feature::EnhancedSelector:
            if (currentContext == Context::CHTL) {
                error += "\n提示: {{选择器}}语法只能在CHTL JS文件或局部script块中使用";
            }
            break;
            
        case Feature::ArrowOperator:
            if (currentContext == Context::CHTL) {
                error += "\n提示: ->操作符是CHTL JS特有语法，不能在CHTL中使用";
            }
            break;
            
        case Feature::LocalScriptBlock:
            if (currentContext == Context::CHTLJS) {
                error += "\n提示: script {}块是CHTL语法，不能在CHTL JS中使用";
            }
            break;
            
        case Feature::UseStatement:
            if (currentContext == Context::CHTLJS) {
                error += "\n提示: use语句是CHTL特有语法，CHTL JS使用module {}进行导入";
            }
            break;
            
        default:
            break;
    }
    
    return error;
}

std::string SyntaxConstraint::GetContextName(Context ctx) {
    switch (ctx) {
        case Context::CHTL: return "CHTL";
        case Context::CHTLJS: return "CHTL JS";
        case Context::Script: return "局部script";
        case Context::Style: return "局部style";
        case Context::Module: return "模块导入";
        default: return "未知";
    }
}

std::string SyntaxConstraint::GetFeatureName(Feature feature) {
    switch (feature) {
        // CHTL特有
        case Feature::UseStatement: return "use语句";
        case Feature::TemplateDeclaration: return "[Template]声明";
        case Feature::CustomDeclaration: return "[Custom]声明";
        case Feature::OriginEmbed: return "[Origin]嵌入";
        case Feature::ImportStatement: return "[Import]导入";
        case Feature::NamespaceDeclaration: return "[Namespace]命名空间";
        case Feature::ConfigurationGroup: return "[Configuration]配置组";
        case Feature::ExceptConstraint: return "except约束";
        case Feature::LocalStyleBlock: return "style {}块";
        case Feature::LocalScriptBlock: return "script {}块";
        case Feature::CEEquivalence: return "CE等价语法(:和=)";
        case Feature::UnquotedLiteral: return "无引号字面量";
        
        // CHTL JS特有
        case Feature::EnhancedSelector: return "{{选择器}}增强语法";
        case Feature::ArrowOperator: return "->操作符";
        case Feature::ListenBlock: return "listen {}块";
        case Feature::DelegateBlock: return "delegate {}块";
        case Feature::AnimateBlock: return "animate {}块";
        case Feature::VirtualObject: return "vir虚对象";
        case Feature::EventBinding: return "&->事件绑定";
        case Feature::ModuleImport: return "module {}导入";
        case Feature::CJMODExtension: return "CJMOD扩展语法";
        
        // 共享
        case Feature::ElementDeclaration: return "元素声明";
        case Feature::AttributeAssignment: return "属性赋值";
        case Feature::TextNode: return "text {}节点";
        case Feature::Comments: return "注释";
        case Feature::StringLiteral: return "字符串字面量";
        case Feature::NumberLiteral: return "数字字面量";
        case Feature::VariableReference: return "变量引用";
        
        default: return "未知特性";
    }
}

bool SyntaxConstraint::ValidateSyntax(Feature feature, std::string& error) {
    if (IsFeatureAllowed(feature)) {
        error.clear();
        return true;
    }
    
    error = GetFeatureError(feature);
    
    if (strictMode) {
        LOG_ERROR(error);
    } else {
        LOG_INFO(error);
    }
    
    return false;
}

void SyntaxConstraint::Reset() {
    currentContext = Context::CHTL;
    contextStack.clear();
    contextStack.push_back(currentContext);
    UpdateAllowedFeatures();
}

// SyntaxBoundaryChecker实现

SyntaxBoundaryChecker::SyntaxBoundaryChecker() 
    : constraint(std::make_shared<SyntaxConstraint>(true)) {
}

void SyntaxBoundaryChecker::RecordViolation(const std::string& message, int line, int column) {
    std::stringstream ss;
    
    if (line > 0) {
        ss << "[行 " << line;
        if (column > 0) {
            ss << ", 列 " << column;
        }
        ss << "] ";
    }
    
    ss << message;
    violations.push_back(ss.str());
    
    LOG_ERROR("语法边界违规: " + ss.str());
}

std::string SyntaxBoundaryChecker::GenerateViolationReport() const {
    if (violations.empty()) {
        return "没有发现语法边界违规";
    }
    
    std::stringstream report;
    report << "发现 " << violations.size() << " 个语法边界违规:\n";
    report << "=====================================\n";
    
    for (size_t i = 0; i < violations.size(); ++i) {
        report << (i + 1) << ". " << violations[i] << "\n";
    }
    
    report << "=====================================\n";
    report << "建议: 请确保CHTL和CHTL JS的语法不要混用\n";
    
    return report.str();
}

} // namespace CHTL