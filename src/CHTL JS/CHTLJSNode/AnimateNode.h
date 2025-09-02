#pragma once

#include "CHTLJSBaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTLJS {

/**
 * 动画关键帧结构
 */
struct AnimationKeyframe {
    double At;                                          // 关键帧时刻（0.0-1.0）
    std::unordered_map<std::string, std::string> Properties; // CSS属性
    
    AnimationKeyframe() : At(0.0) {}
    AnimationKeyframe(double at) : At(at) {}
};

/**
 * 动画状态结构
 */
struct AnimationState {
    std::unordered_map<std::string, std::string> Properties; // CSS属性
    
    AnimationState() = default;
};

/**
 * CHTL JS动画节点
 * 表示animate{}动画函数，封装requestAnimationFrame
 */
class AnimateNode : public CHTLJSBaseNode {
private:
    std::string m_AnimationName;                        // 动画名称
    std::vector<std::string> m_Targets;                 // 目标选择器列表
    int m_Duration;                                     // 持续时间（ms）
    std::string m_Easing;                               // 缓动函数
    
    AnimationState m_BeginState;                        // 起始状态
    AnimationState m_EndState;                          // 结束状态
    std::vector<AnimationKeyframe> m_Keyframes;         // 关键帧列表
    
    int m_Loop;                                         // 循环次数（-1为无限循环）
    std::string m_Direction;                            // 播放方向
    int m_Delay;                                        // 开始延迟（ms）
    std::string m_Callback;                             // 完成回调函数
    
    bool m_HasBeginState;                               // 是否有起始状态
    bool m_HasEndState;                                 // 是否有结束状态
    bool m_HasKeyframes;                                // 是否有关键帧
    bool m_HasCallback;                                 // 是否有回调
    bool m_IsInfiniteLoop;                              // 是否为无限循环

public:
    /**
     * 构造函数
     * @param animationName 动画名称
     * @param line 行号
     * @param column 列号
     */
    AnimateNode(const std::string& animationName = "", size_t line = 0, size_t column = 0);
    
    /**
     * 析构函数
     */
    ~AnimateNode() override = default;
    
    /**
     * 获取动画名称
     * @return 动画名称
     */
    const std::string& GetAnimationName() const { return m_AnimationName; }
    
    /**
     * 设置动画名称
     * @param name 新动画名称
     */
    void SetAnimationName(const std::string& name) { m_AnimationName = name; }
    
    // 目标管理
    /**
     * 添加动画目标
     * @param target 目标选择器
     */
    void AddTarget(const std::string& target);
    
    /**
     * 获取所有目标
     * @return 目标列表
     */
    const std::vector<std::string>& GetTargets() const { return m_Targets; }
    
    /**
     * 检查是否有目标
     * @return 是否有目标
     */
    bool HasTargets() const { return !m_Targets.empty(); }
    
    // 动画属性管理
    /**
     * 设置持续时间
     * @param duration 持续时间（ms）
     */
    void SetDuration(int duration) { m_Duration = duration; }
    
    /**
     * 获取持续时间
     * @return 持续时间
     */
    int GetDuration() const { return m_Duration; }
    
    /**
     * 设置缓动函数
     * @param easing 缓动函数
     */
    void SetEasing(const std::string& easing) { m_Easing = easing; }
    
    /**
     * 获取缓动函数
     * @return 缓动函数
     */
    const std::string& GetEasing() const { return m_Easing; }
    
    /**
     * 设置循环次数
     * @param loop 循环次数
     */
    void SetLoop(int loop);
    
    /**
     * 获取循环次数
     * @return 循环次数
     */
    int GetLoop() const { return m_Loop; }
    
    /**
     * 设置播放方向
     * @param direction 播放方向
     */
    void SetDirection(const std::string& direction) { m_Direction = direction; }
    
    /**
     * 获取播放方向
     * @return 播放方向
     */
    const std::string& GetDirection() const { return m_Direction; }
    
    /**
     * 设置延迟
     * @param delay 延迟时间（ms）
     */
    void SetDelay(int delay) { m_Delay = delay; }
    
    /**
     * 获取延迟
     * @return 延迟时间
     */
    int GetDelay() const { return m_Delay; }
    
    /**
     * 设置回调函数
     * @param callback 回调函数
     */
    void SetCallback(const std::string& callback);
    
    /**
     * 获取回调函数
     * @return 回调函数
     */
    const std::string& GetCallback() const { return m_Callback; }
    
