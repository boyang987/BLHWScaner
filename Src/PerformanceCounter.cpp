
#include "PerformanceCounter.h"

#include <vector>
using std::vector;

#include "Wmi\\LWMISystemClasses.h"
#include "Wmi\\LWMIHardwareClasses.h"

#include "Memory\\LMemory.h"

#include "Pdh\\LPdh.h"

/// @brief PDH磁盘ID
/// 在使用PDH获取磁盘信息时, 该结构可以确定一个磁盘
struct PdhDiskId 
{
    wchar_t IdIndex; ///< ID索引, 如0, 1, 2...
    wstring DeviceId; ///< 设备ID, 如"\\\\.\\PhysicalDrive0"
    wstring LogicalName; ///< 逻辑磁盘分区名称, 多个分区名称之间用空格隔开, 如"C: D:"
};

/// @brief 性能计数器实现类
class CPerformanceCounter
{
public:
    /// @brief 构造函数
    CPerformanceCounter()
    {
        this->ScanFixedDiskId(m_fixedDiskIdList);
        m_pCPUSpeedPdh = NULL;
        m_pCPUSpeedPdh = new LPdh(L"\\Processor Information(_Total)\\% Processor Performance");
    }

    /// @brief 析构函数
    ~CPerformanceCounter()
    {
        if (NULL != m_pCPUSpeedPdh)
        {
            delete m_pCPUSpeedPdh;
            m_pCPUSpeedPdh = NULL;
        }
    }

    /// @brief 获取内存性能
    /// @param[out] memoryPerformance 存储内存性能
    /// @return 成功返回true, 失败返回false
    bool GetMemoryPerformance(OUT MemoryPerformance& memoryPerformance)
    {
        LMemory memory;
        int loadPercent = memory.GetLoadPercent();
        if (loadPercent == -1)
            return false;

        memoryPerformance.LoadPercentage = (unsigned long)loadPercent;

        int totalSize = memory.GetTotalSize();
        if (totalSize == -1)
            return false;

        memoryPerformance.TotalSize = (unsigned long)totalSize;

        int availSize = memory.GetAvailableSize();
        if (availSize == -1)
            return false;

        memoryPerformance.AvailableSize = (unsigned long)availSize;

        return true;
    }

    /// @brief 获取处理器性能
    /// @param[out] processorPerformance 存储处理器性能
    /// @return 成功返回true, 失败返回false
    bool GetProcessorPerformance(OUT ProcessorPerformance& processorPerformance)
    {
        long speedData = 0;
        m_pCPUSpeedPdh->CollectDataLong(200, speedData);
        processorPerformance.SpeedPercentage = (unsigned long)speedData;

        LWMI::LProcessorManager processorManager;
        if (!processorManager.GetProcessorLoadPercentage(0, processorPerformance.LoadPercentage))
            return false;


        return true;
    }

    /// @brief 获取磁盘性能
    /// 该方法只能获取固定磁盘的性能
    /// @param[in] diskPerformance 存储磁盘性能
    /// @return 成功返回true, 失败返回false
    bool GetDiskPerformance(OUT DiskPerformance& diskPerformance)
    {
        diskPerformance.Count = m_fixedDiskIdList.size();
        for (unsigned int i = 0; i < m_fixedDiskIdList.size(); i++)
        {
            diskPerformance.DiskDriveID[i] = m_fixedDiskIdList[i].DeviceId;

            // Path Sample: "\\PhysicalDisk(0 C: D:)\\% Disk Time"
            wstring counterPath = L"\\PhysicalDisk(";
            counterPath += m_fixedDiskIdList[i].IdIndex;
            counterPath += L' ';
            counterPath += m_fixedDiskIdList[i].LogicalName;
            counterPath += L")\\% Disk Time";

            LPdh pdh(counterPath);

            long value = 0;
            pdh.CollectDataLong(200, value);
            diskPerformance.UsageRate[i] = (unsigned int)value;
        }

        return true;
    }

private:
    /// @brief 扫描固定磁盘ID
    /// @param[out] idList 存储ID
    void ScanFixedDiskId(OUT vector<PdhDiskId>& idList)
    {
        idList.clear();

        LWMI::LDiskDriveManager diskDriveManager;

        for (int i = 0; i < diskDriveManager.GetDiskCount(); i++)
        {
            LWMI::LDiskDriveManager::LDISK_TYPE diskType;
            diskDriveManager.GetDiskType(i, diskType);
            if (diskType != LWMI::LDiskDriveManager::FIXED_DISK)
            {
                continue;
            }

            
            PdhDiskId pdhDiskId;
            wstring deviceId; 
            diskDriveManager.GetDiskDeviceID(i, deviceId);
            pdhDiskId.DeviceId = deviceId;
            pdhDiskId.IdIndex = deviceId[deviceId.length()-1];

            wstring logicalName;
            diskDriveManager.GetDiskLogicalName(i, logicalName);
            for (unsigned int j = 0; j < logicalName.length(); j++)
            {
                if (L';' == logicalName[j])
                    logicalName[j] = L' ';
            }
            pdhDiskId.LogicalName = logicalName;

            idList.push_back(pdhDiskId);
        }
    }

private:
    vector<PdhDiskId> m_fixedDiskIdList; ///< 固定磁盘ID列表
    LPdh* m_pCPUSpeedPdh; ///< CPU当前频率数据收集器
};

PerformanceCounter::PerformanceCounter()
{
    m_pCPerformanceCounter = 0;
    m_pCPerformanceCounter = new CPerformanceCounter();
}

PerformanceCounter::~PerformanceCounter()
{
    if (m_pCPerformanceCounter != 0)
    {
        delete m_pCPerformanceCounter;
        m_pCPerformanceCounter = 0;
    }
}

bool PerformanceCounter::GetMemoryPerformance(OUT MemoryPerformance& memoryPerformance)
{
    return m_pCPerformanceCounter->GetMemoryPerformance(memoryPerformance);
}

bool PerformanceCounter::GetProcessorPerformance(OUT ProcessorPerformance& processorPerformance)
{
    return m_pCPerformanceCounter->GetProcessorPerformance(processorPerformance);
}

bool PerformanceCounter::GetDiskPerformance(OUT DiskPerformance& diskPerformance)
{
    return m_pCPerformanceCounter->GetDiskPerformance(diskPerformance);
}