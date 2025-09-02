#include "TokenPrint.h"
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace CHTL {
namespace Test {

void TokenPrint::PrintCHTLTokens(const std::vector<CHTLToken>& tokens, const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::setw(88) << std::left << title << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    if (tokens.empty()) {
        std::cout << "  📝 Token流为空" << std::endl;
        return;
    }
    
    std::cout << "\n📊 Token统计: " << tokens.size() << " 个Token" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        PrintCHTLToken(tokens[i], i);
    }
    
    std::cout << "\n" << GenerateCHTLTokenStatistics(tokens) << std::endl;
}

void TokenPrint::PrintCHTLJSTokens(const std::vector<CHTLJS::CHTLJSToken>& tokens, const std::string& title) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::setw(88) << std::left << title << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    if (tokens.empty()) {
        std::cout << "  📝 Token流为空" << std::endl;
        return;
    }
    
    std::cout << "\n📊 Token统计: " << tokens.size() << " 个Token" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        PrintCHTLJSToken(tokens[i], i);
    }
    
    std::cout << "\n" << GenerateCHTLJSTokenStatistics(tokens) << std::endl;
}

void TokenPrint::PrintCHTLToken(const CHTLToken& token, size_t index) {
    std::cout << "  " << std::setw(3) << index << " | ";
    std::cout << std::setw(20) << std::left << FormatCHTLTokenType(token.Type) << " | ";
    std::cout << std::setw(6) << token.Line << ":" << std::setw(3) << token.Column << " | ";
    std::cout << "\"" << TruncateString(token.Value) << "\"" << std::endl;
}

void TokenPrint::PrintCHTLJSToken(const CHTLJS::CHTLJSToken& token, size_t index) {
    std::cout << "  " << std::setw(3) << index << " | ";
    std::cout << std::setw(20) << std::left << FormatCHTLJSTokenType(token.Type) << " | ";
    std::cout << std::setw(6) << token.Line << ":" << std::setw(3) << token.Column << " | ";
    std::cout << "\"" << TruncateString(token.Value) << "\"" << std::endl;
}

std::string TokenPrint::GenerateCHTLTokenStatistics(const std::vector<CHTLToken>& tokens) {
    std::unordered_map<CHTLTokenType, size_t> typeCount;
    
    for (const auto& token : tokens) {
        typeCount[token.Type]++;
    }
    
    std::ostringstream oss;
    oss << "📈 Token类型统计:" << std::endl;
    
    for (const auto& pair : typeCount) {
        oss << "  " << FormatCHTLTokenType(pair.first) << ": " << pair.second << " 个" << std::endl;
    }
    
    return oss.str();
}

std::string TokenPrint::GenerateCHTLJSTokenStatistics(const std::vector<CHTLJS::CHTLJSToken>& tokens) {
    std::unordered_map<CHTLJS::CHTLJSTokenType, size_t> typeCount;
    
    for (const auto& token : tokens) {
        typeCount[token.Type]++;
    }
    
    std::ostringstream oss;
    oss << "📈 Token类型统计:" << std::endl;
    
    for (const auto& pair : typeCount) {
        oss << "  " << FormatCHTLJSTokenType(pair.first) << ": " << pair.second << " 个" << std::endl;
    }
    
    return oss.str();
}

std::string TokenPrint::ExportCHTLTokensAsTable(const std::vector<CHTLToken>& tokens) {
    std::ostringstream oss;
    
    oss << "索引\t类型\t行号\t列号\t值" << std::endl;
    oss << "────────────────────────────────────────────────────────────────" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        oss << i << "\t" << FormatCHTLTokenType(token.Type) << "\t" 
            << token.Line << "\t" << token.Column << "\t\"" << token.Value << "\"" << std::endl;
    }
    
    return oss.str();
}

std::string TokenPrint::ExportCHTLJSTokensAsTable(const std::vector<CHTLJS::CHTLJSToken>& tokens) {
    std::ostringstream oss;
    
    oss << "索引\t类型\t行号\t列号\t值" << std::endl;
    oss << "────────────────────────────────────────────────────────────────" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        oss << i << "\t" << FormatCHTLJSTokenType(token.Type) << "\t" 
            << token.Line << "\t" << token.Column << "\t\"" << token.Value << "\"" << std::endl;
    }
    
    return oss.str();
}

