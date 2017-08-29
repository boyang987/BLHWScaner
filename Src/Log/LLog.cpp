
#include "LLog.h"

#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#include <ctime>
#include <cstring>

#include <map>
using std::map;

#include <Windows.h>

/// @brief 手动锁类
class CManuLock
{
public:
    /// @brief 构造函数
    CManuLock()
    {
        InitializeCriticalSection(&m_criSection);
    }

    /// @brief 析构函数
    ~CManuLock()
    {
        DeleteCriticalSection(&m_criSection);
    }

    /// @brief 上锁
    void Lock()
    {
        EnterCriticalSection(&m_criSection);
    }

    /// @brief 解锁
    void UnLock()
    {
        LeaveCriticalSection(&m_criSection);
    }

private:
    CManuLock(const CManuLock&);
    void operator = (const CManuLock&);

    CRITICAL_SECTION m_criSection; ///< 临界区变量
};

/// @brief 自动锁类
class CAutoLock
{
public:
    /// @brief 构造函数 
    /// @param[in] pLock 手动锁对象
    CAutoLock(IN CManuLock* pLock)
    {
        if (pLock != 0)
        {
            m_pManuLock = pLock;
            m_pManuLock->Lock();
        }
        
    }

    /// @brief 析构函数
    ~CAutoLock()
    {
        if (m_pManuLock != 0)
        {
            m_pManuLock->UnLock();
        }
    }

private:
    CAutoLock(const CAutoLock& );
    void operator = (const CAutoLock&);

    CManuLock* m_pManuLock; ///< 手动锁对象
};

/// @brief 调试LOG类
/// 使用该类写LOG后可以直接写到硬盘中, 不会存在写到缓存中的情况
class CDebugLog
{
public:
    /// @brief 构造函数
    CDebugLog()
    {
        m_hFile = NULL;
        m_pWriteBuffer = 0;
        m_bufferUsedSize = 0;
        m_bufferTotalSize = 512;

        m_fillChar = '\0';
    }

    /// @brief析构函数
    ~CDebugLog()
    {
        this->Clear();
    }

    /// @brief 追加打开文件
    /// 如果文件存在则定位到文件未进行写
    /// @param[in] fileName 文件名
    /// @return 成功返回true, 失败返回false
    bool AppendOpen(IN const wstring& fileName)
    {
        this->Clear();

        // 打开文件
        m_hFile = CreateFileW(
            fileName.c_str(), 
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ, 
            NULL, 
            OPEN_EXISTING, 
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, 
            NULL);
        if (0 == m_hFile || INVALID_HANDLE_VALUE == m_hFile)
        {
            // 如果文件不存在则创建新文件
            return this->NewOpen(fileName);
        }

        m_pWriteBuffer = new unsigned char[m_bufferTotalSize];
        if (0 == m_pWriteBuffer)
        {
            this->Clear();
            return false;
        }

        memset(m_pWriteBuffer, m_fillChar, m_bufferTotalSize);

        // 移动到文件尾
        SetFilePointer(m_hFile, 0, 0, FILE_END);
        // 换行
        this->Write(L"\r\n", 4);

        return true;
    }

    /// @brief 新打开文件
    /// 如果文件已经存在, 则替换原有文件
    /// @param[in] fileName 文件名
    /// @return 成功返回true, 失败返回false
    bool NewOpen(IN const wstring& fileName)
    {
        this->Clear();

        // 创建文件
        m_hFile = CreateFileW(
            fileName.c_str(), 
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ, 
            NULL, 
            CREATE_ALWAYS, 
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, 
            NULL);
        if (0 == m_hFile || INVALID_HANDLE_VALUE == m_hFile)
        {
            this->Clear();
            return false;
        }

        m_pWriteBuffer = new unsigned char[m_bufferTotalSize];
        if (0 == m_pWriteBuffer)
        {
            this->Clear();
            return false;
        }

        memset(m_pWriteBuffer, m_fillChar, m_bufferTotalSize);

        // 预先在文件头写上UNICODE标志符
        char unicode[2] = {0};
        unicode[0] = (char)0xFF;
        unicode[1] = (char)0xFE;
        this->Write(unicode, 2);

        return true;
    }

