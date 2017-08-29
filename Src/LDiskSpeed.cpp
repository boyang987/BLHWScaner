
#include "LDiskSpeed.h"

#include <ctime>
#include <cstdlib>
#include <process.h>

#include <vector>
using std::vector;

#include <Windows.h>

/// @brief 错误消息字符串列表
static wchar_t* ErrorMsgList[] =
{
    L"No Error",
    L"Open File Error",
    L"Write File Error",
    L"Read File Error",
    L"Alloc Memory Error",
    L"Test Abort Error"
};

/// @brief 产生随机整数
/// @param[in] min 随机整数的最小值
/// @param[in] max 随机整数的最大值
/// @return 随机整数
static int RandInt(int min, int max) 
{
    return rand()%(max - min + 1) + min;
}

/// @brief 创建随机缓冲区
/// 缓冲区中将被随机填充值
/// @param[in] bufferSize 缓冲区大小
/// @return 缓冲区指针
static char* CreateRandomBuffer(unsigned long bufferSize)
{
    char* pBuffer = new char[bufferSize];
    if (0 != pBuffer)
    {
        for (unsigned int i = 0; i < bufferSize; i++)
        {
            pBuffer[i] = (char)RandInt(0, 255);
        }
    }

    return pBuffer;
}

/// @brief 获取Windows错误码
/// @param[out] pWStr 存储错误信息
static void GetWindowsError(OUT wstring* pWStr)
{
    if (0 == pWStr)
        return;

    wchar_t windowsErrorCode[256] = {0};
    wsprintfW(windowsErrorCode, L"Windows Error Code: 0x%x", GetLastError());
    (*pWStr) = windowsErrorCode;
}

/// @brief 磁盘速度测试文件结构
struct LDiskSpeedTestFile
{
    wstring Path; ///< 测试文件路径
    unsigned long Size; ///< 测试文件大小, 测试文件最大为2^32-1(即4G-1)
};

/// @brief 磁盘顺序测试
class CDiskSequenceTest
{
public:

    /// @brief 构造函数
    CDiskSequenceTest()
    {
        m_bStopTest = false;

        m_testFile.Path = L"";
        m_testFile.Size = 0;

        m_testState.Error = DST_NO_ERROR;
        m_testState.ErrorMsg = ErrorMsgList[DST_NO_ERROR];
        m_testState.TestDone = true;
        m_testState.ReadSpeed = 0.0f;
        m_testState.WriteSpeed = 0.0f;
    }

    /// @brief 析构函数
    ~CDiskSequenceTest()
    {

    }

    /// @brief 开始测试
    bool Start(IN const wstring& filePath, IN unsigned int fileSize)
    {
        if (false == m_testState.TestDone)
            return false;

        // 文件大小超出范围16M~4G
        if (fileSize < 16 || fileSize >= 4096)
            return false;
       

        m_bStopTest = false;

        m_testFile.Path = filePath;
        m_testFile.Size = (unsigned long)fileSize * 1024 * 1024;

        m_testState.TestDone = false;
        m_testState.Error = DST_NO_ERROR;
        m_testState.ErrorMsg = ErrorMsgList[DST_NO_ERROR];
        m_testState.ErrorMsgWindows = L"";
        m_testState.WriteSpeed = 0.0f;
        m_testState.ReadSpeed = 0.0f;

        _beginthread(DiskSequenceTestThread, 0, (void*)this);

        return true;
    }

    /// @brief 获取测试状态
    LDiskSpeedTestState& GetState()
    {
        return m_testState;
    }

