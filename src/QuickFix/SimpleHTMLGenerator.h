#pragma once

#include <string>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

/**
 * 简单HTML生成器
 * 快速修复版本，确保能够正确编译CHTL代码
 */
class SimpleHTMLGenerator {
public:
    static std::string GenerateHTML(const std::string& chtlCode) {
        std::cout << "🔥 SimpleHTMLGenerator: 开始生成HTML..." << std::endl;
        std::cout << "   📝 源代码长度: " << chtlCode.length() << std::endl;
        
        std::ostringstream html;
        
        html << "<!DOCTYPE html>\n";
        html << "<html lang=\"zh-CN\">\n";
        html << "<head>\n";
        html << "    <meta charset=\"UTF-8\">\n";
        html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        
        // 提取标题
        std::string title = ExtractTitle(chtlCode);
        html << "    <title>" << title << "</title>\n";
        std::cout << "   📋 提取标题: " << title << std::endl;
        
        // 提取并生成CSS
        std::string css = ExtractAndGenerateCSS(chtlCode);
        if (!css.empty()) {
            html << "    <style>\n";
            html << "        " << css;
            html << "    </style>\n";
            std::cout << "   🎨 提取CSS，长度: " << css.length() << std::endl;
        }
        
        html << "</head>\n";
        html << "<body>\n";
        
        // 提取并生成HTML内容
        std::string bodyContent = ExtractAndGenerateBodyContent(chtlCode);
        html << bodyContent;
        std::cout << "   📄 提取body内容，长度: " << bodyContent.length() << std::endl;
        
        // 提取并生成JavaScript
        std::string js = ExtractAndGenerateJavaScript(chtlCode);
        if (!js.empty()) {
            html << "    <script>\n";
            html << "        " << js;
            html << "    </script>\n";
            std::cout << "   ⚡ 提取JS，长度: " << js.length() << std::endl;
        }
        
        html << "</body>\n";
        html << "</html>\n";
        
        std::string result = html.str();
        std::cout << "   ✅ HTML生成完成，总长度: " << result.length() << std::endl;
        
        return result;
    }

private:
    static std::string ExtractTitle(const std::string& chtlCode) {
        std::regex titleRegex(R"(title\s*\{\s*text\s*\{\s*([^}]+)\s*\})");
        std::smatch match;
        
        if (std::regex_search(chtlCode, match, titleRegex)) {
            std::string title = match[1].str();
            // 去除多余空格
            title = std::regex_replace(title, std::regex(R"(\s+)"), " ");
            title = std::regex_replace(title, std::regex(R"(^\s+|\s+$)"), "");
            return title;
        }
        
        return "CHTL生成页面";
    }
    
    static std::string ExtractAndGenerateCSS(const std::string& chtlCode) {
        std::ostringstream css;
        
        // 查找style块
        std::regex styleRegex(R"(style\s*\{([^{}]*(?:\{[^}]*\}[^{}]*)*)\})");
        std::sregex_iterator iter(chtlCode.begin(), chtlCode.end(), styleRegex);
        std::sregex_iterator end;
        
        while (iter != end) {
            std::string styleContent = (*iter)[1].str();
            
            // 简单处理CSS属性
            std::regex propRegex(R"((\w+(?:-\w+)*)\s*:\s*([^;]+);?)");
            std::sregex_iterator propIter(styleContent.begin(), styleContent.end(), propRegex);
            std::sregex_iterator propEnd;
            
            while (propIter != propEnd) {
                std::string property = (*propIter)[1].str();
                std::string value = (*propIter)[2].str();
                
                css << "        " << property << ": " << value << ";\n";
                ++propIter;
            }
            
            ++iter;
        }
        
        return css.str();
    }
    
    static std::string ExtractAndGenerateBodyContent(const std::string& chtlCode) {
        std::ostringstream html;
        
        // 查找div元素
        std::regex divRegex(R"(div\s*\{([^{}]*(?:\{[^}]*\}[^{}]*)*)\})");
        std::sregex_iterator iter(chtlCode.begin(), chtlCode.end(), divRegex);
        std::sregex_iterator end;
        
        while (iter != end) {
            std::string divContent = (*iter)[1].str();
            
            html << "    <div";
            
            // 提取属性
            std::string attributes = ExtractAttributes(divContent);
            if (!attributes.empty()) {
                html << " " << attributes;
            }
            
            html << ">\n";
            
            // 提取文本内容
            std::string textContent = ExtractTextContent(divContent);
            if (!textContent.empty()) {
                html << "        " << textContent << "\n";
            }
            
            html << "    </div>\n";
            
            ++iter;
        }
        
        return html.str();
    }
    
    static std::string ExtractAttributes(const std::string& content) {
        std::ostringstream attrs;
        
        // 提取id
        std::regex idRegex(R"(id\s*:\s*([^;]+);?)");
        std::smatch match;
        if (std::regex_search(content, match, idRegex)) {
            attrs << "id=\"" << match[1].str() << "\"";
        }
        
        // 提取class
        std::regex classRegex(R"(class\s*:\s*([^;]+);?)");
        if (std::regex_search(content, match, classRegex)) {
            if (attrs.tellp() > 0) attrs << " ";
            attrs << "class=\"" << match[1].str() << "\"";
        }
        
        return attrs.str();
    }
    
    static std::string ExtractTextContent(const std::string& content) {
        std::regex textRegex(R"(text\s*\{\s*([^}]+)\s*\})");
        std::smatch match;
        
        if (std::regex_search(content, match, textRegex)) {
            std::string text = match[1].str();
            text = std::regex_replace(text, std::regex(R"(\s+)"), " ");
            text = std::regex_replace(text, std::regex(R"(^\s+|\s+$)"), "");
            return text;
        }
        
        return "";
    }
    
    static std::string ExtractAndGenerateJavaScript(const std::string& chtlCode) {
        std::ostringstream js;
        
        // 查找script块
        std::regex scriptRegex(R"(script\s*\{([^{}]*(?:\{[^}]*\}[^{}]*)*)\})");
        std::sregex_iterator iter(chtlCode.begin(), chtlCode.end(), scriptRegex);
        std::sregex_iterator end;
        
        while (iter != end) {
            std::string scriptContent = (*iter)[1].str();
            js << scriptContent << "\n";
            ++iter;
        }
        
        return js.str();
    }
};

} // namespace CHTL