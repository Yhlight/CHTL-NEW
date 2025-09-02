#include "CHTLUnifiedScanner.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& sourceCode)
    : m_SourceCode(sourceCode), m_CurrentPosition(0), m_CurrentLine(1), 
      m_CurrentColumn(1), m_HasError(false) {
    InitializeKeywordMaps();
}

void CHTLUnifiedScanner::InitializeKeywordMaps() {
    // CHTL核心关键字
    m_CHTLKeywords["text"] = true;
    m_CHTLKeywords["style"] = true;
    m_CHTLKeywords["script"] = true;  // 局部script属于CHTL
    m_CHTLKeywords["[Template]"] = true;
    m_CHTLKeywords["[Custom]"] = true;
    m_CHTLKeywords["[Origin]"] = true;
    m_CHTLKeywords["[Configuration]"] = true;
    m_CHTLKeywords["[Import]"] = true;
    m_CHTLKeywords["[Namespace]"] = true;
    m_CHTLKeywords["[Info]"] = true;
    m_CHTLKeywords["[Export]"] = true;
    m_CHTLKeywords["@Style"] = true;
    m_CHTLKeywords["@Element"] = true;
    m_CHTLKeywords["@Var"] = true;
    m_CHTLKeywords["@Html"] = true;
    m_CHTLKeywords["@JavaScript"] = true;
    m_CHTLKeywords["@Chtl"] = true;
    m_CHTLKeywords["@CJmod"] = true;
    m_CHTLKeywords["@Config"] = true;
    m_CHTLKeywords["inherit"] = true;
    m_CHTLKeywords["delete"] = true;
    m_CHTLKeywords["insert"] = true;
    m_CHTLKeywords["after"] = true;
    m_CHTLKeywords["before"] = true;
    m_CHTLKeywords["replace"] = true;
    m_CHTLKeywords["except"] = true;
    m_CHTLKeywords["from"] = true;
    m_CHTLKeywords["as"] = true;
    m_CHTLKeywords["use"] = true;
    m_CHTLKeywords["html5"] = true;
    
    // CHTL JS核心关键字（在script{}内使用）
    m_CHTLJSKeywords["module"] = true;
    m_CHTLJSKeywords["load"] = true;
    m_CHTLJSKeywords["listen"] = true;
    m_CHTLJSKeywords["delegate"] = true;
    m_CHTLJSKeywords["animate"] = true;
    m_CHTLJSKeywords["vir"] = true;
    m_CHTLJSKeywords["target"] = true;
    m_CHTLJSKeywords["duration"] = true;
    m_CHTLJSKeywords["easing"] = true;
    m_CHTLJSKeywords["begin"] = true;
    m_CHTLJSKeywords["when"] = true;
    m_CHTLJSKeywords["end"] = true;
    m_CHTLJSKeywords["loop"] = true;
    m_CHTLJSKeywords["direction"] = true;
    m_CHTLJSKeywords["delay"] = true;
    m_CHTLJSKeywords["callback"] = true;
    m_CHTLJSKeywords["at"] = true;
}

bool CHTLUnifiedScanner::Scan() {
    Reset();
    
    while (!IsAtEnd()) {
        size_t fragmentStart = m_CurrentPosition;
        size_t fragmentLine = m_CurrentLine;
        size_t fragmentColumn = m_CurrentColumn;
        
        // 跳过空白字符和注释
        SkipWhitespaceAndComments();
        if (IsAtEnd()) break;
        
        // 读取初始片段
        std::string initialFragment = ReadInitialFragment();
        if (initialFragment.empty()) continue;
        
        // 检查片段完整性，实现可变长度切片
        std::string completeFragment = EnsureFragmentCompleteness(initialFragment);
        
        // 识别片段类型
        FragmentType type = IdentifyFragmentType(completeFragment);
        
        // 创建代码片段
        CodeFragment fragment(type, completeFragment, fragmentStart, 
                            m_CurrentPosition, fragmentLine, fragmentColumn);
        
        // 对CHTL和CHTL JS片段进行最小语法单元二次切割
        if (type == FragmentType::CHTL_FRAGMENT || type == FragmentType::CHTL_JS_FRAGMENT) {
            auto syntaxUnits = PerformMinimalUnitCutting(fragment);
            
            // 根据上下文决定是否需要聚合多个单元
            if (ShouldAggregateUnits(syntaxUnits)) {
                // 保持适当聚合，避免过度细分
                fragment.Content = AggregateUnits(syntaxUnits);
            }
        }
        
        m_Fragments.push_back(fragment);
    }
    
    return !m_HasError;
}

