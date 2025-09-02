#include "AnimateNode.h"
#include "Visitor.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTLJS {

AnimateNode::AnimateNode(const std::string& animationName, size_t line, size_t column)
    : CHTLJSBaseNode(CHTLJSNodeType::ANIMATE_NODE, animationName, line, column),
      m_AnimationName(animationName), m_Duration(1000), m_Easing("ease"), 
      m_Loop(1), m_Delay(0), m_HasBeginState(false), m_HasEndState(false), 
      m_HasKeyframes(false), m_HasCallback(false), m_IsInfiniteLoop(false) {
    
    SetName(animationName.empty() ? "animate" : animationName);
    SetCHTLJSFunction(true);
    SetSupportsVirtualObject(true);
}

void AnimateNode::AddTarget(const std::string& target) {
    if (std::find(m_Targets.begin(), m_Targets.end(), target) == m_Targets.end()) {
        m_Targets.push_back(target);
    }
}

void AnimateNode::SetLoop(int loop) {
    m_Loop = loop;
    m_IsInfiniteLoop = (loop == -1);
}

void AnimateNode::SetCallback(const std::string& callback) {
    m_Callback = callback;
    m_HasCallback = !callback.empty();
}

void AnimateNode::SetBeginState(const AnimationState& state) {
    m_BeginState = state;
    m_HasBeginState = !state.Properties.empty();
}

void AnimateNode::AddBeginStateProperty(const std::string& property, const std::string& value) {
    m_BeginState.Properties[property] = value;
    m_HasBeginState = true;
}

void AnimateNode::SetEndState(const AnimationState& state) {
    m_EndState = state;
    m_HasEndState = !state.Properties.empty();
}

void AnimateNode::AddEndStateProperty(const std::string& property, const std::string& value) {
    m_EndState.Properties[property] = value;
    m_HasEndState = true;
}

void AnimateNode::AddKeyframe(const AnimationKeyframe& keyframe) {
    m_Keyframes.push_back(keyframe);
    m_HasKeyframes = true;
    
    // 按时刻排序
    std::sort(m_Keyframes.begin(), m_Keyframes.end(), 
             [](const AnimationKeyframe& a, const AnimationKeyframe& b) {
                 return a.At < b.At;
             });
}

void AnimateNode::AddKeyframe(double at, const std::unordered_map<std::string, std::string>& properties) {
    AnimationKeyframe keyframe(at);
    keyframe.Properties = properties;
    AddKeyframe(keyframe);
}

void AnimateNode::ParseAnimateContent(const std::string& animateContent) {
    // 解析animate{}块内容
    
    // 解析基础属性
    std::regex propertyPattern(R"(([a-zA-Z][a-zA-Z0-9_]*)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(animateContent.begin(), animateContent.end(), propertyPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);
        
        ParseAnimationProperty(property, value);
    }
    
    // 解析begin状态块
    std::regex beginPattern(R"(begin\s*:\s*\{([^}]*)\})");
    std::smatch beginMatch;
    if (std::regex_search(animateContent, beginMatch, beginPattern)) {
        auto beginProps = ParseCSSStateBlock(beginMatch[1].str());
        for (const auto& prop : beginProps) {
            AddBeginStateProperty(prop.first, prop.second);
        }
    }
    
    // 解析end状态块
    std::regex endPattern(R"(end\s*:\s*\{([^}]*)\})");
    std::smatch endMatch;
    if (std::regex_search(animateContent, endMatch, endPattern)) {
        auto endProps = ParseCSSStateBlock(endMatch[1].str());
        for (const auto& prop : endProps) {
            AddEndStateProperty(prop.first, prop.second);
        }
    }
    
    // 解析when关键帧
    std::regex whenPattern(R"(when\s*:\s*\[([^\]]*)\])");
    std::smatch whenMatch;
    if (std::regex_search(animateContent, whenMatch, whenPattern)) {
        ParseWhenKeyframes(whenMatch[1].str());
    }
}

