

#include "LCpuStress.h"

#include <process.h>

#include <Windows.h>

/// @高精度计时器类
class LTimer
{
public:
    LTimer()
    {
        // 获取CPU的时钟频率(即每秒的滴答数)
        QueryPerformanceFrequency((LARGE_INTEGER*)&m_performanceFreq);
    }

    /// @brief 开始计时
    void Start()
    {
        m_startTime = 0;
        m_time = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
    }

    /// @brief 结束计时
    void End()
    {
        LONGLONG currentTime = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
        m_time = currentTime - m_startTime;
    }

    /// @brief 获取计时时间
    /// @return 计时时间(单位毫秒)
    double Time()
    {
        double time = 0.0f;
        time = (double)(m_time * 1000) / (double)m_performanceFreq;
        return time;
    }

private:
    LONGLONG m_time; ///< 计时时间 
    LONGLONG m_startTime; ///< 计时器开始时间 
    LONGLONG m_performanceFreq; ///< CPU时钟频率(即每秒钟的滴答数)
};

/// @brief 线程参数
struct SThreadParam
{
    unsigned int Id; ///< 标记线程ID
    CCpuStressTest* PStressTest; ///< CPU压力测试对象指针
};

/// @brief CPU压力测试实现类
class CCpuStressTest
{
public:
    /// @brief 构造函数
    CCpuStressTest()
    {
        m_testState.TestDone = true;
        m_testState.LogicalCoreNum = 0;
        for (unsigned int i = 0; i < MAX_CPU_LOGICAL_CORE_NUM; i++)
        {
            m_testState.Score[i] = 0;
            m_threadTestDone[i] = false;
        }

        m_threadNum = 0;
        m_bStopTest = false;
        m_timeOut = 0;
    }

    /// @brief 析构函数
    ~CCpuStressTest()
    {

    }

    /// @brief 开始测试
    /// @param[in] timeOut 超时时间, 单位秒
    /// @return 成功返回true, 失败返回false, 测试正在进行中会返回false
    bool Start(IN unsigned int timeOut)
    {
        if (!m_testState.TestDone)
            return false;

        m_testState.TestDone = false;
        for (unsigned int i = 0; i < MAX_CPU_LOGICAL_CORE_NUM; i++)
        {
            m_testState.Score[i] = 0;
            m_threadTestDone[i] = false;
        }

        m_bStopTest = false;

        m_timeOut = timeOut;

        // 获取逻辑核心数量
        SYSTEM_INFO systemInfo = {0};
        GetNativeSystemInfo(&systemInfo);
        m_testState.LogicalCoreNum = systemInfo.dwNumberOfProcessors;
        m_threadNum = systemInfo.dwNumberOfProcessors;

        for (unsigned int i = 0; i < m_threadNum; i++)
        {
            m_threadParam[i].Id = i;
            m_threadParam[i].PStressTest = this;
            _beginthread(CpuStressTestThread, 0, (void*)&(m_threadParam[i]));
        }

         _beginthread(CpuStressMonitorThread, 0, (void*)this);

        return true;
    }

    /// @brief 获取测试状态
    /// @return 测试状态
    const LCpuStressTestState& GetState()
    {
        return m_testState;
    }

    /// @brief 停止测试
    void Stop()
    {
        m_bStopTest = true;
    }

private:
    unsigned int m_timeOut; ///< 超时时间
    bool m_bStopTest; ///< 标记是否结束测试
    unsigned int m_threadNum; ///< 线程数量
    bool m_threadTestDone[MAX_CPU_LOGICAL_CORE_NUM]; ///< 标记每个测试线程是否结束
    LCpuStressTestState m_testState; ///< 测试状态

    SThreadParam m_threadParam[MAX_CPU_LOGICAL_CORE_NUM]; ///< 记录线程ID

private:
        /// @brief CPU压力监视线程
    static void CpuStressMonitorThread(void* pParam)
    {
        CCpuStressTest* pCpuStressTest = (CCpuStressTest*)pParam;

        while (true)
        {
            bool threadRun = false;

            for (unsigned int i = 0; i < pCpuStressTest->m_threadNum; i++)
            {
                if (!pCpuStressTest->m_threadTestDone[i])
                    threadRun = true;
            }

            if (!threadRun)
                break;

            Sleep(200);
        }


        pCpuStressTest->m_testState.TestDone = true;
        
    }

