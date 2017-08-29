

#include "LCpuTemp.h"

#include <Windows.h>

#include "LWinRing0.h"



/// @brief CPU温度实现接口
class CCpuTemp
{
public:
    static bool gInitWinRing0Success; ///< 标志是否成功初始化WinRing0

public:
    /// @brief 虚析构函数
    virtual ~CCpuTemp() = 0{}

    /// @brief 获取CPU温度
    /// @param[out] coreNum 存储CPU物理核心数
    /// @param[out] temp 存储温度
    /// @return 成功返回true, 失败返回false
    virtual bool Get(OUT unsigned int& coreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM]) = 0;

protected:
    /// @brief 获取处理器核心数
    /// @param[out] physicalCoreNumber 物理核心数
    /// @param[out] logicalProcessorNumber 逻辑处理器数量
    /// @return 成功返回true, 失败返回false
    bool GetProcessorCoreNumber(unsigned long& physicalCoreNumber, unsigned long& logicalProcessorNumber)
    {
        bool bRet = false;
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION processorInforList = NULL;
        DWORD dwRequireSize = 0;
        SYSTEM_INFO systemInfo = {0};
        DWORD listCount = 0;

        BOOL dwRet = GetLogicalProcessorInformation(processorInforList, &dwRequireSize);
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        processorInforList = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)new char[dwRequireSize];

        dwRet = GetLogicalProcessorInformation(processorInforList, &dwRequireSize);
        if (FALSE == dwRet)
        {
            bRet = false;
            goto SAFE_EXIT;
        }

        listCount = dwRequireSize/sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        physicalCoreNumber = 0;
        for (unsigned long i = 0; i < listCount; i++)
        {
            if (processorInforList[i].Relationship == RelationProcessorCore)
            {
                physicalCoreNumber += 1;
            }
        }

        // 获取逻辑处理器数量

        GetNativeSystemInfo(&systemInfo);
        logicalProcessorNumber = systemInfo.dwNumberOfProcessors;

        bRet = true;

SAFE_EXIT:
        if (processorInforList != NULL)
        {
            delete processorInforList;
        }

        return bRet;
    }
};

bool CCpuTemp::gInitWinRing0Success = false;

/// @brief IntelCPU温度类
class CIntelCpuTemp : public CCpuTemp
{
public:
    /// @brief 构造函数
    CIntelCpuTemp()
    {

    }

    /// @brief 析构函数
    ~CIntelCpuTemp()
    {

    }

    /// @brief 获取CPU温度
    /// @param[out] coreNum 存储CPU物理核心数
    /// @param[out] temp 存储温度
    /// @return 成功返回true, 失败返回false
    virtual bool Get(OUT unsigned int& coreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM])
    {
        if (!gInitWinRing0Success)
            return false;

        ZeroMemory(temp, sizeof(unsigned int) * MAX_PROCESSOR_PHYSICAL_CORE_NUM);

        /*
        DTS( Digital Thermal Senser)方式获取CPU温度，通过读取MSR来实现
        现在Intel处理器CPU中每个都集合了DTS，用来实时监测CPU的温度， 
        当温度达到TCC( Thermal Control Circuit)激活温度时， 将会通过降低电压、主频、风扇调节等形式调节温度， 我们称这个温度为Tjunction
        而我们从MSR（Model Specific Register）读到的温度是距离Tjunction的温差，而不是实际的温度，称之为Delta
        所以实际的温度为：Tjunction- Delta
        并且不同CPU的Tjunction还不一样，可以为85℃、100℃、105℃等, 需要查阅Intel用户手册
        以下为Intel 酷睿CPU温度的读取过程:
        */

        // 使用CPUID指令0获取CPU支持的最大命令数
        // 如果最大命令数小于6, 那么CPU不支持DTS
        DWORD eax = 0;
        DWORD ebx = 0;
        DWORD ecx = 0;
        DWORD edx = 0;

        LWinRing0::Cpuid(0, &eax, &ebx, &ecx, &edx);

        DWORD maxCmdNum = eax;
        if (maxCmdNum < 6)
            return false;

        // 使用CPUID指令6, 查看CPU是否支持DTS
        // eax第一位为1则表示支持DTS, 反之不支持DTS
        eax = 0;
        ebx = 0;
        ecx = 0;
        edx = 0;

        LWinRing0::Cpuid(6, &eax, &ebx, &ecx, &edx);

        if (0 == (eax & 1))
            return false;

        // 使用0xee执行rdmsr指令, 如果exa的第30位为1, 则表示Tjunction为85, 否则为100
        eax = 0;
        edx = 0;

        LWinRing0::Rdmsr(0xee, &eax, &edx);
        DWORD tjunction = 0;
        if (1 == (eax&0x20000000))
            tjunction = 85;
        else
            tjunction = 100;

        unsigned long processorCoreNum = 0;
        unsigned long logicalProcessorNum = 0;
        if (!this->GetProcessorCoreNumber(processorCoreNum, logicalProcessorNum))
            return false;

        DWORD step =  logicalProcessorNum/processorCoreNum;

        coreNum = processorCoreNum;

        // 使用0x19c执行rdmsr指令, eax的16:23位表示当前DTS值
        // 分别获取每个逻辑处理器的温度
        for (DWORD processorIndex = 0; processorIndex < logicalProcessorNum; processorIndex += step)
        {
            DWORD threadMask = 1;
            threadMask = threadMask << processorIndex;
            DWORD oldMask = SetThreadAffinityMask(GetCurrentThread(), threadMask);
            if (0 == oldMask)
                return false;

            eax = 0;
            edx = 0;
            LWinRing0::Rdmsr(0x19c, &eax, &edx);
            DWORD delta = (eax&0x007f0000) >> 16;
            temp[processorIndex/step] = tjunction - delta;

            SetThreadAffinityMask(GetCurrentThread(), oldMask);
        }

        return true;
    }
};


