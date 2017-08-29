/// @file LCpuTemp.h
/// @brief 该头文件中声明了CPU温度获取类
/// 
/// 该头文件申明的类关联WinRing0, 并且需要管理员权限
/// Detail:
/// @author Burnell_Liu  
/// @version   
/// @date 1:9:2015

#ifndef _LCPUTEMP_H_
#define _LCPUTEMP_H_

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

/// @brief 最大处理器物理核心数
#ifndef MAX_PROCESSOR_PHYSICAL_CORE_NUM
#define MAX_PROCESSOR_PHYSICAL_CORE_NUM 64
#endif

class CCpuTemp;

/// @brief CPU温度接口
class LCpuTemp
{
public:
    /// @brief 构造函数
    /// @param[in] winRing0Path WinRing0 DLL路径
    explicit LCpuTemp(IN const wstring& winRing0Path);

    /// @brief 析构函数
    ~LCpuTemp();

    /// @brief 获取CPU温度
    /// @param[out] coreNum 存储CPU物理核心数
    /// @param[out] temp 存储温度, 单位摄氏度
    /// @return 成功返回true, 失败返回false
    bool Get(OUT unsigned int& coreNum, OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM]);

private:
    CCpuTemp* m_pCpuTemp;
};

#endif