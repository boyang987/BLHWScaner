

#ifndef _LCPUID_H_
#define _LCPUID_H_

#include <string>
using std::string;

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief CPUID指令类
class LCPUID
{
public:
    LCPUID();
    ~LCPUID();

    /// @brief 获取CPU厂商信息
    /// @param[out] vendor
    /// @return 成功返回true, 失败返回false(CPU不支持)
    bool GetVendor(OUT string& vendor);

    /// @brief 获取CPU商标信息
    /// @param[out] brand
    /// @return 成功返回true, 失败返回false(CPU不支持)
    bool GetBrand(OUT string& brand);
    
private:
    /// @brief 获取CPU支持的最大命令
    /// @return CPU支持的最大命令
    unsigned int GetMaxCommand();
    
    /// @brief 获取CPU支持的最大扩展命令
    /// @return CPU支持的最大扩展命令
    unsigned int GetMaxExtendedCommand();

    /// @brief 执行CPUID命令
    /// @param[in] cmd 命令值
    /// @param[out] cpuInfo 输出的内容
    /// @return 成功返回true, 失败返回false(cpu不支持该命令)
    bool ExecuteCPUID(IN unsigned int cmd, OUT unsigned int cpuInfo[4]);

private:
    unsigned int m_maxCommand; ///< CPU支持的最大命令
    unsigned int m_maxExtendedCommand; ///< CPU支持的最大扩展命令
};

#endif