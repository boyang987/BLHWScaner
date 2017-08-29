
#include "LSystemMetrics.h"

#include <Windows.h>

namespace LSystemMetrics
{
    bool IsTouchScreenSupported()
    {
        int iRet = GetSystemMetrics(SM_DIGITIZER);

        if ((iRet&NID_INTEGRATED_TOUCH) == NID_INTEGRATED_TOUCH)
            return true;

        if ((iRet&NID_EXTERNAL_TOUCH) == NID_EXTERNAL_TOUCH)
            return true;

        return false;
    }
}