
#include "Win8API.h"

#include <Windows.h>
#include <WinBase.h>

/// @brief 判断是否是Win8或Win8以上系统
static bool IsWindows8orLater()
{
    OSVERSIONINFOW osVersionInfo = {0};
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    GetVersionExW(&osVersionInfo);
    if (osVersionInfo.dwMajorVersion >= 6 
        &&osVersionInfo.dwMinorVersion >= 2)
        return true;
    
    return false;
}

typedef int (*GetBIOSModeFun)();


int GetBIOSMode()
{
    if (!IsWindows8orLater())
    {
        return BIOS_UNKNOWN;
    }

    HMODULE hWin8API = LoadLibraryW(L".\\Win8API\\Win8API.dll");
    GetBIOSModeFun GetBIOSMode = (GetBIOSModeFun)GetProcAddress(hWin8API, "GetBIOSMode");
    if (NULL == GetBIOSMode)
        return BIOS_UNKNOWN;

    return GetBIOSMode();

    FreeLibrary(hWin8API);
    hWin8API = NULL;
    return BIOS_UNKNOWN;
}

