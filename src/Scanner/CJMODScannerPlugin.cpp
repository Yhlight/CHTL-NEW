#include "CJMODScannerPlugin.h"
#include "CHTLUnifiedScanner.h"
#include <algorithm>
#include <regex>
#include <iostream>

CJMODScannerPlugin::CJMODScannerPlugin()
    : m_IsActive(false), m_PointerA(0), m_PointerB(0), m_CollectingState(false), m_CutPosition(0) {
}

void CJMODScannerPlugin::SetSourceCode(const std::string& sourceCode) {
    m_SourceCode = sourceCode;
    m_PointerA = 0;
    m_PointerB = 0;
    m_CollectingState = false;
    m_CutPosition = 0;
    m_CollectedFragments.clear();
    m_CutFragments.clear();
}

void CJMODScannerPlugin::Activate() {
    m_IsActive = true;
    std::cout << "🔥 CJMOD扫描挂件已激活（双指针扫描法 + 前置截取法）" << std::endl;
}

void CJMODScannerPlugin::Deactivate() {
    m_IsActive = false;
    std::cout << "⚪ CJMOD扫描挂件已停用" << std::endl;
}

std::vector<CHTL::CodeFragment> CJMODScannerPlugin::SmartScan(const std::string& keyword) {
    if (!m_IsActive) {
        return {};
    }
    
    // 智能选择扫描方法
    if (keyword.empty() || keyword.find("**") != std::string::npos) {
        // 对于操作符关键字，使用双指针扫描法
        return DoublePointerScan(keyword);
    } else {
        // 对于普通关键字，使用前置截取法
        return PrefixCutScan(keyword);
    }
}

std::vector<CHTL::CodeFragment> CJMODScannerPlugin::DoublePointerScan(const std::string& keyword) {
    std::vector<CHTL::CodeFragment> fragments;
    
    if (!m_IsActive || m_SourceCode.empty()) {
        return fragments;
    }
    
    std::cout << "🔍 启动双指针扫描法，关键字: " << keyword << std::endl;
    
    // 重置状态
    m_PointerA = 0;
    m_PointerB = 0;
    m_CollectingState = false;
    m_CollectedFragments.clear();
    
    // 1. 先进行一定长度的预先扫描
    if (!PrescanForCJMODSyntax(50)) {
        std::cout << "   📋 预扫描：未发现CJMOD语法，移动指针A" << std::endl;
        MovePointerAToSuitablePosition();
    } else {
        std::cout << "   📋 预扫描：发现CJMOD语法特征" << std::endl;
    }
    
    // 2. 指针A和指针B同步向前，类似滑动窗口
    while (m_PointerA < m_SourceCode.length() && m_PointerB < m_SourceCode.length()) {
        SyncMovePointers();
        
        // 检查指针A是否遇到关键字
        if (!keyword.empty() && m_PointerA + keyword.length() <= m_SourceCode.length()) {
            if (m_SourceCode.substr(m_PointerA, keyword.length()) == keyword) {
                std::cout << "   🎯 指针A遇到关键字: " << keyword << " 位置: " << m_PointerA << std::endl;
                
                // 通知指针B进入收集状态
                EnterCollectionState();
                
                // 收集完整片段
                std::string fragmentContent = CollectCompleteFragment();
                if (!fragmentContent.empty()) {
                    fragments.push_back(CreateCodeFragment(
                        fragmentContent, 
                        CHTL::FragmentType::CHTL_JS_FRAGMENT, 
                        m_PointerB, 
                        m_PointerA
                    ));
                    std::cout << "   ✅ 收集片段: " << fragmentContent.substr(0, 50) << "..." << std::endl;
                }
            }
        }
    }
    
    std::cout << "   📊 双指针扫描完成，收集片段: " << fragments.size() << " 个" << std::endl;
    return fragments;
}

std::vector<CHTL::CodeFragment> CJMODScannerPlugin::PrefixCutScan(const std::string& keyword) {
    std::vector<CHTL::CodeFragment> fragments;
    
    if (!m_IsActive || m_SourceCode.empty() || keyword.empty()) {
        return fragments;
    }
    
    std::cout << "✂️  启动前置截取扫描法，关键字: " << keyword << std::endl;
    
    // 重置状态
    m_CutPosition = 0;
    m_CutFragments.clear();
    
    while (m_CutPosition < m_SourceCode.length()) {
        // 扫描到关键字
        size_t keywordPos = ScanToKeyword(keyword);
        
        if (keywordPos != std::string::npos) {
            std::cout << "   🎯 发现关键字: " << keyword << " 位置: " << keywordPos << std::endl;
            
            // 回头截取关键字前面部分的参数
            std::string prefixParams = CutPrefixParameters(keywordPos);
            if (!prefixParams.empty()) {
                fragments.push_back(CreateCodeFragment(
                    prefixParams, 
                    CHTL::FragmentType::CHTL_JS_FRAGMENT, 
                    m_CutPosition, 
                    keywordPos
                ));
                std::cout << "   ✂️  截取前置参数: " << prefixParams.substr(0, 30) << "..." << std::endl;
            }
            
            // 获取完整片段（包含关键字）
            std::string completeFragment = GetCompleteFragmentFromKeyword(keywordPos);
            if (!completeFragment.empty()) {
                fragments.push_back(CreateCodeFragment(
                    completeFragment, 
                    CHTL::FragmentType::CHTL_JS_FRAGMENT, 
                    keywordPos, 
                    keywordPos + completeFragment.length()
                ));
                std::cout << "   ✅ 获取完整片段: " << completeFragment.substr(0, 30) << "..." << std::endl;
            }
            
            // 移动到关键字后
            m_CutPosition = keywordPos + keyword.length();
        } else {
            break;
        }
    }
    
    std::cout << "   📊 前置截取扫描完成，截取片段: " << fragments.size() << " 个" << std::endl;
    return fragments;
}