    /// @brief 停止测试
    void Stop()
    {
        m_bStopTest = true;
    }

private:
    LDiskSpeedTestFile m_testFile; ///< 测试文件
    LDiskSpeedTestState m_testState; ///< 测试状态
    bool m_bStopTest; ///< 标记是否停止测试

private:
    /// @brief 顺序测试
    /// @param[in] pParam 参数
    static void DiskSequenceTestThread(void* pParam)
    {
        /*
        测试算法:
        先以16M的尺寸为单位, 持续向受测分区写入生产1个达到指定大小的文件,
        然后再以同样的单位尺寸读取
        */

        CDiskSequenceTest* pDiskSequenceTest = (CDiskSequenceTest*)pParam;
        LDiskSpeedTestState& testState = pDiskSequenceTest->m_testState;
        LDiskSpeedTestFile& testFile = pDiskSequenceTest->m_testFile;

        HANDLE hFile = 0; // 文件句柄

        clock_t clockBegin; // 计时开始
        clock_t clockEnd; // 计时结束

        // 分配一个16M的读写缓冲区
        const unsigned long RW_BUFFER_SIZE = (unsigned long)(16) * 1024 * 1024;

        char* pRWBuffer = CreateRandomBuffer(RW_BUFFER_SIZE);
        if (0 == pRWBuffer)
        {
            testState.Error = DST_ALLOC_MEMORY_ERROR;
            testState.ErrorMsg = ErrorMsgList[DST_ALLOC_MEMORY_ERROR];
            goto SAFE_EXIT;
        }
        
        // 创建文件, 关闭文件句柄后会自动删除文件, 程序意外关闭后, 也可删除测试文件
        hFile = CreateFileW(
            testFile.Path.c_str(), 
            GENERIC_WRITE | GENERIC_READ, 
            0, 
            NULL, 
            CREATE_ALWAYS, 
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_DELETE_ON_CLOSE, 
            NULL);
        if (0 == hFile || INVALID_HANDLE_VALUE == hFile)
        {
            testState.Error = DST_OPEN_FILE_ERROR;
            testState.ErrorMsg = ErrorMsgList[DST_OPEN_FILE_ERROR];
            GetWindowsError(&testState.ErrorMsgWindows);

            goto SAFE_EXIT;
        }

        // 进行写测试
        clockBegin = clock();
        for (unsigned long i = 0; i < testFile.Size/RW_BUFFER_SIZE; i++)
        {
            unsigned long count = 0;
            BOOL ret = WriteFile(hFile, (LPCVOID)pRWBuffer, RW_BUFFER_SIZE, &count, NULL);
            if (FALSE == ret)
            {
                testState.Error = DST_WRITE_FILE_ERROR;
                testState.ErrorMsg = ErrorMsgList[DST_WRITE_FILE_ERROR];
                GetWindowsError(&testState.ErrorMsgWindows);

                goto SAFE_EXIT;
            }
            
            if (pDiskSequenceTest->m_bStopTest)
            {
                testState.Error = DST_TEST_ABORT_ERROR;
                testState.ErrorMsg = ErrorMsgList[DST_TEST_ABORT_ERROR];
                goto SAFE_EXIT;
            }

            clockEnd = clock();

            // 经历时间, 换算成秒, 计算写入速度
            float time = (float)(clockEnd-clockBegin);
            time = time/1000.0f;
            testState.WriteSpeed = (float)(RW_BUFFER_SIZE/1024/1024) * (i + 1.0f)/time;

        }

        // 进行读测试
        SetFilePointer(hFile, 0, 0, FILE_BEGIN);
        clockBegin = clock();
        for (unsigned long i = 0; i < testFile.Size/RW_BUFFER_SIZE; i++)
        {
            unsigned long count = 0;
            BOOL ret = ReadFile(hFile, (LPVOID)pRWBuffer, RW_BUFFER_SIZE, &count, NULL);
            if (FALSE == ret)
            {
                testState.Error = DST_READ_FILE_ERROR;
                testState.ErrorMsg = ErrorMsgList[DST_READ_FILE_ERROR];
                GetWindowsError(&testState.ErrorMsgWindows);

                goto SAFE_EXIT;
            }

            if (pDiskSequenceTest->m_bStopTest)
            {
                testState.Error = DST_TEST_ABORT_ERROR;
                testState.ErrorMsg = ErrorMsgList[DST_TEST_ABORT_ERROR];
                goto SAFE_EXIT;
            }

            clockEnd = clock();

            // 经历时间, 换算成秒, 计算读取速度
            float time = (float)(clockEnd-clockBegin);
            time = time/1000.0f;
            testState.ReadSpeed = (float)(RW_BUFFER_SIZE/1024/1024) * (i + 1.0f)/time;
        }
        

SAFE_EXIT:

        // 设置测试状态
        testState.TestDone = true;

        // 释放资源
        if (0 != hFile)
        {
            CloseHandle(hFile);
            hFile = 0;
        }
        if (0 != pRWBuffer)
        {
            delete[] pRWBuffer;
            pRWBuffer = 0;
        }
    }
};



class CDisk4KRandomTest
{
public:

    /// @brief 构造函数
    CDisk4KRandomTest()
    {
        m_bStopTest = false;

        m_testState.Error = DST_NO_ERROR;
        m_testState.ErrorMsg = ErrorMsgList[DST_NO_ERROR];
        m_testState.TestDone = true;
        m_testState.ReadSpeed = 0.0f;
        m_testState.WriteSpeed = 0.0f;

        m_testFile.Path = L"";
        m_testFile.Size = 0;

        m_testFileHandle = 0;
    }