std::string CHTLUnifiedScanner::ReadInitialFragment() {
    size_t startPos = m_CurrentPosition;
    
    // 根据当前字符判断读取策略
    char currentChar = GetNextChar();
    
    if (currentChar == '[') {
        // 可能是CHTL块语法：[Template]、[Custom]、[Origin]等
        return ReadCHTLBlockSyntax();
    }
    else if (currentChar == '{' && PeekChar(-2) == '}' && PeekChar(-3) == '{') {
        // 可能是CHTL JS增强选择器：{{selector}}
        return ReadCHTLJSEnhancedSelector();
    }
    else if (std::isalpha(currentChar) || currentChar == '_') {
        // 可能是标识符、关键字或元素名
        return ReadIdentifierOrKeyword();
    }
    else if (currentChar == '/' && PeekChar() == '/') {
        // 单行注释
        return ReadSingleLineComment();
    }
    else if (currentChar == '/' && PeekChar() == '*') {
        // 多行注释
        return ReadMultiLineComment();
    }
    else if (currentChar == '-' && PeekChar() == '-') {
        // 生成器注释
        return ReadGeneratorComment();
    }
    else if (currentChar == '"' || currentChar == '\'') {
        // 字符串字面量
        return ReadStringLiteral(currentChar);
    }
    else if (currentChar == '{') {
        // 块结构
        return ReadBlock();
    }
    else {
        // 其他字符，读取到下一个有意义的分隔符
        return ReadUntilDelimiter();
    }
}

std::string CHTLUnifiedScanner::EnsureFragmentCompleteness(const std::string& initialFragment) {
    std::string currentFragment = initialFragment;
    
    while (!IsAtEnd()) {
        // 窥视下一个片段的起始部分
        size_t peekLength = std::min(static_cast<size_t>(50), m_SourceCode.length() - m_CurrentPosition);
        std::string nextFragmentStart = m_SourceCode.substr(m_CurrentPosition, peekLength);
        
        // 检查当前片段和下一片段是否能组成完整的CHTL或CHTL JS代码
        if (!CheckFragmentCompleteness(currentFragment, nextFragmentStart)) {
            break; // 当前截断位置合理
        }
        
        // 需要扩增切片范围
        size_t expandLength = CalculateExpandLength(currentFragment, nextFragmentStart);
        std::string expandedPart = ReadString(expandLength);
        currentFragment += expandedPart;
    }
    
    return currentFragment;
}

bool CHTLUnifiedScanner::CheckFragmentCompleteness(const std::string& currentFragment, 
                                                  const std::string& nextFragment) {
    // 检查CHTL块结构完整性
    if (IsIncompleteCHTLBlock(currentFragment, nextFragment)) {
        return true; // 需要扩增
    }
    
    // 检查CHTL JS函数完整性
    if (IsIncompleteCHTLJSFunction(currentFragment, nextFragment)) {
        return true; // 需要扩增
    }
    
    // 检查括号平衡
    if (!IsBracketBalanced(currentFragment)) {
        return true; // 需要扩增
    }
    
    // 检查字符串字面量完整性
    if (IsIncompleteStringLiteral(currentFragment)) {
        return true; // 需要扩增
    }
    
    return false; // 片段完整，无需扩增
}

