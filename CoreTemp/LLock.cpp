#include "LLock.h"

#include <Windows.h>


LManualLock::LManualLock()
{
    m_pLock = NULL;

    CRITICAL_SECTION* pCriticalSection = new CRITICAL_SECTION();
	InitializeCriticalSection(pCriticalSection);
    m_pLock = (void*)pCriticalSection;
}

LManualLock::~LManualLock()
{
    if (NULL != m_pLock)
    {
	    DeleteCriticalSection((CRITICAL_SECTION*)m_pLock);
        delete m_pLock;
        m_pLock = NULL;
    }
}

void LManualLock::Lock()
{
	EnterCriticalSection((CRITICAL_SECTION*)m_pLock);
}

void LManualLock::UnLock()
{
	LeaveCriticalSection((CRITICAL_SECTION*)m_pLock);
}

LAutoLock::LAutoLock(LManualLock* pManualLock)
{
	m_pManualLock = pManualLock;
	m_pManualLock->Lock();
}

LAutoLock::~LAutoLock()
{
	m_pManualLock->UnLock();
}