void AnimateNode::ParseAnimationProperty(const std::string& property, const std::string& value) {
    if (property == "target") {
        ParseTargetSelectors(value);
    }
    else if (property == "duration") {
        m_Duration = std::stoi(value);
    }
    else if (property == "easing") {
        m_Easing = value;
    }
    else if (property == "loop") {
        SetLoop(std::stoi(value));
    }
    else if (property == "direction") {
        m_Direction = value;
    }
    else if (property == "delay") {
        m_Delay = std::stoi(value);
    }
    else if (property == "callback") {
        SetCallback(value);
    }
}

void AnimateNode::ParseTargetSelectors(const std::string& targetContent) {
    // 解析目标选择器
    // 支持单个选择器、选择器数组、DOM对象
    
    if (targetContent.front() == '[' && targetContent.back() == ']') {
        // 选择器数组：[{{selector1}}, {{selector2}}]
        std::string arrayContent = targetContent.substr(1, targetContent.length() - 2);
        
        std::regex selectorPattern(R"(\{\{[^}]+\}\})");
        std::sregex_iterator iter(arrayContent.begin(), arrayContent.end(), selectorPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            AddTarget(match.str());
        }
    }
    else if (targetContent.find("{{") != std::string::npos) {
        // 单个选择器：{{selector}}
        AddTarget(targetContent);
    }
    else {
        // DOM对象或其他
        AddTarget(targetContent);
    }
}

void AnimateNode::ParseWhenKeyframes(const std::string& whenContent) {
    // 解析when关键帧数组
    // 格式：[{at: 0.4, opacity: 0.5, transform: 'scale(1.1)'}, {at: 0.8, ...}]
    
    std::regex keyframePattern(R"(\{([^}]*)\})");
    std::sregex_iterator iter(whenContent.begin(), whenContent.end(), keyframePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string keyframeContent = match[1].str();
        
        AnimationKeyframe keyframe;
        
        // 解析at属性
        std::regex atPattern(R"(at\s*:\s*([\d.]+))");
        std::smatch atMatch;
        if (std::regex_search(keyframeContent, atMatch, atPattern)) {
            keyframe.At = std::stod(atMatch[1].str());
        }
        
        // 解析CSS属性
        auto cssProps = ParseCSSStateBlock(keyframeContent);
        keyframe.Properties = cssProps;
        
        AddKeyframe(keyframe);
    }
}

std::unordered_map<std::string, std::string> AnimateNode::ParseCSSStateBlock(const std::string& stateContent) {
    std::unordered_map<std::string, std::string> properties;
    
    std::regex propertyPattern(R"(([a-zA-Z\-]+)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(stateContent.begin(), stateContent.end(), propertyPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 跳过at属性（已在关键帧解析中处理）
        if (property == "at") {
            continue;
        }
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t'\""));
        value.erase(value.find_last_not_of(" \t'\"") + 1);
        
        properties[property] = value;
    }
    
    return properties;
}

std::string AnimateNode::GenerateAnimationCode() const {
    std::ostringstream code;
    
    code << "(function() {\n";
    code << "  // CHTL JS Animation: " << m_AnimationName << "\n";
    code << "  var targets = [];\n";
    
    // 生成目标查找代码
    code << GenerateTargetQueryCode();
    
    code << "  \n";
    code << "  function animate() {\n";
    code << "    var startTime = performance.now();\n";
    code << "    var duration = " << m_Duration << ";\n";
    
    // 设置起始状态
    if (m_HasBeginState) {
        code << "    \n";
        code << "    // Set begin state\n";
        code << "    targets.forEach(function(target) {\n";
        for (const auto& prop : m_BeginState.Properties) {
            code << "      target.style." << prop.first << " = '" << prop.second << "';\n";
        }
        code << "    });\n";
    }
    
    code << "    \n";
    code << "    function frame(currentTime) {\n";
    code << "      var elapsed = currentTime - startTime;\n";
    code << "      var progress = Math.min(elapsed / duration, 1);\n";
    
    // 生成缓动函数代码
    if (!m_Easing.empty() && m_Easing != "linear") {
        code << GenerateEasingCode();
    }
    
    code << "      \n";
    
    // 生成关键帧代码
    if (m_HasKeyframes) {
        code << GenerateKeyframeCode();
    }
    
    // 设置结束状态
    code << "      if (progress >= 1) {\n";
    if (m_HasEndState) {
        code << "        targets.forEach(function(target) {\n";
        for (const auto& prop : m_EndState.Properties) {
            code << "          target.style." << prop.first << " = '" << prop.second << "';\n";
        }
        code << "        });\n";
    }
    
    // 处理回调
    if (m_HasCallback) {
        code << "        (" << m_Callback << ")();\n";
    }
    
    // 处理循环
    if (m_IsInfiniteLoop) {
        code << "        // Infinite loop - restart animation\n";
        code << "        startTime = performance.now();\n";
        code << "        requestAnimationFrame(frame);\n";
    }
    else if (m_Loop > 1) {
        code << "        // TODO: Implement finite loop logic\n";
    }
    
    code << "      } else {\n";
    code << "        requestAnimationFrame(frame);\n";
    code << "      }\n";
    code << "    }\n";
    code << "    \n";
    
    // 处理延迟
    if (m_Delay > 0) {
        code << "    setTimeout(function() {\n";
        code << "      requestAnimationFrame(frame);\n";
        code << "    }, " << m_Delay << ");\n";
    }
    else {
        code << "    requestAnimationFrame(frame);\n";
    }
    
    code << "  }\n";
    code << "  \n";
    code << "  animate();\n";
    code << "})();\n";
    
    return code.str();
}