    /// @brief 析构函数
    ~CDisk4KRandomTest()
    {
        if (m_testFileHandle != 0 &&
            m_testFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_testFileHandle);
            m_testFileHandle = 0;
        }

    }

    /// @brief 开始测试
    bool Start(IN const wstring& filePath, IN unsigned int fileSize)
    {
        if (false == m_testState.TestDone)
            return false;

        // 文件要求小于等于4G
        if (fileSize > 4096)
            return false;

        m_bStopTest = false;

        m_testFile.Path = filePath;

        // 做乘法时遇到整型常量溢出,只需要把第一个乘数强制转换为无符号32位数
        m_testFile.Size = (unsigned long)fileSize * 1024 * 1024;

        m_testState.TestDone = false;
        m_testState.Error = DST_NO_ERROR;
        m_testState.ErrorMsg = ErrorMsgList[DST_NO_ERROR];
        m_testState.ErrorMsgWindows = L"";
        m_testState.WriteSpeed = 0.0f;
        m_testState.ReadSpeed = 0.0f;

        if (m_testFileHandle != 0 &&
            m_testFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_testFileHandle);
            m_testFileHandle = 0;
        }

        // 创建读写地址列表, 以4K为每个读写单位
        m_rwAddressList.clear();
        m_rwAddressList.resize(m_testFile.Size/(4 * 1024));
        for (unsigned long i = 0; i < m_rwAddressList.size(); i++)
        {
            m_rwAddressList[i] = i;
        }
        // 打乱读写地址中的顺序
        for (int j = 0; j < 3; j++)
        {
            for (unsigned long i = 0; i < m_rwAddressList.size(); i++)
            {
                int randIndex = RandInt(0, m_rwAddressList.size()-1);
                m_rwAddressList[i] = m_rwAddressList[randIndex];
            }
        }
        

        _beginthread(Disk4KRandomTestThread, 0, (void*)this);

        return true;
    }

    /// @brief 获取测试状态
    LDiskSpeedTestState& GetState()
    {
        return m_testState;
    }

    /// @brief 停止测试
    void Stop()
    {
        if (true == m_testState.TestDone)
            return;

        m_bStopTest = true;

    }

private:
    /// @brief 4K随机写测试
    /// 该项测试最多进行20秒
    bool Disk4KRandomWriteTest()
    {
        // 写缓冲区4K
        const unsigned long  WRITE_BUFFER_SIZE = (unsigned long)(4) * 1024;

        // 标识函数返回值
        bool bRet = false;
        
        // 计时开始变量
        clock_t clockBegin;
        // 计时结束变量
        clock_t clockEnd; 

        // 定义写缓冲区
        char* pWriteBuffer = 0;
        

        // 分配一个写缓冲区
        pWriteBuffer = CreateRandomBuffer(WRITE_BUFFER_SIZE);
        if (0 == pWriteBuffer)
        {
            bRet = false;
            m_testState.Error = DST_ALLOC_MEMORY_ERROR;
            m_testState.ErrorMsg = ErrorMsgList[DST_ALLOC_MEMORY_ERROR];
            goto SAFE_EXIT;
        }

        // 进行写测试
        clockBegin = clock();
        clockEnd = clock();
        for (unsigned long i = 0; i < m_rwAddressList.size(); i++)
        {
            unsigned long randAddr = m_rwAddressList[i];
            SetFilePointer(m_testFileHandle, randAddr * WRITE_BUFFER_SIZE, 0, FILE_BEGIN);

            unsigned long count = 0;
            BOOL ret = WriteFile(m_testFileHandle, (LPCVOID)pWriteBuffer, WRITE_BUFFER_SIZE, &count, NULL);

            if (FALSE == ret)
            {
                bRet = false;
                m_testState.Error = DST_WRITE_FILE_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_WRITE_FILE_ERROR];
                GetWindowsError(&m_testState.ErrorMsgWindows);

                goto SAFE_EXIT;
            }

            if (m_bStopTest)
            {
                bRet = false;
                m_testState.Error = DST_TEST_ABORT_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_TEST_ABORT_ERROR];
                goto SAFE_EXIT;
            }

            clockEnd = clock();

            // 经历时间, 换算成秒, 计算写入速度
            float time = (float)(clockEnd-clockBegin);
            time = time/1000.0f;
            m_testState.WriteSpeed = (float)WRITE_BUFFER_SIZE/1024.0f/1024.0f * (i + 1)/time;
        }

        bRet = true;

