
#ifndef _LSYSTEMMETRICS_H_
#define _LSYSTEMMETRICS_H_

#ifndef IN
#define IN
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef OUT
#define OUT
#endif

namespace LSystemMetrics
{
    /// @brief 判断是否支持触摸屏
    /// @return true(支持)，false(不支持)
    bool IsTouchScreenSupported();
}



#endif