std::string AnimateNode::GenerateTargetQueryCode() const {
    std::ostringstream code;
    
    for (const auto& target : m_Targets) {
        if (target.find("{{") != std::string::npos) {
            // 增强选择器
            code << "  var target = " << target << ";\n";
            code << "  if (target) targets.push(target);\n";
        }
        else {
            // DOM对象或其他
            code << "  var target = " << target << ";\n";
            code << "  if (target) targets.push(target);\n";
        }
    }
    
    return code.str();
}

std::string AnimateNode::GenerateKeyframeCode() const {
    std::ostringstream code;
    
    for (const auto& keyframe : m_Keyframes) {
        code << "      if (progress >= " << keyframe.At << ") {\n";
        code << "        targets.forEach(function(target) {\n";
        
        for (const auto& prop : keyframe.Properties) {
            code << "          target.style." << prop.first << " = '" << prop.second << "';\n";
        }
        
        code << "        });\n";
        code << "      }\n";
    }
    
    return code.str();
}

std::string AnimateNode::GenerateEasingCode() const {
    std::ostringstream code;
    
    // 生成缓动函数代码
    if (m_Easing == "ease-in") {
        code << "      progress = progress * progress;\n";
    }
    else if (m_Easing == "ease-out") {
        code << "      progress = 1 - (1 - progress) * (1 - progress);\n";
    }
    else if (m_Easing == "ease-in-out") {
        code << "      progress = progress < 0.5 ? 2 * progress * progress : 1 - 2 * (1 - progress) * (1 - progress);\n";
    }
    else if (m_Easing.find("cubic-bezier") != std::string::npos) {
        code << "      // Custom cubic-bezier: " << m_Easing << "\n";
        code << "      // TODO: Implement cubic-bezier calculation\n";
    }
    
    return code.str();
}

bool AnimateNode::ValidateAnimationProperties() const {
    // 验证动画属性
    if (m_Targets.empty()) {
        return false; // 必须有目标
    }
    
    if (m_Duration <= 0) {
        return false; // 持续时间必须大于0
    }
    
    if (!IsValidEasing(m_Easing)) {
        return false; // 无效的缓动函数
    }
    
    // 验证关键帧时刻
    for (const auto& keyframe : m_Keyframes) {
        if (keyframe.At < 0.0 || keyframe.At > 1.0) {
            return false; // 关键帧时刻必须在0-1之间
        }
    }
    
    return true;
}

std::string AnimateNode::GenerateUniqueAnimationName() const {
    static int animationCounter = 0;
    return "chtljs_anim_" + std::to_string(++animationCounter);
}