SAFE_EXIT:
        if (0 != pWriteBuffer)
        {
            delete[] pWriteBuffer;
            pWriteBuffer = 0;
        }

        return bRet;
    }

    /// @brief 4K随机读测试
    /// 该项测试最多进行20秒
    bool Disk4KRandomReadTest()
    {
        // 读缓冲区4K
        const unsigned long  READ_BUFFER_LEN = (unsigned long)(4) * 1024;

        // 标识函数返回值
        bool bRet = false;

        // 计时开始变量
        clock_t clockBegin;
        // 计时结束变量
        clock_t clockEnd; 

        // 定义读缓冲区
        char* pReadBuffer = 0;


        // 分配一个读缓冲区
        pReadBuffer = CreateRandomBuffer(READ_BUFFER_LEN);
        if (0 == pReadBuffer)
        {
            bRet = false;
            m_testState.Error = DST_ALLOC_MEMORY_ERROR;
            m_testState.ErrorMsg = ErrorMsgList[DST_ALLOC_MEMORY_ERROR];
            goto SAFE_EXIT;
        }

        // 进行读测试
        clockBegin = clock();
        clockEnd = clock();
        for (unsigned long i = 0; i < m_rwAddressList.size(); i++)
        {
            // 4K * 256K = 1G
            unsigned long randAddr = m_rwAddressList[i];
            SetFilePointer(m_testFileHandle, randAddr * READ_BUFFER_LEN, 0, FILE_BEGIN);

            unsigned long count = 0;
            BOOL ret = ReadFile(m_testFileHandle, (LPVOID)pReadBuffer, READ_BUFFER_LEN, &count, NULL);

            if (FALSE == ret)
            {
                bRet = false;
                m_testState.Error = DST_READ_FILE_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_READ_FILE_ERROR];
                GetWindowsError(&m_testState.ErrorMsgWindows);

                goto SAFE_EXIT;
            }

            if (m_bStopTest)
            {
                bRet = false;
                m_testState.Error = DST_TEST_ABORT_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_TEST_ABORT_ERROR];
                goto SAFE_EXIT;
            }

            clockEnd = clock();

            // 经历时间, 换算成秒, 计算读取速度
            float time = (float)(clockEnd-clockBegin);
            time = time/1000.0f;
            m_testState.ReadSpeed = (float)READ_BUFFER_LEN/1024.0f/1024.0f * (i + 1)/time;
        }

        bRet = true;

SAFE_EXIT:
        if (0 != pReadBuffer)
        {
            delete[] pReadBuffer;
            pReadBuffer = 0;
        }

        return bRet;
    }

    /// @brief 生成文件
    /// @return 成功返回true, 失败返回false
    bool GenerateTestFile()
    {
        // 512KB
        const unsigned long WRITE_BUFFER_SIZE = 512 * 1024;

        bool bRet = false; // 函数返回值
        char* pWriteBuffer = 0; // 写缓冲区

        // 创建文件, 关闭文件句柄后会自动删除文件, 程序意外关闭后, 也可删除测试文件
        m_testFileHandle = CreateFileW(
            m_testFile.Path.c_str(), 
            GENERIC_WRITE | GENERIC_READ, 
            0, 
            NULL, 
            CREATE_ALWAYS,
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_DELETE_ON_CLOSE, 
            NULL);
        if (0 == m_testFileHandle || INVALID_HANDLE_VALUE == m_testFileHandle)
        {
            m_testState.Error = DST_OPEN_FILE_ERROR;
            m_testState.ErrorMsg = ErrorMsgList[DST_OPEN_FILE_ERROR];
            GetWindowsError(&m_testState.ErrorMsgWindows);
            bRet = false;

            goto SAFE_EXIT;
        }

        // 初始化写入缓冲区
        pWriteBuffer = CreateRandomBuffer(WRITE_BUFFER_SIZE);
        if (0 == pWriteBuffer)
        {
            bRet = false;
            m_testState.Error = DST_ALLOC_MEMORY_ERROR;
            m_testState.ErrorMsg = ErrorMsgList[DST_ALLOC_MEMORY_ERROR];
            goto SAFE_EXIT;
        }

       
        // 生成测试文件
        for (unsigned long i = 0; i < m_testFile.Size/WRITE_BUFFER_SIZE; i++)
        {
            unsigned long count = 0;
            BOOL ret = WriteFile(m_testFileHandle, (LPCVOID)pWriteBuffer, WRITE_BUFFER_SIZE, &count, NULL);
            if (FALSE == ret)
            {
                bRet = false;
                m_testState.Error = DST_WRITE_FILE_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_WRITE_FILE_ERROR];
                GetWindowsError(&m_testState.ErrorMsgWindows);

                goto SAFE_EXIT;
            }

            if (m_bStopTest)
            {
                bRet = false;
                m_testState.Error = DST_TEST_ABORT_ERROR;
                m_testState.ErrorMsg = ErrorMsgList[DST_TEST_ABORT_ERROR];
                goto SAFE_EXIT;
            }

        }

        bRet = true;

