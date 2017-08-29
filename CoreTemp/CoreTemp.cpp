
#define _EXPORTING_CORETEMP
#include "CoreTemp.h"

#include <cstring>
#include <Windows.h>

#include "LCpuTemp.h"
#include "LGpuTemp.h"

LCpuTemp* gCpuTemp = NULL; ///< CPU温度对象指针
LGpuTemp* gGpuTemp = NULL; ///< GPU温度对象指针


/// @brief 进程加载DLL
/// @param[in] hModule DLL模块句柄
static void DllProcessAttach(IN HMODULE hModule)
{
    const int NAME_MAX_LEN = 1024;
    wchar_t dllPath[NAME_MAX_LEN] = {0};
    DWORD dwRet = GetModuleFileNameW(hModule, dllPath, NAME_MAX_LEN);
    if (dwRet == 0)
        return;

    wchar_t* pEndLoc =  wcsrchr(dllPath, L'\\');
    if (pEndLoc != NULL)
    {
        (*pEndLoc) = L'\0';
    }

    wchar_t winRing0Path[NAME_MAX_LEN] = {0};
    wsprintfW(winRing0Path, L"%s\\%s", dllPath, L"WinRing0.dll");

    if (gCpuTemp != 0)
    {
        delete gCpuTemp;
        gCpuTemp = 0;
    }

    gCpuTemp = new LCpuTemp(winRing0Path);

    if (gGpuTemp != 0)
    {
        delete gGpuTemp;
        gGpuTemp = 0;
    }

    gGpuTemp = new LGpuTemp();

}

/// @brief 进程卸载DLL
static void DllProcessDetach()
{
    if (gCpuTemp != 0)
    {
        delete gCpuTemp;
        gCpuTemp = 0;
    }

    if (gGpuTemp != 0)
    {
        delete gGpuTemp;
        gGpuTemp = 0;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD canReason, LPVOID lpReserved)
{
    switch (canReason)
    {
    case DLL_PROCESS_ATTACH:
        DllProcessAttach(hModule);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        DllProcessDetach();
        break;
    }
    return TRUE;
}

bool GetCpuTemp(
    OUT unsigned int* pCoreNum, 
    OUT unsigned int temp[MAX_PROCESSOR_PHYSICAL_CORE_NUM])
{
    if (pCoreNum == NULL)
        return false;

    if (gCpuTemp == NULL)
        return false;

    return gCpuTemp->Get((*pCoreNum), temp);
}

bool GetGpuTemp(
    OUT unsigned int* pSensorNum, 
    OUT unsigned int temp[MAX_GPU_SENSORS_NUMBER])
{
    if (pSensorNum == NULL)
        return false;

    if (gGpuTemp == NULL)
        return false;

    return gGpuTemp->Get((*pSensorNum), temp);
}