bool AnimateNode::IsValidEasing(const std::string& easing) const {
    // 验证缓动函数
    static const std::vector<std::string> validEasings = {
        "linear", "ease", "ease-in", "ease-out", "ease-in-out"
    };
    
    if (std::find(validEasings.begin(), validEasings.end(), easing) != validEasings.end()) {
        return true;
    }
    
    // 检查cubic-bezier格式
    std::regex cubicBezierPattern(R"(cubic-bezier\(\s*[\d.]+\s*,\s*[\d.]+\s*,\s*[\d.]+\s*,\s*[\d.]+\s*\))");
    return std::regex_match(easing, cubicBezierPattern);
}

void AnimateNode::Accept(CHTLJSNodeVisitor* visitor) {
    if (visitor) {
        // visitor->VisitAnimateNode(this);
        // 暂时使用基础访问
    }
}

std::unique_ptr<CHTLJSBaseNode> AnimateNode::Clone() const {
    auto cloned = std::make_unique<AnimateNode>(m_AnimationName, GetLine(), GetColumn());
    
    // 复制所有属性
    cloned->m_Targets = m_Targets;
    cloned->m_Duration = m_Duration;
    cloned->m_Easing = m_Easing;
    cloned->m_BeginState = m_BeginState;
    cloned->m_EndState = m_EndState;
    cloned->m_Keyframes = m_Keyframes;
    cloned->m_Loop = m_Loop;
    cloned->m_Direction = m_Direction;
    cloned->m_Delay = m_Delay;
    cloned->m_Callback = m_Callback;
    cloned->m_HasBeginState = m_HasBeginState;
    cloned->m_HasEndState = m_HasEndState;
    cloned->m_HasKeyframes = m_HasKeyframes;
    cloned->m_HasCallback = m_HasCallback;
    cloned->m_IsInfiniteLoop = m_IsInfiniteLoop;
    
    return cloned;
}

std::string AnimateNode::GenerateJavaScript() const {
    return GenerateAnimationCode();
}

std::string AnimateNode::ToString(int indent) const {
    std::ostringstream oss;
    std::string indentStr = GenerateIndent(indent);
    
    oss << indentStr << "AnimateNode '" << m_AnimationName << "'";
    
    oss << " [Targets: " << m_Targets.size() << "]";
    oss << " [Duration: " << m_Duration << "ms]";
    
    if (!m_Easing.empty()) {
        oss << " [Easing: " << m_Easing << "]";
    }
    
    if (m_HasBeginState) {
        oss << " [Begin: " << m_BeginState.Properties.size() << " props]";
    }
    
    if (m_HasEndState) {
        oss << " [End: " << m_EndState.Properties.size() << " props]";
    }
    
    if (m_HasKeyframes) {
        oss << " [Keyframes: " << m_Keyframes.size() << "]";
    }
    
    if (m_IsInfiniteLoop) {
        oss << " [Infinite Loop]";
    }
    else if (m_Loop > 1) {
        oss << " [Loop: " << m_Loop << "]";
    }
    
    if (m_Delay > 0) {
        oss << " [Delay: " << m_Delay << "ms]";
    }
    
    if (m_HasCallback) {
        oss << " [Callback]";
    }
    
    oss << " @(" << GetLine() << ":" << GetColumn() << ")\n";
    
    // 输出目标详情
    if (!m_Targets.empty()) {
        oss << indentStr << "  Targets:\n";
        for (const auto& target : m_Targets) {
            oss << indentStr << "    " << target << "\n";
        }
    }
    
    // 输出起始状态
    if (m_HasBeginState) {
        oss << indentStr << "  Begin State:\n";
        for (const auto& prop : m_BeginState.Properties) {
            oss << indentStr << "    " << prop.first << ": " << prop.second << "\n";
        }
    }
    
    // 输出结束状态
    if (m_HasEndState) {
        oss << indentStr << "  End State:\n";
        for (const auto& prop : m_EndState.Properties) {
            oss << indentStr << "    " << prop.first << ": " << prop.second << "\n";
        }
    }
    
    // 输出关键帧
    if (m_HasKeyframes) {
        oss << indentStr << "  Keyframes:\n";
        for (const auto& keyframe : m_Keyframes) {
            oss << indentStr << "    at " << keyframe.At << ": " << keyframe.Properties.size() << " props\n";
        }
    }
    
    return oss.str();
}

} // namespace CHTLJS