std::vector<SyntaxUnit> CHTLUnifiedScanner::PerformMinimalUnitCutting(const CodeFragment& fragment) {
    std::vector<SyntaxUnit> units;
    
    if (fragment.Type == FragmentType::CHTL_JS_FRAGMENT) {
        // CHTL JS特殊切割：{{box}}->click → {{box}}-> + click
        units = CutCHTLJSEnhancedSelector(fragment.Content);
    }
    else if (fragment.Type == FragmentType::CHTL_FRAGMENT) {
        // CHTL语法切割：模板、自定义、原始嵌入等
        units = CutCHTLTemplateSyntax(fragment.Content);
    }
    
    return units;
}

std::vector<SyntaxUnit> CHTLUnifiedScanner::CutCHTLJSEnhancedSelector(const std::string& content) {
    std::vector<SyntaxUnit> units;
    
    // 使用正则表达式匹配CHTL JS增强选择器模式
    std::regex selectorPattern(R"(\{\{[^}]+\}\}(?:->|\.))");
    std::regex virPattern(R"(vir\s+\w+\s*=)");
    std::regex listenPattern(R"(->listen\s*\{)");
    std::regex delegatePattern(R"(->delegate\s*\{)");
    std::regex animatePattern(R"(animate\s*\{)");
    std::regex eventBindPattern(R"(&->\s*\w+\s*\{)");
    
    size_t pos = 0;
    std::string remaining = content;
    
    while (pos < content.length()) {
        std::smatch match;
        
        // 匹配增强选择器：{{box}}->
        if (std::regex_search(remaining, match, selectorPattern)) {
            if (match.position() > 0) {
                // 添加选择器前的内容
                std::string beforeSelector = remaining.substr(0, match.position());
                units.emplace_back(beforeSelector, pos, pos + beforeSelector.length());
                pos += beforeSelector.length();
            }
            
            // 添加选择器部分
            std::string selector = match.str();
            units.emplace_back(selector, pos, pos + selector.length(), true);
            pos += selector.length();
            
            // 更新remaining
            remaining = remaining.substr(match.position() + match.length());
            continue;
        }
        
        // 如果没有匹配到特殊模式，添加剩余内容
        if (!remaining.empty()) {
            units.emplace_back(remaining, pos, pos + remaining.length());
            break;
        }
    }
    
    return units;
}

std::vector<SyntaxUnit> CHTLUnifiedScanner::CutCHTLTemplateSyntax(const std::string& content) {
    std::vector<SyntaxUnit> units;
    
    // CHTL模板语法切割模式
    std::regex templatePattern(R"(\[(?:Template|Custom|Origin|Configuration|Import|Namespace)\])");
    std::regex atTypePattern(R"(@(?:Style|Element|Var|Html|JavaScript|Chtl|CJmod|Config))");
    std::regex inheritPattern(R"(inherit\s+@\w+)");
    std::regex deletePattern(R"(delete\s+[\w\[\]@.#,\s]+)");
    std::regex insertPattern(R"(insert\s+(?:after|before|replace|at\s+(?:top|bottom)))");
    
    size_t pos = 0;
    std::string remaining = content;
    
    while (pos < content.length()) {
        std::smatch match;
        bool foundMatch = false;
        
        // 检查各种CHTL语法模式
        std::vector<std::pair<std::regex*, std::string>> patterns = {
            {&templatePattern, "template"},
            {&atTypePattern, "attype"},
            {&inheritPattern, "inherit"},
            {&deletePattern, "delete"},
            {&insertPattern, "insert"}
        };
        
        for (auto& pattern : patterns) {
            if (std::regex_search(remaining, match, *pattern.first)) {
                if (match.position() > 0) {
                    // 添加匹配前的内容
                    std::string beforeMatch = remaining.substr(0, match.position());
                    units.emplace_back(beforeMatch, pos, pos + beforeMatch.length());
                    pos += beforeMatch.length();
                }
                
                // 添加匹配的语法单元
                std::string matchedUnit = match.str();
                units.emplace_back(matchedUnit, pos, pos + matchedUnit.length());
                pos += matchedUnit.length();
                
                // 更新remaining
                remaining = remaining.substr(match.position() + match.length());
                foundMatch = true;
                break;
            }
        }
        
        if (!foundMatch) {
            // 没有找到特殊模式，添加剩余内容
            if (!remaining.empty()) {
                units.emplace_back(remaining, pos, pos + remaining.length());
            }
            break;
        }
    }
    
    return units;
}

