/// @file LGpuTemp.h
/// @brief 该文件声明了GPU温度获取类
/// 
/// Detail: 该头文件申明的类关联nvapi.lib
/// @author Burnell_Liu Email:burnell_liu@qq.com
/// @version   
/// @date 6:11:2015


#ifndef _LGPUTEMP_H_
#define _LGPUTEMP_H_

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

/// @brief 最大GPU温度传感器数量
#ifndef MAX_GPU_SENSORS_NUMBER
#define MAX_GPU_SENSORS_NUMBER 16
#endif

class CGpuTemp;

/// @brief GPU温度接口
/// 该类只能获取到独立显卡的温度
class LGpuTemp
{
public:
    /// @brief 构造函数
    LGpuTemp();


    /// @brief 析构函数
    ~LGpuTemp();

    /// @brief 获取GPU温度
    /// @param[out] sensorsNum 存储GPU温度传感器数量
    /// @param[out] temp 存储温度, 单位摄氏度
    /// @return 成功返回true, 失败返回false
    bool Get(OUT unsigned int& sensorsNum, OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER]);

private:
    CGpuTemp* m_pGpuTemp; ///< GPU实现类
};



#endif