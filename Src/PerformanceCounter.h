

#ifndef _PERFORMANCECOUNTER_H_
#define _PERFORMANCECOUNTER_H_

#include <string>
using std::wstring;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif


/// @brief 内存性能
struct MemoryPerformance
{
    unsigned long TotalSize; ///< 总内存大小, 单位M
    unsigned long AvailableSize; ///< 可用内存大小, 单位M
    unsigned long LoadPercentage; ///< 内存使用率, 范围(1-100)
};

/// @brief 处理器性能
struct ProcessorPerformance
{
    unsigned long LoadPercentage; ///< 当前CPU使用率, 范围(1-100)
    unsigned long SpeedPercentage; ///< 当前CPU频率百分比(针对最大频率)
};

/// @brief 最大获取磁盘性能数量
#ifndef MAX_DISKPERFORMANCE_NUMBER
#define MAX_DISKPERFORMANCE_NUMBER 8
#endif

/// @brief Disk性能
struct DiskPerformance
{
    unsigned int Count; ///< 获取到性能的磁盘数量
    unsigned int UsageRate[MAX_DISKPERFORMANCE_NUMBER]; ///< 磁盘使用率
    wstring DiskDriveID[MAX_DISKPERFORMANCE_NUMBER]; ///< 磁盘驱动器ID, 如 L"\\\\.\\PhysicalDrive0"
};

class CPerformanceCounter;

/// @brief 性能计数器类
class PerformanceCounter
{
public:
    /// @brief 构造函数
    PerformanceCounter();

    /// @brief 析构函数
    ~PerformanceCounter();

    /// @brief 获取内存性能
    /// @param[out] memoryPerformance 存储内存性能
    /// @return 成功返回true, 失败返回false
    bool GetMemoryPerformance(OUT MemoryPerformance& memoryPerformance);

    /// @brief 获取处理器性能
    /// 使用该方法会耗费当前线程200毫秒时间
    /// @param[out] processorPerformance 存储处理器性能
    /// @return 成功返回true, 失败返回false
    bool GetProcessorPerformance(OUT ProcessorPerformance& processorPerformance);

    /// @brief 获取磁盘性能
    /// 该方法只能获取固定磁盘的性能, 使用该方法会耗费当前线程200毫秒时间
    /// @param[in] diskPerformance 存储磁盘性能
    /// @return 成功返回true, 失败返回false
    bool GetDiskPerformance(OUT DiskPerformance& diskPerformance);

private:
    CPerformanceCounter* m_pCPerformanceCounter; ///< 性能计数器实现对象
};


#endif