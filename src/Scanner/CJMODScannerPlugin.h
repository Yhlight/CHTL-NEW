#pragma once

#include "../CHTL JS/CJMODSystem/CJMODCore.h"
#include <string>
#include <vector>

// 前向声明
namespace CHTL {
    struct CodeFragment;
    enum class FragmentType;
}

/**
 * CJMOD扫描器插件
 * 作为统一扫描器的"挂件"，只有Import CJMOD后才启用
 * 提供双指针扫描法和前置截取法两种强大的扫描方式
 * 严格按照完善选择器自动化与引用规则.ini实现
 */
class CJMODScannerPlugin {
private:
    std::string m_SourceCode;                   // 源代码
    bool m_IsActive;                           // 是否激活
    
    // 双指针扫描法状态
    size_t m_PointerA;                         // 前导指针
    size_t m_PointerB;                         // 收集指针
    bool m_CollectingState;                    // 是否处于收集状态
    std::vector<std::string> m_CollectedFragments; // 收集的片段
    
    // 前置截取法状态
    size_t m_CutPosition;                      // 截取位置
    std::vector<std::string> m_CutFragments;   // 截取的片段

public:
    /**
     * 构造函数
     */
    CJMODScannerPlugin();
    
    /**
     * 设置源代码
     * @param sourceCode 源代码
     */
    void SetSourceCode(const std::string& sourceCode);
    
    /**
     * 激活CJMOD扫描
     */
    void Activate();
    
    /**
     * 停用CJMOD扫描
     */
    void Deactivate();
    
    /**
     * 检查是否激活
     * @return 是否激活
     */
    bool IsActive() const { return m_IsActive; }
    
    /**
     * 双指针扫描法
     * 类似滑动窗口的扫描方式，一开始位于0点，先进行一定长度的预先扫描
     * 若没有CJMOD语法，则将指针A移动到合适的位置
     * 指针A和指针B同步向前，类似滑动窗口
     * 当指针A遇到关键字时，会通知指针B进入收集状态，确保能够收集到完整片段
     * @param keyword 关键字
     * @return 扫描结果片段
     */
    std::vector<CHTL::CodeFragment> DoublePointerScan(const std::string& keyword);
    
    /**
     * 前置截取扫描法
     * 扫到关键字后，会回头截取关键字前面部分的参数
     * 避免错误地发送给编译器，以此获取完整片段
     * @param keyword 关键字
     * @return 截取的片段
     */
    std::vector<CHTL::CodeFragment> PrefixCutScan(const std::string& keyword);
    
    /**
     * 智能选择扫描方法
     * 根据代码复杂度和关键字类型选择最适合的扫描方法
     * @param keyword 关键字
     * @return 扫描结果
     */
    std::vector<CHTL::CodeFragment> SmartScan(const std::string& keyword);

private:
    /**
     * 预先扫描检测CJMOD语法
     * @param length 扫描长度
     * @return 是否发现CJMOD语法
     */
    bool PrescanForCJMODSyntax(size_t length);
    
    /**
     * 移动指针A到合适位置
     */
    void MovePointerAToSuitablePosition();
    
    /**
     * 同步移动两个指针
     */
    void SyncMovePointers();
    
    /**
     * 进入收集状态
     */
    void EnterCollectionState();
    
    /**
     * 收集完整片段
     * @return 收集的片段
     */
    std::string CollectCompleteFragment();
    
    /**
     * 扫描到关键字位置
     * @param keyword 关键字
     * @return 关键字位置
     */
    size_t ScanToKeyword(const std::string& keyword);
    
    /**
     * 回头截取前置参数
     * @param keywordPos 关键字位置
     * @return 截取的参数
     */
    std::string CutPrefixParameters(size_t keywordPos);
    
    /**
     * 获取完整片段（包含关键字）
     * @param keywordPos 关键字位置
     * @return 完整片段
     */
    std::string GetCompleteFragmentFromKeyword(size_t keywordPos);
    
    /**
     * 创建代码片段
     * @param content 内容
     * @param type 类型
     * @param start 开始位置
     * @param end 结束位置
     * @return 代码片段
     */
    CHTL::CodeFragment CreateCodeFragment(const std::string& content, CHTL::FragmentType type, size_t start, size_t end);
};