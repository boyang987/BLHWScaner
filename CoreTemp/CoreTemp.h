
#ifndef _CORE_TEMP_H_
#define _CORE_TEMP_H_

#ifdef _EXPORTING_CORETEMP
#define CORETEMP_API_DECLSPEC __declspec(dllexport)
#else
#define CORETEMP_API_DECLSPEC __declspec(dllimport)
#endif

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

/// @brief 最大GPU温度传感器数量
#ifndef MAX_GPU_SENSORS_NUMBER
#define MAX_GPU_SENSORS_NUMBER 16
#endif

extern "C"
{
    /// @brief 获取CPU温度
    /// 需要管理员权限
    /// @param[out] pCoreNum 存储CPU物理核心数
    /// @param[out] temp 存储CPU物理核心温度
    /// @return 成功返回true, 失败返回false
    CORETEMP_API_DECLSPEC bool GetCpuTemp(
        OUT unsigned int* pCoreNum, 
        OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM]);

    /// @brief 获取GPU温度
    /// 只能获取独立显卡的温度
    /// @param[out] pSensorNum 存储GPU温度传感器数量
    /// @param[out] temp 存储温度
    /// @return 成功返回true, 失败返回false
    CORETEMP_API_DECLSPEC bool GetGpuTemp(
        OUT unsigned int* pSensorNum, 
        OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER]);
};

#endif