SAFE_EXIT:
        if (0 != pWriteBuffer)
        {
            delete[] pWriteBuffer;
            pWriteBuffer = 0;
        }

        return bRet;
    }

    /// @brief 删除测试文件
    void DeleteTestFile()
    {
        if (m_testFileHandle != 0 && m_testFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_testFileHandle);
            m_testFileHandle = 0;
        }
    }

private:
    LDiskSpeedTestState m_testState; ///< 测试状态
    LDiskSpeedTestFile m_testFile; ///< 测试文件
    bool m_bStopTest; ///< 标识是否停止测试
    vector<unsigned long> m_rwAddressList; ///< 随机读写地址列表, 以4K为每个读写单位

    HANDLE m_testFileHandle; ///< 测试文件句柄

private:
     /// @brief 4K随机测试
    /// @param[in] pParam 参数
    static void Disk4KRandomTestThread(void* pParam)
    {
        /*
        测试算法:
        先以512KB的单位尺寸生成指定大小的测试文件,
        然后在其地址范围内进行随机4KB单位尺寸进行写入及读取测试
        */

        CDisk4KRandomTest* pDisk4KRandomTest = (CDisk4KRandomTest*)pParam;
        LDiskSpeedTestState& testState = pDisk4KRandomTest->m_testState;

        
        // 生成测试文件
        bool bRet = pDisk4KRandomTest->GenerateTestFile();
        if (!bRet)
            goto SAFE_EXIT;

        // 进行写测试
        bRet = pDisk4KRandomTest->Disk4KRandomWriteTest();
        if (!bRet)
            goto SAFE_EXIT;

        // 进行读测试
        bRet = pDisk4KRandomTest->Disk4KRandomReadTest();
        if (!bRet)
            goto SAFE_EXIT;

SAFE_EXIT:

        // 设置测试状态
        testState.TestDone = true;

        // 删除测试文件
        pDisk4KRandomTest->DeleteTestFile();


    }
};

LDiskSequenceTest::LDiskSequenceTest()
{
    m_pDiskSequenceTest = 0;
    m_pDiskSequenceTest = new CDiskSequenceTest();
}

LDiskSequenceTest::~LDiskSequenceTest()
{
    if (0 != m_pDiskSequenceTest)
    {
        delete m_pDiskSequenceTest;
        m_pDiskSequenceTest = 0;
    }
}

bool LDiskSequenceTest::Start(IN const wstring& filePath, IN unsigned int fileSize)
{
    return m_pDiskSequenceTest->Start(filePath, fileSize);
}

LDiskSpeedTestState LDiskSequenceTest::GetState()
{
    return m_pDiskSequenceTest->GetState();
}

void LDiskSequenceTest::Stop()
{
    m_pDiskSequenceTest->Stop();
}

LDisk4KRandomTest::LDisk4KRandomTest()
{
    m_pDisk4KRandomTest = 0;
    m_pDisk4KRandomTest = new CDisk4KRandomTest();
}

LDisk4KRandomTest::~LDisk4KRandomTest()
{
    if (m_pDisk4KRandomTest != 0)
    {
        delete m_pDisk4KRandomTest;
        m_pDisk4KRandomTest = 0;
    }
}

bool LDisk4KRandomTest::Start(IN const wstring& filePath, IN unsigned int fileSize)
{
    return m_pDisk4KRandomTest->Start(filePath, fileSize);
}

LDiskSpeedTestState LDisk4KRandomTest::GetState()
{
    return m_pDisk4KRandomTest->GetState();
}

void LDisk4KRandomTest::Stop()
{
    m_pDisk4KRandomTest->Stop();
}