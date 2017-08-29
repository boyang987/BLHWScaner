

#include "TemperatureProbe.h"

#include <vector>
using std::vector;

#include <Windows.h>

#include "DiskController\\LDiskController.h"
#include "SMARTPaser\\LSMARTPaser.h"

typedef bool (*GetCpuTempFun)(OUT unsigned int* pCoreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM]);

typedef bool (*GetGpuTempFun)(OUT unsigned int* pSensorNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER]);

/// @brief 温度探测实现类
class CTemperatureProbe
{
public:
    /// @brief 构造函数
    CTemperatureProbe()
    {
        m_pGetCpuTemp = NULL;
        m_pGetGpuTemp = NULL;
        m_hCoreTemp = NULL;

        m_hCoreTemp = LoadLibraryW(L".\\CoreTemp\\CoreTemp.dll");

        if (m_hCoreTemp == NULL)
            return;

        m_pGetCpuTemp = (GetCpuTempFun)GetProcAddress(m_hCoreTemp, "GetCpuTemp");

        m_pGetGpuTemp = (GetGpuTempFun)GetProcAddress(m_hCoreTemp, "GetGpuTemp");

        this->ScanSataDiskId(m_sataDiskIdList);

    }

    /// @brief 析构函数
    ~CTemperatureProbe()
    {
        if (m_hCoreTemp != NULL)
        {
            FreeLibrary(m_hCoreTemp);
            m_hCoreTemp = NULL;
        }
    }

    /// @brief 获取CPU温度
    /// @param[out] OUT cpuTemp 存储CPU温度信息
    /// @return 成功返回true, 失败返回false
    bool GetCpuTemp(OUT CpuTempInfor& cpuTemp)
    {
        cpuTemp.CoreNum = 0;

        if (m_pGetCpuTemp == NULL)
            return false;

        return m_pGetCpuTemp(&cpuTemp.CoreNum, cpuTemp.CoreTemp);
    }

    /// @brief 获取GPU温度
    /// @param[out] OUT gpuTemp 存储GPU温度信息
    /// @return 成功返回true, 失败返回false
    bool GetGpuTemp(OUT GpuTempInfor& gpuTemp)
    {
        gpuTemp.SensorsNum = 0;

        if (m_pGetGpuTemp == NULL)
            return false;

        return m_pGetGpuTemp(&gpuTemp.SensorsNum, gpuTemp.Temp);
    }

    /// @brief 获取磁盘温度
    /// 该方法只能获取SATA(IDE)磁盘的温度
    /// @param[out] diskTemp 存储磁盘温度
    void GetDiskTemp(OUT DiskTempInforArray& diskTemp)
    {
        diskTemp.Count = 0;

        for (unsigned int i = 0; i < m_sataDiskIdList.size(); i++)
        {
            const wstring& diskDriveID = m_sataDiskIdList[i];

            // 打开磁盘控制器
            LIDEDiskController diskController(diskDriveID);
            if (!diskController.DeviceExist())
                continue;

            // 获取SMART数据
            unsigned char smartData[362] = {0};
            if (!diskController.GetSMARTData(smartData))
                continue;

            // 解析SMART数据
            LSMARTParser smartParser(smartData);
            unsigned int temp = 0;
            if (!smartParser.GetTemperature(temp))
                continue;

            diskTemp.Temp[diskTemp.Count] = temp;
            diskTemp.DiskDriveID[diskTemp.Count] = diskDriveID;

            diskTemp.Count += 1;
            if(diskTemp.Count >= MAX_DISKTEMP_NUMBER)
                break;

        }
    }

private:
    /// @brief 扫描SATA磁盘ID
    /// 只有SATA才能获取SMART数据, 才能获取到温度
    /// @param[out] idList 存储磁盘ID
    void ScanSataDiskId(OUT vector<wstring>& idList)
    {
        idList.clear();

        for (int i = 0; i < 25; i++)
        {
            wchar_t diskDriveId[256] = {0};
            wsprintfW(diskDriveId, L"\\\\.\\PhysicalDrive%d", i);

            // 打开磁盘控制器
            LIDEDiskController diskController(diskDriveId);
            if (!diskController.DeviceExist())
                continue;

            // 获取SMART数据
            unsigned char smartData[362] = {0};
            if (!diskController.GetSMARTData(smartData))
                continue;

            idList.push_back(diskDriveId);
        }
    }

private:
    GetCpuTempFun m_pGetCpuTemp; ///< 获取CPU温度函数指针
    GetGpuTempFun m_pGetGpuTemp; ///< 获取GPU温度函数指针
    HMODULE m_hCoreTemp; ///< CoreTemp DLL句柄
    vector<wstring> m_sataDiskIdList; ///< 存储SATA磁盘ID
};

TemperatureProbe::TemperatureProbe()
{
    m_pTemperatureProbe = new CTemperatureProbe();
}

TemperatureProbe::~TemperatureProbe()
{
    if (m_pTemperatureProbe != 0)
    {
        delete m_pTemperatureProbe;
        m_pTemperatureProbe = 0;
    }
}

bool TemperatureProbe::GetCpuTemp(OUT CpuTempInfor& cpuTemp)
{
    return m_pTemperatureProbe->GetCpuTemp(cpuTemp);
}

bool TemperatureProbe::GetGpuTemp(OUT GpuTempInfor& gpuTemp)
{
    return m_pTemperatureProbe->GetGpuTemp(gpuTemp);
}

void TemperatureProbe::GetDiskTemp(OUT DiskTempInforArray& diskTemp)
{
    m_pTemperatureProbe->GetDiskTemp(diskTemp);
}