    /// @brief 打印一行LOG
    /// @param[in] wstr LOG信息
    /// @return 成功返回true, 失败返回false
    bool PrintLine(IN const wstring& wstr)
    {
        CAutoLock lock(&m_fileLock);
        this->Write(wstr.c_str(), wstr.length() * 2);
        return this->Write(L"\r\n", 4);
    }

    /// @brief 关闭文件
    void Close()
    {
        this->Clear();
    }

private:
    /// @brief 写文件
    /// @param[in] pBuffer 写缓冲区
    /// @param[in] size 写缓冲区大小
    /// @return 成功返回true, 失败返回false
    bool Write(IN const void* pBuffer, unsigned int size)
    {
        if (NULL == m_hFile || 
            INVALID_HANDLE_VALUE == m_hFile)
            return false;

        if (0 == pBuffer ||
            0 == size)
            return false;

        unsigned long count = 0;
        BOOL iRet = FALSE;

        if ((m_bufferUsedSize + size) < m_bufferTotalSize)
        {
            memcpy(m_pWriteBuffer + m_bufferUsedSize, pBuffer, size);

            iRet = WriteFile(m_hFile, (LPCVOID)m_pWriteBuffer, m_bufferTotalSize, &count, NULL);
            if (FALSE == iRet)
            {
                memset(m_pWriteBuffer + m_bufferUsedSize, m_fillChar, size);
                return false;
            }

            SetFilePointer(m_hFile, -1 * (long)m_bufferTotalSize, 0, FILE_CURRENT);
            m_bufferUsedSize += size;
            return true;
        }

        if ((m_bufferUsedSize + size) == m_bufferTotalSize)
        {
            memcpy(m_pWriteBuffer + m_bufferUsedSize, pBuffer, size);

            iRet = WriteFile(m_hFile, (LPCVOID)m_pWriteBuffer, m_bufferTotalSize, &count, NULL);
            if (FALSE == iRet)
            {
                memset(m_pWriteBuffer + m_bufferUsedSize, m_fillChar, size);
                return false;
            }

            m_bufferUsedSize = 0;
            memset(m_pWriteBuffer, m_fillChar, m_bufferTotalSize);

            return true;
        }

        if ((m_bufferUsedSize + size) > m_bufferTotalSize)
        {
            unsigned int newWriteCount = m_bufferTotalSize-m_bufferUsedSize;
            memcpy(m_pWriteBuffer + m_bufferUsedSize, pBuffer, newWriteCount);
            iRet = WriteFile(m_hFile, (LPCVOID)m_pWriteBuffer, m_bufferTotalSize, &count, NULL);
            if (FALSE == iRet)
            {
                memset(m_pWriteBuffer + m_bufferUsedSize, m_fillChar, newWriteCount);
                return false;
            }

            m_bufferUsedSize = 0;
            memset(m_pWriteBuffer, m_fillChar, m_bufferTotalSize);

            void* pNewBuffer = (char*)pBuffer + newWriteCount;
            return this->Write(pNewBuffer, size-newWriteCount);

        }

        return false;
    }

    /// @brief 清理资源
    void Clear()
    {
        if (NULL != m_hFile &&
            INVALID_HANDLE_VALUE != m_hFile)
        {
            CloseHandle(m_hFile);
            m_hFile = 0;
        }

        if (0 != m_pWriteBuffer)
        {
            delete[] m_pWriteBuffer;
            m_pWriteBuffer = 0;
        }

        m_bufferUsedSize = 0;
    }

private:
    HANDLE m_hFile; ///< 文件句柄

    char m_fillChar; ///< 填充字符
    unsigned char* m_pWriteBuffer; ///< 写缓冲区
    unsigned int m_bufferUsedSize; ///< 缓冲区中已被使用的大小
    unsigned int m_bufferTotalSize; ///< 缓冲区总大小

    CManuLock m_fileLock; ///< 文件锁对象, 用于多线程
};

namespace LLog
{
   

    /// @brief LOG属性结构
    struct SLogProperty 
    {
        CDebugLog* DebugLog; ///< 文件指针
        bool ShowThreadId; ///< 标识是否显示线程Id
        bool ShowTime; ///< 标识是否显示当前时间
        bool IsAppendOpen; ///< 标志是否以追加方法打开LOG
    };