    // 动画状态管理
    /**
     * 设置起始状态
     * @param state 起始状态
     */
    void SetBeginState(const AnimationState& state);
    
    /**
     * 添加起始状态属性
     * @param property 属性名
     * @param value 属性值
     */
    void AddBeginStateProperty(const std::string& property, const std::string& value);
    
    /**
     * 获取起始状态
     * @return 起始状态
     */
    const AnimationState& GetBeginState() const { return m_BeginState; }
    
    /**
     * 设置结束状态
     * @param state 结束状态
     */
    void SetEndState(const AnimationState& state);
    
    /**
     * 添加结束状态属性
     * @param property 属性名
     * @param value 属性值
     */
    void AddEndStateProperty(const std::string& property, const std::string& value);
    
    /**
     * 获取结束状态
     * @return 结束状态
     */
    const AnimationState& GetEndState() const { return m_EndState; }
    
    // 关键帧管理
    /**
     * 添加关键帧
     * @param keyframe 关键帧
     */
    void AddKeyframe(const AnimationKeyframe& keyframe);
    
    /**
     * 添加关键帧
     * @param at 时刻
     * @param properties 属性映射
     */
    void AddKeyframe(double at, const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * 获取所有关键帧
     * @return 关键帧列表
     */
    const std::vector<AnimationKeyframe>& GetKeyframes() const { return m_Keyframes; }
    
    /**
     * 检查是否有关键帧
     * @return 是否有关键帧
     */
    bool HasKeyframes() const { return m_HasKeyframes; }
    
    // 状态检查
    /**
     * 检查是否有起始状态
     * @return 是否有起始状态
     */
    bool HasBeginState() const { return m_HasBeginState; }
    
    /**
     * 检查是否有结束状态
     * @return 是否有结束状态
     */
    bool HasEndState() const { return m_HasEndState; }
    
    /**
     * 检查是否有回调
     * @return 是否有回调
     */
    bool HasCallback() const { return m_HasCallback; }
    
    /**
     * 检查是否为无限循环
     * @return 是否为无限循环
     */
    bool IsInfiniteLoop() const { return m_IsInfiniteLoop; }
    
    // 动画解析
    /**
     * 解析动画内容
     * @param animateContent animate{}块内容
     */
    void ParseAnimateContent(const std::string& animateContent);
    
    /**
     * 解析动画属性
     * @param property 属性名
     * @param value 属性值
     */
    void ParseAnimationProperty(const std::string& property, const std::string& value);
    
    /**
     * 解析when关键帧数组
     * @param whenContent when数组内容
     */
    void ParseWhenKeyframes(const std::string& whenContent);
    
    /**
     * 解析CSS状态块
     * @param stateContent 状态块内容
     * @return 状态属性映射
     */
    std::unordered_map<std::string, std::string> ParseCSSStateBlock(const std::string& stateContent);
    
    // 代码生成
    /**
     * 生成requestAnimationFrame动画代码
     * @return JavaScript动画代码
     */
    std::string GenerateAnimationCode() const;
    
    /**
     * 生成目标元素查找代码
     * @return 目标查找代码
     */
    std::string GenerateTargetQueryCode() const;
    
    /**
     * 生成关键帧处理代码
     * @return 关键帧代码
     */
    std::string GenerateKeyframeCode() const;
    
    /**
     * 生成缓动函数代码
     * @return 缓动函数代码
     */
    std::string GenerateEasingCode() const;
    
    // 访问者模式实现
    void Accept(CHTLJSNodeVisitor* visitor) override;
    
    // 克隆实现
    std::unique_ptr<CHTLJSBaseNode> Clone() const override;
    
    // JavaScript代码生成
    std::string GenerateJavaScript() const override;
    
    // 调试输出
    std::string ToString(int indent = 0) const override;

private:
    /**
     * 验证动画属性
     * @return 是否有效
     */
    bool ValidateAnimationProperties() const;
    
    /**
     * 生成唯一动画名称
     * @return 唯一名称
     */
    std::string GenerateUniqueAnimationName() const;
    
    /**
     * 解析目标选择器
     * @param targetContent 目标内容
     */
    void ParseTargetSelectors(const std::string& targetContent);
    
    /**
     * 验证缓动函数
     * @param easing 缓动函数
     * @return 是否有效
     */
    bool IsValidEasing(const std::string& easing) const;
};

} // namespace CHTLJS