std::string TokenPrint::FormatCHTLTokenType(CHTLTokenType type) {
    switch (type) {
        case CHTLTokenType::IDENTIFIER: return "标识符";
        case CHTLTokenType::STRING_LITERAL: return "字符串字面量";
        case CHTLTokenType::NUMBER: return "数字";
        case CHTLTokenType::LEFT_BRACKET: return "左方括号";
        case CHTLTokenType::RIGHT_BRACKET: return "右方括号";
        case CHTLTokenType::LEFT_BRACE: return "左大括号";
        case CHTLTokenType::RIGHT_BRACE: return "右大括号";
        case CHTLTokenType::AT_SYMBOL: return "@符号";
        case CHTLTokenType::COLON: return "冒号";
        case CHTLTokenType::SEMICOLON: return "分号";
        case CHTLTokenType::COMMA: return "逗号";
        case CHTLTokenType::DOT: return "点";
        case CHTLTokenType::EQUALS: return "等号";
        case CHTLTokenType::TEMPLATE: return "Template关键字";
        case CHTLTokenType::CUSTOM: return "Custom关键字";
        case CHTLTokenType::ORIGIN: return "Origin关键字";
        case CHTLTokenType::CONFIGURATION: return "Configuration关键字";
        case CHTLTokenType::IMPORT: return "Import关键字";
        case CHTLTokenType::NAMESPACE: return "Namespace关键字";
        case CHTLTokenType::EXCEPT: return "Except关键字";
        case CHTLTokenType::TEXT: return "Text关键字";
        case CHTLTokenType::STYLE: return "Style关键字";
        case CHTLTokenType::SCRIPT: return "Script关键字";
        case CHTLTokenType::WHITESPACE: return "空白字符";
        case CHTLTokenType::NEWLINE: return "换行符";
        case CHTLTokenType::EOF_TOKEN: return "文件结束";
        case CHTLTokenType::UNKNOWN: return "未知";
        default: return "未定义";
    }
}

std::string TokenPrint::FormatCHTLJSTokenType(CHTLJS::CHTLJSTokenType type) {
    switch (type) {
        case CHTLJS::CHTLJSTokenType::IDENTIFIER: return "标识符";
        case CHTLJS::CHTLJSTokenType::STRING_LITERAL: return "字符串字面量";
        case CHTLJS::CHTLJSTokenType::NUMBER: return "数字";
        case CHTLJS::CHTLJSTokenType::LEFT_BRACE: return "左大括号";
        case CHTLJS::CHTLJSTokenType::RIGHT_BRACE: return "右大括号";
        case CHTLJS::CHTLJSTokenType::LEFT_PAREN: return "左圆括号";
        case CHTLJS::CHTLJSTokenType::RIGHT_PAREN: return "右圆括号";
        case CHTLJS::CHTLJSTokenType::ARROW: return "箭头操作符";
        case CHTLJS::CHTLJSTokenType::EVENT_BIND: return "事件绑定操作符";
        case CHTLJS::CHTLJSTokenType::ENHANCED_SELECTOR: return "增强选择器";
        case CHTLJS::CHTLJSTokenType::MODULE: return "Module关键字";
        case CHTLJS::CHTLJSTokenType::SCRIPT: return "Script关键字";
        case CHTLJS::CHTLJSTokenType::VIR: return "Vir关键字";
        case CHTLJS::CHTLJSTokenType::LISTEN: return "Listen关键字";
        case CHTLJS::CHTLJSTokenType::DELEGATE: return "Delegate关键字";
        case CHTLJS::CHTLJSTokenType::ANIMATE: return "Animate关键字";
        case CHTLJS::CHTLJSTokenType::FUNCTION: return "Function关键字";
        case CHTLJS::CHTLJSTokenType::VAR: return "Var关键字";
        case CHTLJS::CHTLJSTokenType::LET: return "Let关键字";
        case CHTLJS::CHTLJSTokenType::CONST: return "Const关键字";
        case CHTLJS::CHTLJSTokenType::WHITESPACE: return "空白字符";
        case CHTLJS::CHTLJSTokenType::NEWLINE: return "换行符";
        case CHTLJS::CHTLJSTokenType::EOF_TOKEN: return "文件结束";
        case CHTLJS::CHTLJSTokenType::UNKNOWN: return "未知";
        default: return "未定义";
    }
}

std::string TokenPrint::TruncateString(const std::string& str, size_t maxLength) {
    if (str.length() <= maxLength) {
        return str;
    }
    
    return str.substr(0, maxLength - 3) + "...";
}

} // namespace Test
} // namespace CHTL