bool CJMODScannerPlugin::PrescanForCJMODSyntax(size_t length) {
    // 预扫描检查是否有CJMOD语法特征
    std::string prescanText = m_SourceCode.substr(m_PointerA, std::min(length, m_SourceCode.length() - m_PointerA));
    
    // 检查CJMOD特征：$占位符、**操作符、{{选择器}}等
    bool hasCJMODSyntax = prescanText.find('$') != std::string::npos || 
                         prescanText.find("**") != std::string::npos ||
                         prescanText.find("{{") != std::string::npos ||
                         prescanText.find("vir ") != std::string::npos ||
                         prescanText.find("->") != std::string::npos;
    
    return hasCJMODSyntax;
}

void CJMODScannerPlugin::MovePointerAToSuitablePosition() {
    // 移动指针A到合适的位置（跳过非CJMOD代码）
    while (m_PointerA < m_SourceCode.length()) {
        char ch = m_SourceCode[m_PointerA];
        
        if (ch == '$' || 
            (m_PointerA + 1 < m_SourceCode.length() && m_SourceCode.substr(m_PointerA, 2) == "**") ||
            (m_PointerA + 1 < m_SourceCode.length() && m_SourceCode.substr(m_PointerA, 2) == "{{") ||
            (m_PointerA + 3 < m_SourceCode.length() && m_SourceCode.substr(m_PointerA, 4) == "vir ")) {
            break;
        }
        
        m_PointerA++;
    }
}

void CJMODScannerPlugin::SyncMovePointers() {
    // 同步移动两个指针
    if (!m_CollectingState) {
        m_PointerA++;
        m_PointerB = m_PointerA;
    }
}

void CJMODScannerPlugin::EnterCollectionState() {
    m_CollectingState = true;
    std::cout << "   🔄 进入收集状态，指针B位置: " << m_PointerB << std::endl;
}

std::string CJMODScannerPlugin::CollectCompleteFragment() {
    // 确保能够收集到完整片段
    std::string fragment;
    size_t start = m_PointerB;
    
    // 收集到下一个分隔符或结束
    while (m_PointerA < m_SourceCode.length()) {
        char ch = m_SourceCode[m_PointerA];
        if (ch == ';' || ch == '}' || ch == '\n') {
            break;
        }
        m_PointerA++;
    }
    
    fragment = m_SourceCode.substr(start, m_PointerA - start);
    m_CollectingState = false;
    
    // 去掉首尾空白
    fragment.erase(0, fragment.find_first_not_of(" \t\n\r"));
    fragment.erase(fragment.find_last_not_of(" \t\n\r") + 1);
    
    return fragment;
}

size_t CJMODScannerPlugin::ScanToKeyword(const std::string& keyword) {
    size_t pos = m_SourceCode.find(keyword, m_CutPosition);
    return pos;
}

std::string CJMODScannerPlugin::CutPrefixParameters(size_t keywordPos) {
    // 回头截取关键字前面部分的参数
    // 避免错误地发送给编译器
    
    if (keywordPos <= m_CutPosition) {
        return "";
    }
    
    std::string prefix = m_SourceCode.substr(m_CutPosition, keywordPos - m_CutPosition);
    
    // 去掉首尾空白
    prefix.erase(0, prefix.find_first_not_of(" \t\n\r"));
    prefix.erase(prefix.find_last_not_of(" \t\n\r") + 1);
    
    return prefix;
}

std::string CJMODScannerPlugin::GetCompleteFragmentFromKeyword(size_t keywordPos) {
    // 获取从关键字开始的完整片段
    size_t start = keywordPos;
    size_t end = m_SourceCode.find_first_of(";\n}", start);
    
    if (end == std::string::npos) {
        end = m_SourceCode.length();
    }
    
    std::string fragment = m_SourceCode.substr(start, end - start);
    
    // 去掉首尾空白
    fragment.erase(0, fragment.find_first_not_of(" \t\n\r"));
    fragment.erase(fragment.find_last_not_of(" \t\n\r") + 1);
    
    return fragment;
}

CHTL::CodeFragment CJMODScannerPlugin::CreateCodeFragment(const std::string& content, CHTL::FragmentType type, size_t start, size_t end) {
    CHTL::CodeFragment fragment;
    fragment.Content = content;
    fragment.Type = type;
    fragment.StartPosition = start;
    fragment.EndPosition = end;
    fragment.LineNumber = 1; // 简化行号计算
    fragment.ColumnNumber = 1;
    return fragment;
}