    static SLogProperty s_logProperty = 
    {
        0,
        false,
        false,
        false
    };

    /// @brief 转换为宽字节字符串
    /// @param[in] strSrc 源字符串
    /// @param[in] strDest 存储转换后的字符串
    /// @return 成功返回true, 失败返回false
    bool StringToWString(const string& strSrc, wstring& strDest)
    {
        int nSize = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.length(), 0, 0);  
        if(nSize <= 0)
            return false;  
        wchar_t* pwszDst = new wchar_t[nSize+1];

        int iRet = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), strSrc.length(), pwszDst, nSize);

        pwszDst[nSize] = 0; 

        strDest.clear();
        strDest.assign(pwszDst);

        delete[] pwszDst;

        return true;
    }

    /// @brief 获取辅助信息
    void GetAssistInfor(OUT wstring* pWStr)
    {
        if (pWStr == 0)
            return;
        pWStr->clear();

        if (s_logProperty.ShowThreadId)
        {
            static map<DWORD, DWORD> s_threadIdMap;
            // LOG中增加当前线程ID, 用以在多线程中区分不同线程的LOG
            DWORD dwId = GetCurrentThreadId();
            if (s_threadIdMap.count(dwId) == 0)
            {
                s_threadIdMap[dwId] = 0;
                s_threadIdMap[dwId] = s_threadIdMap.size();
            }


            wchar_t threadId[64] = {0};
            wsprintfW(threadId, L"[Thread %u]", s_threadIdMap[dwId]);
            pWStr->append(threadId);
        }


        if (s_logProperty.ShowTime)
        {
            // LOG中增加当前时间
            time_t t = time(0);
            wchar_t szTime[64] = {0}; 
            tm tmTemp;
            localtime_s(&tmTemp, &t);
            wcsftime(szTime, sizeof(szTime), L"[%H:%M:%S]",&tmTemp );
            pWStr->append(szTime);
        }
    }

    bool Open(IN const wchar_t* szFileName)
    {
        if (s_logProperty.DebugLog != 0)
        {
            delete s_logProperty.DebugLog;
            s_logProperty.DebugLog = 0;
        }

        s_logProperty.DebugLog = new CDebugLog();
        bool bRet = false;
        if (s_logProperty.IsAppendOpen)
            bRet = s_logProperty.DebugLog->AppendOpen(szFileName);
        else
            bRet = s_logProperty.DebugLog->NewOpen(szFileName);

        return bRet;
    }

    void Close()
    {
        if (s_logProperty.DebugLog != 0)
        {
            delete s_logProperty.DebugLog;
            s_logProperty.DebugLog = 0;
        }
    }

    void SetAppendOpen(IN bool bFlag)
    {
        s_logProperty.IsAppendOpen = bFlag;
    }

    void ShowThreadId(IN bool bFlag)
    {
        s_logProperty.ShowThreadId = bFlag;
    }

    void ShowTime(IN bool bFlag)
    {
        s_logProperty.ShowTime = bFlag;
    }

    void WriteLineW(IN const wchar_t* szFormat, ...)
    {
        if (s_logProperty.DebugLog == 0)
            return;

        va_list args;
        va_start(args, szFormat);
        wchar_t printfBuffer[1024] = {0};
        vswprintf_s(printfBuffer, szFormat, args);
        va_end(args);

        wstring message;
        GetAssistInfor(&message);
        message.append(printfBuffer);

        s_logProperty.DebugLog->PrintLine(message);
    }

    void WriteLineA(IN const char* szFormat, ...)
    {
        if (s_logProperty.DebugLog == 0)
            return;

        va_list args;
        va_start(args, szFormat);
        char printfBuffer[1024] = {0};
        vsprintf_s(printfBuffer, szFormat, args);
        va_end(args);

        wstring logMessage;
        GetAssistInfor(&logMessage);

        wstring message;
        StringToWString(printfBuffer, message);

        logMessage += message;
        s_logProperty.DebugLog->PrintLine(logMessage);
    }
}