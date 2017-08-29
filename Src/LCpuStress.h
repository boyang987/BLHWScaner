/// @file LCpuStress.h
/// @brief CPU压力测试头文件 
/// 
/// Detail:
/// @author Burnell Liu Email:burnell_liu@outlook.com
/// @version   
/// @date 2016:3:25

#ifndef _LCPUSTRESS_H_
#define _LCPUSTRESS_H_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define MAX_CPU_LOGICAL_CORE_NUM 64

/// @brief CPU压力测试状态
struct LCpuStressTestState 
{
    bool TestDone; ///< 标记测试是否已经结束
    unsigned int LogicalCoreNum; ///< CPU逻辑核心数量
    unsigned long Score[MAX_CPU_LOGICAL_CORE_NUM]; ///< 每个CPU逻辑核心的得分(一定时间内计算圆周率PI的位数)
};

class CCpuStressTest;

/// @brief CPU压力测试
class LCpuStressTest
{
public:
    /// @brief 构造函数
    LCpuStressTest();

    /// @brief 析构函数
    ~LCpuStressTest();

    /// @brief 开始测试
    /// @param[in] timeOut 超时时间, 单位秒
    /// @return 成功返回true, 失败返回false, 测试正在进行中会返回false
    bool Start(IN unsigned int timeOut);

    /// @brief 获取测试状态
    /// @return 测试状态
    const LCpuStressTestState& GetState();

    /// @brief 停止测试
    void Stop();

private:
    CCpuStressTest* m_pCpuStressTest; ///< CPU压力测试实现对象

private:
    // 禁止拷贝构造函数和赋值操作符
    LCpuStressTest(const LCpuStressTest&);
    LCpuStressTest& operator = (const LCpuStressTest&);
};

#endif