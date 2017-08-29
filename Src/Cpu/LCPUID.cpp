
#include "LCPUID.h"

#include <cstring>

#include <intrin.h>

#define CPUID_MIN_COMMAND 0x0
#define CPUID_MIN_EXTENDED_COMMAND 0x80000000

LCPUID::LCPUID()
{
    m_maxCommand = CPUID_MIN_COMMAND;
    m_maxExtendedCommand = CPUID_MIN_EXTENDED_COMMAND;

    m_maxCommand = this->GetMaxCommand();
    m_maxExtendedCommand = this->GetMaxExtendedCommand();
}

LCPUID::~LCPUID()
{

}

bool LCPUID::GetVendor(OUT string& vendor)
{
    const unsigned int  CMD_GET_VENDOR_ID = 0; // 获取厂商信息
    char strVendor[16] = {0};
    unsigned int buffer[4] = {0};

    vendor.clear();
    bool bRet = this->ExecuteCPUID(CMD_GET_VENDOR_ID, buffer);
   
    if (bRet)
    {
        memcpy(strVendor, &buffer[1], 4);
        memcpy(strVendor + 4, &buffer[3], 4);
        memcpy(strVendor + 8, &buffer[2], 4);
        vendor = strVendor;
    }

    return bRet;
}

bool LCPUID::GetBrand(OUT string& brand)
{
    const int CMD_GET_BRAND = 0x80000002;
    char strBrand[50] = {0};

    brand.clear();
    for (unsigned int i = 0; i < 3; i++) // 依次执行3个指令
    {
        unsigned int buffer[4] = {0};
        bool bRet = this->ExecuteCPUID(CMD_GET_BRAND + i, buffer);
        if (!bRet)
            return false;

        memcpy(strBrand + i * 16, &buffer, 16); // 每次执行结束后，保存四个寄存器里的asc码到数组
    } 

    brand = strBrand;
    return true;
}

unsigned int LCPUID::GetMaxCommand()
{
    unsigned int buffer[4] = {0};
    this->ExecuteCPUID(CPUID_MIN_COMMAND, buffer);

    unsigned int maxCommand = buffer[0];
    return maxCommand;
}

unsigned int LCPUID::GetMaxExtendedCommand()
{
    unsigned int buffer[4] = {0};
    this->ExecuteCPUID(CPUID_MIN_EXTENDED_COMMAND, buffer);

    unsigned int maxCommand = buffer[0];
    return maxCommand;
}

bool LCPUID::ExecuteCPUID(IN unsigned int cmd, OUT unsigned int cpuInfo[4])
{
    // 检查命令的范围值
    bool bRet = false;
    if ((cmd >= CPUID_MIN_COMMAND && cmd <= m_maxCommand) ||
        (cmd >= CPUID_MIN_EXTENDED_COMMAND && cmd <= m_maxExtendedCommand))
        bRet = true;

    if (!bRet)
        return bRet;

    __cpuid((int*)cpuInfo, cmd);

    return bRet;
}