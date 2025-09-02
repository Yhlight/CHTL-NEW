#include "State.h"

namespace CHTL_JS
{
    CHTLJSState::CHTLJSState()
    {
        m_stateStack.push_back(CHTLJSCompileState::GLOBAL_SCOPE);
    }

    void CHTLJSState::PushState(CHTLJSCompileState state)
    {
        m_stateStack.push_back(state);
    }

    void CHTLJSState::PopState()
    {
        if (m_stateStack.size() > 1)
        {
            m_stateStack.pop_back();
        }
    }

    CHTLJSCompileState CHTLJSState::GetCurrentState() const
    {
        if (m_stateStack.empty())
        {
            throw std::runtime_error("Error: CHTL_JS state stack is empty.");
        }
        return m_stateStack.back();
    }

    bool CHTLJSState::IsInState(CHTLJSCompileState state) const
    {
        return GetCurrentState() == state;
    }
}