FragmentType CHTLUnifiedScanner::IdentifyFragmentType(const std::string& content) {
    // 检查是否包含CHTL JS特征
    if (IsCHTLJSSyntax(content)) {
        return FragmentType::CHTL_JS_FRAGMENT;
    }
    
    // 检查是否包含CHTL特征
    if (IsCHTLSyntax(content)) {
        return FragmentType::CHTL_FRAGMENT;
    }
    
    // 检查是否为CSS
    if (IsCSSSyntax(content)) {
        return FragmentType::CSS_FRAGMENT;
    }
    
    // 默认为JavaScript
    return FragmentType::JS_FRAGMENT;
}

bool CHTLUnifiedScanner::IsCHTLSyntax(const std::string& content) {
    // 检查CHTL关键字
    for (const auto& keyword : m_CHTLKeywords) {
        if (content.find(keyword.first) != std::string::npos) {
            return true;
        }
    }
    
    // 检查CHTL特有的语法模式
    std::regex cHTLPatterns[] = {
        std::regex(R"(\[(?:Template|Custom|Origin|Configuration|Import|Namespace)\])"),
        std::regex(R"(@(?:Style|Element|Var|Html|JavaScript|Chtl|CJmod|Config))"),
        std::regex(R"(text\s*\{)"),
        std::regex(R"(style\s*\{)"),
        std::regex(R"(script\s*\{)"),  // 局部script属于CHTL
        std::regex(R"(inherit\s+@)"),
        std::regex(R"(delete\s+)"),
        std::regex(R"(insert\s+(?:after|before|replace))"),
        std::regex(R"(except\s+)"),
        std::regex(R"(from\s+\w+)"),
        std::regex(R"(use\s+(?:html5|@Config))"),
        std::regex(R"(&(?::hover|::before|::after)?)") // 上下文推导
    };
    
    for (const auto& pattern : cHTLPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCHTLJSSyntax(const std::string& content) {
    // 检查CHTL JS关键字
    for (const auto& keyword : m_CHTLJSKeywords) {
        if (content.find(keyword.first) != std::string::npos) {
            return true;
        }
    }
    
    // 检查CHTL JS特有的语法模式
    std::regex cHTLJSPatterns[] = {
        std::regex(R"(\{\{[^}]+\}\})"),              // 增强选择器 {{selector}}
        std::regex(R"(->(?:listen|delegate|textContent|addEventListener))"), // ->操作符
        std::regex(R"(&->\s*\w+)"),                 // 事件绑定操作符 &->
        std::regex(R"(vir\s+\w+\s*=)"),            // 虚对象 vir
        std::regex(R"(module\s*\{)"),              // 模块导入 module{}
        std::regex(R"(load:\s*[^,}]+)"),           // 模块加载 load:
        std::regex(R"(listen\s*\{)"),              // 增强监听器 listen{}
        std::regex(R"(delegate\s*\{)"),            // 事件委托 delegate{}
        std::regex(R"(animate\s*\{)"),             // 动画 animate{}
        std::regex(R"(target:\s*\{\{)"),           // 动画目标
        std::regex(R"(duration:\s*\d+)"),          // 动画持续时间
        std::regex(R"(begin:\s*\{)"),              // 动画起始状态
        std::regex(R"(when:\s*\[)"),               // 动画关键帧
        std::regex(R"(end:\s*\{)")                 // 动画结束状态
    };
    
    for (const auto& pattern : cHTLJSPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCSSSyntax(const std::string& content) {
    // 检查CSS特征
    std::regex cssPatterns[] = {
        std::regex(R"([.#][\w-]+\s*\{)"),           // CSS选择器
        std::regex(R"([\w-]+:\s*[^;]+;)"),          // CSS属性
        std::regex(R"(@(?:media|keyframes|import))"), // CSS at-rules
        std::regex(R"((?:hover|focus|active|visited)(?:\s*\{|:))") // CSS伪类
    };
    
    for (const auto& pattern : cssPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsJavaScriptSyntax(const std::string& content) {
    // 检查JavaScript特征
    std::regex jsPatterns[] = {
        std::regex(R"(\b(?:function|var|let|const|if|else|for|while|return|class)\b)"),
        std::regex(R"(\w+\s*\([^)]*\)\s*\{)"),      // 函数调用
        std::regex(R"(\w+\.\w+)"),                  // 对象属性访问
        std::regex(R"(console\.log)"),              // 常见JS方法
        std::regex(R"(addEventListener)"),          // DOM方法
        std::regex(R"(document\.)"),                // DOM对象
        std::regex(R"(window\.)"),                  // 全局对象
        std::regex(R"(=>\s*\{)")                    // 箭头函数
    };
    
    for (const auto& pattern : jsPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

std::string CHTLUnifiedScanner::ReadCHTLBlockSyntax() {
    std::string result;
    result += '[';
    
    // 读取到匹配的]
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        result += ch;
        if (ch == ']') {
            break;
        }
    }
    
    // 继续读取可能的类型标识符和名称
    SkipWhitespace();
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '@' || PeekChar() == '_')) {
        result += GetNextChar();
    }
    
    // 读取到块开始或分号
    SkipWhitespace();
    if (PeekChar() == '{') {
        result += ReadBlock();
    }
    else if (PeekChar() == ';') {
        result += GetNextChar();
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadCHTLJSEnhancedSelector() {
    std::string result;
    
    // 回退到{{开始
    m_CurrentPosition -= 2;
    
    // 读取完整的增强选择器
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        result += ch;
        
        if (ch == '}' && PeekChar() == '}') {
            result += GetNextChar(); // 添加第二个}
            break;
        }
    }
    
    // 检查是否有->操作符
    SkipWhitespace();
    if (PeekChar() == '-' && PeekChar(2) == '>') {
        result += GetNextChar(); // -
        result += GetNextChar(); // >
        
        // 读取方法名
        SkipWhitespace();
        while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_')) {
            result += GetNextChar();
        }
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadIdentifierOrKeyword() {
    std::string result;
    
    // 回退一个字符
    m_CurrentPosition--;
    
    while (!IsAtEnd() && (std::isalnum(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        result += GetNextChar();
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadBlock() {
    std::string result;
    result += '{';
    
    int braceCount = 1;
    while (!IsAtEnd() && braceCount > 0) {
        char ch = GetNextChar();
        result += ch;
        
        if (ch == '{') {
            braceCount++;
        }
        else if (ch == '}') {
            braceCount--;
        }
        else if (ch == '"' || ch == '\'') {
            // 处理字符串中的括号
            result += ReadStringLiteralContent(ch);
        }
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadStringLiteral(char quote) {
    std::string result;
    result += quote;
    
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        result += ch;
        
        if (ch == quote) {
            break;
        }
        else if (ch == '\\') {
            // 处理转义字符
            if (!IsAtEnd()) {
                result += GetNextChar();
            }
        }
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadStringLiteralContent(char quote) {
    std::string result;
    
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        result += ch;
        
        if (ch == quote) {
            break;
        }
        else if (ch == '\\') {
            // 处理转义字符
            if (!IsAtEnd()) {
                result += GetNextChar();
            }
        }
    }
    
    return result;
}

void CHTLUnifiedScanner::SkipWhitespaceAndComments() {
    while (!IsAtEnd()) {
        char ch = PeekChar();
        
        if (std::isspace(ch)) {
            GetNextChar();
        }
        else if (ch == '/' && PeekChar(2) == '/') {
            // 跳过单行注释
            while (!IsAtEnd() && PeekChar() != '\n') {
                GetNextChar();
            }
        }
        else if (ch == '/' && PeekChar(2) == '*') {
            // 跳过多行注释
            GetNextChar(); // /
            GetNextChar(); // *
            while (!IsAtEnd()) {
                if (PeekChar() == '*' && PeekChar(2) == '/') {
                    GetNextChar(); // *
                    GetNextChar(); // /
                    break;
                }
                GetNextChar();
            }
        }
        else {
            break;
        }
    }
}

void CHTLUnifiedScanner::SkipWhitespace() {
    while (!IsAtEnd() && std::isspace(PeekChar())) {
        GetNextChar();
    }
}

char CHTLUnifiedScanner::GetNextChar() {
    if (IsAtEnd()) {
        return '\0';
    }
    
    char ch = m_SourceCode[m_CurrentPosition++];
    UpdateLineColumn(ch);
    return ch;
}

char CHTLUnifiedScanner::PeekChar(size_t offset) {
    size_t peekPos = m_CurrentPosition + offset - 1;
    if (peekPos >= m_SourceCode.length()) {
        return '\0';
    }
    return m_SourceCode[peekPos];
}

std::string CHTLUnifiedScanner::ReadString(size_t length) {
    if (m_CurrentPosition + length > m_SourceCode.length()) {
        length = m_SourceCode.length() - m_CurrentPosition;
    }
    
    std::string result = m_SourceCode.substr(m_CurrentPosition, length);
    
    for (size_t i = 0; i < length; ++i) {
        UpdateLineColumn(m_SourceCode[m_CurrentPosition++]);
    }
    
    return result;
}

bool CHTLUnifiedScanner::IsAtEnd() const {
    return m_CurrentPosition >= m_SourceCode.length();
}

void CHTLUnifiedScanner::UpdateLineColumn(char ch) {
    if (ch == '\n') {
        m_CurrentLine++;
        m_CurrentColumn = 1;
    }
    else {
        m_CurrentColumn++;
    }
}

bool CHTLUnifiedScanner::ValidateUTF8(const std::string& str) {
    try {
        std::wstring wstr = m_UTF8Converter.from_bytes(str);
        return true;
    }
    catch (const std::range_error&) {
        return false;
    }
}

const std::vector<CodeFragment>& CHTLUnifiedScanner::GetFragments() const {
    return m_Fragments;
}

void CHTLUnifiedScanner::Reset() {
    m_CurrentPosition = 0;
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_Fragments.clear();
    m_HasError = false;
    m_ErrorMessage.clear();
}

void CHTLUnifiedScanner::SetSourceCode(const std::string& sourceCode) {
    m_SourceCode = sourceCode;
    Reset();
}

std::string CHTLUnifiedScanner::GetErrorMessage() const {
    return m_ErrorMessage;
}

bool CHTLUnifiedScanner::UpdateFragmentContent(size_t fragmentIndex, const std::string& newContent) {
    if (fragmentIndex >= m_Fragments.size()) {
        return false;
    }
    
    // 更新片段内容
    m_Fragments[fragmentIndex].Content = newContent;
    
    // 可选：更新片段的其他信息
    m_Fragments[fragmentIndex].IsComplete = true;
    
    return true;
}

size_t CHTLUnifiedScanner::GetFragmentCount() const {
    return m_Fragments.size();
}

// 辅助方法实现
bool CHTLUnifiedScanner::IsIncompleteCHTLBlock(const std::string& current, const std::string& next) {
    // 检查CHTL块是否完整
    std::regex blockStart(R"(\[(?:Template|Custom|Origin|Configuration|Import|Namespace)\]\s*@?\w*\s*\w*\s*$)");
    if (std::regex_search(current, blockStart)) {
        // 检查下一片段是否有块开始
        if (next.find('{') != std::string::npos) {
            return true; // 需要扩增
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsIncompleteCHTLJSFunction(const std::string& current, const std::string& next) {
    // 检查CHTL JS函数是否完整
    std::regex functionStart(R"((?:listen|delegate|animate|module)\s*$)");
    if (std::regex_search(current, functionStart)) {
        if (next.find('{') != std::string::npos) {
            return true; // 需要扩增
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsBracketBalanced(const std::string& content) {
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char ch = content[i];
        
        if (!inString) {
            if (ch == '"' || ch == '\'') {
                inString = true;
                stringChar = ch;
            }
            else if (ch == '{') braceCount++;
            else if (ch == '}') braceCount--;
            else if (ch == '(') parenCount++;
            else if (ch == ')') parenCount--;
            else if (ch == '[') bracketCount++;
            else if (ch == ']') bracketCount--;
        }
        else {
            if (ch == stringChar && (i == 0 || content[i-1] != '\\')) {
                inString = false;
            }
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0 && !inString;
}

bool CHTLUnifiedScanner::IsIncompleteStringLiteral(const std::string& content) {
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char ch = content[i];
        
        if (!inString) {
            if (ch == '"' || ch == '\'') {
                inString = true;
                stringChar = ch;
            }
        }
        else {
            if (ch == stringChar && (i == 0 || content[i-1] != '\\')) {
                inString = false;
            }
        }
    }
    
    return inString; // 如果仍在字符串中，说明不完整
}

size_t CHTLUnifiedScanner::CalculateExpandLength(const std::string& current, const std::string& next) {
    // 根据当前片段和下一片段内容，计算需要扩增的长度
    
    // 如果是CHTL块语法，扩增到块结束
    if (IsIncompleteCHTLBlock(current, next)) {
        size_t bracePos = next.find('{');
        if (bracePos != std::string::npos) {
            return bracePos + 1;
        }
    }
    
    // 如果是CHTL JS函数，扩增到函数结束
    if (IsIncompleteCHTLJSFunction(current, next)) {
        size_t bracePos = next.find('{');
        if (bracePos != std::string::npos) {
            return bracePos + 1;
        }
    }
    
    // 默认扩增策略
    return std::min(static_cast<size_t>(20), next.length());
}

bool CHTLUnifiedScanner::ShouldAggregateUnits(const std::vector<SyntaxUnit>& units) {
    // 如果单元数量太多且都很小，应该聚合
    if (units.size() > 10) {
        size_t totalLength = 0;
        for (const auto& unit : units) {
            totalLength += unit.Content.length();
        }
        
        // 平均单元长度小于5个字符时聚合
        return (totalLength / units.size()) < 5;
    }
    
    return false;
}

std::string CHTLUnifiedScanner::AggregateUnits(const std::vector<SyntaxUnit>& units) {
    std::string result;
    for (const auto& unit : units) {
        result += unit.Content;
    }
    return result;
}

std::string CHTLUnifiedScanner::ReadSingleLineComment() {
    std::string result = "//";
    GetNextChar(); // 跳过第二个/
    
    while (!IsAtEnd() && PeekChar() != '\n') {
        result += GetNextChar();
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadMultiLineComment() {
    std::string result = "/*";
    GetNextChar(); // 跳过*
    
    while (!IsAtEnd()) {
        char ch = GetNextChar();
        result += ch;
        
        if (ch == '*' && PeekChar() == '/') {
            result += GetNextChar(); // 添加/
            break;
        }
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadGeneratorComment() {
    std::string result = "--";
    GetNextChar(); // 跳过第二个-
    
    while (!IsAtEnd() && PeekChar() != '\n') {
        result += GetNextChar();
    }
    
    return result;
}

std::string CHTLUnifiedScanner::ReadUntilDelimiter() {
    std::string result;
    
    while (!IsAtEnd()) {
        char ch = PeekChar();
        
        if (std::isspace(ch) || ch == '{' || ch == '}' || ch == ';' || 
            ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']') {
            break;
        }
        
        result += GetNextChar();
    }
    
    return result;
}



} // namespace CHTL