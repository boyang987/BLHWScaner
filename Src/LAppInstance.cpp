
#include "LAppInstance.h"

#include <Windows.h>

/// @brief 应用程序实例类
class CAppInstance
{
public:
    /// @brief 构造函数
    explicit CAppInstance(IN const wstring& name)
    {
        m_hMutex = NULL;
        m_bIsAlreadyExist = false;

        m_hMutex = CreateMutexW(NULL, FALSE, name.c_str());
        if (NULL == m_hMutex)
            return;

        if (ERROR_ALREADY_EXISTS == GetLastError())
        {
            m_bIsAlreadyExist = true;
        }
    }

    /// @brief 析构函数
    ~CAppInstance()
    {
        if (NULL != m_hMutex)
        {
            CloseHandle(m_hMutex);
            m_hMutex = NULL;
        }
    }

    /// @brief 判断是否已经存在该实例 
    bool IsAlreadyExist()
    {
        return m_bIsAlreadyExist;
    }

private:
    bool m_bIsAlreadyExist; ///< 标志是否已经存在
    HANDLE m_hMutex; ///< 互斥体句柄
};

LAppInstance::LAppInstance(IN const wstring& name)
{
    m_pAppInstance = new CAppInstance(name);
}

LAppInstance::~LAppInstance()
{
    if (NULL != m_pAppInstance)
    {
        delete m_pAppInstance;
    }
}

bool LAppInstance::IsAlreadyExist()
{
    if (NULL == m_pAppInstance)
        return false;

    return m_pAppInstance->IsAlreadyExist();
}