/// @brief AMDCPU温度类
class CAMDCpuTemp : public CCpuTemp
{
public:
    /// @brief 构造函数
    CAMDCpuTemp()
    {

    }

    /// @brief 析构函数
    ~CAMDCpuTemp()
    {

    }

    /// @brief 获取CPU温度
    /// @param[out] coreNum 存储CPU物理核心数
    /// @param[out] temp 存储温度
    /// @return 成功返回true, 失败返回false
    virtual bool Get(OUT unsigned int& coreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM])
    {
        if (!gInitWinRing0Success)
            return false;

        unsigned long physicalCoreNum = 0;
        unsigned long logicalCoreNum = 0;
        this->GetProcessorCoreNumber(physicalCoreNum, logicalCoreNum);

        coreNum = physicalCoreNum;

        unsigned int coreTemp = 0;

        const WORD PCI_CONFIG_ADDRESS = 0XCF8;
        const WORD PCI_CONFIG_DATA = 0XCFC;

        // 扫描所有PCI设备找到设备: VID 0x1022 DID 0x141D
        for(unsigned int bus = 0; bus <= 255; bus++)
        {
            for(unsigned int dev = 0; dev < 32; dev++)
            {
                for(unsigned int func = 0; func < 8; func++)
                {
                    DWORD dwAddr=0X80000000+(bus<<16)+(dev<<11)+(func<<8);

                    // 读取设备ID和厂商ID
                    LWinRing0::WriteIoPortDword(PCI_CONFIG_ADDRESS, dwAddr);
                    DWORD dwData = LWinRing0::ReadIoPortDword(PCI_CONFIG_DATA);

                    // 值为0XFFFFFFFF表示不存在该PCI设备
                    if(dwData==0XFFFFFFFF)
                    {
                        continue;
                    }

                    DWORD vid=dwData&0XFFFF;
                    DWORD did=(dwData>>16)&0XFFFF;

                    if (vid == 0X1022 &&
                        did == 0X141D)
                    {
                        // 寄存器的值高8位为温度值
                        LWinRing0::WriteIoPortDword(PCI_CONFIG_ADDRESS, dwAddr|0XA4);
                        DWORD dwTemp = LWinRing0::ReadIoPortDword(PCI_CONFIG_DATA);
                        dwTemp = dwTemp >> 24;
                        dwTemp = dwTemp & 0XFF;
                        coreTemp = (int)dwTemp;

                        break;

                    }
                }

                if (coreTemp != 0)
                    break;
            }

            if (coreTemp != 0)
                break;
        }

        for (unsigned int i = 0; i < coreNum; i++)
        {
            temp[i] = coreTemp;
        }

        if (coreTemp != 0)
            return true;

        return false;
    }
};

LCpuTemp::LCpuTemp(IN const wstring& winRing0Path)
{
    m_pCpuTemp = NULL;

    if (!CCpuTemp::gInitWinRing0Success)
    {
        CCpuTemp::gInitWinRing0Success = LWinRing0::InitializeWinRing0(winRing0Path);
        if (!CCpuTemp::gInitWinRing0Success)
            return;
    }

    // 获取CPU厂商信息
    char vendorBuffer[20] = {0};
    int iRet = LWinRing0::Cpuid(
        0, 
        (unsigned long*)&vendorBuffer[0], 
        (unsigned long *)&vendorBuffer[4], 
        (unsigned long*)&vendorBuffer[8], 
        (unsigned long*)&vendorBuffer[12]);

    if (0 == iRet)
        return;

    //AMD:      "AuthenticAMD"
    //Intel:    "GenuineIntel"
    //注意这里：edx与 ecx的顺序和字符串中二者的顺序是反的, 所以调换后两个寄存器
    unsigned long ecx = *((unsigned long *)&vendorBuffer[8]);
    unsigned long edx = *((unsigned long *)&vendorBuffer[12]);
    *((unsigned long *)&vendorBuffer[12]) = ecx;
    *((unsigned long *)&vendorBuffer[8]) = edx;

    if (0 == strcmp((char*)(&vendorBuffer[4]), "GenuineIntel"))
    {
        m_pCpuTemp = new CIntelCpuTemp();
    }
    else if (0 == strcmp((char*)(&vendorBuffer[4]), "AuthenticAMD"))
    {
        m_pCpuTemp = new CAMDCpuTemp();
    }
}

LCpuTemp::~LCpuTemp()
{
    if (m_pCpuTemp != 0)
    {
        delete m_pCpuTemp;
        m_pCpuTemp = 0;
    }
}

bool LCpuTemp::Get(OUT unsigned int& coreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM])
{
    if (NULL == m_pCpuTemp)
        return false;


    return m_pCpuTemp->Get(coreNum, temp);
}