    /// @brief CPU压力测试线程
    /// @param[in] pParam 参数
    static void CpuStressTestThread(void* pParam)
    {
        SThreadParam* pThreadParam = (SThreadParam*)pParam;

        // 将测试线程相关到指定CPU核心上运行
        DWORD threadMask = 1;
        threadMask = threadMask << pThreadParam->Id;
        SetThreadAffinityMask(GetCurrentThread(), threadMask);

        CCpuStressTest* pCpuStressTest = pThreadParam->PStressTest;
        CalculatePI(
            pCpuStressTest->m_timeOut, 
            pCpuStressTest->m_bStopTest,
            pCpuStressTest->m_testState.Score[pThreadParam->Id]);

        pCpuStressTest->m_threadTestDone[pThreadParam->Id] = true;
        
    }

    /// @brief 计算PI
    /// @param[in] timeOut 超时时间, 单位秒
    /// @param[in] bStopTest 标记是否停止测试
    /// @param[out] floatLen 存储计算出的精度值
    /// @return 计算出PI的精度值
    static void CalculatePI(IN unsigned int timeOut, IN bool& bStopTest, OUT unsigned long& floatLen)
    {
        /*
        PI/2 = 1 + 1/3 + 1/3 * 2/5 + 1/3 * 2/5 * 3/7...
        */

        const int MAX_FLOAT_LEN = 80000; //定义最大计算精度

        unsigned long pi[MAX_FLOAT_LEN] = {0}; // 存储计算出的PI值, 数组中的每个值代表十进制中的一位
        unsigned long z[MAX_FLOAT_LEN] = {0}; // 每次迭代pi值的新部分, 如2/3, 2/3 * 2/5, ...

        pi[1] = 2; // 设置PI的开始值为2
        z[1] = 2; // 设置新部分的开始值为2

        unsigned long a = 1; // 每次迭代值为 1, 2, 3, 4, ...
        unsigned long b = 3; // 每次迭代值为 3, 5, 7, 9, ...

        floatLen = 0;

        LTimer timer;
        timer.Start();

        while (true)
        {
            unsigned long d = 0;
            unsigned long c = 0; 

            // z *= a;
            d = 0; // 乘法中的进位值
            for (int i = MAX_FLOAT_LEN - 1; i > 0; i--)
            {
                c = z[i] * a + d;
                z[i] = c % 10;
                d = c / 10;
            }

            // z /= b;
            d = 0; // 除法中的余数值
            for (int i = 0; i < MAX_FLOAT_LEN; i++)
            {
                c = z[i] + d * 10;
                z[i] = c / b;
                d = c % b;
            }

            // pi += z;
            unsigned long maxFloatFlag = 0; // 如果z[i]全部为0, 则maxFloatFlag为0, 表示计算达到最大精度
            for (int i = MAX_FLOAT_LEN - 1; i > 0; i--)
            {
                c = pi[i] + z[i];
                pi[i] = c % 10;
                pi[i - 1] += c / 10;
                maxFloatFlag |= z[i];
            }

            a++;
            b += 2;

            // 达到最大精度, 直接返回
            if (maxFloatFlag == 0)
            {
                floatLen = MAX_FLOAT_LEN;
                break;
            }

            // 计算精度
            for (int i = 0; i < MAX_FLOAT_LEN; i++)
            {
                if (z[i] != 0)
                {
                    floatLen = (unsigned long)i;
                    break;
                }

            }

            // 超时或者停止测试, 则退出
            timer.End();
            if (timer.Time() >= timeOut * 1000 || bStopTest)
            {
                break;
            }
        }

    }
};

LCpuStressTest::LCpuStressTest()
{
    m_pCpuStressTest = new CCpuStressTest();
}

LCpuStressTest::~LCpuStressTest()
{
    if (m_pCpuStressTest != 0)
    {
        delete m_pCpuStressTest;
        m_pCpuStressTest = 0;
    }
}

bool LCpuStressTest::Start(IN unsigned int timeOut)
{
    return m_pCpuStressTest->Start(timeOut);
}

const LCpuStressTestState& LCpuStressTest::GetState()
{
    return m_pCpuStressTest->GetState();
}

void LCpuStressTest::Stop()
{
    return m_pCpuStressTest->Stop();
}